/**
 * @file main.c
 * @author Brian Bradley (brian.bradley.p@gmail.com)
 * @brief Sample usage for gas sensor
 * @date 2021-10-06
 * 
 * @copyright Copyright (C) 2021 LION
 * 
 * SPDX-License-Identifier: Apache-2.0
 * 
 */
#include <zephyr.h>
#include <drivers/sensor.h>
#include <drivers/sensor/mics4514.h>

#define GAS_SENSOR	DT_LABEL(DT_NODELABEL(gas_sensor))

void main(void)
{
	const struct device *gas;
	int ret;

	gas = device_get_binding(GAS_SENSOR);
	if (gas == NULL) {
		printk("Error getting gas sensor device: %s\n", GAS_SENSOR);
	}

	while (1) {
		struct sensor_value co = {0};
		struct sensor_value no2 = {0};

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
		printk("CO(ppm)=%d.%d, NO2(ppm)=%d.%d\n", co.val1, co.val2, no2.val1, no2.val2);
		k_msleep(1000);
	}
}
