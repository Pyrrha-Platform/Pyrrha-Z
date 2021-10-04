# Copyright (c) 2021 LION
# SPDX-License-Identifier: Apache-2.0

zephyr_library()
zephyr_library_sources_ifdef(CONFIG_MICS4514 mics4514.c)
