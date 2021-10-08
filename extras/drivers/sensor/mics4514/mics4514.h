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

#define NUM_MICS4514_ADC_CHANNELS 2

enum MICS4514_ADC_CHANNELS{
	ADC_CHANNEL_CO = 0,
	ADC_CHANNEL_NO2
};

#if DT_PROP_LEN(DT_DRV_INST(0), io_channels) != NUM_MICS4514_ADC_CHANNELS
#error "MiCS4511 requires (2) ADC channels to be configured"
#elif !DT_SAME_NODE( \
	DT_PHANDLE_BY_IDX(DT_DRV_INST(0), io_channels, 0), \
	DT_PHANDLE_BY_IDX(DT_DRV_INST(0), io_channels, 1))
#error "Channels have to use the same ADC."
#endif

/**
 * @brief Device driver data for mics4514
 * 
 */
struct mics4514_data {
	struct adc_channel_cfg ch_cfg[NUM_MICS4514_ADC_CHANNELS];
	uint16_t raw_data[NUM_MICS4514_ADC_CHANNELS];
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
	uint8_t adc_channel[NUM_MICS4514_ADC_CHANNELS];
	uint32_t rload_red;
	uint32_t rload_ox;
};

#endif /* PYRRHA_DRIVERS_SENSOR_MICS4514_MICS4514_H_ */
