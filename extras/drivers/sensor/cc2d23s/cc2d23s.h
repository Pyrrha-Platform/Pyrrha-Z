/**
 * @file cc2d23s.h
 * @author Brian Bradley (brian.bradley.p@gmail.com)
 * @brief 
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

/**
 * @brief 
 * 
 */
struct cc2d23s_data {
	const struct device *i2c_master;
	uint32_t reg_val;
	uint8_t config;
};

/**
 * @brief 
 * 
 */
struct cc2d23s_config {
	const char *i2c_bus;
	uint16_t i2c_addr;
};

/**
 * @brief 
 * 
 * @param dev 
 * @param val 
 * @return int 
 */
int cc2d23s_read(const struct device *dev, uint32_t *val);
/**
 * @brief 
 * 
 * @param dev 
 * @param val 
 * @return int 
 */
int cc2d23s_write(const struct device *dev, uint8_t *val);

#endif /* PYRRHA_DRIVERS_SENSOR_CC2D23S_CC2D23S_H_ */
