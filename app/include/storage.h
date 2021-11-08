/**
 * @file storage.h
 * @author Brian Bradley (brian.bradley.p@gmail.com)
 * @date 2021-11-05
 * 
 * @copyright Copyright (C) 2021 LION
 * 
 * SPDX-License-Identifier: Apache-2.0
 * 
 */
#ifndef __PYRRHA_STORAGE_H
#define __PYRRHA_STORAGE_H
#include <zephyr.h>
#include <collector.h>

/**
 * Callback function for handling records found while 
 * walking through flash circular buffer
 *
 * Type of function which is expected to be called after
 * walking through each record
 *
 * If cb wants to stop the walk, it should return non-zero value.
 * 
 * @param record Pointer to record which must be handled
 * @return 0 continue walking, non-zero stop walking.
 */
typedef int (*record_handler_cb)(struct pyrrha_data *record);

/**
 * @brief Walk through all available stored records, and
 * handle them with a record handler `cb`
 * 
 * @param cb : Record handler, each stored record will have
 * this function applied to it
 * @retval positive number of records succesfully handled
 * @retval -errno on failure. This can come from the storage backend,
 * or from `cb`
 */
int record_walk(record_handler_cb cb);

/**
 * @brief Checks to see if there are any stored, unsent records
 * 
 * @retval true : records available
 * @retval false : no records available
 */
bool stored_record_is_available();

/**
 * @brief Get the next (oldest available) record, if there is one
 * 
 * @param record : structure to store record, if one is found
 * @retval 0 : successfully got a record
 * @retval -ENODATA : no records available
 * @retval -errno otherwise 
 * 
 * @note It is important to rotate the data with `rotate_record_buffer`
 * once the data is no longer needed (e.g. it was successfully sent)
 */
int get_next_record(struct pyrrha_data * record);

/**
 * @brief Store a new record for later retrieval
 * 
 * @param record : record to store
 * @retval 0 on success
 * @retval -errno otherwise
 */
int store_new_record(struct pyrrha_data * record);

/**
 * @brief Deletes the oldest record from the circular buffer
 * 
 * @retval 0 on success
 * @retval -errno otherwise
 */
int rotate_record_buffer();

#endif