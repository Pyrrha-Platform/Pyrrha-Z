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
 * @retval -ENOLCK if alert mutex could not be locked within 250ms
 * @retval -errno otherwise
 */
int generate_alerts(struct pyrrha_data * data);

/**
 * @brief Override the current alert condition.
 * This will block local alert generation until a release is issued.
 * 
 * @param color : hex representation of led color used in alarm
 * @retval 0 on success
 * @retval -ENOLCK if alert mutex could not be locked within 250ms
 */
int override_alert(uint32_t color);

/**
 * @brief Release the current override for the LED. For instance, 
 * if the host device disconnects and can no longer control the alarm
 * 
 * @retval 0 on success
 * @retval -ENOLCK if alert mutex could not be locked within 250ms
 */
int release_alert_override();

#endif