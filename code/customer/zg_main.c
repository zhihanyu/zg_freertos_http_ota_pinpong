/*
 * Copyright (C) 2017 XRADIO TECHNOLOGY CO., LTD. All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *    1. Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *    2. Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the
 *       distribution.
 *    3. Neither the name of XRADIO TECHNOLOGY CO., LTD. nor the names of
 *       its contributors may be used to endorse or promote products derived
 *       from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */


#include "sys/ZG_system.h"
static esp_err_t event_handler(void *ctx, ZG_system_event_t *event)
{
    uint8_t dhcpc_ret = DHCP_START;
    switch(*event)
    {
        case FACTORY_SETTING_EVENT:
            printf("event:factory-setting.\n");
            set_led_switch(MODE_ON);
            set_led_mode(MODE_RESET);
            set_led_speed(0x01);
            break;
        case CONNECTED_ROUTER_EVENT:
            printf("event:router connected");
            if(get_tcp_Client_switch() == 1)
            {
                int status = tcp_Client_start();
                printf("tcp client start status : %d\n\n",status);
                auto_report_init();                    
            }  
            connect_to_routing_Indicator();
            break;
            
        case RECONNECTED_ROUTER_EVENT:
            printf("evnet:reconnect to router.\n");
            USER_DBG("dhcp start!\n");
            do {            
                wl_os_mdelay(500);
                dhcpc_ret = dhcpc_start(0, 0);
            }while (DHCP_TIMEOUT == dhcpc_ret);
            
        case OTA_PROCESS_EVENT:
            printf("event:ota process\n");

            uint8_t progress = get_fota_progress();
            colort_struct_t color;
            color.W = 0;
            color.C = 0;
            color.ColorFlag = 0xf0;
            if(progress <= 100)
            { 
                set_led_mode(MODE_STATIC);
                color.R = 255 * progress / 100;
                color.G = color.R;
                color.B = color.R;
                Set_color(&color);
            }
            USER_DBG("remain heap[%d].", xPortGetFreeHeapSize());
            break;

        case OTA_SUCCESS_EVENT:
            printf("event:ota success\n");
            USER_DBG("remain heap[%d].", xPortGetFreeHeapSize());
            set_led_mode(MODE_STATIC);
            
            color.R = 0;
            color.G = 0;
            color.B = 255;
            Set_color(&color);
            wl_os_mdelay(3000);
            ota_reboot();
        break;

        case  OTA_FAIL_EVENT:
            printf("event:ota fail\n");
            set_led_mode(MODE_STATIC);
            color.R = 255;
            color.G = 255;
            color.B = 0;
            Set_color(&color);
            wl_os_mdelay(3000);
            ota_reboot(); 
        break;
    }
    return 0;
}


int user_main(void)
{
    USER_DBG("user_main.\n");
    ZG_device_info_conf("AK001-ZJ2131","35_04_20191109_ZG-ZW",0x35,7);
    ZG_event_loop_init(event_handler,NULL);
    ZG_system_start();
    ZG_watchdog_start();
    return 0;
}
