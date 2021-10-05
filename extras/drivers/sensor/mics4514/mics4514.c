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
#include <drivers/sensor/mics4514.h>
#include "mics4514.h"

#ifdef CONFIG_ADC_NRFX_SAADC
#define MICS4514_GAIN ADC_GAIN_1_4
#define MICS4514_REF ADC_REF_VDD_1_4
#define MICS4514_RESOLUTION 12
#else
#define MICS4514_GAIN ADC_GAIN_1
#define MICS4514_REF ADC_REF_VDD_1
#define MICS4514_RESOLUTION 12
#endif

LOG_MODULE_REGISTER(MICS4514, CONFIG_SENSOR_LOG_LEVEL);

static struct adc_sequence_options options = {
	.interval_us = 12,
	.extra_samplings = 0,
};

static struct adc_sequence adc_table = {
	.options = &options,
};

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
	struct mics4514_data *drv_data = dev->data;
	const struct mics4514_config *cfg = dev->config;

	if (!device_is_ready(cfg->adc)) {
		LOG_ERR("ADC device is not ready.");
		return -EINVAL;
	}

	#if DT_INST_NODE_HAS_PROP(0, preheat_gpios)
		if (!device_is_ready(cfg->gpio)) {
			LOG_ERR("Preheat GPIO device is not ready.");
			return -EINVAL;
		}

		/* Configure GPIO but don't turn it on */
		gpio_pin_configure(cfg->gpio, cfg->gpio_cfg.pin, GPIO_OUTPUT_INACTIVE | cfg->gpio_cfg.flags);
	#endif 

	drv_data->ch_cfg = (struct adc_channel_cfg){
		.gain = MICS4514_GAIN,
		.reference = MICS4514_REF,
		.acquisition_time = ADC_ACQ_TIME_DEFAULT,
		.channel_id = cfg->adc_channel,
		#ifdef CONFIG_ADC_NRFX_SAADC
			.input_positive = SAADC_CH_PSELP_PSELP_AnalogInput0 + cfg->adc_channel,
		#endif
	};

	adc_table.buffer = &drv_data->sensor.raw;
	adc_table.buffer_size = sizeof(drv_data->sensor.raw);
	adc_table.resolution = MICS4514_RESOLUTION;
	adc_table.channels = BIT(cfg->adc_channel);

	adc_channel_setup(cfg->adc, &drv_data->ch_cfg);

	return 0;
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
	struct mics4514_data *data = dev->data;
	switch ((int)chan) {
		case SENSOR_CHAN_ALL:
		case SENSOR_CHAN_MICS4514_CO:
		case SENSOR_CHAN_MICS4514_NO2:
			return mics4514_read_sensor(dev, &data->sensor);
		default:
			return -ENOTSUP;
	}
	return 0;
}

static struct mics4514_data mics4514_data;
static const struct mics4514_config mics4514_cfg = {
	.adc = DEVICE_DT_GET(DT_INST_IO_CHANNELS_CTLR(0)),
	#if DT_INST_NODE_HAS_PROP(0, preheat_gpios)
	.gpio = DEVICE_DT_GET(DT_INST_PHANDLE(0, preheat_gpios)),
	.gpio_cfg = {
		.label = DT_INST_GPIO_LABEL(0, preheat_gpios),
		.pin = DT_INST_GPIO_PIN(0, preheat_gpios),
		.flags = DT_INST_GPIO_FLAGS(0, preheat_gpios)
	},
	#endif
	.adc_channel = DT_INST_IO_CHANNELS_INPUT(0),
	.rload_ox = DT_PROP(DT_DRV_INST(0), rload_ox_ohms),
	.rload_red = DT_PROP(DT_DRV_INST(0), rload_red_ohms)
};

static const struct sensor_driver_api mics4514_api_funcs = {
	.sample_fetch = mics4514_sample_fetch,
	.channel_get = mics4514_channel_get,
};

DEVICE_DT_INST_DEFINE(0, mics4514_init, NULL,
		    &mics4514_data, &mics4514_cfg, POST_KERNEL,
		    CONFIG_SENSOR_INIT_PRIORITY, &mics4514_api_funcs);
