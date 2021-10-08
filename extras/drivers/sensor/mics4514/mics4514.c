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
#include <math.h>
#include "mics4514.h"

#ifdef CONFIG_ADC_NRFX_SAADC
#define MICS4514_GAIN ADC_GAIN_1_4
#define MICS4514_REF ADC_REF_INTERNAL
#define MICS4514_RESOLUTION 12
#else
#define MICS4514_GAIN ADC_GAIN_1
#define MICS4514_REF ADC_REF_INTERNAL
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
 * @brief Curve interpolation details
 * 
 */
struct curve_interp_cfg{
	float slope;
	float y_int;
};

/**
 * Figures derived from interpolation of concentration curves in sensor datasheet.
 * Interpolation tables collected here: https://myscope.net/auswertung-der-airpi-gas-sensoren/
 * 
 */
static const struct curve_interp_cfg co_curve_cfg = {
	.slope = -1.1859,
	.y_int = 0.6201
};

static const struct curve_interp_cfg no2_curve_cfg = {
	.slope = 0.9682,
	.y_int = -0.8108
};

/**
 * @brief Calculate gas concentration based on sensor readings and curve characteristics
 * 
 * @param curve : sensor characteristic curve data
 * @param vs_mv : voltage measured at sensor
 * @param rload_ohms : load resistance on sense pin
 * @param r0_ohms : sensor resistance at 1000ppm gas concentration
 * @return double : gas concetration in ppm
 */
static double calculate_ppm(const struct curve_interp_cfg * curve, int32_t vs_mv, uint32_t rload_ohms, uint32_t r0_ohms)
{
	if (curve == NULL){
		return 0;
	}
	/* Calculate sense resistance (Rs) */
	float rs = (float) rload_ohms * (CONFIG_MICS4514_SENSOR_VREF_MV - vs_mv) / vs_mv;

	/* Calculate ratio of sense resistance (Rs) to calibrated sense resistance (R0) = Rs/R0 */
	float rs_r0 = (float)rs / r0_ohms;

	/* Calculate gas concentration in ppm according to sensor characteristic curves 
	Details on interpolation here: https://myscope.net/auswertung-der-airpi-gas-sensoren/
	*/
	double ppm = pow(10, curve->slope * log10(rs_r0) + curve->y_int);

	return ppm;
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
	const struct mics4514_config *cfg = dev->config;
	int rc = -ENOTSUP;
	switch ((int)chan){
		case SENSOR_CHAN_MICS4514_NO2:
		case SENSOR_CHAN_MICS4514_CO:
		case SENSOR_CHAN_ALL:
			rc = adc_read(cfg->adc, &adc_table);
			if (rc == -EBUSY){
				/* If the device is busy, wait 100 usec and try again */
				k_usleep(100);
				rc = adc_read(cfg->adc, &adc_table);
			}
		default: return rc;
	}
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
	const struct mics4514_config *cfg = dev->config;
	uint16_t vref = adc_ref_internal(cfg->adc);
	
	int rc = -ENOTSUP;
	switch ((int)chan){
		case SENSOR_CHAN_MICS4514_CO:
		{
			int32_t mv = (int32_t)data->raw_data[ADC_CHANNEL_CO];
			rc = adc_raw_to_millivolts(vref,
						data->ch_cfg[ADC_CHANNEL_CO].gain,
						adc_table.resolution,
						&mv);
			if (rc == 0){
				double ppm = calculate_ppm(&co_curve_cfg, mv, cfg->rload_red, CONFIG_MICS4514_DEFAULT_CO_R0);
				val->val1 = (int32_t)ppm;
				val->val2 = (int32_t)((double)(ppm - val->val1)*1000000);
			}
			break;
		}
		case SENSOR_CHAN_MICS4514_NO2:
		{
			int32_t mv = (int32_t)data->raw_data[ADC_CHANNEL_NO2];
			rc = adc_raw_to_millivolts(vref,
						data->ch_cfg[ADC_CHANNEL_NO2].gain,
						adc_table.resolution,
						&mv);
			if (rc == 0){
				double ppm = calculate_ppm(&no2_curve_cfg, mv, cfg->rload_ox, CONFIG_MICS4514_DEFAULT_NO2_R0);
				val->val1 = (int32_t)ppm;
				val->val2 = (int32_t)((double)(ppm - val->val1)*1000000);
			}
			break;
		}
		default: break;
	}
	return rc;
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

	adc_table.buffer = drv_data->raw_data;
	adc_table.buffer_size = sizeof(drv_data->raw_data);
	adc_table.resolution = MICS4514_RESOLUTION;
	adc_table.channels = 0;

	for (uint8_t i = 0; i < NUM_MICS4514_ADC_CHANNELS; i++){
		drv_data->ch_cfg[i] = (struct adc_channel_cfg){
			.gain = MICS4514_GAIN,
			.reference = MICS4514_REF,
			.acquisition_time = ADC_ACQ_TIME_DEFAULT,
			.channel_id = cfg->adc_channel[i],
			#ifdef CONFIG_ADC_NRFX_SAADC
				.input_positive = SAADC_CH_PSELP_PSELP_AnalogInput0 + cfg->adc_channel[i],
			#endif
		};

		adc_table.channels |= BIT(cfg->adc_channel[i]);
		adc_channel_setup(cfg->adc, &drv_data->ch_cfg[i]);
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
	.adc_channel = {
		DT_INST_IO_CHANNELS_INPUT_BY_IDX(0, 0),
		DT_INST_IO_CHANNELS_INPUT_BY_IDX(0, 1),
	},
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
