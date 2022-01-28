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
LOG_MODULE_REGISTER(alerts, CONFIG_PYRRHA_LOG_LEVEL);
#define OVER_TEMPERATURE(temp) (temp > CONFIG_TEMPERATURE_ALERT_THRESHOLD_CELCIUS)
#define OVER_CO(co) (co > CONFIG_CO_ALERT_THRESHOLD_PPM)
#define OVER_NO2(no2) (no2 > CONFIG_NO2_ALERT_THRESHOLD_PPM)

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

K_MUTEX_DEFINE(mtx_alert);
static color_t g_alert_override = {0};

/**
 * @brief Set the alert led to a specific color
 * 
 * @param color : rgb representation of desired color 
 * @retval 0 on success
 * @retval -errno otherwise
 */
static int set_alert_led(color_t color){
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
