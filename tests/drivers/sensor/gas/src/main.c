/**
 * @file main.c
 * @author Brian Bradley (brian.bradley.p@gmail.com)
 * @brief Unit tests for gas sensors
 * @date 2021-10-04
 * 
 * @copyright Copyright (C) 2021 LION
 * 
 * SPDX-License-Identifier: Apache-2.0
 * 
 */

#include <zephyr.h>
#include <ztest.h>
#include <drivers/sensor.h>
#include <drivers/sensor/mics4514.h>

#define GAS_SENSOR_LABEL DT_LABEL(DT_NODELABEL(gas_sensor))

const struct device *get_gas_sensor_device(void){
    const struct device * dev = device_get_binding(GAS_SENSOR_LABEL);
    zassert_not_null(dev, "failed: dev '%s' is null", GAS_SENSOR_LABEL);
    return dev;
}

static void test_get_sensor_value(int16_t channel){
    struct sensor_value value;
	const struct device *dev = get_gas_sensor_device();

	zassert_true(sensor_sample_fetch_chan(dev, channel) == 0, "Sample fetch failed");
	zassert_true(sensor_channel_get(dev, channel, &value) == 0, "Get sensor value failed");
}

void test_get_sensor_value_not_supp(int16_t channel)
{
	const struct device *dev = get_gas_sensor_device();

	zassert_true(sensor_sample_fetch_chan(dev, channel) == -ENOTSUP, "Unsupported channels should return -ENOTSUP");
}

static bool is_channel_supported(int16_t channel){
	/* Only the "all" channel is supported in the default channels */
	return channel == SENSOR_CHAN_ALL;
}

static void test_unspported_channel(void){

	/* for all channels */
	for (int c = 0; c < SENSOR_CHAN_ALL; c++){
		/* If the channel is unsupported */
		if (!is_channel_supported(c)){
			test_get_sensor_value_not_supp(c);
		}
	}
}

static void test_get_co(void){
    test_get_sensor_value(SENSOR_CHAN_MICS4514_CO);
}

static void test_get_no2(void){
    test_get_sensor_value(SENSOR_CHAN_MICS4514_NO2);
}

void test_main(void)
{
    ztest_test_suite(gas_sensor_tests,
		ztest_unit_test(test_get_co),
        ztest_unit_test(test_get_no2),
		ztest_unit_test(test_unspported_channel)
	);
	ztest_run_test_suite(gas_sensor_tests);
}
