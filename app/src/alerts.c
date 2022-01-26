/**
 * @file alerts.c
 * @author Brian Bradley (brian.bradley.p@gmail.com)
 * @brief 
 * @date 2022-01-26
 * 
 * @copyright Copyright (C) 2022 LION
 * 
 * SPDX-License-Identifier: Apache-2.0
 * 
 */

#include <alerts.h>
#include <logging/log.h>
LOG_MODULE_REGISTER(alerts, CONFIG_PYRRHA_LOG_LEVEL);
#define OVER_TEMPERATURE(temp) (temp > CONFIG_TEMPERATURE_ALERT_THRESHOLD_CELCIUS)
#define OVER_CO(co) (co > CONFIG_CO_ALERT_THRESHOLD_PPM)
#define OVER_NO2(no2) (no2 > CONFIG_NO2_ALERT_THRESHOLD_PPM)

/**
 * @brief generate an alert from sensor data exceeding set thresholds
 */
static void threshold_alert(void){
    return;
}
int generate_alerts(struct pyrrha_data * data){
    if (OVER_TEMPERATURE(data->rht.temperature.val1) ||
        OVER_CO(data->gas.co.val1) ||
        OVER_NO2(data->gas.no2.val1))
    {
        threshold_alert();
    }
    return 0;
}