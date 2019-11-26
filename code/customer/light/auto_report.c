
#include "sys/ZG_system.h"

static bool Active_query_flag = 0;

static report_type_t report_info;
static bool color_update = 0;
static bool Is_music_report;

static int StartT,StartE,StartA;

static TaskHandle_t xHandleTaskReport = NULL;

void  set_report_reson(report_reson_t reson)
{
    report_info.reason = reson;
    if(reson == ACTIVE_QUERY)
    {
        Active_query_flag = 1;
    }
}

void  set_color_updated_flag(bool val)
{
    color_update = val;
}


static void report_task(void *arg)
{
    for(;;)
    {
        if(get_wifi_connection_status(portMAX_DELAY) == BIT0)
        {
            if(get_tcp_Client_connect_status(portMAX_DELAY) == pdTRUE)
            {
                bool update_flag = 0;
                vTaskDelay(1000 / portTICK_RATE_MS);

                /*音乐模式停止三秒后，自动上报当前状态*/
                if(get_led_mode() == 0x62)
                {
                    if(report_info.r != get_red_value())
                    {
                        StartA = 0;
                        StartT = 0;
                        StartE = 0;
                        report_info.r = get_red_value();
                        Is_music_report = 1;
                    }
                    if(report_info.g != get_green_value())
                    {

                        StartA = 0;
                        StartT = 0;
                        StartE = 0;
                        report_info.g = get_green_value();
                        Is_music_report = 1;
                    }
                    if(report_info.b != get_blue_value())
                    {
                        StartA = 0;
                        StartT= 0;
                        StartE = 0;
                        report_info.b = get_blue_value();
                        Is_music_report = 1;
                    }
                    if(report_info.r == get_red_value() && report_info.g == get_green_value() && report_info.b == get_blue_value())
                    {
                        if(Is_music_report == 1)
                        {
                            StartE = ZG_system_time() - StartT;
                            StartT = ZG_system_time();
                            StartA += StartE;
                            if(StartA/1000 >= 10)
                            {
                                StartA = 0;
                            }
                            printf("%d\n",StartA);
                            if(StartA / 1000 >= 3)
                            {
                                Is_music_report = 0;
                                StartA = 0;
                                update_flag = 1;
                                printf("(report):music done\n");
                                if( get_red_value() == 0 && get_green_value() == 0 && get_blue_value() == 0)
                                {
                                   set_led_switch(MODE_OFF);
                                }
                            }
                        }
                    }
                }
                if(report_info.switch_status != get_led_switch())
                {
                    report_info.switch_status = get_led_switch();
                    printf("(report):switch changed -> %02X\n",get_led_switch());
                    update_flag = 1;
                }
                 //如果日出日落途中，或者是延时开关，则等待过程结束后才上报
                if(report_info.light_model != get_led_mode())
                {
                    printf("(report):light model changed %02X -> %02X\n",report_info.light_model,get_led_mode());
                    report_info.light_model = get_led_mode();
                    update_flag = 1;
                }
                if(report_info.speed != get_led_speed())
                {
                    printf("(report):light speed changed %d -> %d\n",report_info.speed,get_led_speed());
                    report_info.speed = get_led_speed();
                    update_flag = 1;
                }
                if((get_led_mode() == 0x61 && color_update )|| update_flag == 1)
                {
                    color_update = 0;
                    if(report_info.color_flag !=  get_Color_flag())
                    {
                        report_info.color_flag =  get_Color_flag();
                        printf("(report):color flag changed -> %02X\n",get_Color_flag());
                        update_flag = 1;
                    }
                    if(report_info.w != get_warm_value())
                    {
                        printf("(report):Warm  [%d] -> [%d]\n",report_info.w,get_warm_value());
                        report_info.w = get_warm_value();
                        update_flag = 1;
                    }
                    if(report_info.c != get_cold_value())
                    {
                        printf("(report):Cold  [%d] -> [%d]\n",report_info.c,get_cold_value());
                        report_info.c = get_cold_value();
                        update_flag = 1;
                    }
                    if(report_info.r != get_red_value())
                    {
                        printf("(report):red   [%d] -> [%d]\n",report_info.r,get_red_value());
                        report_info.r = get_red_value();
                        update_flag = 1;
                    }
                    if(report_info.g != get_green_value())
                    {
                        printf("(report):green [%d] -> [%d]\n",report_info.g,get_green_value());
                        report_info.g = get_green_value();
                        update_flag = 1;
                    }
                    if(report_info.b != get_blue_value())
                    {
                        printf("(report):blue  [%d] -> [%d]\n",report_info.b,get_blue_value());
                        report_info.b = get_blue_value();
                        update_flag = 1;
                    }
                }
                /*   上报  F0  F1  F2  F3  F4  02  1 1 高 低 【开/关机】+【模式值】+ 【速度值】＋【红色数据】＋【绿色数据】＋【蓝色数据】+  【暖白数据】+【冷白数据】+ 【静态标示】+ 【接线类型】+【版本号】+【上报原因】
                * */
                if(update_flag || Active_query_flag)
                {
                    uint8_t send[13] = {0};
                    uint8_t send_data[40] = {0};
                 
                    send[0] = get_led_switch();
                    send[1] = 0x00;
                    send[2] = get_led_mode();
                    send[3] = get_led_speed();
                    send[4] = get_red_value();
                    send[5] = get_green_value();
                    send[6] = get_blue_value();
                    send[7] = get_warm_value();
                    send[8] = get_cold_value();
                    send[9] = get_Color_flag();
                    send[10] = get_color_output_sort() << 4 | 0x03;;
                    send[11] = ZG_get_device_protocol_version();
                    send[12] = report_info.reason;
                    printf("___________device report__________\n");
                    int len = reponse_data_pack(PACK_DEICE_REPORT,BYTE_FORMAT,send,send_data,13);
                    tcp_Client_send(send_data,len);
   
                    Active_query_flag = false;
                    printf("__________________________________\n");
                }
            }
        }
    }
    vTaskDelete(NULL);
}


void auto_report_init()
{
    if(xHandleTaskReport == NULL)
        xTaskCreate(report_task, "report_task", AUTO_REPORT_TASK_STACK_SZ, NULL, AUTO_REPORT_TASK_PRIO, &xHandleTaskReport);
}