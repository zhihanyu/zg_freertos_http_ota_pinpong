#ifndef __PROTOCOL_H__
#define __PROTOCOL_H__



#ifdef __cplusplus
extern "C" {
#endif

#define PARSER_SUCCESS         0
#define PARSER_HEAD_FAIL     -1
#define PARSER_HEAD_CRC_FAIL -2
#define PARSER_DATA_FAIL      -3
#define PARSER_DATA_CRC_FAIL  -4

#define BYTE_FORMAT    1
#define STRING_FORMAT  2
#define JSON_FORMAT    3

#define PACK_CONNECT_REQUESE  0x00
#define PACK_PING_CHECK       0x01
#define PACK_DEICE_REPORT     0x02
#define PACK_LED_PROTOCOL     0xc3
#define PACK_FOTA_RESPONSE    0xd0

#define CLOUD_QUERY  0xf0

int protocol_head_parser(uint8_t *pbuff,unsigned short *length);

int  data_crc_check(uint8_t  *pbuff,unsigned short length);

uint8_t data_checkSum(uint8_t * dat,unsigned char len);

int reponse_data_pack(char type,char format, uint8_t *body,uint8_t*output,unsigned short length);
#ifdef __cplusplus
}
#endif


#endif 
