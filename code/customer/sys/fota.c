
#include "sys/ZG_system.h"

static char ota_path[256];
static uint8_t ota_code[16];
static char host_name[50];
static uint16_t ota_port = 80;
static bool IS_ota_running = 0;

static TaskHandle_t xHandleTaskOta = NULL;
static xQueueHandle progress_queue = NULL;


int fota_conf(char *host , int host_len,uint16_t port,char *path,int path_len,char *code,int code_len)
{
  ota_port = port;
  memcpy(host_name,host,host_len);
  memcpy(ota_path,path,path_len);
  memcpy(ota_code,code,code_len);
  printf("configure -> %s %d %s %s \n", host_name, ota_port,ota_path,ota_code);
  return 0;
}

void fota_progress_send(uint8_t val)
{
  xQueueGenericSend(progress_queue,&val,0, queueSEND_TO_BACK);
}

uint8_t get_fota_progress()
{
   uint8_t progress =0;
   if(xQueueReceive(progress_queue,&progress,0) == pdTRUE){

       return progress;
   }else{
       return 255;
   }
}

bool Is_ota_start()
{
    return IS_ota_running;
}

void ota_task(void *arg)
{
    int return_type = 0;
    if(IS_ota_running == false)
    {
        /*fetch mac*/
        char mac_str[13] = {0};
        wifi_get_mac_info(mac_str);
        /*fetch aes encryption code*/
        char aes_code[11] = {0};
        uint8_t encry_code[32] = {0};
        AES_encrypt(ota_code,encry_code);
        memcpy(aes_code,encry_code + 22,10);
        printf("origin_code: %s \n", ota_code);
        printf("encry_code: %s \n", aes_code);

        char image_name[S907X_MAX_OTA_IMAGE_NAME_LEN] = "s907x_ota1_ota2_wlan.bin";
        const char GET_FORMAT[S907X_MAX_OTA_URL_FORMAT_LEN] = "http://%s:%d%s&filename=%s&checkcode={%s}&mac=%s";
        char *http_url = malloc(S907X_MAX_OTA_URL_LEN);
        if (NULL == http_url) 
        {
            printf("malloc fail.\n");
            return_type = -1;
        }
        snprintf(http_url, S907X_MAX_OTA_URL_LEN, GET_FORMAT, host_name, ota_port, ota_path, image_name, aes_code, mac_str);

        IS_ota_running = true;
        printf("url:%s\n",http_url);
        if (ota_get_image(http_url) != OTA_STATUS_OK) 
        { 
            printf("ota download error\n");
            return_type = -1;
        }
        //校验固件，此处选择不进行其他校验算法校验，调用该接口将更新 Boot config 信息 
        if (ota_verify_image()  != OTA_STATUS_OK) 
        { 
            printf("ota verify error\n");
            return_type = -1;
        } 
       //  IS_ota_running = false;
        if(return_type == -1)
        {
            UDP_Send("+ok=up_error\r",strlen("+ok=up_error\r"));
            ZG_event_send( OTA_FAIL_EVENT);
        }
        else
        {
            UDP_Send("+ok=up_success\r",strlen("+ok=up_success\r"));
            ZG_event_send( OTA_SUCCESS_EVENT);
        }

        free(http_url);
    }
    xHandleTaskOta = NULL;
    vTaskDelete(NULL); 
}

int zg_ota_start()
{
    USER_DBG("To create ota task, remain heap[%d].", xPortGetFreeHeapSize());

   if(progress_queue == NULL){

       progress_queue = xQueueCreate(1, 1);
   }

    s907x_zg_ota_process_start();
     
    xTaskCreate(&ota_task, "ota_task", FOTA_TASK_STACK_SZ, NULL, FOTA_TASK_PRIO, &xHandleTaskOta);
    USER_DBG("After create ota task, remain heap[%d].", xPortGetFreeHeapSize());
    return 0;
}
