#ifndef _ZG_SYS_H_
#define _ZG_SYS_H_

/*
 task priority
 tcp sever 6 
 udp 5
 tcp client 7
 store  2
 system 1

*/

#ifdef __cplusplus
extern "C" {
#endif
//#include "sys/ZG_system.h"
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

#include "s907x_zg_config.h"
#include "s907x_fota.h"
#include "s907x_store.h"
#include "s907x_wlan.h"
#include "s907x_zg_utils.h"

#include "lwip/inet.h"
#include "lwip/sockets.h"
#include "lwip/err.h"
#include "lwip/sys.h"
#include "lwip/netdb.h"
#include "lwip_conf.h"
#include "mbedtls/aes.h"
#include "FreeRTOS.h"
#include "event_groups.h"
#include "task.h"
#include "queue.h"
#include "timers.h"

#include "net/WIFI_config.h"

#include "socket/udp_perf.h"
#include "socket/tcp_perf.h"
#include "socket/tcp_client.h"

#include "protocol_handle/protocol_handle.h"
#include "protocol_handle/proto_parser_pack.h"

#include "sys/encryption.h"
#include "sys/store.h"
#include "sys/RTC_time.h"
#include "sys/fota.h"
#include "sys/debug_info.h"
#include "sys/ZG_AT_CMD.h"

#include "light/clock.h"
#include "light/auto_report.h"
#include "light/Light_fun.h"
#include "light/color.h"
#include "light/IR.h"
#include "light/Remote.h"




typedef int32_t esp_err_t;

/* Definitions for error constants. */
#define ESP_OK          0       /*!< esp_err_t value indicating success (no error) */
#define ESP_FAIL        -1      /*!< Generic esp_err_t code indicating failure */

#define ZG_WATCHDOG_TIMEOUT         10000   /* Watchdog will reset system if not been freshed for 10seconds */
#define ZG_WATCHDOG_FRESH_INTER     8000    /* Fresh every 8seconds */
#if S907X_BUILD_FOR_ZG 
#define ZG_system_time()  wl_get_systemtick()

#else
#define BIT0    0x00000001

#define ZG_system_time()  OS_GetTicks()
#endif
/*

#include "fota.h"
	*/
void ZG_system_start();
void ZG_watchdog_start();

esp_err_t ZG_device_info_conf(char *dev_id,char * ver,uint8_t dev_code,uint8_t pro_ver);

void ZG_device_mcu_ver_conf(char * ver);

char *ZG_get_device_version();
char *ZG_get_device_id();
char *ZG_get_mcu_version();
uint8_t ZG_get_device_code();
uint8_t ZG_get_device_protocol_version();

typedef enum {
    FACTORY_SETTING_EVENT = 0,
    CONNECTED_ROUTER_EVENT,
    RECONNECTED_ROUTER_EVENT,
    OTA_PROCESS_EVENT,
    OTA_SUCCESS_EVENT,
    OTA_FAIL_EVENT
} ZG_system_event_t;


/**
  * @brief  Application specified event callback function
  *
  * @param  void *ctx : reserved for user
  * @param  system_event_t *event : event type defined in this file
  *
  * @return ESP_OK : succeed
  * @return others : fail
  */
typedef esp_err_t (*ZG_system_event_cb_t)(void *ctx, ZG_system_event_t *event);

/**
  * @brief  Initialize event loop
  *         Create the event handler and task
  *
  * @param  system_event_cb_t cb : application specified event callback, it can be modified by call esp_event_set_cb
  * @param  void *ctx : reserved for user
  *
  * @return ESP_OK : succeed
  * @return others : fail
  */
esp_err_t ZG_event_loop_init(ZG_system_event_cb_t cb, void *ctx);
esp_err_t ZG_event_send(ZG_system_event_t event);

#ifdef __cplusplus
}
#endif



#endif 
