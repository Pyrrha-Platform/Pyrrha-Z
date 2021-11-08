/**
 * @file timestamp.h
 * @author Brian Bradley (brian.bradley.p@gmail.com)
 * @date 2021-11-08
 * 
 * @copyright Copyright (C) 2021 LION
 * 
 * SPDX-License-Identifier: Apache-2.0
 * 
 */
#ifndef __PYRRHA_TIMESTAMP_H
#define __PYRRHA_TIMESTAMP_H

#include <zephyr.h>

/**
 * @brief Get current time
 * 
 * @retval : timestamp represented in seconds since epoch (unix time)
 * @retval -errno on error
 */

int get_timestamp(void);

/**
 * @brief Set the current time
 * 
 * @param time : timestamp represented in seconds since epoch (unix time)
 * @retval 0 on success
 * @retval -errno otherwise
 */
int set_timestamp(uint32_t time);

#endif