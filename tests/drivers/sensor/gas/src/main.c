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
#define MICS4514_PREHEAT_ENABLED (DT_NODE_HAS_PROP(DT_NODELABEL(gas_sensor), preheat_gpios) \
									&& (CONFIG_MICS4514_PREHEAT_SECONDS > 0))


const struct device *get_gas_sensor_device(void){
    const struct device * dev = device_get_binding(GAS_SENSOR_LABEL);
    zassert_not_null(dev, "failed: dev '%s' is null", GAS_SENSOR_LABEL);
    return dev;
}

static void test_get_sensor_value(int16_t channel, int compare){
    struct sensor_value value;
	const struct device *dev = get_gas_sensor_device();
	zassert_true(sensor_sample_fetch_chan(dev, channel) == compare, "Sample fetch failed");
	zassert_true(sensor_channel_get(dev, channel, &value) == compare, "Get sensor value failed");
}

void test_get_sensor_value_not_supp(int16_t channel)
{
	const struct device *dev = get_gas_sensor_device();

	zassert_true(sensor_sample_fetch_chan(dev, channel) == -ENOTSUP, "Unsupported channels should return -ENOTSUP");
}

static void test_unspported_channel(void){

	/* for all channels */
	for (int c = 0; c < SENSOR_CHAN_ALL; c++){
		/* All standard channels are unsupported */
		test_get_sensor_value_not_supp(c);
	}
}

static void test_get_co(void){
    test_get_sensor_value(SENSOR_CHAN_MICS4514_CO, 0);
}

static void test_get_no2(void){
    test_get_sensor_value(SENSOR_CHAN_MICS4514_NO2, 0);
}

#if MICS4514_PREHEAT_ENABLED
static void test_preheat(void){
	/* We should receive an error when fetching and getting during preheat */
	test_get_sensor_value(SENSOR_CHAN_MICS4514_CO, -EBUSY);
	test_get_sensor_value(SENSOR_CHAN_MICS4514_NO2, -EBUSY);

	/* Wait until preheat is complete so that other tests can pass */
	k_sleep(K_SECONDS(CONFIG_MICS4514_PREHEAT_SECONDS + 1));
}
#endif

void test_main(void)
{
    ztest_test_suite(gas_sensor_tests,
		#if MICS4514_PREHEAT_ENABLED
		ztest_unit_test(test_preheat),
		#endif
		ztest_unit_test(test_get_co),
        ztest_unit_test(test_get_no2),
		ztest_unit_test(test_unspported_channel)
	);
	ztest_run_test_suite(gas_sensor_tests);
}
