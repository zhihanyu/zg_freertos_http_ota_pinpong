#ifndef _WIFI_CONFIG_H_
#define _WIFI_CONFIG_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "s907x_zg_config.h"

typedef struct wifi_info_tt
{
   char     ssid[33];
   uint8_t  method;
   uint8_t  ssid_len;
   char     pwd[65];
   uint8_t  pwd_len;
}wifi_info_t;

typedef enum{
    ZG_MODE_NONE = 0,
	ZG_STA_MODE = 1,
	ZG_AP_MODE = 2,
    ZG_MODE_BUTT
}ZG_wifi_mode_t;

typedef enum{
	RESET_BY_POWER,
	RESET_BY_CMD
}reset_type_t;

void wifi_ssid_conf(uint8_t* str,int len);
void wifi_password_conf(uint8_t *str,int len);
void wifi_Adapter_start(ZG_wifi_mode_t mode);

int Restore_factory_settings_func(reset_type_t type);
void WIFI_Init();

ZG_wifi_mode_t wifi_get_mode();
void wifi_get_conf_msg(wifi_info_t *wifi_msg);
void wifi_get_mac_info(char *mac_str);
void wifi_get_ip_info(char *ip_addr);
void wifi_set_ip_info(char *ip_addr);

int get_wifi_connection_status(unsigned int xTicksToWait);
#if S907X_NET_SERVER_ENABLE
void set_wifi_connection_status(int status);
#endif
void wifi_scan_ap();
int get_ap_rssi();

void wifi_STA_Start();

#ifdef __cplusplus
}
#endif

#endif /* _COMMAND_H_ */
