# Pyrrha-Z

This repository contains the next generation application
firmware for the Pyrrha (created by Prometeo) platform

The firmware is built on the Zephyr RTOS.

## Getting Started

Before getting started, make sure you have a proper Zephyr development
environment. You can follow the official
[Zephyr Getting Started Guide](https://docs.zephyrproject.org/latest/getting_started/index.html).

### Initialization

The first step is to initialize the workspace folder (``pyrrha-workspace``) where
 ``Pyrrha-Z`` and all Zephyr modules will be cloned. You can do
that by running:

```shell
# initialize pyrrha-workspace with Pyrrha-Z (main branch)
west init -m https://github.com/zephyrproject-rtos/Pyrrha-Z --mr main pyrrha-workspace
# update Zephyr modules
cd pyrrha-workspace
west update
```

### Build & Run

The application can be built by running:

```shell
west build -b $BOARD -s app
```

where `$BOARD` is the target board, `nrf52840dk_nrf52840` for example

A sample debug configuration is also provided. You can apply it by running:

```shell
west build -b $BOARD -s app -- -DOVERLAY_CONFIG=`configs/debug.conf`
```

Note that you may also use it together with `configs/rtt.conf` if using Segger RTT. Once
you have built the application you can flash it by running:

```shell
west flash
```
