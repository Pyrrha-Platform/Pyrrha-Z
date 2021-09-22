/**
 * @file cc2d23s.c
 * @author Brian Bradley (brian.bradley.p@gmail.com)
 * @brief 
 * @date 2021-09-22
 * 
 * @copyright Copyright (C) 2021 LION
 * 
 * SPDX-License-Identifier: Apache-2.0
 * 
 */

#include <errno.h>
#include <zephyr.h>
#include <device.h>
#include <kernel.h>
#include <drivers/i2c.h>
#include <init.h>
#include <sys/byteorder.h>
#include <sys/__assert.h>
#include <logging/log.h>

#include "cc2d23s.h"

LOG_MODULE_REGISTER(CC2D23S, CONFIG_SENSOR_LOG_LEVEL);

int cc2d23s_read(const struct device *dev, uint32_t *val)
{
    return -ENOTSUP;
}

int cc2d23s_write(const struct device *dev, uint8_t *val)
{
    return -ENOTSUP;
}

int cc2d23s_init(const struct device *dev)
{
    return -ENOTSUP;
}

static struct cc2d23s_data cc2d23s_data;
static const struct cc2d23s_config cc2d23s_cfg = {
	.i2c_bus = DT_INST_BUS_LABEL(0),
	.i2c_addr = DT_INST_REG_ADDR(0),
};

DEVICE_DT_INST_DEFINE(0, cc2d23s_init, NULL,
		    &cc2d23s_data, &cc2d23s_cfg, POST_KERNEL,
		    CONFIG_SENSOR_INIT_PRIORITY, &cc2d23s_api_funcs);
