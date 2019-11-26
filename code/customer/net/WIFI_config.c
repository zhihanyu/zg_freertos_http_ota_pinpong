
#include "sys/ZG_system.h"

#define     S907X_WDG_IN_SERVER

#ifdef      S907X_WDG_IN_SERVER
#define     S907X_WDG_REFTIME_MS        2000   
static      wdg_hdl_t wdg_hdl;
#endif

static uint8_t mac_addr[6];
static char mac_addr_str[13];

#if S907X_BUILD_FOR_ZG
char ip_info[S907X_MAX_IP_STR_LEN] = S907X_ZG_DEF_UDP_SERVER_IP;
#else
char ip_info[20] = "10.10.123.3";
#endif

static wifi_info_t wifi_set;

static ZG_wifi_mode_t wifi_mode;

static EventGroupHandle_t wifi_event_group;

static TimerHandle_t rstimer;
static uint8_t reset_count;

extern unsigned char ledMode;

void wifi_scan_ap()
{
	
	 wlan_sta_scan_once();

}

int ZG_restart_system(void)
{
#if S907X_BUILD_FOR_ZG
#ifndef    S907X_WDG_IN_SERVER 
		wl_os_mdelay(50);
		NVIC_SystemReset();
		USER_DBG("system restart.\n");
#else
		wdg_hdl.time_ms = S907X_WDG_REFTIME_MS;
		s907x_hal_wdg_init(&wdg_hdl);
		s907x_hal_wdg_start(&wdg_hdl);

		//if refresh watch dog timeout,system restarted
		USER_DBG("system restart by watch dog.\n");
#endif
		
		return 0;
#else
	HAL_Status status = HAL_ERROR;
	WDG_HwInitParam hwParam;
	WDG_InitParam param;

	hwParam.event = WDG_EVT_RESET_CPU;
	hwParam.resetCpuMode = WDG_RESET_CPU_CORE;
	hwParam.timeout = WDG_TIMEOUT_2SEC;
	hwParam.resetCycle = 1;

	param.hw = hwParam;

	status = HAL_WDG_Init(&param);
	if (status != HAL_OK)
		printf("Wdg Init Error %d\n", status);
	else
		printf("WatchDog ResetCpu Mode Started.\n");
		HAL_WDG_Start();
	return status;
#endif
}



void rstimer_cb(void *arg)
{
	USER_DBG("factory config info will be setting +\n");
	
	reset_count = 0;
	ZG_data_save(RESTORE_STORE,&reset_count);

	xTimerStop(rstimer,0);
	
	USER_DBG("factory config info will be setting -\n");
}

int Restore_factory_settings_func(reset_type_t type)
{
	int return_type = 0;
	ZG_data_read(RESTORE_STORE,&reset_count);
	if(reset_count == 0xff){

		reset_count = 0;
	}
	printf("\nreset count : %d \n",reset_count);
	if(type == RESET_BY_POWER){

		reset_count++;
	}else{

			reset_count = 3;
		//restart system 
			ZG_restart_system();
	}

	if(reset_count >= 4){
		reset_count = 0;
				return_type = -1;
	}

	ZG_data_save(RESTORE_STORE,&reset_count);

	if(rstimer == NULL)
		 rstimer = xTimerCreate("timer_hb", 8000, pdTRUE,(void *)0,rstimer_cb);
	xTimerStart(rstimer,0);

	return return_type;
}


static void save_ssid(char *ptr,int len)
{

		printf("store: ssid -> %s\n",ptr);
		uint8_t ssid_t[33] = {0};

		ssid_t[0] = len;
		memcpy(ssid_t + 1,ptr,len);
		ZG_data_save(DEV_SSID_STORE,ssid_t);
	
}

static void save_password(char *ptr,int len)
{

			printf("store: password -> %s\n",ptr);
			uint8_t pwd_t[65] = {0};
			pwd_t[0] = len;
			if(len != 0 && ptr != NULL){
				
				 memcpy(pwd_t + 1,ptr,len);
			}
			ZG_data_save(DEV_PWD_STORE,pwd_t);
	
}

static void save_wifi_mode(ZG_wifi_mode_t mode)
{
	if(wifi_mode != mode ){
		wifi_mode = mode;
			printf("wifi mode %d -> %d\n",mode,wifi_mode );
 		ZG_data_save(WIFI_MODE_STORE, (unsigned char *)&wifi_mode);
	}
}

void net_event_callback(uint32_t event, uint32_t data,void *arg)
{
#if ZG_NET_SERVER_ENBALE
	uint16_t type = EVENT_SUBTYPE(event);

	printf("%s msg (%u, %u)\n", __func__, type, data);
	switch (type) {
	case NET_CTRL_MSG_WLAN_CONNECTED:
		printf("NET_CTRL_MSG_WLAN_CONNECTED\n");
		break;
	case NET_CTRL_MSG_WLAN_SCAN_SUCCESS:
		printf("NET_CTRL_MSG_WLAN_SCAN_SUCCESS\n");
				wlan_sta_scan_results_t results; 
			results.size = 10;    //最多获取 10 个 AP 节点信息 
			results.ap = malloc(results.size * sizeof(wlan_sta_ap_t)); 
			
			if (results.ap == NULL) { 

				 printf("wifi:scan malloc fail\n");
			} 
			if (wlan_sta_scan_result(&results) == 0) { 

				char *scan_temp = malloc(100);
				char *scan_buff = malloc(1000);
				memset((uint8_t *)scan_buff,0,1000);
									/**
						 +ok=
						Ch,SSID,BSSID,Security,Indicator
						11,hings-net,14:75:90:79:20:87,WPAPSKWPA2PSK/AES,27
						7,Xiaomi_95A1,78:11:DC:1C:95:A2,WPAPSKWPA2PSK/AES,32
						 */
					strcat(scan_buff,"+ok=\n");
						strcat(scan_buff,"Ch,SSID,BSSID,Security,Indicator\n");
						for (int i = 0; i < results.num; i++) {

								memset(scan_temp,0,100);
								sprintf(scan_temp,
										"%d,%s,%02X:%02X:%02X:%02X:%02X:%02X,%s,%d\n",
										results.ap[i].channel,
										(char *)results.ap[i].ssid.ssid,
										results.ap[i].bssid[0], results.ap[i].bssid[1], results.ap[i].bssid[2], results.ap[i].bssid[3], results.ap[i].bssid[4], results.ap[i].bssid[5],
										"WPAPSKWPA2PSK/AES",
										129 + results.ap[i].rssi);
											 
								strcat(scan_buff,scan_temp);
						}
						UDP_Send(scan_buff,strlen(scan_buff));
			}else{

				 printf("scan fail\n");
			} 
			free(results.ap);
		break;
	case NET_CTRL_MSG_WLAN_CONNECT_FAILED:
		printf("NET_CTRL_MSG_WLAN_CONNECT_FAILED\n");
			xEventGroupClearBits(wifi_event_group, BIT0);
		break;
	case NET_CTRL_MSG_NETWORK_UP:

		printf("NET_CTRL_MSG_NETWORK_UP: %s\n", ip_info);
		if(memcmp(ip_info,"10.10.123.3",11) != 0){

			xEventGroupSetBits(wifi_event_group, BIT0);
				ZG_event_send(CONNECTED_ROUTER_EVENT);
				}
		break;
	case NET_CTRL_MSG_NETWORK_DOWN:
		printf("NET_CTRL_MSG_NETWORK_DOWN\n");
	
		break;
	case NET_CTRL_MSG_WLAN_DISCONNECTED:
			printf("NET_CTRL_MSG_WLAN_DISCONNECTED\n");
			xEventGroupClearBits(wifi_event_group, BIT0);
	case NET_CTRL_MSG_WLAN_SCAN_FAILED:
	case NET_CTRL_MSG_WLAN_4WAY_HANDSHAKE_FAILED:
		printf("do nothing msg (%u, %u)\n", type, data);
		break;
	default:
		printf("unknown msg (%u, %u)\n", type, data);
		break;
	}
#endif
}

int get_wifi_connection_status(unsigned int xTicksToWait)
{
	return xEventGroupWaitBits(wifi_event_group, BIT0, false, true, xTicksToWait);
}

#if S907X_NET_SERVER_ENABLE
void set_wifi_connection_status(int status)
{
		if (S907X_ZG_ENABLE == status) {
				xEventGroupSetBits(wifi_event_group, BIT0);
		}
		else if (S907X_ZG_DISABLE == status) {
			xEventGroupClearBits(wifi_event_group, BIT0);
		}
}
#endif

void wifi_ssid_conf(uint8_t * str,int len)
{
	 memset(wifi_set.ssid,0,33);
	 memcpy(wifi_set.ssid,str,len);
	 wifi_set.ssid_len = len;
	 save_ssid(wifi_set.ssid,len);
}

void wifi_password_conf(uint8_t *str,int len)
{
		int length = len;
		memset(wifi_set.pwd,0,65);
		if(str == NULL){
				length = 0;
		}
		
		if( length != 0){
			memcpy(wifi_set.pwd,str,length);
			wifi_set.pwd_len = length;
			save_password(wifi_set.pwd,length);
		}
		else
		{  
				save_password(wifi_set.pwd,0);
		}
 
#if 0
	memset(wifi_set.pwd,0,65);
	if(len != 0){
		
		memcpy(wifi_set.pwd,str,len);
		wifi_set.pwd_len = len;
		save_password(wifi_set.pwd,len);
	}
	else
	{  
					
		 save_password(wifi_set.pwd,64);
	}
#endif
 
}


void wifi_STA_Start()
{
#if S907X_BUILD_FOR_ZG
		if(HAL_OK != s907x_zg_wlan_start(S907X_MODE_STA, &wifi_set)){
				USER_DBG("sta start wtih ssid:%s password:%s fail!\n", wifi_set.ssid, wifi_set.pwd);
				return;
		}

		USER_DBG("sta start success\n");
		save_wifi_mode(ZG_STA_MODE);

#else
	net_switch_mode(WLAN_MODE_STA);
	wlan_sta_set(wifi_set.ssid, wifi_set.ssid_len, wifi_set.pwd);
	wlan_sta_enable();
	save_wifi_mode(ZG_STA_MODE);
	printf("wifi_init_sta finished.SSID:%s password:%s\n",
	wifi_set.ssid, wifi_set.pwd);
#endif
}


static void AP_IP_Config()
{
	// net_config(struct netif *nif, uint8_t bring_up);
}

static void wifi_AP_Start()
{
#if S907X_BUILD_FOR_ZG
		if(HAL_OK != s907x_zg_wlan_start(S907X_MODE_AP, &wifi_set)){
				USER_DBG("ap start wtih ssid:%s password:%s fail!\n", wifi_set.ssid, wifi_set.pwd);
				return;
		}
		USER_DBG("wifi_init_softap finished.SSID:%s password:%s\n",wifi_set.ssid, wifi_set.pwd);
		save_wifi_mode(ZG_AP_MODE);
#else
		net_switch_mode(WLAN_MODE_HOSTAP);
		/* disable AP to set params*/
		wlan_ap_disable();

		wlan_ap_set(wifi_set.ssid, wifi_set.ssid_len, wifi_set.pwd);

		wlan_ap_enable();

		printf("wifi_init_softap finished.SSID:%s password:%s\n",
		wifi_set.ssid, wifi_set.pwd);

		save_wifi_mode(ZG_AP_MODE);
#endif
}

void wifi_Adapter_start(ZG_wifi_mode_t mode)
{

	if(ZG_AP_MODE == mode){

		wifi_AP_Start();
	}else if (ZG_STA_MODE == mode){

		wifi_STA_Start();
	}
}

void factory_ap_conf(void)
{
		printf("factory ap mode \n");
		char dev_name[20] = {0};
		//default netif id S907X_DEV0_ID
#if S907X_NET_SERVER_ENABLE
		char mac_address[S907X_MAX_MAC_ADDR_STR_LEN] = {0};
		s907x_wlan_get_mac_address(S907X_DEV0_ID,  mac_address);
		sprintf(dev_name,"LEDnet%02X%02X%02X",
				mac_address[3], mac_address[4], mac_address[5]);
#else
		sprintf(dev_name,"LEDnet%02X%02X%02X",mac_addr[3],mac_addr[4],mac_addr[5]);
#endif
		printf("device name:%s\n",dev_name);
		/* switch to ap mode */
		net_switch_mode(WLAN_MODE_HOSTAP);
		wifi_ssid_conf((uint8_t *)dev_name,12);
		wifi_password_conf(NULL,64);
		wifi_Adapter_start(ZG_AP_MODE);
		tcp_client_deinit();
}

int net_service_init(void)
{
#if ZG_NET_SERVER_ENBALE
	observer_base *observer;

	printf("foneric_net_service_init, register network observer\n");
	observer = sys_callback_observer_create(CTRL_MSG_TYPE_NETWORK,
						NET_CTRL_MSG_ALL,
						net_event_callback,NULL);
	if (observer == NULL)
		return -1;
	if (sys_ctrl_attach(observer) != 0)
		return -1;
#endif
	return 0;
}

void wifi_get_ip_info(char *ip_addr)
{
	memcpy(ip_addr,ip_info,strlen(ip_info));
}

void wifi_set_ip_info(char *ip_addr)
{
		memset(ip_info, 0, sizeof(ip_info));
		memcpy(ip_info, ip_addr, strlen(ip_addr));
}

void wifi_get_mac_info(char *mac_str)
{
	memcpy(mac_str, mac_addr_str, 12);
}

int get_ap_rssi()
{
#if S907X_BUILD_FOR_ZG
		s907x_link_info_t info;
		if(s907x_wlan_get_link_infor(&info)){
				USER_DBG("get link infor error\n");
		return 0;
		}
		if(!info.is_connected){
				USER_DBG("link is not connected\n");
				return 0;
		}
		return info.rssi;

#else
	wlan_sta_ap_t *ap = malloc(sizeof(wlan_sta_ap_t)); 
	if (ap == NULL) { 
		 printf("get ap info error\n");
	} 
	wlan_sta_ap_info(ap); 
	return ap->rssi;
#endif
}

void wifi_get_conf_msg(wifi_info_t *wifi_msg)
{
	*wifi_msg = wifi_set;
}

ZG_wifi_mode_t wifi_get_mode()
{
	return wifi_mode;
}

void WIFI_Init()
{
#if S907X_BUILD_FOR_ZG
		int wlan_netif = S907X_DEV0_ID;
		wifi_get_netif(&wlan_netif);
		wlan_get_mac_addr(wlan_netif, mac_addr, 6);
#else
		wlan_get_mac_addr(g_wlan_netif, mac_addr, 6);
#endif
		wifi_event_group = xEventGroupCreate();
		sprintf(mac_addr_str,"%02X%02X%02X%02X%02X%02X",mac_addr[0],mac_addr[1],mac_addr[2],mac_addr[3],mac_addr[4],mac_addr[5]);
		printf("mac sting:%s\n", mac_addr_str);
		net_service_init();

		if(Restore_factory_settings_func(RESET_BY_POWER) == -1){
				goto factory_ap_mode;
		}

		ZG_data_read(WIFI_MODE_STORE, (unsigned char *)&wifi_mode);
		if(wifi_mode <= ZG_MODE_NONE || ZG_MODE_BUTT <= wifi_mode){
			goto factory_ap_mode;
		}else{
			uint8_t tmp[65] = {0};
			ZG_data_read(DEV_SSID_STORE,tmp);
			printf("system read:ssid length : %d\n", tmp[0]);
			if(tmp[0] > 32){ // length
				 goto factory_ap_mode;
			}else{
				wifi_set.ssid_len = tmp[0];
				memcpy(wifi_set.ssid,tmp + 1,tmp[0]);
				printf("system read:ssid:%s\n", wifi_set.ssid);
			}
			ZG_data_read(DEV_PWD_STORE,tmp);
			printf("system read:password length : %d\n", tmp[0]);
			if(tmp[0] > 64){ // length
				 goto factory_ap_mode;
			}else{
				wifi_set.pwd_len = tmp[0];
				memcpy(wifi_set.pwd,tmp + 1,tmp[0]);
			}
			printf("system read:password : %s\n",wifi_set.pwd);
			s907x_start_action_task(s907x_action_start_wifi, &wifi_mode);
		}

		return;

		factory_ap_mode:
				factory_ap_conf();
				if(ledMode != MODE_LIGHT_TEST)
				{
						ZG_event_send(FACTORY_SETTING_EVENT);
				}     
} 

