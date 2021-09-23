/**
 * @file cc2d23s.c
 * @author Brian Bradley (brian.bradley.p@gmail.com)
 * @brief Implementation of cc2d23s sensor driver
 * @date 2021-09-22
 * 
 * @copyright Copyright (C) 2021 LION
 * 
 * SPDX-License-Identifier: Apache-2.0
 * 
 */

#define DT_DRV_COMPAT amphenol_cc2d23s

#include <errno.h>
#include <zephyr.h>
#include <device.h>
#include <kernel.h>
#include <drivers/i2c.h>
#include <init.h>
#include <sys/__assert.h>
#include <logging/log.h>

#include "cc2d23s.h"

LOG_MODULE_REGISTER(CC2D23S, CONFIG_SENSOR_LOG_LEVEL);

/**
 * @brief Perform an I2C read transaction on the cc2d23s
 * 
 * @param dev device containing driver data
 * @param buf buffer containing the data which was read back from the device
 * @param len number of bytes to read into `buf`
 * @retval 0 on success
 * @retval -errno otherwise
 */
static int cc2d23s_read(const struct device *dev, uint8_t *buf, uint32_t len)
{
    const struct cc2d23s_data *data = dev->data;
	const struct cc2d23s_config *cfg = dev->config;
	return i2c_read(data->i2c_master, buf, len, cfg->i2c_addr);
}

/**
 * @brief Perform an I2C write transaction on the cc2d23s
 * 
 * @param dev device containing driver data
 * @param buf buffer containing the data to be sent to the device
 * @param len number of bytes to written from `buf`
 * @retval 0 on success
 * @retval -errno otherwise
 */
static int cc2d23s_write(const struct device *dev, uint8_t * buf, uint32_t len)
{
    const struct cc2d23s_data *data = dev->data;
	const struct cc2d23s_config *cfg = dev->config;

	return i2c_write(data->i2c_master, buf, len, cfg->i2c_addr);
}

/**
 * @brief Enter command mode on the cc2d23s as described
 * in section 4.9 in the ChipCap2 Application Guide
 * 
 * @param dev device containing driver data
 * @retval 0 on success
 * @retval -errno otherwise 
 */
static int cc2d23s_enter_command_mode(const struct device * dev){
	uint8_t buf[] = {CC2D23S_REG_COMMAND_MODE, 0, 0};
	return cc2d23s_write(dev, buf, sizeof(buf));
}

/**
 * @brief Ends command mode on the cc2d23s 
 * and enters normal operation mode as described
 * in section 4.9 in the ChipCap2 Application Guide
 * 
 * @param dev device containing driver data
 * @retval 0 on success
 * @retval -errno otherwise
 */
static int cc2d23s_enter_normal_mode(const struct device * dev){
	uint8_t buf[] = {CC2D23S_REG_NORMAL_MODE, 0, 0};
	return cc2d23s_write(dev, buf, sizeof(buf));
}

/**
 * @brief Read sensor data from cc2d23s
 * 
 * @param dev device containing driver data
 * @param rht temperature and humidity data where measurements will be placed
 * @retval 0 on success
 * @retval -errno otherwise
 */
static int cc2d23s_read_rht(const struct device * dev, struct cc2d23s_sensor_data * rht){
	if (rht == NULL){
		return -ENODEV;
	}
	uint8_t buf[4] = {0};
	int rc = cc2d23s_read(dev, buf, sizeof(buf));
	if (rc == 0){
		rht->status = buf[0] >> 6;
		rht->humidity = 100 * (((buf[0] & 0x3f) << 8) + buf[1]) >> 14;
		rht->temperature = (165 * ((buf[2] << 6) + (buf[3] >> 2)) >> 14) - 40; 
	}
	return rc;
}

/**
 * @brief Initialize and configure cc2d23s sensor
 * 
 * @param dev device containing driver data
 * @retval 0 on success
 * @retval -errno otherwise
 */
static int cc2d23s_init(const struct device *dev)
{
    struct cc2d23s_data *data = dev->data;
	const struct cc2d23s_config *cfg = dev->config;
	struct cc2d23s_sensor_data * rht = &data->rht;
	int rc = 0;
	
	memset(rht, 0, sizeof(struct cc2d23s_sensor_data));

	data->i2c_master = device_get_binding(cfg->i2c_bus);
	if (!data->i2c_master) {
		LOG_DBG("i2c master not found: %s", cfg->i2c_bus);
		return -EINVAL;
	}

	return rc;
}

/**
 * @brief Fetch the temperature and humidity
 * 
 * @param dev device containing driver data
 * @param chan sensor channel to access. Only temperature and humidity channels are supported
 * @retval 0 if success
 * @retval -errno otherwise 
 */
static int cc2d23s_sample_fetch(const struct device *dev, enum sensor_channel chan)
{
	struct cc2d23s_data *data = dev->data;

	if(chan == SENSOR_CHAN_ALL || chan == SENSOR_CHAN_AMBIENT_TEMP || chan == SENSOR_CHAN_HUMIDITY){
		return cc2d23s_read_rht(dev, &data->rht);
	}
	return -ENOTSUP;
}

/**
 * @brief Get the result of the sensor measurement in integer and fractional
 * form
 * 
 * @param dev device containing driver data
 * @param chan sensor channel to access. Only temperature and humidity channels are supported
 * @param val sensor value containing the integer/fractional form of the
 * register data
 * @retval 0
 */
static int cc2d23s_channel_get(const struct device *dev,
			       enum sensor_channel chan,
			       struct sensor_value *val)
{
	struct cc2d23s_data *data = dev->data;
	struct cc2d23s_sensor_data * rht = &data->rht;

	switch (chan) {
		case SENSOR_CHAN_AMBIENT_TEMP:
			val->val1 = rht->temperature;
			val->val2 = 0;
			break;
		case  SENSOR_CHAN_HUMIDITY:
			val->val1 = rht->humidity;
			val->val2 = 0;
			break;
		default:
			return -ENOTSUP;
	}
	return 0;
}

static struct cc2d23s_data cc2d23s_data;
static const struct cc2d23s_config cc2d23s_cfg = {
	.i2c_bus = DT_INST_BUS_LABEL(0),
	.i2c_addr = DT_INST_REG_ADDR(0),
};

static const struct sensor_driver_api cc2d23s_api_funcs = {
	.sample_fetch = cc2d23s_sample_fetch,
	.channel_get = cc2d23s_channel_get,
};

DEVICE_DT_INST_DEFINE(0, cc2d23s_init, NULL,
		    &cc2d23s_data, &cc2d23s_cfg, POST_KERNEL,
		    CONFIG_SENSOR_INIT_PRIORITY, &cc2d23s_api_funcs);
