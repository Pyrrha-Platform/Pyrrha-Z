/**
 * @file collector.h
 * @author Brian Bradley (brian.bradley.p@gmail.com)
 * @date 2021-11-03
 * 
 * @copyright Copyright (C) 2021 LION
 * 
 * SPDX-License-Identifier: Apache-2.0
 * 
 */

#ifndef __PYRRHA_COLLECTOR_H
#define __PYRRHA_COLLECTOR_H

#include <gas.h>
#include <rht.h>

#define ERR_GAS_SENSOR  BIT(0)
#define ERR_RHT_SENSOR  BIT(1)

struct pyrrha_data{
    struct gas_sensor_data gas;
    struct rht_data rht;
    uint8_t err;
};

/**
 * @brief This thread will aggregate all sensor data and prepare it
 * for encoding/saving/messaging
 * 
 */
void data_collection_process(void);

#endif
