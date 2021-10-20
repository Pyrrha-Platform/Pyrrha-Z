/*
 * Copyright (c) 2021 LION
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr.h>
#include "app_version.h"

#include <logging/log.h>
#include <drivers/gpio.h>
#include <drivers/sensor.h>
#include <drivers/sensor/mics4514.h>

/* The devicetree node identifier for the "led0" alias. */
#define LED0_NODE DT_ALIAS(led0)

#if DT_NODE_HAS_STATUS(LED0_NODE, okay)
#define LED0	DT_GPIO_LABEL(LED0_NODE, gpios)
#define PIN	DT_GPIO_PIN(LED0_NODE, gpios)
#define FLAGS	DT_GPIO_FLAGS(LED0_NODE, gpios)
#else
/* A build error here means your board isn't set up to blink an LED. */
#error "Unsupported board: led0 devicetree alias is not defined"
#define LED0	""
#define PIN	0
#define FLAGS	0
#endif

#define RHT	DT_LABEL(DT_NODELABEL(rht_sensor))
#define GAS_SENSOR	DT_LABEL(DT_NODELABEL(gas_sensor))

LOG_MODULE_REGISTER(main, CONFIG_PYRRHA_LOG_LEVEL);

void main(void)
{
	const struct device *led, *rht, *gas;
	bool led_is_on = true;
	int ret;

	printk("Starting Pyrrha firmware version %s\n", APP_VERSION_STR);

	led = device_get_binding(LED0);
	if (led == NULL) {
		LOG_DBG("Error getting led device");
	}

	rht = device_get_binding(RHT);
	if (rht == NULL) {
		LOG_DBG("Error getting rht device: %s", RHT);
	}

	gas = device_get_binding(GAS_SENSOR);
	if (gas == NULL) {
		printk("Error getting gas sensor device: %s\n", GAS_SENSOR);
	}

	gpio_pin_configure(led, PIN, GPIO_OUTPUT_ACTIVE | FLAGS);

	while (1) {
		struct sensor_value temperature = {0};
		struct sensor_value humidity = {0};
		struct sensor_value co = {0};
		struct sensor_value no2 = {0};

		if (rht != NULL){
			/* Get rht data */
			if ((ret = sensor_sample_fetch(rht)) != 0){
				LOG_ERR("rht sensor fetch error");
			}
			else{
				sensor_channel_get(rht, SENSOR_CHAN_AMBIENT_TEMP,
					&temperature);
				sensor_channel_get(rht, SENSOR_CHAN_HUMIDITY,
					&humidity);
			}
		}

		if (gas != NULL){
			if ((ret = sensor_sample_fetch(gas)) != 0){
				printk("gas sensor fetch error: %d\n", ret);
			}
			else{
				sensor_channel_get(gas, SENSOR_CHAN_MICS4514_CO,
					&co);
				sensor_channel_get(gas, SENSOR_CHAN_MICS4514_NO2,
					&no2);
			}
		}

		/* Toggle LED */
		gpio_pin_set(led, PIN, (int)led_is_on);
		led_is_on = !led_is_on;
		LOG_DBG("LED=%d, temperature (C)=%d, humidity (%%)=%d, CO(ppm)=%d.%d, NO2(ppm)=%d.%d", \
		led_is_on, temperature.val1, humidity.val1, co.val1, co.val2, no2.val1, no2.val2);
		k_msleep(1000);
	}
}
