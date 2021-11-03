/**
 * @file rht.c
 * @author Brian Bradley (brian.bradley.p@gmail.com)
 * @date 2021-11-03
 * 
 * @copyright Copyright (C) 2021 LION
 * 
 * SPDX-License-Identifier: Apache-2.0
 * 
 */
#include <zephyr.h>
#include <rht.h>
#include <logging/log.h>

LOG_MODULE_REGISTER(rht_sensor, CONFIG_PYRRHA_LOG_LEVEL);

int capture_rht_data(struct rht_data * data){
    const struct device * rht = DEVICE_DT_GET(DT_NODELABEL(rht_sensor));
    int ret;
    if (data == NULL){
        return -EFAULT;
    }
    if ((ret = sensor_sample_fetch(rht)) != 0){
        LOG_DBG("rht sensor fetch error: %d", ret);
    }
    else{
        sensor_channel_get(rht, SENSOR_CHAN_AMBIENT_TEMP,
            &data->temperature);
        sensor_channel_get(rht, SENSOR_CHAN_HUMIDITY,
            &data->humidity);
    }
    return ret;
}
