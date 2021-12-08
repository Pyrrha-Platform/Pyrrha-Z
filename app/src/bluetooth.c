/**
 * @file bluetooth.c
 * @author Brian Bradley (brian.bradley.p@gmail.com)
 * @brief 
 * @date 2021-11-05
 * 
 * @copyright Copyright (C) 2021 LION
 * 
 * SPDX-License-Identifier: Apache-2.0
 * 
 */


#include <zephyr.h>
#include <init.h>
#include <bluetooth.h>
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
	printk("val %u %d\n", val, host_connected);
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

int notify_client(char * buffer, size_t len){
	if (host_connected){
    	return bt_gatt_notify(NULL, &pyrrha_svc.attrs[1], buffer, len);
	}
	return -ENODEV;
}

static void connected(struct bt_conn *conn, uint8_t err)
{
	char addr[BT_ADDR_LE_STR_LEN];

	bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));

	if (err) {
		printk("Failed to connect to %s (%u)\n", addr, err);
		return;
	}

	printk("Connected %s\n", addr);

	if (bt_conn_set_security(conn, BT_SECURITY_L2)) {
		printk("Failed to set security\n");
	}
}

static void disconnected(struct bt_conn *conn, uint8_t reason)
{
	char addr[BT_ADDR_LE_STR_LEN];
	host_connected = false;
	bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));

	printk("Disconnected from %s (reason 0x%02x)\n", addr, reason);
}

static void security_changed(struct bt_conn *conn, bt_security_t level,
			     enum bt_security_err err)
{
	char addr[BT_ADDR_LE_STR_LEN];

	bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));

	if (!err) {
		printk("Security changed: %s level %u\n", addr, level);
	} else {
		printk("Security failed: %s level %u err %d\n", addr, level,
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
		printk("Bluetooth init failed (err %d)\n", err);
		return;
	}

	printk("Bluetooth initialized\n");

	if (IS_ENABLED(CONFIG_SETTINGS)) {
		settings_load();
	}

	err = bt_le_adv_start(BT_LE_ADV_CONN_NAME, ad, ARRAY_SIZE(ad), NULL, 0);
	if (err) {
		printk("Advertising failed to start (err %d)\n", err);
		return;
	}

	printk("Advertising successfully started\n");
}

static int bluetooth_init(){
    int ret;
	ret = bt_enable(bt_ready);
	if (ret) {
		printk("Bluetooth init failed (err %d)\n", ret);
	}
    return ret;
}

SYS_INIT(bluetooth_init, APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY);
