#ifndef _S907X_WLAN_H_
#define _S907X_WLAN_H_

#include "s907x.h"
#include "lwip_conf.h"
#include "wifi_config.h"
#include "at_smartlink.h"

#define S907X_ZG_DEF_UDP_SERVER_IP      "10.10.123.3"

#define S907X_NET_EVENT_NO_TIME_LIMIT   0xFFFFFFFF
#define S907X_MAX_LEN_PER_SCAN_INFO     100
#define S907X_MAX_LEN_SCAN_INFOS        3000
#define S907X_MAX_IP_STR_LEN            20
#define S907X_MAX_MAC_ADDR_STR_LEN      24

#define S907X_ZG_ENABLE                 1
#define S907X_ZG_DISABLE                0

#define S907X_WLAN_AUTO_CONN_INTERVAL   10
#define S907X_WLAN_RECONN_INTERVAL      1000    //1000 MS
#define S907X_WLAN_RESCAN_INTERVAL      3000    //3000 MS

#define S907X_WLAN_ISO_RSSI_COMP        129

void net_switch_mode(u8 mode);
void wifi_get_netif(int *netif);
void wifi_event_callback(void);
void wlan_sta_scan_once(void);
int s907x_zg_wlan_start(u8 wf_m, wifi_info_t *wf_inf);
int wlan_get_mac_addr(u8 netif_id, u8* mac, int mac_len);

void s907x_zg_event_scan_cb(s907x_scan_result_t *presult);
void s907x_zg_event_stamode_connected(s907x_event_data *s907x_data, void *context);
void s907x_zg_event_stamode_disconnected(s907x_event_data *s907x_data, void *context);

void s907x_wlan_send_scan_result(void);
void s907x_action_start_wifi(void *ctx);










#endif



