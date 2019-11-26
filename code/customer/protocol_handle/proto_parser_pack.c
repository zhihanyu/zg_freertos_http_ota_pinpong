

#include "sys/ZG_system.h"

/**
* @brief 将发送内容封装成与服务器交互的报文，具体参考 Wi-Fi_通信协议_2.0
* @author hings
* @note 
* @param[in]  char type :   数据类型
*             - PACK_connet_request 服务器连接请求
*             - PACK_ping_check     心跳包
*             - PACK_device_report  设备上报状态
*             - PACK_Ledprotocol    灯的控制协议
*             - PACK_FOTA_reponse   OTA进度上报
* @param[in] format : 数据格式
*             - BYTE_Format
*             - String_Format
*             - JSON_Format
* @param[in] body : 发送内容
* @param[in] output : 完整报文
* @param[in] length : body的长度
* @return 0 : success ,else fail
*/
int  reponse_data_pack(char type,char format, uint8_t *body,uint8_t *output,unsigned short length)
{

	int i;
	output[0] = 0xF0;
	output[1] = 0xF1;
	output[2] = 0xF2;
	output[3] = 0xF3;
	output[4] = 0xF4;
	output[5] = type;
	output[6] = 0x01;
	output[7] = format; //鏁版嵁绫诲瀷  json
	output[8]=0x00;
	output[9]=length;
	memcpy(output+10,body,length);

	body[10+length]  = 0;

	for(i=0;i<10+length;i++)
	{
	   	output[10+length] = output[10+length] + output[i];
	}
	printf("pack:head:%02x %02x %02x %02x %02x %02x %02x %02x %02x %02x \n",
		output[0],output[1],output[2],output[3],output[4],
		output[5],output[6],output[7],output[8],output[9]);

	if(format == BYTE_FORMAT)
	{
	  	printf("pack:body\n");
	  	for(i=10;i< length + 10;i++)
	  	{
			printf("%02x ",output[i]);
	  	}
	  	printf("\n");
	}
	else
	{
	  	printf("pack:body:%s\n",body);
	}
	printf("pack:crc:%02x\n",output[10+length]);
	return length + 11;
}

/**
* @brief 解析服务器报文
* @author hings
* @note 
* @param[in]  pbuff  : 输入和输出数据缓冲区，输入接收报文，输出报文body
* @param[in] length : length of pbuff
* @return 0 : success ,else fail
*/
int protocol_head_parser(uint8_t *pbuff,unsigned short *length)
{
	int i;
	unsigned char CRC = 0;

	if(pbuff[0] == 0xC0 && pbuff[1] == 0xC1 && pbuff[2] == 0xC2 && pbuff[3] == 0xC3 && pbuff[4] == 0xC4 && pbuff[5] == 3 && pbuff[6] == 1)
	{

		for(i=0;i<*length-1;i++)
		{
			CRC = CRC + pbuff[i];
		}
		if(pbuff[*length-1] == CRC)
		{
			CRC = 0;
			for(i=10;i<*length-2;i++)
			{
				CRC = CRC + pbuff[i];
			}
			if(pbuff[*length-2]  == CRC)
			{
				memcpy(pbuff,pbuff+10,*length - 11);
				*length -= 11;
				return PARSER_SUCCESS;
			}
			else
			{
				return PARSER_DATA_CRC_FAIL;
			}
		}
		else
		{
			return PARSER_HEAD_CRC_FAIL;
		}
	}
	else
	{
		return PARSER_HEAD_FAIL;
	}
}
/**
* @brief 数据校验
* @author hings
* @note 
* @param[in]  pbuff  : 输入数据，
* @param[in] length  : length of pbuff
* @return 0 : success ,else fail
*/
int  data_crc_check(uint8_t  *pbuff,unsigned short length)
{
	int i;
	uint8_t CRC = 0;
	for(i=0;i<length-1;i++)
	{
		CRC = CRC + pbuff[i];
	}
	if(pbuff[length-1]  == CRC)
	{
		return PARSER_SUCCESS;
	}
	else
	{
		return -1;
	}
}
/**
* @brief 求数据校验和
* @author hings
* @note 
* @param[in]  pbuff  : 输入数据，
* @param[in] length  : length of pbuff
* @return checkSum   : 校验和
*/
uint8_t data_checkSum(uint8_t * dat,unsigned char len)
{
	int i;
	unsigned char checkSum = 0;
	for(i = 0; i < len ;i++)
	{
		checkSum += dat[i];
	}
	return checkSum;
}