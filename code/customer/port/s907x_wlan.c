#include "s907x.h"
#include "s907x_wlan.h"
#include "s907x_zg_config.h"
#include "lwip/ip_addr.h"
#include "dhcps.h"
#include "WIFI_config.h"
#include "sys/ZG_system.h"
#include "lwip/netif.h"

char g_scan_result_num = 0;
char g_scan_buff[S907X_MAX_LEN_SCAN_INFOS] = {0};

void net_switch_mode(u8 mode)
{
  s907x_wlan_off();
  wl_os_mdelay(100);
  s907x_wlan_on(mode);
}

void wifi_get_netif(int *netif)
{
    //sta mode or ap mode  -> netif id 0
    *netif = S907X_DEV0_ID;
}

static void s907x_zg_wlan_change_priority(bool set_higher)
{
    TaskHandle_t light_hdl = NULL;

    light_hdl = light_get_task_handler();
    if (NULL == light_hdl) {
        USER_DBG("Get light task handler fail.");
        return;
    }
    
    if (TRUE == set_higher) {
        vTaskPrioritySet(light_hdl, WLAN_INIT_PRIO);
    }
    else {
        vTaskPrioritySet(light_hdl, APP_PRIO_MAX);
    }
}

static void s907x_zg_wlan_scan_cb(s907x_scan_result_t *presult)
{
    wlan_target_infor_t *target;
    ASSERT(presult);
    target = (wlan_target_infor_t*)presult->context;
    if(presult->max_nums < 1) {
        USER_DBG("scan result : no network!\n");
        if(target && target->sema) {
            wl_send_sema((sema_t*)&target->sema);
        }
        return;
    }
    if(presult->id == 0){
        USER_DBG("-----------------------scan result------------------\n");
    }
    USER_DBG("scan id %02d max ap %02d bssid "MAC_FMT" channel %02d security = %d rssi %02d ssid %s \n",
                presult->id,
                presult->max_nums,
                presult->scan_info.bssid[0],
                presult->scan_info.bssid[1],
                presult->scan_info.bssid[2],
                presult->scan_info.bssid[3],
                presult->scan_info.bssid[4],
                presult->scan_info.bssid[5],
                presult->scan_info.channel,
                presult->scan_info.security,
                presult->scan_info.rssi,
                presult->scan_info.ssid);
    if (NULL == target) {
        return;
    }
    if((strlen(target->ssid) == presult->scan_info.ssid_len) 
        && !strncmp(target->ssid, presult->scan_info.ssid, presult->scan_info.ssid_len)) {
        target->match =  TRUE;
        target->channel = presult->scan_info.channel;
        target->security = (s907x_security_e)presult->scan_info.security;
    }
    if(presult->id == presult->max_nums - 1) {
        if(target->sema) {
            wl_send_sema((sema_t*)&target->sema);
        }
    }
}

void wlan_sta_scan_once(void)
{
    int ret;
    sema_t  sema;
    
    s907x_zg_wlan_change_priority(TRUE);
    wl_init_sema(&sema, 0, sema_binary);
    ret = s907x_wlan_scan(s907x_zg_event_scan_cb, S907X_DEFAULT_SCAN_AP_NUMS, &sema);

    if(ret) {
        wl_free_sema(&sema);
        return;
    }
    wl_wait_sema(&sema, portMAX_DELAY);
    wl_free_sema(&sema);
    s907x_zg_wlan_change_priority(FALSE);

    wl_os_mdelay(500);
    
    s907x_wlan_send_scan_result();
}


static int wlan_connect_normal(s907x_sta_init_t *init)
{
    int ret = HAL_OK;
    wlan_target_infor_t target;
    
    s907x_zg_wlan_change_priority(TRUE);
    do {
        memset(&target, 0, sizeof(wlan_target_infor_t));
        target.match = FALSE;
        strcpy(target.ssid, init->ssid);
        USER_DBG("target.ssid:%s\n",target.ssid);
        
        wl_init_sema(&target.sema, 0, sema_binary);
        ret = s907x_wlan_scan(s907x_zg_wlan_scan_cb, S907X_DEFAULT_SCAN_AP_NUMS, &target);
        if(ret) {
            wl_free_sema(&target.sema);
            continue;
        }        
        wl_wait_sema(&target.sema, portMAX_DELAY);
        wl_free_sema(&target.sema);
        
        if(0 == target.match){
            USER_DBG("ret = %d\n",ret);
            ret = HAL_ERROR;
            wl_os_mdelay(S907X_WLAN_RESCAN_INTERVAL);
            continue;
        }

        //set security
        init->security = target.security;
        USER_DBG("target.security = %d\n", target.security);
        //set connection mode
        init->conn.mode =  CONN_MODE_BLOCKING;
        init->conn.blocking_timeout = ZG_WIFI_BLOCK_TIME;
        init->auto_conn.enable = 1;
        init->auto_conn.interval_s = S907X_WLAN_AUTO_CONN_INTERVAL;
        init->auto_conn.cnt = 255;
        init->auto_conn.use_staticip = 0;
        ret = s907x_wlan_start_sta(init);
        
        USER_DBG("start sta, ret = %d\n",ret);
        if (HAL_OK == ret) {
            break;
        } 
        else {
            wl_os_mdelay(S907X_WLAN_RECONN_INTERVAL);
        }
    }while (1);
    s907x_zg_wlan_change_priority(FALSE);
    
    USER_DBG("ret = %d\n",ret);
    return ret;
}

static int wlan_set_sta(wifi_info_t *wf_info)
{
    int ret = HAL_OK;
    int sta_if_idx = S907X_DEV0_ID;
    u8 mac[6];
    u8 phymode;
    char host_name[32] = {0};
    struct netif *sta_if = NULL;
    s907x_sta_init_t s907x_sta_init = {0};
    
    sta_if = LwIP_GetNetif(sta_if_idx);
    s907x_wlan_event_reg(S907X_EVENT_STAMODE_CONNECTED, s907x_zg_event_stamode_connected, NULL);
    s907x_wlan_event_reg(S907X_EVENT_STAMODE_DISCONNECTED, s907x_zg_event_stamode_disconnected, NULL);

    s907x_wlan_get_mac_address(sta_if_idx, mac);
    sprintf(host_name, "sta_%02X%02X%02X", mac[3], mac[4], mac[5]);
    lwip_set_hostname(sta_if, host_name);
    
    s907x_sta_init.ssid = wf_info->ssid;
    s907x_sta_init.ssid_len = wf_info->ssid_len;
    s907x_sta_init.password = wf_info->pwd;
    s907x_sta_init.password_len = wf_info->pwd_len;
    if (s907x_sta_init.ssid_len != strlen(s907x_sta_init.ssid)) {
        printf("Error, related ssid[%s], len[%d], stored ssid len[%d].\n",
            s907x_sta_init.ssid,
            strlen(s907x_sta_init.ssid),
            s907x_sta_init.ssid_len);
    }
    if (s907x_sta_init.password_len != strlen(s907x_sta_init.password)) {
        printf("Error, related pwd[%s], len[%d], stored pwd len[%d].\n",
            s907x_sta_init.password,
            strlen(s907x_sta_init.password),
            s907x_sta_init.password_len);
    }
    USER_DBG("s907x_sta_init.password_len = %d\n", s907x_sta_init.password_len);
    
    //phymode = 2; // Set to 802.11bgn mode
    phymode = S907X_PHYMODE_BG;
    s907x_wlan_set_phy_mode(phymode);
    //only ssid password, try to scan target ssid
    ret =  wlan_connect_normal(&s907x_sta_init);
    if(!ret){
        dhcpc_start(0, 0);
        USER_DBG("dhcp start!\n");
    }
    return ret;
}

static int wlan_set_ap(wifi_info_t *wf_info)
{
    //int security;
    int ret = HAL_OK;
    int id;
    int mode;
    unsigned char phymode;
    struct ip_addr ipaddr;
    struct ip_addr netmask;
    struct ip_addr gw;
    char ip_str[S907X_MAX_IP_STR_LEN] = {0};
    s907x_ap_init_t ap_init = {0};
    
    ap_init.ssid = wf_info->ssid;
    ap_init.ssid_len = strlen(wf_info->ssid);
    ap_init.password = wf_info->pwd;
    ap_init.password_len = strlen(wf_info->pwd);

    if ((ap_init.ssid_len != wf_info->ssid_len) 
            || (ap_init.password_len != wf_info->pwd_len)) {
        printf("Note, strlen(ssid) is [%d], ssid_len is [%d], strlen(pwd) is [%d], pwd_len is [%d].",
                ap_init.ssid_len, wf_info->ssid_len,
                ap_init.password_len, wf_info->pwd_len);
    }

    if ((0 == ap_init.password_len) || (64 == wf_info->pwd_len)) {
        //set default none 
        ap_init.security = S907X_SECURITY_NONE;
    }
    else {
        ap_init.security = S907X_SECURITY_WPA2_AES;
    }
    ap_init.channel = 1;// chn5 default
    //ap_init.is_hidded_ssid = 0; 
    //phymode = 1; // Set to 802.11bg mode
    phymode = S907X_PHYMODE_BG;
    s907x_wlan_set_phy_mode(phymode);
    
    mode = s907x_wlan_get_mode();
    if(S907X_MODE_AP == mode) {
        id = S907X_DEV0_ID;
    } 
    else if(S907X_MODE_STA_AP == mode) {
        id = S907X_DEV1_ID;
    }

    //country code default
    //phymode default
    s907x_zg_wlan_change_priority(TRUE);
    ret = s907x_wlan_start_ap(&ap_init);
    USER_DBG("start ap ret:%d\n",ret);
    if(!ret){
        //static ip
        wifi_get_ip_info(ip_str);
        inet_aton(ip_str, (struct in_addr *)&ipaddr);
        IP4_ADDR(&netmask, AP_NETMASK_ADDR0, AP_NETMASK_ADDR1, AP_NETMASK_ADDR2, AP_NETMASK_ADDR3);
        gw.addr = ipaddr.addr;
        netif_set_addr(LwIP_GetNetif(id), &ipaddr, &netmask, &gw);
    }
    s907x_zg_wlan_change_priority(FALSE);
    
    dhcps_init(LwIP_GetNetif(id));
    if(ret == HAL_OK){
      USER_DBG("start ap success.\n");
    }
    return ret;
}

int s907x_zg_wlan_start(u8 wf_m, wifi_info_t *wf_info)
{
    int ret = HAL_ERROR;
    u8 mode = S907X_MODE_NONE;
    
    if(wf_m <= ZG_MODE_NONE || ZG_MODE_BUTT <= wf_m){
        return ret;
    } 
    
    mode = s907x_wlan_get_mode();
    switch (wf_m) {
        case ZG_STA_MODE:
            mode = S907X_MODE_STA;
            break;
        case ZG_AP_MODE:
            mode = S907X_MODE_AP;
            break;
    }

    if (S907X_MODE_NONE == mode) {
        return ret;
    }
    
    s907x_wlan_off(); 
    wl_os_mdelay(50);
    s907x_wlan_on(mode);
    
    if(S907X_MODE_AP == mode){
        ret = wlan_set_ap(wf_info);
    }
    else if(S907X_MODE_STA == mode){
       ret = wlan_set_sta(wf_info);
    }
      
    return ret;
    
}

void s907x_action_start_wifi(void *ctx)
{
    ZG_wifi_mode_t wifi_mode = *(ZG_wifi_mode_t *)ctx;

    vTaskDelay(200 / portTICK_PERIOD_MS);

    wifi_Adapter_start(wifi_mode);

    wl_destory_threadself();
}

static void get_mac_netif(u8 id, u8* mac)
{
    s907x_wlan_get_mac_address(id, mac);
}

int wlan_get_mac_addr(u8 netif_id, u8* mac, int mac_len)
{
    int ret = -1;

    get_mac_netif(netif_id, mac);
    ret = 0;

    return ret;
}

void s907x_wlan_send_scan_result(void)
{
    UDP_Send(g_scan_buff, strlen(g_scan_buff));
}

static int s907x_sta_net_connection_ready(void)
{
    ip_addr_t ip_addr = {0};
    char local_ip_str[S907X_MAX_IP_STR_LEN] = {0};
    char if_ip_str[S907X_MAX_IP_STR_LEN] = {0};
    struct netif *sta_if = NULL;

    wifi_get_ip_info(local_ip_str);
    if ((strlen(local_ip_str) == strlen(S907X_ZG_DEF_UDP_SERVER_IP))
            && (memcmp(local_ip_str, S907X_ZG_DEF_UDP_SERVER_IP, strlen(S907X_ZG_DEF_UDP_SERVER_IP)) == 0)) {
        USER_DBG("Resotred ip address is default value [%s].", local_ip_str);
        return FALSE;
    }

    sta_if = LwIP_GetNetif(S907X_DEV0_ID);
    if ((NULL == sta_if) || (NULL == LwIP_GetIP(sta_if))) {
        printf("Get ip addr for netif fail.\n");
        return FALSE;
    }

    memcpy(&ip_addr.addr, LwIP_GetIP(sta_if), sizeof(ip_addr_t));
    memcpy(if_ip_str, inet_ntoa(ip_addr), S907X_MAX_IP_STR_LEN);
    if ((strlen(if_ip_str) != strlen(local_ip_str))
            || (memcmp(if_ip_str, local_ip_str, strlen(local_ip_str)) != 0)) {
        USER_DBG("Local restored ip [%s] not equal to ip [%s] for sta interface.",
                local_ip_str, if_ip_str);
        return FALSE;
    }

    return TRUE;
}

void s907x_zg_event_scan_cb(s907x_scan_result_t *presult)
{
    sema_t *target_sema;
    char scan_info[S907X_MAX_LEN_PER_SCAN_INFO] = {0};

    /** send scan infomations to APP:
     +ok=
    Ch,SSID,BSSID,Security,Indicator
    11,hings-net,14:75:90:79:20:87,WPAPSKWPA2PSK/AES,27
    7,Xiaomi_95A1,78:11:DC:1C:95:A2,WPAPSKWPA2PSK/AES,32
    */
    ASSERT(presult);
    target_sema = (sema_t *)presult->context;
    if(presult->max_nums < 1) {
        USER_DBG("scan result : no network!\n");
        if(target_sema) {
            wl_send_sema(target_sema);
        }
        return;
    } 
    if(presult->id == 0){
        USER_DBG("-----------------------scan result------------------\n");
        memset(g_scan_buff, 0, sizeof(g_scan_buff));
        g_scan_result_num = 0;
        strcat(g_scan_buff,"+ok=\n");
        strcat(g_scan_buff,"Ch,SSID,BSSID,Security,Indicator\n");
    }
    USER_DBG("scan id %02d max ap %02d bssid "MAC_FMT" channel %02d security = %d rssi %02d ssid %s \n",
                presult->id,
                presult->max_nums,
                presult->scan_info.bssid[0],
                presult->scan_info.bssid[1],
                presult->scan_info.bssid[2],
                presult->scan_info.bssid[3],
                presult->scan_info.bssid[4],
                presult->scan_info.bssid[5],
                presult->scan_info.channel,
                presult->scan_info.security,
                presult->scan_info.rssi,
                presult->scan_info.ssid);

    if ((g_scan_result_num < 32) && (strlen(presult->scan_info.ssid) != 0)) {
        memset(scan_info, 0, 100);
        sprintf(scan_info, "%d,%s,%02X:%02X:%02X:%02X:%02X:%02X,%s,%d\n",
                presult->scan_info.channel,
                presult->scan_info.ssid,
                presult->scan_info.bssid[0],
                presult->scan_info.bssid[1],
                presult->scan_info.bssid[2],
                presult->scan_info.bssid[3],
                presult->scan_info.bssid[4],
                presult->scan_info.bssid[5],
                "WPAPSKWPA2PSK/AES",
                presult->scan_info.rssi + S907X_WLAN_ISO_RSSI_COMP);
               
        strcat(g_scan_buff, scan_info);
        g_scan_result_num++;
        USER_DBG("Restore scan buff message, idx[%d].", presult->id);
    }
    if(presult->id == presult->max_nums - 1) {
        if(target_sema) {
            wl_send_sema(target_sema);
        }
    }
}

// This event cb will be called after got ip.
void s907x_zg_event_sta_got_ip(uint32 addr)
{
    ip_addr_t ip_addr = {0};
    char ip_str[S907X_MAX_IP_STR_LEN] = {0};

    ip_addr.addr = addr;
    memcpy(ip_str, inet_ntoa(ip_addr), S907X_MAX_IP_STR_LEN);
    USER_DBG("Sta interface get ip[%s]", ip_str);
    wifi_set_ip_info(ip_str);

    if(TRUE == s907x_sta_net_connection_ready()) {
        USER_DBG("Send connected router event and set wifi status to enabled.\n");
        set_wifi_connection_status(S907X_ZG_ENABLE);
        ZG_event_send(CONNECTED_ROUTER_EVENT);
    }
    else {
        USER_DBG("Network not up.");
    }
}

void s907x_zg_event_stamode_connected(s907x_event_data *s907x_data, void *context)
{
    printf("NET_CTRL_MSG_WLAN_CONNECTED\n");

    USER_DBG("ssid %s bssid "MAC_FMT" channel %d\n",
            s907x_data->esp_data.connected.ssid,
            s907x_data->esp_data.connected.bssid[0],
            s907x_data->esp_data.connected.bssid[1],
            s907x_data->esp_data.connected.bssid[2],
            s907x_data->esp_data.connected.bssid[3],
            s907x_data->esp_data.connected.bssid[4],
            s907x_data->esp_data.connected.bssid[5],
            s907x_data->esp_data.connected.channel);

    /* Send network up event for re-connection for wireless */
    if (TRUE == s907x_sta_net_connection_ready()) {
        set_wifi_connection_status(S907X_ZG_ENABLE);
        ZG_event_send(RECONNECTED_ROUTER_EVENT);
    }
}

void s907x_zg_event_stamode_disconnected(s907x_event_data *s907x_data, void *context)
{
    printf("NET_CTRL_MSG_WLAN_CONNECT_FAILED\n");

    USER_DBG("reason = %d ssid %s bssid "MAC_FMT" \n",
            s907x_data->esp_data.disconnected.reason,
            s907x_data->esp_data.connected.ssid,
            s907x_data->esp_data.connected.bssid[0],
            s907x_data->esp_data.connected.bssid[1],
            s907x_data->esp_data.connected.bssid[2],
            s907x_data->esp_data.connected.bssid[3],
            s907x_data->esp_data.connected.bssid[4],
            s907x_data->esp_data.connected.bssid[5]);

    set_wifi_connection_status(S907X_ZG_DISABLE);
}

