/* tcp_perf Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/


#ifndef __TCP_CLIENT_H__
#define __TCP_CLIENT_H__



#ifdef __cplusplus
extern "C" {
#endif

typedef struct{
  
  uint16_t content_len;
  uint8_t content[250];

}tcp_content_send_t;

void tcp_client_deinit();
int tcp_Client_init();

int  set_tcp_Client_conf(char *host,int len,uint32_t port);
void get_tcp_Client_conf(char *host,int *len,uint32_t* port);

int  tcp_Client_start();
int  tcp_Client_stop();
int  tcp_Client_pause();
void tcp_Client_exit(void);

int  tcp_Client_send(unsigned char *pdata,int len);
int  get_tcp_Client_connect_status(unsigned int xTicksToWait);
int  get_tcp_Client_switch();

#ifdef __cplusplus
}
#endif


#endif /*#ifndef __TCP_PERF_H__*/

