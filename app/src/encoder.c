/**
 * @file encoder.c
 * @author Brian Bradley (brian.bradley.p@gmail.com)
 * @brief Encode received sensor data and save it for messaging
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
#include <encoder.h>
#include <data/json.h>

LOG_MODULE_REGISTER(encoder, CONFIG_PYRRHA_LOG_LEVEL);

static const struct json_obj_descr sensor_descr[] = {
    JSON_OBJ_DESCR_PRIM_NAMED(struct sensor_value, "i", val1, JSON_TOK_NUMBER),
    JSON_OBJ_DESCR_PRIM_NAMED(struct sensor_value, "f", val2, JSON_TOK_NUMBER),
};

static const struct json_obj_descr gas_sensor_descr[] = {
    JSON_OBJ_DESCR_OBJECT_NAMED(struct gas_sensor_data, "co", co, sensor_descr),
    JSON_OBJ_DESCR_OBJECT_NAMED(struct gas_sensor_data, "no2", no2, sensor_descr),
};

static const struct json_obj_descr rht_sensor_descr[] = {
    JSON_OBJ_DESCR_OBJECT_NAMED(struct rht_data, "temp", temperature, sensor_descr),
    JSON_OBJ_DESCR_OBJECT_NAMED(struct rht_data, "humidity", humidity, sensor_descr),
};

static const struct json_obj_descr sensor_data_desc[] = {
    JSON_OBJ_DESCR_PRIM(struct pyrrha_data, timestamp, JSON_TOK_NUMBER),
    JSON_OBJ_DESCR_PRIM(struct pyrrha_data, err, JSON_TOK_NUMBER),
    JSON_OBJ_DESCR_OBJECT(struct pyrrha_data, gas, gas_sensor_descr),
    JSON_OBJ_DESCR_OBJECT(struct pyrrha_data, rht, rht_sensor_descr)
};

int sensor_data_encode(struct pyrrha_data * data, char * buffer, size_t buffer_len){

	ssize_t len = json_calc_encoded_len(sensor_data_desc, ARRAY_SIZE(sensor_data_desc), data);

    if (len > buffer_len){
        LOG_ERR("Encoding json object would result in overflow. Need %d bytes (%d allowed)", len, buffer_len);
        return -ENOMEM;
    }

    json_obj_encode_buf(sensor_data_desc, ARRAY_SIZE(sensor_data_desc), data,
        buffer, buffer_len);

    return 0;
}
