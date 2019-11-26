#ifndef SYSTEM_TASK_H
#define SYSTEM_TASK_H



//bigger is higer
#define IDLE_PRIORITY				(0U)
#define MAX_PRIORITIES              (11U)

#define WLAN_INIT_PRIO			    (IDLE_PRIORITY + 7)	//7 for init/entry task
#define WLAN_INIT_STACK_SZ		    (4096)	
//user thread
#define APP_PRIO_MIN                (IDLE_PRIORITY + 2)  // 2  3  4 for normal application code
#define APP_PRIO_NORAML             (IDLE_PRIORITY + 3)
#define APP_PRIO_MAX                (IDLE_PRIORITY + 4)

//AT
#define AT_TASK_PRIO				(IDLE_PRIORITY + 1)	// 1 for at cmd 
#define AT_TASK_STACK_SZ			(4096)
//start
#define MAIN_TASK_PRIO				(WLAN_INIT_PRIO)	
#define MAIN_TASK_STACK_SZ			(4096) 

#define ACTION_TASK_PRIO            (APP_PRIO_MIN)
#define ACTION_TASK_STACK_SZ        (512)

//sdk demo app task
#define HTTP_CLIENT_PRIO            (APP_PRIO_MIN)
#define HTTP_CLIENT_STACK_SZ        (512)

#define MAIN_UART_RX_PRIO			(APP_PRIO_MIN)	
#define MAIN_UART_RX_STACK_SZ		(512) 

#define PING_TASK_PRIO              (APP_PRIO_MIN)
#define PING_TASK_STACK_SZ          (512)   

#define IPERF_UDP_TASK_PRIO         (APP_PRIO_MIN)
#define IPERF_UDP_TASK_STACK_SZ     (512)   

#define MQTT_TEST_PRIO              (APP_PRIO_MIN)
#define MQTT_TEST_STACK_SZ          (4096)











#endif


