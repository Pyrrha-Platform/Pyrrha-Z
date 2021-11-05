/**
 * @file messenger.h
 * @author Brian Bradley (brian.bradley.p@gmail.com)
 * @date 2021-11-05
 * 
 * @copyright Copyright (C) 2021 LION
 * 
 * SPDX-License-Identifier: Apache-2.0
 * 
 */
#ifndef __PYRRHA_MESSENGER_H
#define __PYRRHA_MESSENGER_H

#include <collector.h>
/**
 * @brief Add a new message to the queue
 * for sending / recording
 * 
 * @param data : sensor data to be sent
 * @retval 0 on success
 * @retval -errno otherwise
 */
int queue_sensor_data(struct pyrrha_data * data);

/**
 * @brief This thread will wait for new messages
 * and attempt to send them, or queue them to be sent
 * 
 */
void data_messaging_process(void);

#endif
