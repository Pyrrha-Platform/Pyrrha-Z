/**
 * @file messenger.c
 * @author Brian Bradley (brian.bradley.p@gmail.com)
 * @date 2021-11-05
 * 
 * @copyright Copyright (C) 2021 LION
 * 
 * SPDX-License-Identifier: Apache-2.0
 * 
 */
#include <zephyr.h>
#include <encoder.h>
#include <collector.h>
#include <messenger.h>
#include <bluetooth.h>
#include <logging/log.h>
#include <storage.h>

LOG_MODULE_REGISTER(messenger, CONFIG_PYRRHA_LOG_LEVEL);

K_MSGQ_DEFINE(sensor_msgq, sizeof(struct pyrrha_data), \
    CONFIG_PYRRHA_MESSAGE_QUEUE_SIZE, CONFIG_PYRRHA_MESSAGE_QUEUE_ALIGNMENT);

int queue_sensor_data(struct pyrrha_data * data){
    int ret = 0;
    while ((ret = k_msgq_put(&sensor_msgq, data, K_NO_WAIT)) != 0) {
        /* message queue is full: purge old data & try again */
        k_msgq_purge(&sensor_msgq);
    }
    return ret;
}

int send_record_handler(struct pyrrha_data * record){
    char buffer[CONFIG_PYRRHA_MESSAGE_BUFF_SIZE] = {0};
    int ret = sensor_data_encode(record, buffer, sizeof(buffer));
    LOG_DBG("Sending record: %s", log_strdup(buffer));
    if (ret == 0){
        ret = notify_client(buffer, strlen(buffer));
        if (ret != 0){
            LOG_DBG("Error notifying client");
        }
    }
    else{
        LOG_DBG("Error encoding record");
    }
    return ret;
}

void data_messaging_process(void)
{
    struct pyrrha_data msg;
	while (1)
	{
		if (k_msgq_get(&sensor_msgq, &msg, K_FOREVER) == 0)
		{
            int ret = 0;
            /* Try to send the old data first */
            if (stored_record_is_available()){
                LOG_DBG("Stored records found. Attempting to send");
                ret = record_walk(send_record_handler);
                if (ret >= 0){
                    LOG_DBG("Sent %d stored records", ret);
                    ret = rotate_record_buffer();
                }
                else{
                    LOG_DBG("Error sending stored records: %d", ret);
                }
            }
            /* Attempt to send the latest data */
            ret = send_record_handler(&msg);

            if (ret != 0){
                /* We have encountered an error trying to send data.
                This message should be stored for later */
                ret = store_new_record(&msg);
                if (ret == -ENOMEM){
                    /* Not enough memory to store this record. Rotate the buffer and try again */
                    LOG_DBG("Low memory, erasing old data");
                    rotate_record_buffer();
                    store_new_record(&msg);
                }
            }
		}
	}
}

K_THREAD_DEFINE(messenger_thread, CONFIG_PYRRHA_MESSAGING_STACKSIZE, data_messaging_process, 
	NULL, NULL, NULL, CONFIG_PYRRHA_MESSAGING_PRIORITY, 0, 0);
