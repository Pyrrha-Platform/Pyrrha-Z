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
};

/**
 * @brief Device driver data for mics4514
 * 
 */
struct mics4514_data {
	const struct device *adc;
	struct adc_channel_cfg ch_cfg;
	struct mics4514_sensor_data sensor;
};

/**
 * @brief configuration data for mics4514
 * 
 */
struct mics4514_config {
	const struct device *adc;
	uint8_t adc_channel;
};

#endif /* PYRRHA_DRIVERS_SENSOR_MICS4514_MICS4514_H_ */
