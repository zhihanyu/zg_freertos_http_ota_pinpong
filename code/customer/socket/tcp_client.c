
#include "sys/ZG_system.h"

static uint32_t severport = 8816;
static char hostname[64];
static int sockfd = -1;

static EventGroupHandle_t cloud_connect_status_event_group;
static EventGroupHandle_t start_event_group;

static uint8_t tcp_switch = 0;

static xQueueHandle monitor_queue;
static xQueueHandle tcp_client_queue;

static SemaphoreHandle_t  kill_tcp_shr;
static SemaphoreHandle_t  reconnect_shr;

static bool tcp_connect_flag = false;

void tcp_Client_task_init();
void tcp_Client_build_login_pkt(void);

static TaskHandle_t xHandleTasktcp = NULL;
static TaskHandle_t xHandleTaskSend = NULL;

typedef enum{
  monitor_event_none = 0,
  monitor_connect_packet = 1,
  monitor_heatbeat_packet,
  monitor_response

}monitor_event_t;

static char mac_str_encryption[12] = {0};
static char mac_str[13] = {0};

uint32_t PowerOn_amount,Login_amount;
static uint8_t HB_time = 60;

static int wifi_disconnect_time,wifi_disconnect_count,wifi_dis_flag;

uint32_t WIFI_disconnect_count_get()
{
	return wifi_disconnect_count;
}

/**
 * @brief 开启tcp client连接
 * @author hings
 * @note 
 * @param[in] None
 *
 * @return none
 */
int tcp_Client_start()
{
    //USER_DBG("lowpower entry...\n");
    //s907x_wlan_enable_sleep(1);

	tcp_Client_build_login_pkt();
	
	printf("tcp switch[%d], hostname[%s].\n", tcp_switch, hostname);
	if(tcp_switch && strlen(hostname) != 0){

		printf("tcp client start\n");
		xEventGroupSetBits(start_event_group, BIT0); 
		tcp_Client_task_init();
		return 0;

	}else{
		if(tcp_switch == 0){

			printf("tcp client switch is off\n");
			return -1;
		}else{

			printf("read error -> host : %s len : %d port %d\n",hostname,strlen(hostname),severport);
			return -2;
		}
	}
}

int tcp_Client_stop()
{

   printf("tcp client stop\n");

   xEventGroupClearBits(start_event_group, BIT0);
   xSemaphoreGive(kill_tcp_shr);//delete tcp
   
   tcp_switch = 0;
   ZG_data_save(CLOUD_SWITCH_STORE,&tcp_switch);

   return 0;
}  

int tcp_Client_pause()
{

   printf("tcp client pause\n");

   xEventGroupClearBits(start_event_group, BIT0);
   xSemaphoreGive(kill_tcp_shr);//delete tcp

   return 0;
}  

void tcp_client_deinit()
{
   if(tcp_switch == 1){

	 printf("tcp client switch turn off \n");
	 tcp_switch = 0;
	 ZG_data_save(CLOUD_SWITCH_STORE,&tcp_switch);

   }else{

	 printf("tcp client already turn off \n");
   }
}


extern uint8_t WF_SSID[33];
static char login_packet[200];
int tcp_Client_init()
{ 

  kill_tcp_shr   = xSemaphoreCreateBinary();  
  reconnect_shr  = xSemaphoreCreateBinary(); 
  cloud_connect_status_event_group = xEventGroupCreate();
  start_event_group = xEventGroupCreate();
  

  /*read device Power-on times*/
  uint8_t tmp[2] = {0};
  ZG_data_read(ST1_STORE,tmp);
  PowerOn_amount = tmp[0] << 8 | tmp[1];
  PowerOn_amount++;
  printf("power on count : %d\n",PowerOn_amount);

  tmp[0] = PowerOn_amount >> 8;
  tmp[1] = PowerOn_amount;
  ZG_data_save(ST1_STORE,tmp);

  /*read cloud switch*/
  ZG_data_read(CLOUD_SWITCH_STORE,&tcp_switch);
  if(tcp_switch > 1){

	  tcp_switch = 0;
  }
  /*read cloud remote port*/
  ZG_data_read(PORT_STORE,tmp);
  severport = tmp[0] << 8 | tmp[1];
  /*read host domain name*/
  uint8_t host_tmp[65] = {0};
  ZG_data_read(HOST_STORE,host_tmp);

  if(host_tmp[0] > 64){

	host_tmp[0] = 64;
  }
  memcpy(hostname,host_tmp + 1,host_tmp[0]);
  return 0;
} 

void tcp_Client_build_login_pkt(void)
{    
	uint8_t aes_dest[32]={0};
	/*Compute the fields encrypted by the mac address*/
	wifi_get_mac_info(mac_str);
	AES_encrypt((uint8_t *)mac_str,aes_dest);
	memcpy(mac_str_encryption,aes_dest + 22,10);
	
	printf("tcp client : init finished!|host : %s | port : %d | AES string : %s |\n",hostname,severport,mac_str_encryption);
	/*cover to login packet*/
	sprintf(login_packet,"\"mac\":\"%s\",\"mid\":\"%s\",\"aes\":\"%s\",\"ver\":%d,\"lver\":\"%s\",\"type\":\"%x\"",
		  mac_str,
		  ZG_get_device_id(),
		  mac_str_encryption,
		  ZG_get_device_protocol_version(),
		  ZG_get_device_version(),
		  ZG_get_device_code()
	  );

}
int set_tcp_Client_conf(char *host,int len,uint32_t port)
{
	
	if(host == NULL || port == 0){

		return -1;
	}else{

		printf("configure tcp client host : %s (%d) port :%d\n",host,len,port);


		memset(hostname,0,sizeof(hostname));
		

		tcp_switch = 1;
		ZG_data_save(CLOUD_SWITCH_STORE,&tcp_switch);
		 /*保存域名和端口在flash*/
		if(severport != port ){

		   severport = port;
		   uint8_t tmp[2] = {0};
		   tmp[0] = port >> 8;
		   tmp[1] = port;
		   ZG_data_save(PORT_STORE,tmp);  
		}
		if(memcmp(hostname,host,len) != 0){
	 
		   uint8_t hosttmp[65] = {0};
		   strncpy(hostname,host,len);
		   hosttmp[0] = len;
		   memcpy(hosttmp + 1,host,len);
		   ZG_data_save( HOST_STORE , hosttmp);
		}

		return 0;
	}
}
void get_tcp_Client_conf(char *host,int *len,uint32_t* port)
{
	int host_len = 0;
	strncpy(host,hostname,strlen(hostname));
	host_len = strlen(host);
	*len = host_len;
	*port = severport;
}

/**
 * @brief 查询tcp client连接状态
 * @author hings
 * @note 
 * @param[in] None
 *
 * @return BIT0 : success   else: fail
 */
int get_tcp_Client_connect_status(unsigned int xTicksToWait)
{

  return xEventGroupWaitBits(cloud_connect_status_event_group, BIT0,false, true, xTicksToWait);

}
int get_tcp_Client_switch()
{
	return tcp_switch;
}

int tcp_Client_send(unsigned char *pdata,int len)
{
  tcp_content_send_t send_tmp;
  memset(&send_tmp,0,sizeof(tcp_content_send_t));
  send_tmp.content_len = len;
  memcpy(send_tmp.content,pdata,len);
  printf("tcp client send %d byte...\n",len);
  return xQueueGenericSend(tcp_client_queue,&send_tmp,0, queueSEND_TO_BACK);
} 

void send_monitor_event(monitor_event_t event)
{
	xQueueGenericSend(monitor_queue,&event,0, queueSEND_TO_BACK);
}

 /**
 * @brief 发送连接包到云服务器上
 * @author hings
 * @note 
 * @param[in] None
 *
 * @return none
 */

static int send_login_packet()
{

  char *Heartbeat_packet = NULL;
  char *json = NULL;


  json = malloc(256);
  Heartbeat_packet = malloc(256);

  char ip_str[20] = {0};
  wifi_info_t wifi_msg;

  wifi_get_ip_info(ip_str);

  memset(&wifi_msg,0,sizeof(wifi_info_t));
  wifi_get_conf_msg(&wifi_msg); 

  Login_amount++;
  sprintf(json,"{\"ssid\":\"%s\",\"ip\":\"%s\",%s,\"st1\":%d,\"st2\":%d,\"wfdc\":%d,\"rssi\":%d}" ,
					wifi_msg.ssid,
					ip_str,
					login_packet,
					PowerOn_amount,
					Login_amount,
					wifi_disconnect_time,
					get_ap_rssi()
  );
	
  int len = reponse_data_pack(PACK_CONNECT_REQUESE,JSON_FORMAT,(uint8_t*)json,(uint8_t*)Heartbeat_packet,strlen(json));

  int send_ret =  tcp_Client_send((uint8_t*)Heartbeat_packet,len);
  send_monitor_event(monitor_connect_packet);
	
  free(Heartbeat_packet);
  free(json);
  return send_ret;
}
 /**
 * @brief 发送心跳包
 * @author hings
 * @note 
 * @param[in] None
 *
 * @return none
 */

static int send_heartbeat_packet()
{

	uint8_t* Heartbeat_packet=(uint8_t*)malloc(100);

	int len = reponse_data_pack(PACK_PING_CHECK,JSON_FORMAT,(uint8_t*)mac_str,Heartbeat_packet,strlen(mac_str));

	int send_ret =  tcp_Client_send(Heartbeat_packet,len);

	send_monitor_event(monitor_heatbeat_packet);

	free(Heartbeat_packet);
	return send_ret;
   
}
 
 /**
 * @brief 定时器：2分钟发送一次心跳包
 * @author hings
 * @note 
 * @param[in] None
 *
 * @return none
 */
static uint32_t hbSt,hbEt,hbAt;
void Heartbeat_keepalive()
{

  hbEt = ZG_system_time() - hbSt;
  if(hbEt > 6000) hbEt = 0;
	   
  hbSt = ZG_system_time();
  hbAt += hbEt;

  if(hbAt/1000 >= HB_time){

	 hbAt = 0;
	 send_heartbeat_packet();
  }

}

static int monitor_fail_count = 0;
static monitor_event_t  status = monitor_event_none;
static monitor_event_t  monitor_type = monitor_event_none;
uint32_t St = 0,Et = 0,At = 0;
static int tcp_client_connection_monitor(void)
{
  /****************************monitor task******************************/
  if(xQueueReceive(monitor_queue, &status, 0) == pdTRUE) {

	  monitor_type = status;
	  if(monitor_type == monitor_response){

		  printf("tcp client:server response success\n");
		  wifi_disconnect_time = 0;
		  monitor_fail_count = 0;
	  }
  }else{

	  if(monitor_type == monitor_connect_packet || monitor_type == monitor_heatbeat_packet){

		  Et = ZG_system_time() - St;
		  if(Et > 6000) Et = 0;
		 
		  St = ZG_system_time();
		  At += Et;

		  if(At / 1000 >= 5){

			  At = 0;
			  monitor_fail_count++;

			  if(monitor_fail_count <= 3){

				  if(monitor_type == monitor_connect_packet){

					  printf("tcp client:timeout resend connect packet\n");
					  send_login_packet();
					  debug_record_login_timeout();
				  }else if(monitor_type == monitor_heatbeat_packet){

					  printf("tcp client:timeout resend hearbeat packet\n");
					  send_heartbeat_packet();
					  debug_record_heartbeat_timeout();
				  }
			  }else{

				  monitor_fail_count = 0;                           
				  printf("tcp client:reconnect\n");
				  
				  return -1;
			  }
				 
		  }
		  
	  }
  }
  return 0;
}

void tcp_client_task(void *arg)
{

#define SERVER_IP "192.168.0.140"
//192.168.0.133:9915
//#define SERVER_IP "192.168.0.133"
#define SERVER_PORT 8815
#define RECV_BUF_SIZE 512
#define SEND_BUF_SIZE 512
	uint8_t connect_fail_count = 0;
	int ret = 0;
	while(1){
	
	if(xSemaphoreTake(kill_tcp_shr, 0) == pdTRUE){

		printf("tcp client:stop the socket\n\n");
		xEventGroupWaitBits(start_event_group, BIT0,false, true, portMAX_DELAY);
	}

	if(get_wifi_connection_status(1000 / portTICK_RATE_MS) == BIT0){


		wifi_dis_flag = 0;//for count wifi drop line

		int  iResult = 0;
		int fd = -1;

		struct addrinfo *addrInfoList = NULL;
		struct addrinfo *cur = NULL;
		struct in_addr *addr;
		bool Is_socket_error = false;

		const struct addrinfo hints = {
			.ai_family = AF_INET,
			.ai_socktype = SOCK_STREAM,
		};

		char remotePort[6] = {0};
	  
		sockfd = -1;
		sprintf(remotePort, "%u", severport);

		printf("connect to %s(%d) : %s\n",hostname,strlen(hostname),remotePort);
		debug_record_connect_count();

		if ((ret = getaddrinfo(hostname, remotePort, &hints, &addrInfoList)) != 0) {

			printf("DNS error :%d res=%p\n", ret, addrInfoList);
			Is_socket_error = true;
			debug_record_connect_code(origin_code);
			debug_record_dns_code(ret);
			goto socketError;
		}
		debug_record_dns_code(ret);
		addr = &((struct sockaddr_in *)addrInfoList->ai_addr)->sin_addr;
		printf( "DNS lookup succeeded. IP:%s\n", inet_ntoa(*addr));

		for (cur = addrInfoList; cur != NULL; cur = cur->ai_next) {

			if (cur->ai_family != AF_INET) {

				printf("socket type error\n");
				sockfd = -1;
				Is_socket_error = true;
			}

			fd = socket(cur->ai_family, cur->ai_socktype, cur->ai_protocol);
			debug_record_connect_code(fd);
			if (fd < 0) {

				printf("tcp client:create socket error\n\n");
				sockfd = -1;
				Is_socket_error = true;
			}
			int connect_code = connect(fd, cur->ai_addr, cur->ai_addrlen);
			debug_record_connect_code(connect_code);
			if (connect_code == 0) {

				sockfd = fd;
				break;
			}
			close(fd);
		}

		socketError:
		
		freeaddrinfo(addrInfoList);
		if ((sockfd < 0) || (Is_socket_error == true)) {

			printf("tcp client:fail to establish tcp\n\n");
			int tcp_list_amount = get_tcp_amaount();
			printf("tcp amount : %d \n",tcp_list_amount);
			if(tcp_list_amount == 0){
			  
			  connect_fail_count++;
			  if(connect_fail_count >= 12 * 5){// 5 minute

				  connect_fail_count = 0;
				  wifi_STA_Start();
			  }
			}
			vTaskDelay(3000 / portTICK_RATE_MS);//reconnect time internal
			continue;
		} 
		connect_fail_count = 0;
		tcp_connect_flag = true;
		printf("success to establish tcp, fd = %d\n\n", sockfd);
		send_login_packet();
	  
		fd_set readset,errorset ;

		hbAt = 0; 

		struct timeval timeout;
		timeout.tv_sec = 3;
		timeout.tv_usec = 0;

		unsigned char* send_buf = (unsigned char*)malloc(SEND_BUF_SIZE);
		unsigned char* recv_buf = (unsigned char*)malloc(SEND_BUF_SIZE);

		while  (1) {

			if(xSemaphoreTake(kill_tcp_shr, 0) == pdTRUE){
			  
				  if(send_buf != NULL){

					  free(send_buf);
					  send_buf = NULL;
				  }
				  if(recv_buf != NULL){
					  
					  free(recv_buf);
					  recv_buf = NULL;
				  }
				 goto stop_connect;// kill socket
			}
		   if(xSemaphoreTake(reconnect_shr, 0) == pdTRUE){

				  break;
			}
			Heartbeat_keepalive();
			if(tcp_client_connection_monitor() == -1){

			  break;
			}

			if(get_wifi_connection_status(0) != BIT0){

				printf("wifi disconnect -> exit\n");
				break;
			}
			/****************************************************************************/
		   // printf("before set zero readset address %x val %x\n",&readset,readset);
			FD_ZERO( &readset);
			FD_ZERO(&errorset);
		   // printf("before set socket readset address %x val %x socket %d\n",&readset,readset,sockfd);
			FD_SET(sockfd,  &readset);        
			errorset = readset;
			iResult = select(sockfd + 1, &readset, (fd_set *)0, &errorset, &timeout);

			if(iResult == -1){
				printf("tcp client:TCP client select failed -> reconnect\n");

				break;  // reconnect
			}
			if(iResult == 0){
				//printf("tcp client:TCP client A select timeout occurred\n");
				continue;
			}
			
			if(FD_ISSET(sockfd , &errorset)){    // error happen

				printf("tcp client:select error_happend !!!!\n");
				break;           
			
			}
			
			if(FD_ISSET(sockfd , &readset)){    // readable
		
				memset(recv_buf,0,RECV_BUF_SIZE);
				int recv_ret  =  recv(sockfd, recv_buf, RECV_BUF_SIZE, 0);
				if(recv_ret > 0){
					
					printf("rev:\n");
					for(int i = 0;i < recv_ret ;i++){

						printf("%02X ",recv_buf[i]);
					}
					printf("\n\n");
					if(recv_buf[0] == 0xa0 && recv_buf[1] == 0xa1 && recv_buf[2] == 0xa2 && recv_buf[3] == 0xa3 && recv_buf[4] == 0xa4){

						send_debug_info(1);
					}
					else if(recv_buf[0] == 0xF0 && recv_buf[1] == 0xF1 && recv_buf[2] == 0xF2
						&& recv_buf[3] == 0xF3 && recv_buf[4] == 0xF4 && recv_buf[5] == 0x01
						&& recv_buf[6] == 0x01 && recv_buf[7] == 0x01 && recv_buf[10] == 0x00 && recv_buf[11] == 0x00){

							send_monitor_event(monitor_response);

					}else if(recv_buf[0] == 0xF0 && recv_buf[1] == 0xF1 && recv_buf[2] == 0xF2
							&& recv_buf[3] == 0xF3  && recv_buf[4] == 0xF4 && recv_buf[5] == 0x00 && recv_buf[6] == 0x01
							&& recv_buf[7] == 0x01 ){

							if(recv_buf[10] > 10 && recv_buf[10] != 0xff){

								HB_time = recv_buf[10];
							}else if(recv_buf[10] == 0xff){

								HB_time = 120;
							}
							send_monitor_event(monitor_response);
							printf("tcp client:login success heartbeat time %d sec\n",HB_time);
							xEventGroupSetBits(cloud_connect_status_event_group, BIT0); 
							set_report_reson(ACTIVE_QUERY);//report device status to cloud server

					}else if(recv_buf[0] == 0xD0 && recv_buf[1] == 0xD1 && recv_buf[2] == 0xD2
						  && recv_buf[3] == 0xD3 && recv_buf[4] == 0xD4 &&recv_buf[5] == 0x00 ){
					  /*ota request*/
 

					}else{
						  protocol_data_parser(recv_buf, recv_ret, CLOUD_DATA_EVENT);
					
					}
				}else {

					printf("tcp client:error [%d]  close tcp transmit, would close socket... -> reconnect\n",recv_ret);
					
					break;
				}
			}
			
		}
		if(0 <= sockfd){

			close(sockfd);
			printf("tcp client:close socket , sockfd:%d  -> reconnect\n", sockfd);
			sockfd = -1;
		}
		if(send_buf != NULL){

			free(send_buf);
			send_buf = NULL;
		}
		if(recv_buf != NULL){
			
			free(recv_buf);
			recv_buf = NULL;
		}
		printf("tcp client:reset tcp client and reconnect to tcp server...\n");
		xEventGroupClearBits(cloud_connect_status_event_group, BIT0); 
		tcp_connect_flag = false; 
		vTaskDelay(5000 / portTICK_RATE_MS);//reconnect time internal
	}else{
		/*count the time and amonut of wifi dorp line */
		 wifi_disconnect_time++;
		 if(wifi_dis_flag == 0){

			 wifi_dis_flag = 1;
			 wifi_disconnect_count++;
		 }
		 printf("tcp client:wait wifi connected [%d] sec -----count [%d]\n",wifi_disconnect_time,wifi_disconnect_count);
		 continue;
	}
	stop_connect:

	if(0 <= sockfd){

		close(sockfd);  
		sockfd=-1;
	}
	xEventGroupClearBits(cloud_connect_status_event_group, BIT0);   
	tcp_connect_flag = false; 
	printf("tcp client:stop the socket\n\n");
	xEventGroupWaitBits(start_event_group, BIT0,false, true, portMAX_DELAY);
  } // end
	xHandleTasktcp = NULL;
	vTaskDelete(NULL);
	
   
}

void tcp_client_send_task(void *arg)
{

  tcp_content_send_t send_tmp;
   for(;;){
		 
#if S907X_BUILD_FOR_ZG
	if (s907x_zg_ota_process_is_start() == pdTRUE){
		printf("ota process begin, exit tcp send task.\n\n");
		break;
	}
#endif
	  if(get_wifi_connection_status(portMAX_DELAY) == BIT0){
		  
		  if(tcp_connect_flag){

			memset(&send_tmp,0,sizeof(tcp_content_send_t));
			if(xQueueReceive(tcp_client_queue,&send_tmp,portMAX_DELAY) == pdTRUE){

			  int send_ret = send(sockfd, send_tmp.content, send_tmp.content_len, 0);
			  if (send_ret <= 0) {

				  printf("send data to tcp server failed -> reconnect\n");
				  xSemaphoreGive(reconnect_shr);
			  } else {

				  printf("send [%d] data to tcp server succeeded\n",send_ret);
			  }
			  vTaskDelay(1500 / portTICK_RATE_MS); 
			}        
			
		  }else{

			vTaskDelay(1000 / portTICK_RATE_MS); 
		  }
	  }
	}
	xHandleTaskSend = NULL;
	vTaskDelete(NULL);  
	
}

void tcp_Client_task_init()
{

   if( monitor_queue == NULL){

	   monitor_queue = xQueueCreate(2, 1);
   }
   if(xHandleTasktcp == NULL){
	   xTaskCreate(&tcp_client_task, "tcp_client_task", TCP_CLIENT_TASK_STACK_SZ, NULL, TCP_CLIENT_TASK_PRIO, &xHandleTasktcp);
   }
   if(xHandleTaskSend == NULL){
	  xTaskCreate(&tcp_client_send_task, "tcp_client_send_task", TCP_CLIENT_SEND_TASK_STACK_SZ, NULL, TCP_CLIENT_SEND_TASK_PRIO, &xHandleTaskSend);
   }
   if(tcp_client_queue == NULL){

	 tcp_client_queue = xQueueCreate(4, sizeof(tcp_content_send_t));
	 if(tcp_client_queue == NULL){

	  printf("creat tcp_client_queue fail\n");
	 }
   }

}
