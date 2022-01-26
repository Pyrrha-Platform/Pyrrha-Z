/**
 * @file alerts.h
 * @author Brian Bradley (brian.bradley.p@gmail.com)
 * @date 2022-01-26
 * 
 * @copyright Copyright (C) 2022 LION
 * 
 * SPDX-License-Identifier: Apache-2.0
 * 
 */


#ifndef __PYRRHA_BLUETOOTH_H
#define __PYRRHA_BLUETOOTH_H

#include <collector.h>

/**
 * @brief Check for and generate any alerts necessary based on
 * sensor data
 * 
 * @param data : sensor data used to match alert conditions
 * @retval 0 on success
 * @retval -errno otherwise
 */
int generate_alerts(struct pyrrha_data * data);

#endif