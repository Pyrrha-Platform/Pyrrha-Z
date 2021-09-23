/**
 * @file main.c
 * @author Brian Bradley (brian.bradley.p@gmail.com)
 * @brief Unit tests for relative humidity and temperature sensors
 * @date 2021-09-22
 * 
 * @copyright Copyright (C) 2021 LION
 * 
 * SPDX-License-Identifier: Apache-2.0
 * 
 */

#include <zephyr.h>
#include <ztest.h>
#include <drivers/sensor.h>

#define RHT_LABEL DT_LABEL(DT_NODELABEL(rht_sensor))

const struct device *get_rht_device(void){
    const struct device * dev = device_get_binding(RHT_LABEL);
    zassert_not_null(dev, "failed: dev '%s' is null", RHT_LABEL);
    return dev;
}

static void test_get_sensor_value(int16_t channel){
    struct sensor_value value;
	const struct device *dev = get_rht_device();

	zassert_true(sensor_sample_fetch_chan(dev, channel) == 0, "Sample fetch failed");
	zassert_true(sensor_channel_get(dev, channel, &value) == 0, "Get sensor value failed");
}

void test_get_sensor_value_not_supp(int16_t channel)
{
	const struct device *dev = get_rht_device();

	zassert_true(sensor_sample_fetch_chan(dev, channel) == -ENOTSUP, "Unsupported channels should return -ENOTSUP");
}

static bool is_channel_supported(int16_t channel){
	switch(channel){
		case SENSOR_CHAN_AMBIENT_TEMP:
		case SENSOR_CHAN_HUMIDITY:
		case SENSOR_CHAN_ALL:
		return true;
		default: return false;
	}
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

static void test_get_temperature(void){
    test_get_sensor_value(SENSOR_CHAN_AMBIENT_TEMP);
}

static void test_get_humidity(void){
    test_get_sensor_value(SENSOR_CHAN_HUMIDITY);
}

void test_main(void)
{
    ztest_test_suite(rht_tests,
		ztest_unit_test(test_get_temperature),
        ztest_unit_test(test_get_humidity),
		ztest_unit_test(test_unspported_channel)
	);
	ztest_run_test_suite(rht_tests);
}
