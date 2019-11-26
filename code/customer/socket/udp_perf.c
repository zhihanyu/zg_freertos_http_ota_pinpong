/* udp_perf Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/


#include "sys/ZG_system.h"

static int mysocket;

static struct sockaddr_in remote_addr;
static unsigned int socklen;
static int datalen ;
//create a udp server socket. return ESP_OK:success ESP_FAIL:error
esp_err_t create_udp_server()
{

  printf ("create udp server port:%d\n", 48899);
  int optval = 1; 

  mysocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_IP);
  setsockopt(mysocket,SOL_SOCKET,SO_BROADCAST,( void *)&optval,sizeof(optval));
  if (mysocket < 0) {

    return ESP_FAIL;
  }
  struct sockaddr_in server_addr;
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(48899);
  server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
  if (bind(mysocket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {

  printf ("create udp server failed\n");
  close(mysocket);
  return ESP_FAIL;
 }
 return ESP_OK;
}

//send or recv data task
static void udp_recv_task()
{
    printf ("udp recv task start!\n");

    /*send&receive first packet*/
    socklen = sizeof(remote_addr);

    char *data_buff = malloc(512); 
    memset(data_buff, 0, 512);

    while(1) {
        
      memset(data_buff,0,512);
      datalen = recvfrom(mysocket, data_buff, 512, 0, (struct sockaddr *)&remote_addr, &socklen);
      if(datalen > 0){

        AT_CMD_parser(data_buff);
      }else{

        break;
      }
    } /*while(1)*/
    free(data_buff);
}

void UDP_Send(char *pdata,uint32_t len)
{
  sendto(mysocket, pdata, len, 0, (struct sockaddr *)&remote_addr, sizeof(remote_addr));
}

void close_udp_socket()
{
  if(mysocket != -1){

   close(mysocket);
  }
}
//this task establish a UDP connection and receive data from UDP
static void udp_conn(void *pvParameters)
{
    USER_DBG("heapsize[%d], everheapsize[%d]",
        xPortGetFreeHeapSize(), xPortGetMinimumEverFreeHeapSize());
  for(;;){
    int socket_ret;
    printf ("task udp_conn start.\n");

    printf ("create_udp_server.\n");
    socket_ret = create_udp_server();

    if(socket_ret == ESP_FAIL) {

      printf ("create udp socket error,stop.\n");
      vTaskDelete(NULL);
    }
    udp_recv_task();
    close_udp_socket();
  }
  vTaskDelete(NULL);
}

void udp_task_init()
{
    USER_DBG("heapsize[%d], everheapsize[%d]",
        xPortGetFreeHeapSize(), xPortGetMinimumEverFreeHeapSize());
   xTaskCreate(&udp_conn, "udp_conn", UDP_SERVRE_TASK_STACK_SZ, NULL, UDP_SERVER_TASK_PRIO, NULL);
}
