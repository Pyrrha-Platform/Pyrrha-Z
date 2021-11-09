/**
 * @file encoder.h
 * @author Brian Bradley (brian.bradley.p@gmail.com)
 * @date 2021-11-03
 * 
 * @copyright Copyright (C) 2021 LION
 * 
 * SPDX-License-Identifier: Apache-2.0
 * 
 */

#ifndef __PYRRHA_ENCODER_H
#define __PYRRHA_ENCODER_H

#include <zephyr.h>
#include <collector.h>
/**
 * @brief encode sensor data into a serialized format for sending / recording
 * 
 * @param data : sensor data to encode
 * @param buffer : string buffer to hold the serialized data after encoding
 * @param buffer_len : maximum length of the encoded data (size of string buffer)
 * @retval 0 on success
 * @retval -ENOMEM on `buffer_len` less than space needed for encoding
 * @retval -errno otherwise
 */
int sensor_data_encode(struct pyrrha_data * data, char * buffer, size_t buffer_len);

#endif
