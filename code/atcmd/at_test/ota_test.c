#include "s907x.h"
#include "system_types.h"
#include "string.h"
#include "ota_test.h"
#include "at_cmd.h"
#include "ota.h"

#if M_AT_TEST

#define OTA_FW_DB_MSG           "OTA_dual_ota2_firmware_20190917_1547.bin"

static int ota_show_config(int argc, char **argv)
{
    char *pc_bin_url = NULL;
    char *pc_bin_name = NULL;
    
    if(argc != 1) {
        AT_DBG_ERR("Usage: AT+OTA=\"%s\"", OTA_TEST_SHOW_CONFIG);
        return AT_RET_ERR;
    }

    pc_bin_url = ota_get_bin_url();
    pc_bin_name = ota_get_bin_name();
    at_rsp("ota bin url[%s], name[%s].\r\n", pc_bin_url, pc_bin_name);
    return AT_RET_OK;
}

static int ota_start(int argc, char **argv)
{
    int ret = AT_RET_ERR;
    if((argc < 2) || (3 < argc)) {
        at_rsp(AT_ERROR);
        return ret;
    }
    
    at_parse_param_str(&argv[1]);
    ota_set_bin_url(argv[1], strlen(argv[1]));

    if (argv[2]) {
        at_parse_param_str(&argv[2]);
        ota_set_bin_name(argv[2], strlen(argv[2]));
    }
    
    ret = ota_http_update_image();
    if (AT_RET_OK != ret) {
        at_rsp(AT_ERROR);
        at_rsp("Download ota bin fail.");
        return ret;
    }
       
    at_rsp(AT_OK);
    at_rsp(OTA_TEST_DOWNLOAD_OK);
    HAL_NVIC_SystemReset();
    return ret;
}   
               

static void ota_debug_message(int argc, char **argv)
{
    at_rsp("OTA firmware message %s.\r\n", OTA_FW_DB_MSG);
    return;
}

void ota_test(void *context)
{
    char  *argv[AT_SET_MAX_ARGC];
    int    argc;
    char  *item;
    
    at_cmd_t *cmd = (at_cmd_t *)context;

    argc = at_get_param(argv, cmd->set);
    if(!argc) {
        return;
    }
    at_parse_param_str(&argv[0]);
    item = argv[0];

    if(!strncmp(item, OTA_TEST_SHOW_CONFIG, strlen(OTA_TEST_SHOW_CONFIG))) {
        ota_show_config(argc, argv);
    }
    else if (!strncmp(item, OTA_TEST_START, strlen(OTA_TEST_START))) {
        ota_start(argc, argv);
    }
    else if (!strncmp(item, OTA_TEST_DEBUG_MSG, strlen(OTA_TEST_DEBUG_MSG))) {
        ota_debug_message(argc, argv);
    }
    else {
        at_rsp("Unkown command %s.", argv[0]);
        at_rsp(AT_ERROR);
    }
}

#endif

