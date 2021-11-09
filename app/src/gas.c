/**
 * @file gas.c
 * @author Brian Bradley (brian.bradley.p@gmail.com)
 * @date 2021-11-03
 * 
 * @copyright Copyright (C) 2021 LION
 * 
 * SPDX-License-Identifier: Apache-2.0
 * 
 */
#include <zephyr.h>
#include <gas.h>
#include <logging/log.h>
#include <drivers/sensor/mics4514.h>

LOG_MODULE_REGISTER(gas_sensor, CONFIG_PYRRHA_LOG_LEVEL);

int capture_gas_sensor_data(struct gas_sensor_data * data){
    const struct device * gas = DEVICE_DT_GET(DT_NODELABEL(gas_sensor));
    int ret;
    if (data == NULL){
        return -EFAULT;
    }
    if ((ret = sensor_sample_fetch(gas)) != 0){
        LOG_DBG("gas sensor fetch error: %d", ret);
    }
    else{
        sensor_channel_get(gas, SENSOR_CHAN_MICS4514_CO,
            &data->co);
        sensor_channel_get(gas, SENSOR_CHAN_MICS4514_NO2,
            &data->no2);
    }
    return ret;
}