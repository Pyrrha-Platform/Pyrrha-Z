/**
 * @file alerts.c
 * @author Brian Bradley (brian.bradley.p@gmail.com)
 * @brief 
 * @date 2022-01-26
 * 
 * @copyright Copyright (C) 2022 LION
 * 
 * SPDX-License-Identifier: Apache-2.0
 * 
 */

#include <alerts.h>
#include <logging/log.h>
#include <zephyr.h>
#include <device.h>
#include <drivers/pwm.h>

LOG_MODULE_REGISTER(alerts, CONFIG_PYRRHA_LOG_LEVEL);

#define OVER_TEMPERATURE(temp) (temp > CONFIG_TEMPERATURE_ALERT_THRESHOLD_CELCIUS)
#define OVER_CO(co) (co > CONFIG_CO_ALERT_THRESHOLD_PPM)
#define OVER_NO2(no2) (no2 > CONFIG_NO2_ALERT_THRESHOLD_PPM)

/*
 * 50hz is flicker fusion threshold. Modulated light will be perceived
 * as steady by our eyes when blinking rate is at least 50.
 */
#define PERIOD_USEC	(USEC_PER_SEC / 50U)

#define RED_LED_NODE DT_ALIAS(red_pwm_led)
#define GREEN_LED_NODE DT_ALIAS(green_pwm_led)
#define BLUE_LED_NODE DT_ALIAS(blue_pwm_led)

#define COLOR_TO_PULSE(color, period)         (color == UINT8_MAX ? (period) : (color * (period) >> 8))

/**
 * @brief union which allows easy set/conversion to hex or rgb directly
 * 
 */
typedef union Color{
    uint32_t hex;
    struct{
        uint8_t red;
        uint8_t green;
        uint8_t blue;
    };
}color_t;

/**
 * @brief Configuration table for pwm led
 * 
 */
struct pwm_cfg{
    const struct device * pwm;
    const uint8_t pin;
    const uint8_t flags;
    const uint32_t period;
};

/**
 * @brief Container for all pwm leds associated with the rgb led
 * 
 */
struct rgb_led_device{
    const struct pwm_cfg red;
    const struct pwm_cfg green;
    const struct pwm_cfg blue;
};

K_MUTEX_DEFINE(mtx_alert);
static color_t g_alert_override = {0};

const struct rgb_led_device g_led = {
    .red = {
        .pwm = DEVICE_DT_GET(DT_PWMS_CTLR(RED_LED_NODE)),
        .pin = DT_PWMS_CHANNEL(RED_LED_NODE),
        .flags = DT_PWMS_FLAGS(RED_LED_NODE), 
        .period = PERIOD_USEC
    },
    .green = {
        .pwm = DEVICE_DT_GET(DT_PWMS_CTLR(GREEN_LED_NODE)),
        .pin = DT_PWMS_CHANNEL(GREEN_LED_NODE),
        .flags = DT_PWMS_FLAGS(GREEN_LED_NODE), 
        .period = PERIOD_USEC
    },
    .blue = {
        .pwm = DEVICE_DT_GET(DT_PWMS_CTLR(BLUE_LED_NODE)),
        .pin = DT_PWMS_CHANNEL(BLUE_LED_NODE),
        .flags = DT_PWMS_FLAGS(BLUE_LED_NODE), 
        .period = PERIOD_USEC
    },
};

/**
 * @brief Set pwm output directly to the coresponding 8-bit color value
 * 
 * @param cfg : configuration table for pwm device
 * @param color : 8-bit color value for the specified pwm
 * @retval 0 on success
 * @retval -ENODEV if no configuration table
 * @retval -EFAULT if pwm set error
 */
static int set_pwm(const struct pwm_cfg * cfg, uint8_t color)
{
    if (cfg == NULL){
        return -ENODEV;
    }
	const struct device *dev = cfg->pwm;
    uint32_t pulse = COLOR_TO_PULSE(color, cfg->period);
	/* Verify pwm_pin_set_usec() */
	if (pwm_pin_set_usec(dev, cfg->pin, cfg->period, pulse, cfg->flags)) {
		LOG_ERR("Fail to set the period and pulse width");
		return -EFAULT;
	}
	return 0;
}

/**
 * @brief Set the alert led to a specific color
 * 
 * @param color : rgb representation of desired color 
 * @retval 0 on success
 * @retval -errno otherwise
 */
static int set_alert_led(color_t color){
    set_pwm(&g_led.red, color.red);
    set_pwm(&g_led.green, color.green);
    set_pwm(&g_led.blue, color.blue);
    return 0;
}

/**
 * @brief generate an alert from sensor data exceeding set thresholds
 */
static void threshold_alert(void){
    const color_t alert_color = {
        .red = 0xff
    };
    set_alert_led(alert_color);
}

int override_alert(uint32_t color){
    if (k_mutex_lock(&mtx_alert, K_MSEC(250)) != 0){
        return -ENOLCK;
    }
    g_alert_override.hex = color;
    set_alert_led(g_alert_override);
    k_mutex_unlock(&mtx_alert);
    return 0;
}

int release_alert_override(){
    return override_alert(0);
}

int generate_alerts(struct pyrrha_data * data){
    if (k_mutex_lock(&mtx_alert, K_MSEC(250)) != 0){
        return -ENOLCK;
    }
    /* Only generate an alert when there is no override condition */
    if (g_alert_override.hex == 0){
        if (OVER_TEMPERATURE(data->rht.temperature.val1) ||
            OVER_CO(data->gas.co.val1) ||
            OVER_NO2(data->gas.no2.val1))
        {
            threshold_alert();
        }
    }
    k_mutex_unlock(&mtx_alert);
    return 0;
}
