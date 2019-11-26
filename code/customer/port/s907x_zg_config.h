#ifndef _S907X_ZG_CONFIG_H_
#define _S907X_ZG_CONFIG_H_

#include "s907x.h"
#include "system_config.h"

#define ZG_PWM_ENABLE                       0
#define ZG_NET_SERVER_ENBALE                0
#define ZG_OTA_ENABLE                       0
#define ZG_OTA_REBOOT_ENABLE                0
#define ZG_OTA_TASK_ENABLE                  0

#define S907X_NET_SERVER_ENABLE             1
#define S907X_OTA_TASK_ENABLE               1
#define S907X_BUILD_FOR_ZG                  ZG_BUILD
#define S907X_STORE_USING_KV                1

#define S907X_MAX_TCP_SERVER_ACCEPT_NUM     4   /* NUM_SOCKETS - 4 */

#define ZG_PWM_PRESCALER_SET              0
#define ZG_PWM_PERIOD_SET                 40000
#define ZG_PWM_CHANNEL_R                  5        //pwm5 io22
#define ZG_PWM_CHANNEL_G                  6        //pwm6 io20
#define ZG_PWM_CHANNEL_B                  0        //pwm0 io3 //swd
#define ZG_PWM_CHANNEL_W                  1        //pwm1 io4 //swd
#define ZG_PWM_CHANNEL_C                  2        //pwm2 io5  

//#define ZG_PWM_CHANNEL_R                  1        //pwm1 io4 //swd
//#define ZG_PWM_CHANNEL_B                  2        //pwm2 io5





#define ZG_PWM_POLARITY_ZERO              0

#define PWM_GROUP1_CH2          ZG_PWM_CHANNEL_R
#define PWM_GROUP1_CH3          ZG_PWM_CHANNEL_G
#define PWM_GROUP2_CH4          ZG_PWM_CHANNEL_B
#define PWM_GROUP2_CH5          ZG_PWM_CHANNEL_W
#define PWM_GROUP3_CH6          ZG_PWM_CHANNEL_C



#define WLAN_MODE_HOSTAP        S907X_MODE_AP
#define WLAN_MODE_STA           S907X_MODE_STA
#define HAL_Status              hal_status_e
#define malloc                  wl_malloc
#define free                    wl_free




#define LIGHT_TASK_PRIO                 (APP_PRIO_MAX)
#define LIGHT_TASK_STACK_SZ             (1024)

#define LIGHT_REMOTE_TASK_PRIO          (APP_PRIO_MIN)
#define LIGHT_REMOTE_TASK_STACK_SZ      (512)

#define STORE_TASK_PRIO                 (APP_PRIO_MIN)
#define STORE_TASK_STACK_SZ             (512)

#define TCP_SERVER_TASK_PRIO            (APP_PRIO_MAX)
#define TCP_SERVER_TASK_STACK_SZ        (1024 * 2)

#define TCP_CLIENT_TASK_PRIO            (APP_PRIO_MAX)
#define TCP_CLIENT_TASK_STACK_SZ        (1024 * 2)

#define TCP_CLIENT_SEND_TASK_PRIO       (APP_PRIO_NORAML)
#define TCP_CLIENT_SEND_TASK_STACK_SZ   (1024)

#define UDP_SERVER_TASK_PRIO            (APP_PRIO_MAX)
#define UDP_SERVRE_TASK_STACK_SZ        (1024)

#define TIMER_CLOCK_TASK_PRIO           (APP_PRIO_MIN)
#define TIMER_CLOCK_TASK_STACK_SZ       (512)

#define AUTO_REPORT_TASK_PRIO           (APP_PRIO_MIN)
#define AUTO_REPORT_TASK_STACK_SZ       (512)

#define FOTA_TASK_PRIO                  (APP_PRIO_NORAML)
#define FOTA_TASK_STACK_SZ              (2048)

#define ZG_EVENT_LOOP_TASK_PRIO         (APP_PRIO_MAX)
#define ZG_EVENT_LOOP_TASK_STACK_SZ     (1024)

#define ZG_WATCHDOG_TASK_PRIO           (APP_PRIO_NORAML)
#define ZG_WATCHDOG_TASK_STACK_SZ       (512)

//test  macro
#define ZG_WIFI_BLOCK_TIME               20000

#endif

