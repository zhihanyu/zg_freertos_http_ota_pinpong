#ifndef __STORE_H__
#define __STORE_H__



#ifdef __cplusplus
extern "C" {
#endif

#define MAGIC_VAL 0x12345678

typedef enum{

    HOST_STORE = 0x01,
    WIFI_MODE_STORE,
    COLOR_STORE,
    CCT_STORE,
    AES_KEY_STORE,
    CLOUD_SWITCH_STORE,
    CLOCK_STORE,
    RESTORE_STORE,
    LED_SWITCH_STORE,
    LED_MODE_STORE,
    LED_SPEED_STORE,
    LED_CUSTOM_MODE_STORE,
    LED_SORT_STORE,
    REMOTE_ADDR_STORE,
    PORT_STORE,
    DEV_SSID_STORE,
    DEV_PWD_STORE,
    PRINTF_EN_STORE,
    ST1_STORE,
    ST2_STORE,
    MAGIC,
    ZG_STORE_MAX,

}store_type_t;

int  ZG_data_read(store_type_t type,unsigned char *dat);
int  ZG_data_save(store_type_t type,unsigned char *dat);
void ZG_store_init(void);
void ZG_store_deinit(void);


#ifdef __cplusplus
}
#endif


#endif /*#ifndef __TCP_PERF_H__*/

