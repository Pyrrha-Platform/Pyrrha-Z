/**
 * @file mics4514.c
 * @author Brian Bradley (brian.bradley.p@gmail.com)
 * @brief Implementation of MiCS-4514 driver
 * @date 2021-10-04
 * 
 * @copyright Copyright (C) 2021 LION
 * 
 * SPDX-License-Identifier: Apache-2.0
 * 
 */
#define DT_DRV_COMPAT amphenol_mics4514

#include <errno.h>
#include <zephyr.h>
#include <device.h>
#include <kernel.h>
#include <drivers/adc.h>
#include <init.h>
#include <sys/__assert.h>
#include <logging/log.h>

#include "mics4514.h"

LOG_MODULE_REGISTER(MICS4514, CONFIG_SENSOR_LOG_LEVEL);

/**
 * @brief Read data from sensor
 * 
 * @param dev device containing driver data
 * @param sensor container for sensor data
 * @retval 0 on success
 * @retval -errno otherwise
 */
static int mics4514_read_sensor(const struct device *dev, struct mics4514_sensor_data *sensor){
    return -ENOTSUP;
}

/**
 * @brief Initialize and configure mics4514 sensor
 * 
 * @param dev device containing driver data
 * @retval 0 on success
 * @retval -errno otherwise
 */
static int mics4514_init(const struct device *dev)
{
	return -ENOTSUP;
}

/**
 * @brief Fetch the sensor data
 * 
 * @param dev device containing driver data
 * @param chan sensor channel to access. 
 * @retval 0 if success
 * @retval -errno otherwise 
 */
static int mics4514_sample_fetch(const struct device *dev, enum sensor_channel chan)
{
	return -ENOTSUP;
}

/**
 * @brief Get the result of the sensor measurement in integer and fractional
 * form
 * 
 * @param dev device containing driver data
 * @param chan sensor channel to access.
 * @param val sensor value containing the integer/fractional form of the
 * register data
 * @retval 0
 */
static int mics4514_channel_get(const struct device *dev,
			       enum sensor_channel chan,
			       struct sensor_value *val)
{
	switch (chan) {
		default:
			return -ENOTSUP;
	}
	return 0;
}

static struct mics4514_data mics4514_data;
static const struct mics4514_config mics4514_cfg = {
};

static const struct sensor_driver_api mics4514_api_funcs = {
	.sample_fetch = mics4514_sample_fetch,
	.channel_get = mics4514_channel_get,
};

DEVICE_DT_INST_DEFINE(0, mics4514_init, NULL,
		    &mics4514_data, &mics4514_cfg, POST_KERNEL,
		    CONFIG_SENSOR_INIT_PRIORITY, &mics4514_api_funcs);
