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
#include <logging/log.h>

LOG_MODULE_REGISTER(messenger, CONFIG_PYRRHA_LOG_LEVEL);

K_MSGQ_DEFINE(sensor_msgq, sizeof(struct pyrrha_data), \
    CONFIG_PYRRHA_MESSAGE_QUEUE_SIZE, CONFIG_PYRRHA_MESSAGE_QUEUE_ALIGNMENT);

int queue_sensor_data(struct pyrrha_data * data){
    int ret = 0;
    while ((ret = k_msgq_put(&sensor_msgq, data, K_NO_WAIT)) != 0) {
        /* message queue is full: purge old data & try again */
        /**
         * TODO: Is this the desired behavior? 
         * Perhaps new data should be ignored instead?
         * 
         */
        k_msgq_purge(&sensor_msgq);
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
            /* Encode sensor data into a serialized object */
			char buffer[CONFIG_PYRRHA_MESSAGE_BUFF_SIZE] = {0};
            sensor_data_encode(&msg, buffer, sizeof(buffer));
            LOG_DBG("message (%d bytes): %s", strlen(buffer), log_strdup(buffer));

            /* Check to see if there is unsent data */

            /* Attempt to send to client */

            /* If client cannot be reached, log the unsent data */
		}
	}
}

K_THREAD_DEFINE(messenger_thread, CONFIG_PYRRHA_MESSAGING_STACKSIZE, data_messaging_process, 
	NULL, NULL, NULL, CONFIG_PYRRHA_MESSAGING_PRIORITY, 0, 0);
