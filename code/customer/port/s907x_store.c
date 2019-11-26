#include "s907x.h"
#include "system_debug.h"
#include "s907x_store.h"
#include "s907x_zg_config.h"
#if S907X_STORE_USING_KV
#include "kv_api.h"
#endif

fdcm_handle_t *fdcm_open(int idx, int addr, int len)
{
    uint8_t *ptr_addr = NULL;

    ptr_addr = wl_malloc(ZG_FLASH_DATA_SIZE);
    if (NULL == ptr_addr) {
        USER_DBG("Alloc memory for fdcm fail.");
    }

    return ptr_addr;
}


int fdcm_read(fdcm_handle_t *fdcm_hdl, uint8_t *out_data, int in_len)
{
#if S907X_STORE_USING_KV
	int err;
	int read_len = in_len;
    err = kv_item_get( fdcm_hdl, out_data, &read_len);
	if(err) {
		return 0;
	} else {
		in_len = read_len;
	}
#else
    s907x_hal_flash_read_resume_tick(ZG_FLASH_DATA_ADDR, out_data, in_len);
#endif

    return in_len;
}

int fdcm_write(fdcm_handle_t *fdcm_hdl, uint8_t *in_data, int in_len)
{
#if S907X_STORE_USING_KV
	int err;

    err = kv_item_set(fdcm_hdl, in_data, in_len);
	if(err) {
		return 0;
	} 
#else
    s907x_hal_flash_erase_resume_tick(EraseSector, ZG_FLASH_DATA_ADDR);
    s907x_hal_flash_write_resume_tick(ZG_FLASH_DATA_ADDR, in_data, in_len);
#endif

    return in_len;
}


