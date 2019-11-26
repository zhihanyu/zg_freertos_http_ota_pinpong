

#include "sys/ZG_system.h"
#include "utils.h"
#define STEP_STAMODE 1
#define STEP_APMODE  2

static uint8_t wifi_conf_select;
static bool Is_router_conf = false;

void connect_to_routing_Indicator()
{ 
    if(Is_router_conf == true)
    {
        Is_router_conf = false;
        set_led_mode(MODE_STATIC);
        colort_struct_t color;
        memset(&color,0,sizeof(colort_struct_t));
        color.R = 0;
        color.G = 0xff;
        color.B = 0x00;
        Set_color(&color);
    }
}


static int g_action_switch = 0;
static int g_action_scan = 0;
static int g_action_rled = 0;


static void action_switch_ap(void *ctx)
{

	vTaskDelay(200 / portTICK_PERIOD_MS);

	if(wifi_conf_select == STEP_APMODE) {
		wifi_Adapter_start(ZG_AP_MODE);
	}
	else if(wifi_conf_select == STEP_STAMODE) {
		Is_router_conf = true;
	  	wifi_Adapter_start(ZG_STA_MODE);
	}

	g_action_switch = 0;

	wl_destory_threadself();
}

static void action_ap_scan(void *ctx)
{

	wifi_scan_ap();

	g_action_scan = 0;

	wl_destory_threadself();
}

static void action_rled(void *ctx)
{

	vTaskDelay(100 / portTICK_PERIOD_MS);

	Restore_factory_settings_func(RESET_BY_CMD);

	g_action_rled = 0;

	wl_destory_threadself();
}


static int String_contrast(char *pdata1,char* pdata2)
{
	return memcmp(pdata1,pdata2,strlen(pdata2));
}

void AT_CMD_parser(char *pdata)
{
    char *send_buff = malloc(256);
    memset(send_buff,0,256);
    printf("%s\n",pdata);
    /*device query,return back device id*/
    if(String_contrast(pdata,"HF-A11ASSISTHREAD") == 0)
    {
        char ip_str[20] = {0};
        char mac_str[13] = {0};
        wifi_get_ip_info(ip_str);
        wifi_get_mac_info(mac_str);

        /*192.168.1.109,DC4F22D20471,AK001-ZJ2101*/
        sprintf(send_buff,"%s,%s,%s",
        ip_str,
        mac_str,
        ZG_get_device_id());
        UDP_Send(send_buff,strlen(send_buff));
    }  

    /**设置关联 AP的 SSID   AT+WSSSID=<ap’s ssid >\r
    *  return +ok\r
    */
    else if(String_contrast(pdata,"AT+WSSSID=") == 0)
    {
        char *ptr1 = NULL;
        char WF_SSID[33] = {0};
        ptr1=(char *)strchr(pdata,'=');
        strncpy(WF_SSID,ptr1 + 1,strlen(ptr1) - 2);
        printf("fetch router ssid : %s length:%d\n",WF_SSID,strlen(WF_SSID));
        wifi_ssid_conf((uint8_t *)WF_SSID,strlen(WF_SSID));
        UDP_Send("+ok\r",4);
        wifi_conf_select = STEP_STAMODE;   
    } 
    /**查询关联 AP的 SSID   AT+WSSSID<CR> 
    * return +ok=<ap’s ssid>\r
    */
    else if(String_contrast(pdata,"AT+WSSSID\r") == 0)
    {
        wifi_info_t wifi_msg;
        memset(&wifi_msg,0,sizeof(wifi_info_t));
        wifi_get_conf_msg(&wifi_msg);
        sprintf(send_buff,"+ok=%s\r",(char *)wifi_msg.ssid);
        UDP_Send(send_buff,strlen(send_buff));
    }
    /**设置 STA的加密参数 AT+WSKEY=< auth，encry，key>
    *  return +ok\r
    */
    else if(String_contrast(pdata,"AT+WSKEY=") == 0)
    {
        char *ptr1 = NULL;
        char *ptr2 = NULL;
        char auth[5] = {0};
        char WF_PWD[65] = {0};
        memcpy(auth,pdata + 9,4);

        if(memcmp(auth,"OPEN",4) != 0)
        {
            ptr1 = (char *)strchr(pdata,',');
            ptr2 = (char *)strchr(ptr1 + 1,',');
            strncpy(WF_PWD,ptr2 + 1,strlen(ptr2) - 2);
            printf("fetch router password : %s length:%d\n",WF_PWD,strlen(WF_PWD));   
        }
        wifi_password_conf((uint8_t *)WF_PWD,strlen(WF_PWD));
        UDP_Send("+ok\r",4);
        wifi_conf_select = STEP_STAMODE;   
    }    
    /**查询STA的加密参数
    *  return +ok\r
    */
    else if(String_contrast(pdata,"AT+WSKEY\r") == 0)
    {
        wifi_info_t wifi_msg;
        memset(&wifi_msg,0,sizeof(wifi_info_t));
        wifi_get_conf_msg(&wifi_msg);

        if(strlen((char *)wifi_msg.pwd) == 0)
        {
            UDP_Send("+ok=OPEN,NONE\r",strlen("+ok=OPEN,NONE\r"));
        }
        else
        {
            sprintf(send_buff,"+ok=WPA2PSK,AES,%s\r",(char *)wifi_msg.pwd);
            UDP_Send(send_buff,strlen(send_buff));
        }
    }
    /**设置AP的加密参数
    *  return +ok=<auth，encry，key>\r
    */
    else if(String_contrast(pdata,"AT+WAKEY=") == 0)
    {
        char *ptr1 = NULL;
        char *ptr2 = NULL;
        char WF_PWD[65] = {0};
        char auth[5] = {0};
        memcpy(auth,pdata + 9,4);
       
        if(memcmp(auth,"OPEN",4) != 0)
        {
            ptr1 = (char *)strchr(pdata,',');
            ptr2 = (char *)strchr(ptr1 + 1,',');
            strncpy(WF_PWD,ptr2 + 1,strlen(ptr2) - 2);

            printf("fetch ap password : %s length:%d\n",WF_PWD,strlen(WF_PWD));
        }
        wifi_password_conf((uint8_t *)WF_PWD,strlen(WF_PWD));
        UDP_Send("+ok\r",4);
        wifi_conf_select = STEP_APMODE;   
    }
    /**查询AP的加密参数
    *  +ok=< auto,encry,key >\r
    */
    else if(String_contrast(pdata,"AT+WAKEY\r") == 0)
    {
        wifi_info_t wifi_msg;
        memset(&wifi_msg,0,sizeof(wifi_info_t));
        wifi_get_conf_msg(&wifi_msg);

        if(strlen((char *)wifi_msg.pwd) == 0)
        {
            UDP_Send("+ok=OPEN,NONE,\r",strlen("+ok=OPEN,NONE,\r"));
        }
        else
        {
            Z_DEBUG();
            sprintf(send_buff,"+ok=WPA2PSK,AES,%s\r",(char *)wifi_msg.pwd);
            UDP_Send(send_buff,strlen(send_buff));
        }
    }
    else if(String_contrast(pdata,"AT+WSLK\r") == 0)
    {
        if(wifi_get_mode() == ZG_STA_MODE && get_wifi_connection_status(0) == BIT0)
        {
            wifi_info_t wifi_msg;
            wifi_get_conf_msg(&wifi_msg);
            sprintf(send_buff,"+ok=(%s)\r",wifi_msg.ssid); 
             
        }
        else 
        {
            UDP_Send("Disconnected\r", strlen("Disconnected\r"));
        }
    }
    /**设置 AP的Wi-Fi配置参数
    * return +ok\r
    */ 
    else if(String_contrast(pdata,"AT+WAP=") == 0)
    {
        char WF_SSID[33] = {0};
        strncpy(WF_SSID,pdata + 13,strlen(pdata) - 19);
        printf("fetch ap ssid : %s length:%d\n",WF_SSID,strlen(WF_SSID));
        wifi_ssid_conf((uint8_t *)WF_SSID,strlen(WF_SSID));
        UDP_Send("+ok\r",4);
        wifi_conf_select = STEP_APMODE;   
    }
    /**查询 AP的Wi-Fi配置参数
    * return +ok=< wifi_mode，ssid，channel >\r
    */ 
    else if(String_contrast(pdata,"AT+WAP\r") == 0)
    {
        /*
        *  +ok=11BGN,LEDnet8795A5,1
        */
        wifi_info_t wifi_msg;
        memset(&wifi_msg,0,sizeof(wifi_info_t));
        wifi_get_conf_msg(&wifi_msg);
        sprintf(send_buff,"+ok=11BGN,%s,%d\r",(char *)wifi_msg.ssid,1);
        UDP_Send(send_buff,strlen(send_buff));
    }
    /**configure device to sta mode
    *  return +ok = \r
    */  
    else if(String_contrast(pdata,"AT+WMODE=STA\r") == 0)
    {
        wifi_conf_select = STEP_STAMODE;   
        UDP_Send("+ok\r",4);
    }
    /**configure device to ap mode
    *  return +ok = \r
    */  
    else if(String_contrast(pdata,"AT+WMODE=AP\r") == 0)
    {
        wifi_conf_select = STEP_APMODE;     
        UDP_Send("+ok\r",4);
    }
    else if(String_contrast(pdata,"AT+WMODE\r") == 0)
    {
        if(wifi_get_mode() == ZG_STA_MODE)
        {
            UDP_Send("+ok=STA\r",strlen("+ok=STA\r"));
        }
        else
        {
            UDP_Send("+ok=AP\r",strlen("+ok=AP\r"));
        }
    }
    /**确认执行
    *  return +ok = \r
    */ 
    else if(String_contrast(pdata,"AT+Z\r") == 0) 
    {
        UDP_Send("+ok\r",4);
  		if(!g_action_switch) {
			g_action_switch = 1;
			s907x_start_action_task(action_switch_ap, NULL);
  		}
    }
    else if(String_contrast(pdata,"AT+MACID\r") == 0)
    {
        uint8_t dest[32] = {0};
        uint8_t Device_mac_buffer[13] = {0};

        wifi_get_mac_info((char *)Device_mac_buffer);
        printf("mac:%s\n",Device_mac_buffer);
        AES_encrypt(Device_mac_buffer,dest);
        printf("mac encpy:%s\n",dest);
        sprintf(send_buff,"+ok=%s\r",dest);
        UDP_Send(send_buff,strlen(send_buff));
        
    }   
    else if(String_contrast(pdata,"AT+RELD\r") == 0)
    {
        UDP_Send("+ok\r",4);

  		if(!g_action_rled) {
			g_action_rled = 1;
			s907x_start_action_task(action_rled, NULL);
  		}
    }
    else if(String_contrast(pdata,"AT+LVER") == 0)
    {
        sprintf(send_buff,"+ok=%s\r",ZG_get_device_version());
        UDP_Send(send_buff,strlen(send_buff));
    }
    else if(String_contrast(pdata,"AT+WSCAN\r") == 0)   
    {
     
  		if(!g_action_scan) {
			g_action_scan = 1;
			s907x_start_action_task(action_ap_scan, NULL);
  		}		
    }    
    /*
    *建立tcp 客户端 AT+SOCKB=<tcp，port，IP>\r
    */
    else if(String_contrast(pdata,"AT+SOCKB=TCP") == 0)
    {
        UDP_Send("+ok\r",4);
        char *ptr1 = NULL;
        char *ptr2 = NULL;
        char port_temp[6] = {0};
        char domain[64] = {0};
        ptr1 = (char *)strchr(pdata,',');
        ptr2 = (char *)strchr(ptr1 + 1,',');

        memcpy(domain,ptr2+1,strlen(ptr2)-2);
        memcpy(port_temp,ptr1+1,ptr2-ptr1-1);
        domain[strlen(ptr2)-2] = '\0';
        printf("AT:fetch domain:%s\n",domain);
        printf("AT:fetch domain len:%d\n",strlen(domain));
        printf("AT:fetch port:%d\n",atoi(port_temp));

        /*如果已经开启了tcp client，且成功连到路由器*/
        if(get_tcp_Client_connect_status(0) == 1 && get_wifi_connection_status(0) == 1)
        {
            tcp_Client_stop();
            set_tcp_Client_conf(domain,strlen(domain),atoi(port_temp));
            tcp_Client_start();
        }
        else
        {
            if(get_wifi_connection_status(0) == BIT0)
            {
                set_tcp_Client_conf(domain,strlen(domain),atoi(port_temp));
                tcp_Client_start();
            }
            else
            {
                set_tcp_Client_conf(domain,strlen(domain),atoi(port_temp));
                /*在wifi连接成功事件callback中启动TCPClient_start()*/
            }
        }
    }
    /**
    * /查询 tcp 客户端网络协议参数  +ok=<tcp，port，IP>\r
    */ 
    else if(String_contrast(pdata,"AT+SOCKB\r") == 0)
    {
        if(get_tcp_Client_switch() == 1)
        {
            char hostname[64] = {0};
            uint32_t port = 0;
            int len = 0;

            get_tcp_Client_conf(hostname,&len,&port);
            sprintf(send_buff,"+ok=TCP,%d,%s\r",port,hostname);
            UDP_Send(send_buff,strlen(send_buff));
        }
        else
        {
            UDP_Send("+ok=",4);
        }
    }
    /**
    *查询 tcp client链接是否已建链接 +ok=on or +ok=off
    */
    else if(String_contrast(pdata,"AT+TCPLKB\r") == 0)
    {
        if(get_tcp_Client_switch() == 1)
        {
            if(get_tcp_Client_connect_status(0) == pdTRUE)
            {
                UDP_Send("+ok=on\r",7);
            }
            else
            {
                UDP_Send("+ok=off\r",8);
            }
        }
        else
        {
            UDP_Send("+ok=off\r",8);
        }
    } 
    /**
    *关闭tcp客户端
    */
    else if(String_contrast(pdata,"AT+SOCKB=NONE\r") == 0)
    {
        if(get_tcp_Client_connect_status(0) == 1)
        {
           tcp_Client_stop();
        }
        UDP_Send("+ok\r",4);
    }
    else if(String_contrast(pdata,"AT+UPURL=") == 0)
    {
        //AT+UPURL=http://wifi.magichue.net/WebMagicHome/upgrade.ashx?version=33_3_20180118&type=33,50525456
        char *ptr1 = NULL;
        char *ptr2 = NULL;
        char host_tmp[50] = {0};
        char port[5] = {0};
        int portconf = 80;
        uint8_t Code_t[16] = {0};
        
        ptr1 = (char *)strchr(pdata,',');
        ptr2 = strchr(ptr1,'\r');
        if( ptr2 == NULL)
        {
            return;
        }
        memcpy(Code_t,ptr1 + 1,ptr2 - ptr1 - 1); 
        printf("OTA fech code :%s(%d)",(char*)Code_t,strlen((char*)Code_t));

        ptr1 = strstr(pdata,"//");
        ptr2 = strchr(ptr1 + 2,':');
        if(ptr2 == NULL)
        {
            ptr2 = strchr(ptr1 + 2,'/');
            memcpy(host_tmp,ptr1 + 2,ptr2 - ptr1 -2);
        }
        else
        {
            memcpy(host_tmp,ptr1 + 2,ptr2 - ptr1 -2);
            ptr1 = strchr(ptr1 + 2,'/');
            memcpy(port,ptr2 + 1,ptr1 - ptr2 -1);
        }
        printf("fetch host : (%d) %s %s\n",strlen(host_tmp),host_tmp,port);
        char  *path_tmp = malloc(200);
        memset(path_tmp,0,200);

        if(strlen(port) != 0)
        {
            portconf = atoi(port);
            ptr2 = (char *)strchr(pdata,',');
            memcpy(path_tmp,ptr1,ptr2 - ptr1);
        }
        else
        {
            ptr1 = (char *)strchr(pdata,',');
            memcpy(path_tmp,ptr2,ptr1 - ptr2 );
        }      
        printf("feth path : (%d) %s \n",strlen(path_tmp),path_tmp);
        UDP_Send("+ok=",4);
        //http://192.168.1.100/OTA/xr_system.img
        fota_conf(host_tmp,strlen(host_tmp),portconf,path_tmp,strlen(path_tmp),(char*)Code_t,strlen((char*)Code_t));
        zg_ota_start();
        free(path_tmp);
    }
    free(send_buff);
}
