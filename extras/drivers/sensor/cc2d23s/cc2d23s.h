/**
 * @file cc2d23s.h
 * @author Brian Bradley (brian.bradley.p@gmail.com)
 * @brief Header file for cc2d23s sensor driver
 * @date 2021-09-22
 * 
 * @copyright Copyright (C) 2021 LION
 * 
 * SPDX-License-Identifier: Apache-2.0
 * 
 */

#ifndef PYRRHA_DRIVERS_SENSOR_CC2D23S_CC2D23S_H_
#define PYRRHA_DRIVERS_SENSOR_CC2D23S_CC2D23S_H_

#include <errno.h>
#include <zephyr/types.h>
#include <device.h>
#include <drivers/sensor.h>
#include <sys/util.h>
#include <drivers/gpio.h>

#define CC2D23S_REG_COMMAND_MODE	0xA0
#define CC2D23S_REG_NORMAL_MODE		0x80

/**
 * @brief data structure containing all data retreived from cc2d23s on fetch
 * 
 */
struct cc2d23s_sensor_data {
	uint8_t status;
	uint16_t humidity;
	int16_t temperature;
};

/**
 * @brief Device driver data for cc2d23s
 * 
 */
struct cc2d23s_data {
	const struct device *i2c_master;
	struct cc2d23s_sensor_data rht;
};

/**
 * @brief configuration data for cc2d23s
 * 
 */
struct cc2d23s_config {
	const char *i2c_bus;
	uint16_t i2c_addr;
};

#endif /* PYRRHA_DRIVERS_SENSOR_CC2D23S_CC2D23S_H_ */
