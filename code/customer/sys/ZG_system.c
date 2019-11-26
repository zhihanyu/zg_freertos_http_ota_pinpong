

#include "sys/ZG_system.h"

static char MCU_ver[30];
static char Device_ver[40];
static char Device_ID[30];

static uint8_t light_protocol_ver;

static uint8_t device_code;

static char s_event_init_flag = 0;
static void *s_event_queue = NULL;
static ZG_system_event_cb_t s_event_handler_cb = NULL;
static void *s_event_ctx = NULL;

esp_err_t ZG_device_info_conf(char *dev_id,char * ver,uint8_t dev_code,uint8_t pro_ver)
{
    if(dev_code == 0 || dev_id == NULL || ver == NULL)
    {
        return ESP_FAIL;
    }
    strncpy(Device_ID,dev_id,strlen(dev_id));
    strncpy(Device_ver,ver,strlen(ver));
    device_code = dev_code;
    light_protocol_ver = pro_ver;
    printf("\n");
    printf("device id -> %s\n",Device_ID);
    printf("device version -> %s\n",Device_ver);
    printf("device code -> %02X \n",device_code);
    printf("device protocol version -> %d\n",light_protocol_ver);
    printf("\n");
    return ESP_OK;  
}

uint8_t ZG_get_device_protocol_version()
{
    return light_protocol_ver;
}

void ZG_device_mcu_ver_conf(char * ver)
{
    strncpy(MCU_ver,ver,strlen(ver));
}

char *ZG_get_device_version()
{
    return Device_ver;
}

char *ZG_get_device_id()
{
	return Device_ID;
}

char *ZG_get_mcu_version()
{
	return MCU_ver;
}

uint8_t ZG_get_device_code()
{
	return device_code;
}

#define ZG_MONITOR_TIMER_PERIOD  1000

static TimerHandle_t monitor_timer; 


static void ZG_monitor_cb(void *arg)
{
    printf("mem %d\n", wl_get_freeheapsize());
}

 
static void ZG_monitor()
{
    monitor_timer = xTimerCreate("timer_monitor", ZG_MONITOR_TIMER_PERIOD, pdTRUE,(void *)0, ZG_monitor_cb);

    xTimerStart(monitor_timer, 0);
}

static void zg_watchdog(void *argv)
{
    wdg_hdl_t wdg_hdl;

    memset(&wdg_hdl, 0, sizeof(wdg_hdl_t));
    wdg_hdl.time_ms = ZG_WATCHDOG_TIMEOUT;
    
    s907x_hal_wdg_init(&wdg_hdl);
    s907x_hal_wdg_start(&wdg_hdl);

    USER_DBG("watchdog start.");

    while(1) {
        if (s907x_zg_ota_process_is_start() == pdTRUE){
            s907x_hal_wdg_deinit(&wdg_hdl);
            s907x_zg_watchdog_exit();
            printf("ota process begin, exit watchdog.\n\n");
            break;
        }
        s907x_hal_wdg_refresh(&wdg_hdl);
        USER_DBG("Refresh watchdog.");
        wl_os_mdelay(ZG_WATCHDOG_FRESH_INTER);
    }
    
    vTaskDelete(NULL); 
}

void ZG_watchdog_start()
{    
    xTaskCreate(&zg_watchdog, "zg_watchdog", ZG_WATCHDOG_TASK_STACK_SZ, NULL, ZG_WATCHDOG_TASK_PRIO, NULL);
}

void ZG_system_start()
{
    ZG_store_init();
    light_init();
    // wl_os_mdelay(500);
    s907x_zg_ota_init();

    tcp_Client_init(); 
    WIFI_Init();

    tcp_server_task_init();
    udp_task_init();
    AES_Data_Func_init();
    ZG_monitor();
}

static esp_err_t ZG_event_post_to_user(ZG_system_event_t *event)
{
    if (s_event_handler_cb) 
    {
        return (*s_event_handler_cb)(s_event_ctx, event);
    }
    return ESP_OK;
}

static void ZG_event_loop_task(void *pvParameters)
{
    while (1)
    {
        ZG_system_event_t evt;
        if (xQueueReceive(s_event_queue, &evt, portMAX_DELAY) == pdPASS) 
        {
            int ret = ZG_event_post_to_user(&evt);
            if (ret != ESP_OK)
            {
                printf( "post event to user fail!");
            }
        }
    }
}

esp_err_t ZG_event_send(ZG_system_event_t event)
{
    if (s_event_queue == NULL)
    {
        printf( "Event loop not initialized via esp_event_loop_init, but esp_event_send called");
        return  ESP_FAIL;
    }
    int ret = xQueueGenericSend(s_event_queue, &event, 0, queueSEND_TO_BACK);
    if (ret != pdPASS) 
    {
        return ESP_FAIL;
    }
    return ESP_OK;
}

QueueHandle_t ZG_event_loop_get_queue(void)
{
    return s_event_queue;
}

esp_err_t ZG_event_loop_init(ZG_system_event_cb_t cb, void *ctx)
{
    if (s_event_init_flag) 
    {
        return ESP_FAIL;
    }
    s_event_handler_cb = cb;
    s_event_ctx = ctx;
    s_event_queue = xQueueCreate(32, sizeof(ZG_system_event_t));
    if(s_event_queue == NULL)
        return  ESP_FAIL;
    xTaskCreate(ZG_event_loop_task, "ZG_event_loop_task", ZG_EVENT_LOOP_TASK_STACK_SZ, NULL, ZG_EVENT_LOOP_TASK_PRIO, NULL);

    s_event_handler_cb = cb;
    s_event_ctx = ctx;
    s_event_init_flag = 1;
    return ESP_OK;
}
