/**
 * @file encoder.c
 * @author Brian Bradley (brian.bradley.p@gmail.com)
 * @brief Encode received sensor data and save it for messaging
 * @date 2021-11-03
 * 
 * @copyright Copyright (C) 2021 LION
 * 
 * SPDX-License-Identifier: Apache-2.0
 * 
 */

#include <zephyr.h>
#include <logging/log.h>
#include <collector.h>
#include <encoder.h>

LOG_MODULE_REGISTER(encoder, CONFIG_PYRRHA_LOG_LEVEL);

int sensor_data_encode(struct pyrrha_data * data){
    printk("temperature (C)=%d, humidity (%%)=%d, CO(ppm)=%d.%d, NO2(ppm)=%d.%d err= 0x%02x\n", \
		data->rht.temperature.val1, data->rht.humidity.val1, data->gas.co.val1, \
        data->gas.co.val2, data->gas.no2.val1, data->gas.no2.val2, data->err);
    return 0;
}
