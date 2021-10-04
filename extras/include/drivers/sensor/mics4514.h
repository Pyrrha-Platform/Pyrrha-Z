/**
 * @file mics4514.h
 * @author Brian Bradley (brian.bradley.p@gmail.com)
 * @brief Extended public API for SGX Sensortech (amphenol) MiCS-4514
 * @date 2021-10-04
 * 
 * @copyright Copyright (C) 2021 LION
 * 
 * SPDX-License-Identifier: Apache-2.0
 * 
 */

#ifndef PYRRHA_INCLUDE_DRIVERS_SENSOR_MICS4514_H_
#define PYRRHA_INCLUDE_DRIVERS_SENSOR_MICS4514_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <drivers/sensor.h>

enum sensor_channel_mics4514 {
	SENSOR_CHAN_MICS4514_CO = SENSOR_CHAN_PRIV_START,
    SENSOR_CHAN_MICS4514_NO2,
	SENSOR_CHAN_MICS4514_C2H5OH,
    SENSOR_CHAN_MICS4514_H2,
    SENSOR_CHAN_MICS4514_NH3,
    SENSOR_CHAN_MICS4514_CH4,
};

#ifdef __cplusplus
}
#endif

#endif /* PYRRHA_INCLUDE_DRIVERS_SENSOR_MICS4514_H_ */
