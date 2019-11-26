/* tcp_perf Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/


#ifndef __TCP_PERF_H__
#define __TCP_PERF_H__



#ifdef __cplusplus
extern "C" {
#endif

#define data_PKTSIZE 2048
#define TCP_SEVER_PORT 5577
#define TCP_SERVER_MAX_CONN     S907X_MAX_TCP_SERVER_ACCEPT_NUM

void tcp_server_task_init();
int  tcp_server_multi_send(unsigned char* buf,int len);
uint8_t get_tcp_amaount();

#ifdef __cplusplus
}
#endif


#endif /*#ifndef __TCP_PERF_H__*/

