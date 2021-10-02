# Copyright (c) 2021 LION
# SPDX-License-Identifier: Apache-2.0

zephyr_library()
zephyr_library_sources_ifdef(CONFIG_CC2D23S cc2d23s.c)
