#ifndef KV_CONFIG_H
#define KV_CONFIG_H

#include "system_config.h"

#define KV_MODULE_STR_HDR_LENGTH    2
#define KV_MODULE_INT_LENGTH        strlen("65536")
#define KV_MODULE_KEY_LENGTH        KV_MODULE_STR_HDR_LENGTH + KV_MODULE_INT_LENGTH + 1


#endif

