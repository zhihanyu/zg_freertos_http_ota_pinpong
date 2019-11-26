/*
 * debug_info.h
 *
 *  Created on: 2019��2��15��
 *      Author: Administrator
 */

#ifndef APP_INCLUDE_DEBUG_INFO_H_
#define APP_INCLUDE_DEBUG_INFO_H_

#define origin_code -88

void debug_record_connect_code(int code);
void debug_record_dns_code(int code);
void debug_record_heartbeat_timeout();
void debug_record_login_timeout();
void debug_record_connect_count();

int send_debug_info(int opt);

#endif /* APP_INCLUDE_DEBUG_INFO_H_ */
