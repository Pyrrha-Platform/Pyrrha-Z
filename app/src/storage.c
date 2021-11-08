/**
 * @file storage.c
 * @author Brian Bradley (brian.bradley.p@gmail.com)
 * @date 2021-11-05
 * 
 * @copyright Copyright (C) 2021 LION
 * 
 * SPDX-License-Identifier: Apache-2.0
 * 
 */
#include <zephyr.h>
#include <storage.h>
#include <stdio.h>
#include <string.h>
#include <fs/fcb.h>
#include <logging/log.h>

LOG_MODULE_REGISTER(storage, CONFIG_PYRRHA_LOG_LEVEL);

struct record_walk_ctx{
	record_handler_cb cb;
	int record_count;
};

#define FCB_DATA_VERSION    1
static struct flash_sector fcb_area[CONFIG_PYRRHA_FCB_NUM_AREAS + 1];
static struct fcb storage_cf = {
    .f_magic = CONFIG_PYRRHA_FCB_MAGIC,
    .f_sectors = fcb_area,
    .f_scratch_cnt = 1,
    .f_version = FCB_DATA_VERSION
};

int record_walk_cb(struct fcb_entry_ctx *entry_ctx, void *arg){
	uint16_t len;
	int rc;
	struct record_walk_ctx *record_ctx = (struct record_walk_ctx *)arg;
	struct pyrrha_data record = {0};
	len = entry_ctx->loc.fe_data_len;

	rc = flash_area_read(entry_ctx->fap,
			     FCB_ENTRY_FA_DATA_OFF(entry_ctx->loc),
			     &record, sizeof(struct pyrrha_data));
	if (rc == 0){
		rc = record_ctx->cb(&record);
		record_ctx->record_count++;
	}
	return rc;
}

int record_walk(record_handler_cb cb){
	struct record_walk_ctx ctx = {
		.cb = cb,
		.record_count = 0
	};
	int rc = fcb_walk(&storage_cf, 0, record_walk_cb, &ctx);
	if (rc == 0){
		return ctx.record_count;
	}
	return rc;
}

bool stored_record_is_available(){
	LOG_DBG("Free sectors available: %d", fcb_free_sector_cnt(&storage_cf));
    return fcb_is_empty(&storage_cf) == 0;
}

int store_new_record(struct pyrrha_data * record){
    struct fcb_entry loc;
    int ret = fcb_append(&storage_cf, sizeof(struct pyrrha_data), &loc);
    if (ret != 0){
		LOG_DBG("No space to append. Rotating");
        fcb_rotate(&storage_cf);
        ret = fcb_append(&storage_cf, sizeof(struct pyrrha_data), &loc);\
    }
    if (ret == 0){
		ret = flash_area_write(storage_cf.fap, FCB_ENTRY_FA_DATA_OFF(loc),
				      record, sizeof(struct pyrrha_data));
		
        ret = fcb_append_finish(&storage_cf, &loc);
    }
    return ret;
}

int rotate_record_buffer(){
    return fcb_rotate(&storage_cf);
}

static int storage_fcb_cfg(struct fcb * cf){
    int rc;
    while (1) {
		rc = fcb_init(FLASH_AREA_ID(storage), cf);
		if (rc) {
			return -EINVAL;
		}
		/*
		 * Check if system was reset in middle of emptying a sector.
		 * This situation is recognized by checking if the scratch block
		 * is missing.
		 */
		if (fcb_free_sector_cnt(cf) < 1) {
			LOG_DBG("Recovering from scratch block corruption");
			rc = flash_area_erase(cf->fap,
					cf->f_active.fe_sector->fs_off,
					cf->f_active.fe_sector->fs_size);

			if (rc) {
				return -EIO;
			}
		} else {
			LOG_DBG("Successful FCB init");
			break;
		}
	}
    return 0;
}

static int storage_backend_init()
{
	uint32_t cnt = sizeof(fcb_area) /
		    sizeof(fcb_area[0]);
	int rc;
	const struct flash_area *fap;

	rc = flash_area_get_sectors(FLASH_AREA_ID(storage), &cnt,
				    fcb_area);
	if (rc == -ENODEV) {
		return rc;
	} else if (rc != 0 && rc != -ENOMEM) {
		k_panic();
	}

	storage_cf.f_sector_cnt = cnt;
	storage_cf.fap = fap;

    rc = storage_fcb_cfg(&storage_cf);
    
	if (rc != 0) {
		LOG_DBG("error configuring fcb: %d", rc);
        /* Error. Erase flash area and try again */
		rc = flash_area_open(FLASH_AREA_ID(storage), &fap);

		if (rc == 0) {
			rc = flash_area_erase(fap, 0, fap->fa_size);
			flash_area_close(fap);
		}

        if (rc != 0) {
			k_panic();
		} else {
			rc = storage_fcb_cfg(&storage_cf);
		}
	}
    
	if (rc != 0) {
		k_panic();
	}
	LOG_DBG("Storage initialized: %d", rc);
	return rc;
}

SYS_INIT(storage_backend_init, APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY);