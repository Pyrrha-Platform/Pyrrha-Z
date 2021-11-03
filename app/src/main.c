/*
 * Copyright (c) 2021 LION
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr.h>
#include "app_version.h"

#include <logging/log.h>
#include <drivers/gpio.h>
#include <drivers/sensor.h>
#include <drivers/sensor/mics4514.h>

LOG_MODULE_REGISTER(main, CONFIG_PYRRHA_LOG_LEVEL);

void main(void)
{
	printk("Starting Pyrrha firmware version %s\n", APP_VERSION_STR);
}
