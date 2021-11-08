/**
 * @file timestamp.c
 * @author Brian Bradley (brian.bradley.p@gmail.com)
 * @date 2021-11-08
 * 
 * @copyright Copyright (C) 2021 LION
 * 
 * SPDX-License-Identifier: Apache-2.0
 * 
 */

#include <timestamp.h>

static uint32_t t0 = 0; 

int get_timestamp(void){
    return t0 + (k_uptime_get_32() / MSEC_PER_SEC);
}

int set_timestamp(uint32_t time){
    t0 = time;
    return 0;
}
