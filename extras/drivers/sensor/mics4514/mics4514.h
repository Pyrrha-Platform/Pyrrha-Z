/**
 * @file mics4514.h
 * @author Brian Bradley (brian.bradley.p@gmail.com)
 * @brief 
 * @date 2021-10-04
 * 
 * @copyright Copyright (C) 2021 LION
 * 
 * SPDX-License-Identifier: Apache-2.0
 * 
 */

#ifndef PYRRHA_DRIVERS_SENSOR_MICS4514_MICS4514_H_
#define PYRRHA_DRIVERS_SENSOR_MICS4514_MICS4514_H_

#include <errno.h>
#include <zephyr/types.h>
#include <device.h>
#include <drivers/sensor.h>
#include <sys/util.h>
#include <drivers/gpio.h>

/**
 * @brief data structure containing all data retreived from mics4514 on fetch
 * 
 */
struct mics4514_sensor_data {
	uint8_t status;
	uint16_t raw;
	uint16_t co;
	uint16_t no2;
};

/**
 * @brief Device driver data for mics4514
 * 
 */
struct mics4514_data {
	struct adc_channel_cfg ch_cfg;
	struct mics4514_sensor_data sensor;
};

#if DT_INST_NODE_HAS_PROP(0, preheat_gpios)
	struct gpio_channel_config {
		const char *label;
		uint8_t pin;
		uint8_t flags;
	};
#endif
/**
 * @brief configuration data for mics4514
 * 
 */
struct mics4514_config {
	const struct device *adc;
	#if DT_INST_NODE_HAS_PROP(0, preheat_gpios)
	const struct device *gpio;
	#endif
	#if DT_INST_NODE_HAS_PROP(0, preheat_gpios)
	const struct gpio_channel_config gpio_cfg;
	#endif
	uint8_t adc_channel;
	uint32_t rload_red;
	uint32_t rload_ox;
};

#endif /* PYRRHA_DRIVERS_SENSOR_MICS4514_MICS4514_H_ */
