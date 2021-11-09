/**
 * @file bluetooth.c
 * @author Brian Bradley (brian.bradley.p@gmail.com)
 * @brief 
 * @date 2021-11-05
 * 
 * @copyright Copyright (C) 2021 LION
 * 
 * SPDX-License-Identifier: Apache-2.0
 * 
 */

#include <bluetooth.h>

int notify_client(char * buffer, size_t len){
    ARG_UNUSED(buffer);
    ARG_UNUSED(len);
    return -ENOTSUP;
}