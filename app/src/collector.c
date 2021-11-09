/**
 * @file collector.c
 * @author Brian Bradley (brian.bradley.p@gmail.com)
 * @brief Aggregate sensor data and send it to the encoder
 * @date 2021-11-03
 * 
 * @copyright Copyright (C) 2021 LION
 * 
 * SPDX-License-Identifier: Apache-2.0
 * 
 */
#include <zephyr.h>
#include <logging/log.h>
#include <collector.h>
#include <messenger.h>
#include <timestamp.h>
LOG_MODULE_REGISTER(collector, CONFIG_PYRRHA_LOG_LEVEL);

void data_collection_process(void){
    struct pyrrha_data data = {0};
    while(1){
        data.err = 0;
        /* Capture data and record any errors for the host to filter */
        data.err |= (capture_gas_sensor_data(&data.gas) != 0 ? ERR_GAS_SENSOR : 0);
        data.err |= (capture_rht_data(&data.rht) != 0 ? ERR_RHT_SENSOR : 0);
        data.timestamp = get_timestamp();
        capture_rht_data(&data.rht);
        queue_sensor_data(&data);
        k_sleep(K_SECONDS(CONFIG_PYRRHA_SAMPLE_PERIOD));
    }
}

K_THREAD_DEFINE(data_collection_thread, CONFIG_PYRRHA_COLLECTION_STACKSIZE, data_collection_process, 
	NULL, NULL, NULL, CONFIG_PYRRHA_COLLECTION_PRIORITY, 0, CONFIG_PYRRHA_COLLECTION_START_DELAY);
