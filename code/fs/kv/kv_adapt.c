/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include "os_api.h"

#include "kv_conf.h"
#include "kv_adapt.h"

#include "hal_flash.h"

#define ROUND_DOWN(a,b) (((a) / (b)) * (b))

static int32_t hal_flash_erase( uint32_t off_set, uint32_t size)
{
    uint32_t addr;
    uint32_t start_addr, end_addr;

    start_addr = ROUND_DOWN((KV_PARTITION + off_set), SECTOR_SIZE);
    end_addr = ROUND_DOWN((KV_PARTITION + off_set + size - 1), SECTOR_SIZE);

    for (addr = start_addr; addr <= end_addr; addr += SECTOR_SIZE) {
        s907x_hal_flash_erase(EraseSector,  addr); //EraseSector
    }

    return 0;
}

static int32_t hal_flash_write( uint32_t *off_set, const void *in_buf , uint32_t in_buf_len)
{
    uint32_t start_addr;

    start_addr = KV_PARTITION + *off_set;

    s907x_hal_flash_write(start_addr, (uint8_t *)in_buf, in_buf_len);

    *off_set += in_buf_len;
    return 0;
}

static int32_t hal_flash_read( uint32_t *off_set, void *out_buf, uint32_t out_buf_len )
{
    uint32_t start_addr;

    start_addr = KV_PARTITION + *off_set;

    s907x_hal_flash_read(start_addr,  out_buf, out_buf_len);
    
    *off_set += out_buf_len;

    return 0;

}

int32_t kv_flash_read(uint32_t offset, void *buf, uint32_t nbytes)
{
    return hal_flash_read( &offset, buf, nbytes );
}

int32_t kv_flash_write(uint32_t offset, void *buf, uint32_t nbytes)
{
    return hal_flash_write( &offset, buf, nbytes );
}

int32_t kv_flash_erase(uint32_t offset, uint32_t size)
{
    return hal_flash_erase( offset, size );
}

void *kv_lock_create(void)
{
    mutex_t *m = NULL;

    wl_init_mutex( (mutex_t *)&m );

    return (void *)m;
}

int32_t kv_lock_free(void *lock)
{
    mutex_t *m = (mutex_t *)lock;

    if ( m == NULL ) {
        return -1;
    }

    wl_free_mutex( (mutex_t *)&m );

    m = NULL;

    return (uint32_t)m;
}

int32_t kv_lock(void *lock)
{
    wl_lock_mutex_to( (mutex_t *)&lock, portMAX_DELAY );

    return 0;
}

int32_t kv_unlock(void *lock)
{
    wl_unlock_mutex( (mutex_t *)&lock );

    return 0;
}

void *kv_sem_create(void)
{
    sema_t *s;

    wl_init_sema( (sema_t *)&s, 0, sema_binary );

    return (void *)s;
}

int32_t kv_sem_free(void *sem)
{
    sema_t *m = (sema_t *)sem;

    wl_free_sema( (sema_t *)&m );

    m = NULL;

    return (uint32_t)m;
}

int32_t kv_sem_wait(void *sem)
{
    wl_wait_sema( (sema_t *)(&sem), portMAX_DELAY );

    return 0;
}

int32_t kv_sem_post_all(void *sem)
{
    wl_send_sema( (sema_t *)(&sem) );

    return 0;
}

int32_t kv_start_task(const char *name, void (*fn)(void *), void *arg,
                      uint32_t stack)
{
    thread_hdl_t task_handle = NULL;

    task_handle = wl_create_thread( (const char *)name, (u32)stack, (u32)KV_TASK_PRIORITY, (thread_func_t)fn, (void *)arg );
    
    return (int32_t)task_handle;
}

void kv_delete_task(void)
{
    wl_destory_thread( NULL );
}

void *kv_malloc(uint32_t size)
{
    return wl_malloc( (u32)size );
}

void kv_free(void *ptr)
{
    wl_free( (void *)ptr );
}
