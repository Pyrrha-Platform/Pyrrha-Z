/**
 * @file bluetooth.h
 * @author Brian Bradley (brian.bradley.p@gmail.com)
 * @date 2021-11-05
 * 
 * @copyright Copyright (C) 2021 LION
 * 
 * SPDX-License-Identifier: Apache-2.0
 * 
 */

#ifndef __PYRRHA_BLUETOOTH_H
#define __PYRRHA_BLUETOOTH_H

#include <zephyr.h>
/**
 * @brief Unimplemented
 * 
 * @param buffer : data to be sent to client
 * @param len : length of data sent to client
 * @retval -ENOTSUP 
 */
int notify_client(char * buffer, size_t len);

#endif