#ifndef _S907X_FOTA_H_
#define _S907X_FOTA_H_

#include "s907x_wlan.h"

#define S907X_MAX_AES_CODE_LEN                  11
#define S907X_MAX_OTA_IMAGE_PATH_LEN            64
#define S907X_MAX_OTA_IMAGE_NAME_LEN            64
#define S907X_MAX_OTA_URL_FORMAT_LEN            64
#define S907X_MAX_OTA_HOSTNAME_LEN              50
#define S907X_MAX_OTA_URL_APPEND_LEN            10
#define S907X_MAX_OTA_URL_LEN                   (S907X_MAX_OTA_URL_FORMAT_LEN + S907X_MAX_IP_STR_LEN + S907X_MAX_AES_CODE_LEN + S907X_MAX_OTA_IMAGE_PATH_LEN + S907X_MAX_OTA_HOSTNAME_LEN + S907X_MAX_OTA_URL_APPEND_LEN)

#define S907X_MAX_OTA_SYNC_WAIT_TIME            5000

#define S907X_MAX_OTA_PROGRESS_STR_LEN          32
#define S907X_OTA_PROGRESS_STR_FORMAT           "+ok=%d"

#define OTA_STATUS_OK                           HAL_OK
#define OTA_STATUS_FAIL                         HAL_ERROR

void s907x_zg_ota_init();
void s907x_zg_ota_process_start(void);
int s907x_zg_ota_process_is_start();
void s907x_zg_watchdog_exit();

int ota_get_image(const char *http_url);
int ota_verify_image(void);
void ota_reboot(void);

void s907x_zg_ota_report_progress(int cur_progress);

#endif

