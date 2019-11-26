#ifndef S907X_STORE_H
#define S907X_STORE_H

#include "hal_flash.h"

#if S907X_STORE_USING_KV

#define fdcm_handle_t           char 

#else
#define fdcm_handle_t           uint8_t

#endif
#define ZG_FLASH_DATA_ADDR      FLASH_DATA_USER_START
/* fdcm_hdl not used now ,just alloc small memory, or should using SECTOR_SIZE instead.*/
#define ZG_FLASH_DATA_SIZE      FLASH_PAGE_SIZE

fdcm_handle_t * fdcm_open(int idx, int addr, int len);
int fdcm_read(fdcm_handle_t *fdcm_hdl, uint8_t *out_data, int in_len);
int fdcm_write(fdcm_handle_t *fdcm_hdl, uint8_t *in_data, int in_len);

#endif

