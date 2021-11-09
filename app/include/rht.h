/**
 * @file gas.h
 * @author Brian Bradley (brian.bradley.p@gmail.com)
 * @brief Headers for rht sensor integration
 * @date 2021-11-03
 * 
 * @copyright Copyright (C) 2021 LION
 * 
 * SPDX-License-Identifier: Apache-2.0
 * 
 */

#ifndef __PYRRAH_RHT_SENSOR_H
#define __PYRRAH_RHT_SENSOR_H

#include <drivers/sensor.h>

struct rht_data{
    struct sensor_value temperature;
    struct sensor_value humidity;
};

/**
 * @brief Capture the latest rht sensor data is possible
 * 
 * @param data : Pointer to where rht data should be stored on successful acquisition
 * @retval 0 on success
 * @retval -errno otherwise 
 */
int capture_rht_data(struct rht_data * data);

#endif
