/**
 * @file gas.h
 * @author Brian Bradley (brian.bradley.p@gmail.com)
 * @brief Headers for gas sensor integration
 * @date 2021-11-03
 * 
 * @copyright Copyright (C) 2021 LION
 * 
 * SPDX-License-Identifier: Apache-2.0
 * 
 */

#ifndef __PYRRAH_GAS_SENSOR_H
#define __PYRRAH_GAS_SENSOR_H

#include <zephyr.h>
#include <drivers/sensor.h>

struct gas_sensor_data{
    struct sensor_value co;
    struct sensor_value no2;
};

/**
 * @brief Capture the latest gas sensor data is possible
 * 
 * @param data : Pointer to where gas sensor data should be stored on successful acquisition
 * @retval 0 on success
 * @retval -errno otherwise 
 */
int capture_gas_sensor_data(struct gas_sensor_data * data);

#endif
