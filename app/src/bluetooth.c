/**
 * @file bluetooth.c
 * @author Brian Bradley (brian.bradley.p@gmail.com)
 * @date 2021-11-05
 * 
 * @copyright Copyright (C) 2021 LION
 * 
 * SPDX-License-Identifier: Apache-2.0
 * 
 */

#include <zephyr.h>
#include <init.h>
#include <zephyr/types.h>
#include <stddef.h>
#include <string.h>
#include <errno.h>
#include <sys/printk.h>
#include <sys/byteorder.h>
#include <bluetooth.h>
#include <settings/settings.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/conn.h>
#include <bluetooth/uuid.h>
#include <bluetooth/gatt.h>
#include <logging/log.h>

LOG_MODULE_REGISTER(pyrrha_bluetooth, CONFIG_PYRRHA_BLUETOOTH_LOG_LEVEL);

/* Custom Service Variables */
#define BT_UUID_CUSTOM_SERVICE_VAL \
	BT_UUID_128_ENCODE(0x2c32fd5f, 0x5082, 0x437e, 0x8501, 0x959d23d3d2fb)

static struct bt_uuid_128 pyrrha_svc_uuid = BT_UUID_INIT_128(
	BT_UUID_CUSTOM_SERVICE_VAL);

static struct bt_uuid_128 sensors_chrc_uuid = BT_UUID_INIT_128(
	BT_UUID_128_ENCODE(0xdcaaccb4, 0xc1d1, 0x4bc4, 0xb406, 0x8f6f45df0208));


static bool host_connected = false;

static void pyrrha_conn_ready(void){
	host_connected = true;
}

static void pyrrha_ccc_cfg_changed(const struct bt_gatt_attr *attr, uint16_t val)
{
	host_connected = (val == BT_GATT_CCC_NOTIFY);

	if (host_connected) {
		pyrrha_conn_ready();
	}
	LOG_DBG("val %u %d", val, host_connected);
}

BT_GATT_SERVICE_DEFINE(pyrrha_svc,
	/* Vendor Primary Service Declaration */
	BT_GATT_PRIMARY_SERVICE(&pyrrha_svc_uuid.uuid),
	BT_GATT_CHARACTERISTIC(&sensors_chrc_uuid.uuid, BT_GATT_CHRC_NOTIFY,
			       BT_GATT_PERM_NONE, NULL, NULL, NULL),
	BT_GATT_CCC(pyrrha_ccc_cfg_changed,
		    BT_GATT_PERM_READ | BT_GATT_PERM_WRITE),
);

static const struct bt_data ad[] = {
	BT_DATA_BYTES(BT_DATA_FLAGS, (BT_LE_AD_GENERAL | BT_LE_AD_NO_BREDR)),
	BT_DATA_BYTES(BT_DATA_UUID128_ALL, BT_UUID_CUSTOM_SERVICE_VAL),
};

static void exchange_func(struct bt_conn *conn, uint8_t err, struct bt_gatt_exchange_params *params)
{
	if (!err) {
		LOG_DBG("MTU exchange done");
	} else {
		LOG_ERR("MTU exchange failed (err %" PRIu8 ")", err);
	}
}

int notify_client(char * buffer, size_t len){
	
	if (host_connected){
		LOG_HEXDUMP_DBG(buffer, len, "notification data");
    	return bt_gatt_notify(NULL, &pyrrha_svc.attrs[1], buffer, len);
	}
	return -ENODEV;
}

static void connected(struct bt_conn *conn, uint8_t err)
{
	char addr[BT_ADDR_LE_STR_LEN];

	bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));

	if (err) {
		LOG_ERR("Failed to connect to %s (%u)", log_strdup(addr), err);
		return;
	}

	LOG_DBG("Connected %s", log_strdup(addr));

	static struct bt_gatt_exchange_params exchange_params;

	exchange_params.func = exchange_func;
	bt_gatt_exchange_mtu(conn, &exchange_params);

	if (bt_conn_set_security(conn, BT_SECURITY_L2)) {
		LOG_ERR("Failed to set security");
	}
}

static void disconnected(struct bt_conn *conn, uint8_t reason)
{
	char addr[BT_ADDR_LE_STR_LEN];
	host_connected = false;
	bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));

	LOG_DBG("Disconnected from %s (reason 0x%02x)", log_strdup(addr), reason);
}

static void security_changed(struct bt_conn *conn, bt_security_t level,
			     enum bt_security_err err)
{
	char addr[BT_ADDR_LE_STR_LEN];

	bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));

	if (!err) {
		LOG_DBG("Security changed: %s level %u", log_strdup(addr), level);
	} else {
		LOG_WRN("Security failed: %s level %u err %d", log_strdup(addr), level,
		       err);
	}
}

BT_CONN_CB_DEFINE(conn_callbacks) = {
	.connected = connected,
	.disconnected = disconnected,
	.security_changed = security_changed,
};

static void bt_ready(int err)
{
	if (err) {
		LOG_ERR("Bluetooth init failed (err %d)", err);
		return;
	}
	LOG_INF("Bluetooth initialized");
	if (IS_ENABLED(CONFIG_SETTINGS)) {
		settings_load();
	}
	err = bt_le_adv_start(BT_LE_ADV_CONN_NAME, ad, ARRAY_SIZE(ad), NULL, 0);
	if (err) {
		LOG_ERR("Advertising failed to start (err %d)", err);
		return;
	}
	LOG_DBG("Advertising successfully started");
}

static int bluetooth_init(){
    int ret;
	ret = bt_enable(bt_ready);
	if (ret) {
		LOG_ERR("Bluetooth init failed (err %d)", ret);
	}
    return ret;
}

SYS_INIT(bluetooth_init, APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY);
