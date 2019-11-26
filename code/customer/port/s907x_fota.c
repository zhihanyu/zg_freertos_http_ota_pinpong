#include "s907x_fota.h"
#include "FreeRTOS.h"
#include "event_groups.h"
#include "httpc_api.h"
#include "ota.h"
#include "ota_hash.h"
#include "sys/ZG_system.h"
int g_received_len = 0;
char *g_pc_rsp_buf = NULL;
char *g_pc_flash_buf = NULL;
void *g_pv_httpc_cli = NULL;
ota_image_encap_header_t g_st_ota_header;

static EventGroupHandle_t  g_ota_start_event_group = NULL;

void s907x_zg_ota_init()
{
    g_ota_start_event_group   = xEventGroupCreate();
}

int s907x_zg_ota_process_is_start() 
{
    return xEventGroupWaitBits(g_ota_start_event_group, BIT0, false, true, 0);
}

void s907x_zg_ota_wait_watchdog_exit()
{
    xEventGroupWaitBits(g_ota_start_event_group, BIT1, false, true, S907X_MAX_OTA_SYNC_WAIT_TIME);
    wl_os_mdelay(500);
}

void s907x_zg_watchdog_exit()
{
    xEventGroupSetBits(g_ota_start_event_group, BIT1);
}

void s907x_zg_ota_process_start(void)
{
    USER_DBG("ota process start, related task should be exit\n");
    if(get_tcp_Client_connect_status(0) == BIT0)
    {
        tcp_Client_pause();//destory tcp client task
    }
    xEventGroupSetBits(g_ota_start_event_group, BIT0);
    s907x_zg_ota_wait_watchdog_exit();
    
}

int ota_get_image(const char *http_url)
{

    int ret = OTA_STATUS_OK;
    int httpc_ret = HTTPC_OK;
    int received_len = 0;

    memset(&g_st_ota_header, 0, sizeof(ota_image_encap_header_t));

    ota_init_flash_params();
    printf(OTA_START_TO_UPDATE);

    httpc_ret = ota_http_init_params((void *)&g_pc_flash_buf, (void *)&g_pc_rsp_buf, &g_pv_httpc_cli);
    if (HTTPC_OK != httpc_ret) 
    {
        printf("Init http parameters fail.");
        ret = OTA_STATUS_FAIL;
        goto exit;
    }

    httpc_ret = ota_http_request_new_fw(http_url, g_pv_httpc_cli);
    if (HTTPC_OK != httpc_ret) 
    {
        printf("Request new firmware fail.");
        ret = OTA_STATUS_FAIL;
        goto exit;
    }

    received_len = ota_http_download_new_fw(g_pc_flash_buf, g_pc_rsp_buf, g_pv_httpc_cli, (void *)&g_st_ota_header);

    if (received_len <= 0) {
        printf("Http download new firmware fail.");
        ret = OTA_STATUS_FAIL;
        goto exit;
    }
    g_received_len = received_len;
    return ret;

exit:
    ota_http_deinit_params(g_pc_flash_buf, g_pc_rsp_buf, g_pv_httpc_cli);
    return ret;

}

int ota_verify_image(void)
{
    int ret = OTA_STATUS_OK;
    int httpc_ret = HTTPC_OK;
    int ota_flash_addr = 0;

    ota_flash_addr = ota_get_flash_addr();
    // Calculate the crc value(crc of received data) equal to crc value in ota bin(the last 4bytes).
    httpc_ret = ota_check_image((void *)&g_st_ota_header, ota_flash_addr, g_received_len);
    if (httpc_ret < 0) 
    {
        printf("Image check fail, ret[%d],", httpc_ret);
        ret = OTA_STATUS_FAIL;
        goto exit;
    }

    ret = ota_write_boot_params((void *)&g_st_ota_header, g_received_len);
    if (AT_RET_OK != ret) {
        printf("Write boot param fail.");
        ret = OTA_STATUS_FAIL;
        goto exit;
    }

exit:
    ota_http_deinit_params(g_pc_flash_buf, g_pc_rsp_buf, g_pv_httpc_cli);
    return ret;
}

void ota_reboot(void)
{
    USER_DBG("OTA reboot.");
    HAL_NVIC_SystemReset();
}

void s907x_zg_ota_report_progress(int cur_progress)
{
    
    char progress_str[S907X_MAX_OTA_PROGRESS_STR_LEN] = {0};

    snprintf(progress_str, S907X_MAX_OTA_PROGRESS_STR_LEN, S907X_OTA_PROGRESS_STR_FORMAT, cur_progress);

    UDP_Send(progress_str, strlen(progress_str));
    USER_DBG("%s", progress_str);
    ZG_event_send(OTA_PROCESS_EVENT);
    fota_progress_send(cur_progress);
}

