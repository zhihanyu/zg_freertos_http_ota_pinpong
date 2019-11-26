/* udp_perf Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/


#ifndef __UDP_PERF_H__
#define __UDP_PERF_H__



#ifdef __cplusplus
extern "C" {
#endif

#define EXAMPLE_DEFAULT_PKTSIZE 1024

void UDP_Send(char *pdata,uint32_t len);

void udp_task_init();

#ifdef __cplusplus
}
#endif


#endif /*#ifndef __UDP_PERF_H__*/

