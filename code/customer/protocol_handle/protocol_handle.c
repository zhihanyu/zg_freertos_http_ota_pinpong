
#include "sys/ZG_system.h"
/**
* @brief Handling Interaction Protocol
* @param data
* @note  
* @retval none
*/


extern unsigned char timing_buf[94];
extern unsigned char timeone[7];

void protocol_data_parser(uint8_t *buff ,uint16_t len,protocol_event_t type)
{
  	int crc_check = PARSER_SUCCESS;
  	uint8_t sendbuff[94] = {0};
  	unsigned char *send_data = (unsigned char*)malloc(150);
  	int send_len = 0;

  	colort_struct_t color;

  	struct rtc_time Real_time;
  	memset(&color,0,sizeof(colort_struct_t));
  
  	if(type ==  CLOUD_DATA_EVENT)
  	{
  		crc_check = protocol_head_parser(buff,&len);
		set_report_reson(CLOUD_CONTROL);
  	}
  	else
  	{		
    	set_report_reson(LOCAL_CONTROL);
  	}
  	printf("protocol parser:type : %02X len : %d\n",buff[0],len);
  	if(crc_check == PARSER_SUCCESS)
  	{
	  	switch (buff[0])
	  	{
			case COMMAND_TIME_SET:
			/**
		 	* 校对时间命令：
		 	* 发：	【0X10】＋【0x14】＋【年十个位】＋【月】＋【日】＋【时】＋【分】＋【秒】＋【周】＋【保留位：0x0】＋【0xF0远程，0x0F本地】＋【校验码】(命令长度：12)				
		 	*/	
                Real_time.tm_year = buff[2] + 2000;
                Real_time.tm_mon = buff[3];
                Real_time.tm_mday = buff[4];
                Real_time.tm_hour = buff[5];
                Real_time.tm_min = buff[6];
                Real_time.tm_sec = buff[7];
                Real_time.tm_wday = buff[8];
                RTC_time_Conf(&Real_time);
                timer_clock_start();
                break;

			case COMMAND_TIME_QUERY:
			/**
		 	* 查询时间命令：
		 	* 发：	【0X11】＋【0X1A】＋【0X1B】＋【0xF0远程，0x0F本地】＋【校验码】(命令长度：5)			
		 	* 返回：	【0xF0远程，0x0F本地】+【0X11】＋【0x14】＋【年十个位】＋【月】＋【日】＋【时】＋【分】＋【秒】＋【周】＋【保留位：0x0】＋【校验码】(命令长度：12)			
		 	*/ 
                get_RTC_time(&Real_time);//锟斤拷取RTC
                sendbuff[0] = buff[3];
                sendbuff[1] = 0x11;
                sendbuff[2] = 0x14;
                sendbuff[3] = Real_time.tm_year - 2000;
                sendbuff[4] = Real_time.tm_mon;
                sendbuff[5] = Real_time.tm_mday;
                sendbuff[6] = Real_time.tm_hour;
                sendbuff[7] = Real_time.tm_min;
                sendbuff[8] = Real_time.tm_sec;
                sendbuff[9] = Real_time.tm_wday;
                sendbuff[10] = 0x0;
                sendbuff[11] = data_checkSum(sendbuff,11);

                if(buff[3] ==  CLOUD_QUERY)
                {
                    send_len = reponse_data_pack(PACK_LED_PROTOCOL,BYTE_FORMAT,sendbuff,send_data,12);
                    //tcp_Client_send(send_data,len);
                    tcp_Client_send(send_data,send_len);         
                }
                else
                {
                    tcp_server_multi_send(sendbuff,12);
                }
	   			break;

			case COMMAND_CLOCK_SET:
			/**
		 	* 设置定时内容命令：
			发：	【0X21】＋                                                                                                                                                              
			【1时段定时开关】＋【1时段年十个位】＋【1时段月】＋【1时段日】+【1时段时】＋【1时段分】＋【1时段秒】+【1时段周】＋【1时段模式】＋【1时段40bit RGBWW值】＋【1时段开关】＋
			【2时段定时开关】＋【2时段年十个位】＋【2时段月】＋【2时段日】+【2时段时】＋【2时段分】＋【2时段秒】+【2时段周】＋【2时段模式】＋【2时段40bit RGBWW值】＋【2时段开关】＋
			【3时段定时开关】＋【3时段年十个位】＋【3时段月】＋【1时段日】+【3时段时】＋【3时段分】＋【3时段秒】+【3时段周】＋【3时段模式】＋【3时段40bit RGBWW值】＋【3时段开关】＋
			【4时段定时开关】＋【4时段年十个位】＋【4时段月】＋【4时段日】+【4时段时】＋【4时段分】＋【4时段秒】+【4时段周】＋【4时段模式】＋【4时段40bit RGBWW值】＋【4时段开关】＋
			【5时段定时开关】＋【5时段年十个位】＋【5时段月】＋【5时段日】+【5时段时】＋【5时段分】＋【5时段秒】+【5时段周】＋【5时段模式】＋【5时段40bit RGBWW值】＋【5时段开关】＋   
			【6时段定时开关】＋【6时段年十个位】＋【6时段月】＋【6时段日】+【6时段时】＋【6时段分】＋【6时段秒】+【6时段周】＋【6时段模式】＋【6时段40bit RGBWW值】＋【6时段开关】＋
			【保留位：0x0】＋【0xF0远程，0x0F本地】＋【校验码】(命令长度：94)			
		 	* 
		 	* 	a) 当模式为 0x61时，Value1、2、3、4、5分别代表RGBWW的值，注意RGB CCT不能同时亮，此命令中，RGBWW值中，RGB与CCT中必须有一方为0；如果出现同时不为0，则CCT作为0
			b) 当模式值等于0x00，不考虑Value1、2、3、4、5的值，灯泡RGBWW值取原来的状态。
			c) 当模式值等于0xA1，代表太阳升起，即暖白慢慢变亮；
			当模式值等于0xA2，代表太阳降落；即暖白慢慢变暗；
				如果这两种模式的情况下：
					Value1：代表 时间变化长度，单位分钟(0~255)；
					Value2：代表 起始亮度(0~255)；
					Value3：代表 结束亮度(0~255)，当亮度为0的情况下，模式变化结束后，灯变为关闭状态，亮度值为起始亮度；
					Value4：0x00代表暖白，0x01代表冷白，0x02代表CCT
					Value5：保留位，给0x00即可；
			d) 当模式不等于以上值时，则代表动态模式值，Value1代表速度值，范围：0x01-0x1f；
			e) 定时可用：当为0xF0时定时生效，0x0F时定时失效； 
			f) 定时开关：当为0xF0时为开，0x0F时为关； 
			g) 周：设置循环定时，bit0为空，bit1-bit7分别对应周一到周日，bit7为周日；

			备注：日出、日落的逻辑跟灯泡一样，请参考灯泡日出日落逻辑处理。					
	      	*/ 
  
				timer_clock_Conf(buff,94);
	
				memcpy(&timing_buf[1], &buff[1], 90);	

				
  				//zg_UserDataStore(ADDRESS_TIMER, (char *)&timing_buf, 94);
				for (char i = 1; i < 7; i++)
				{
					if ((timing_buf[i * 15 - 13] != 0) && (timing_buf[i * 15 - 12] != 0) && (timing_buf[i * 15 - 11] != 0))
					{
						timeone[i] = 0x01;	
					}
				}

				
                sendbuff[0] = buff[92];
                sendbuff[1] = 0x21;
                sendbuff[2] = 0x00;
                sendbuff[3] = data_checkSum(sendbuff,3);

                if(buff[92] == 0xf0)
                {
                    send_len = reponse_data_pack(PACK_LED_PROTOCOL,BYTE_FORMAT,sendbuff,send_data,4);
                    tcp_Client_send(send_data,send_len);
                }
                else
                {
                    tcp_server_multi_send(sendbuff,4); 
                }
                break;
			case COMMAND_CLOCK_QUERY:
			/**
			【1时段定时开关】＋【1时段年十个位】＋【1时段月】＋【1时段日】+【1时段时】＋【1时段分】＋【1时段秒】+【1时段周】＋【1时段模式】＋【1时段40bit RGBWW值】＋【1时段开关】＋
			【2时段定时开关】＋【2时段年十个位】＋【2时段月】＋【2时段日】+【2时段时】＋【2时段分】＋【2时段秒】+【2时段周】＋【2时段模式】＋【2时段40bit RGBWW值】＋【2时段开关】＋
			【3时段定时开关】＋【3时段年十个位】＋【3时段月】＋【1时段日】+【3时段时】＋【3时段分】＋【3时段秒】+【3时段周】＋【3时段模式】＋【3时段40bit RGBWW值】＋【3时段开关】＋
			【4时段定时开关】＋【4时段年十个位】＋【4时段月】＋【4时段日】+【4时段时】＋【4时段分】＋【4时段秒】+【4时段周】＋【4时段模式】＋【4时段40bit RGBWW值】＋【4时段开关】＋
			【5时段定时开关】＋【5时段年十个位】＋【5时段月】＋【5时段日】+【5时段时】＋【5时段分】＋【5时段秒】+【5时段周】＋【5时段模式】＋【5时段40bit RGBWW值】＋【5时段开关】＋   
			【6时段定时开关】＋【6时段年十个位】＋【6时段月】＋【6时段日】+【6时段时】＋【6时段分】＋【6时段秒】+【6时段周】＋【6时段模式】＋【6时段40bit RGBWW值】＋【6时段开关】＋
			【保留位：0x0】＋【校验码】(命令长度：94)	
			*/
	        	sendbuff[0] = buff[3];
	        	sendbuff[1] = 0x22;
	        	memcpy(sendbuff + 2,check_timer_clock_data() + 1,90);
	        	sendbuff[92] = 0x00;
	        	sendbuff[93] = data_checkSum(sendbuff,93);

	        	if(buff[3] == 0xf0)
	        	{
	           		send_len = reponse_data_pack(PACK_LED_PROTOCOL,BYTE_FORMAT,sendbuff,send_data,94);
	           		//tcp_Client_send(send_data,len);
	           		tcp_Client_send(send_data,send_len);    
	        	}
	        	else
	        	{
	           		tcp_server_multi_send(sendbuff,94);
	        	} 
	 			break;

			case COMMAND_COLOR_SET:
			/**
		 	* 【0X31】＋【8bit红色数据】＋【8bit绿色数据】＋【8bit蓝色数据】＋【8bit暖白数据】＋【8bit冷白数据】＋【颜色标志位】＋【0xF0远程，0x0F本地】＋【校验码】
		 	* (命令长度：9)；颜色标志位：当为0xF0时，显示RGB的值，当为0X0F时，显示CCT的值；
	     	*/
				set_led_mode(MODE_STATIC);
				color.music_flag = 0;
				color.R = buff[1];
				color.G = buff[2];
				color.B = buff[3];
				color.W = buff[4];
				color.C = buff[5];
				color.ColorFlag = buff[6];
				Set_color(&color);

				if(buff[7] == CLOUD_QUERY) 
				{
					sendbuff[0] = buff[3];
					sendbuff[1] = 0x31;
					sendbuff[2] = 0x00;
					sendbuff[3] = data_checkSum(sendbuff,3);
					send_len = reponse_data_pack(PACK_LED_PROTOCOL,BYTE_FORMAT,sendbuff,send_data,4);
					tcp_Client_send(send_data,send_len);    
				}	
		    	break;		

			case COMMAND_MUSIC_SET_FAST:
				set_led_mode(MODE_MUSIC);
				color.music_flag = 1;
				color.R = buff[1];
				color.G = buff[2];
				color.B = buff[3];
				Set_color(&color);
		    	break;		

			case COMMAND_MUSIC_SET_SLOW:

				set_led_mode(MODE_MUSIC);
				color.R = buff[1];
				color.G = buff[2];
				color.B = buff[3];
				Set_color(&color);
				break;		
			case COMMAND_USERDEFINE_SET:
			/**
		 	* 设置自定义模式的命令：
		 	* 【0X51】＋【第1点32bit色度值】＋【第2点32bit色度值】＋【第3点32bit色度值】＋
                  * 【第4点32bit色度值】＋【第5点32bit色度值】＋【第6点32bit色度值】＋
                  * 【第7点32bit色度值】＋【第8点32bit色度值】＋【第9点32bit色度值】＋
                  * 【第10点32bit色度值】＋【第11点32bit色度值】＋【第12点32bit色度值】＋
                  * 【第13点32bit色度值】＋【第14点32bit色度值】＋【第15点32bit色度值】＋
                  * 【第16点32bit色度值】＋【8bit速度值】＋【8bit CHANGING模式值】＋
                  * 【0XFF】＋【0xF0远程，0x0F本地】＋【校验码】(命令长度：70)
                           * 
                           * 	返回：	"如果是本地(0x0F)：无返回
                  *  如果是远程(0xF0)：【0xF0远程】+ 【0X51】＋【0x00】+【校验码】"	
                           * 
                  *  备注：自定义模式数据命令(下发命令),RGB中不能为：0x01，0x02，0x03，否则当成结束
                  *  收到此命令模式为0x60；
                           */ 
				if(get_led_switch() == MODE_OFF)
				{
				 	set_led_switch(MODE_ON);
				}
				set_led_mode(MODE_USER);
				custom_mode_data_conf(buff + 1);   

				if(buff[68] == CLOUD_QUERY) 
				{
					sendbuff[0] = buff[3];
					sendbuff[1] = 0x51;
					sendbuff[2] = 0x00;
					sendbuff[3] = data_checkSum(sendbuff,3);
					send_len = reponse_data_pack(PACK_LED_PROTOCOL,BYTE_FORMAT,sendbuff,send_data,4);
					tcp_Client_send(send_data,send_len);   
				}
		    	break;
			case COMMAND_USERDEFINE_QUERY:
			/**
			* 查询自定义模式内容的命令：
			* 【0xF0远程，0x0F本地】+【【0X52】＋【第1点32bit色度值】＋【第2点32bit色度值】＋【第3点32bit色度值】＋
			* 【第4点32bit色度值】＋【第5点32bit色度值】＋【第6点32bit色度值】＋
			* 【第7点32bit色度值】＋【第8点32bit色度值】＋【第9点32bit色度值】＋
			* 【第10点32bit色度值】＋【第11点32bit色度值】＋【第12点32bit色度值】＋
			* 【第13点32bit色度值】＋【第14点32bit色度值】＋【第15点32bit色度值】＋
			* 【第16点32bit色度值】＋【8bit速度值】＋【8bit CHANGING模式值】＋
			* 【0XFF】＋【校验码】(命令长度：70) 
			*/

				sendbuff[0] = buff[3];
				sendbuff[1] = 0x52;
				memcpy(&sendbuff[2],get_custom_mode_data() + 1,66);
				sendbuff[68] = 0xff;
				sendbuff[69] = data_checkSum(sendbuff,69);

				if(buff[3] == 0xf0)
				{
					send_len = reponse_data_pack(PACK_LED_PROTOCOL,BYTE_FORMAT,sendbuff,send_data,70);
					tcp_Client_send(send_data,send_len);             
				}
				else
				{
			    	tcp_server_multi_send(sendbuff,70);
				}
		   	 	break;	

			case COMMAND_DYNAMIC_SET:
			/**
		 	* 发：	【0x61】＋【8bit模式值】＋【8bit速度值】＋【0xF0远程，0x0F本地】＋【校验码】(命令长度：5)	
		 	* 返回：	"如果是本地(0x0F)：无返回
		 	* 如果是远程(0xF0)：【0xF0远程】+ 【0x61】＋【0x00】+【校验码】"	
		 	* 备注: 速度值 0x01--0x1F	
		 	*/ 
		  		if(get_led_switch() == MODE_OFF)
		  		{
				 	set_led_switch(MODE_ON);
				}
				set_led_mode(buff[1]);
				set_led_speed(buff[2]);
				if(buff[3] == CLOUD_QUERY) 
				{
					sendbuff[0] = buff[3];
					sendbuff[1] = 0x61;
					sendbuff[2] = 0x00;
					sendbuff[3] = data_checkSum(sendbuff,3);
					send_len = reponse_data_pack(PACK_LED_PROTOCOL,BYTE_FORMAT,sendbuff,send_data,4);
					tcp_Client_send(send_data,send_len);   
				}
		 	  	break;
	        case COMMAND_SET_SORT:

				set_color_output_sort(buff[2]);
				sendbuff[0] = buff[3];
				sendbuff[1] = buff[0];
				sendbuff[2] = 0x00;
				sendbuff[3] = data_checkSum(sendbuff,3);
				if(buff[3] == 0xf0)
				{
					send_len = reponse_data_pack(PACK_LED_PROTOCOL,BYTE_FORMAT,sendbuff,send_data,4);
					tcp_Client_send(send_data,send_len);   
				}
				else
				{
		        	tcp_server_multi_send(sendbuff,4);
				}
	         	break;
		    case COMMAND_SWTICH:	
			/**
		 	* 设置键值（开关命令）命令：
		 	* 发：	【0X71】＋【8bit键值】＋【0xF0远程，0x0F本地】＋【校验码】(命令长度：4)	
                * 返回：	【0xF0远程，0x0F本地】+ 【0X71】＋【开关状态值】＋【校验码】	
                * 备注：键值0x23代表开，0x24代表关； 0x22代表切换，即原来是开即变关，关变开；
                * 需要返回“【开关状态值】
		 	*/ 

				set_led_switch(buff[1]);
				sendbuff[0] = buff[2];
				sendbuff[1] = 0x71;
				sendbuff[2] = get_led_switch();
				sendbuff[3] = data_checkSum(sendbuff,3);
				if(buff[2] == 0xf0)
				{
					send_len = reponse_data_pack(PACK_LED_PROTOCOL,BYTE_FORMAT,sendbuff,send_data,4);
					tcp_Client_send(send_data,send_len);         
				}
				else
				{
					tcp_server_multi_send(sendbuff,4);
				}
		    	break;

			case COMMAND_DEVICE_QUERY:
				/*
				* 设备状态查询命令：	
				*发：	【0X81】+【0X8A】+【0X8B】+【校验码】(命令长度：4)	
				*返回：	【0X81】+【8bit设备名】+【8bit开/关机】+【8bit模式值】+【8bit运行/暂停状态】+ 【8bit速度值】＋【8bit红色数据】＋【8bit绿色数据】＋【8bit蓝色数据】+  【8bit暖白数据】+【版本号】+【8bit冷白数据】+【保留位：0X0】+【校验码】(命令长度：14)	
				*备注：当模块收到设备查询命令后，模块应答此命令，
				*【8bit开/关机】：此刻开机回复0x23，关机回复0x24；
				*【8bit运行/暂停状态】表示当前的运行状态,0x20：运行状态，0x21：暂停状态,在这里该项暂时没有作用；
				*【8bit速度值】表示动态模式速度值，范围：0x01-0x1f，0x01为最快；
				*/
				sendbuff[0] = 0x81;
				sendbuff[1] = ZG_get_device_code();
				sendbuff[2] = get_led_switch();
				sendbuff[3] = get_led_mode();
				sendbuff[4] = 0x01;

				if(get_led_mode() == 0xA1 ||get_led_mode() == 0xA2)
					sendbuff[5] = 0;
   				else
    				sendbuff[5] =  get_led_speed();
    			
				sendbuff[6] = get_red_value();
				sendbuff[7] = get_green_value();
				sendbuff[8] = get_blue_value();
				sendbuff[9] = get_warm_value();
				sendbuff[10] = ZG_get_device_protocol_version();
				sendbuff[11] = get_cold_value();
				sendbuff[12] = get_Color_flag();
				sendbuff[13] = data_checkSum(sendbuff,13);

				if(type ==  CLOUD_DATA_EVENT)
				{
				  	send_len = reponse_data_pack(PACK_LED_PROTOCOL,BYTE_FORMAT,sendbuff,send_data,14);
				  	tcp_Client_send(send_data,send_len);
				}
				tcp_server_multi_send(sendbuff,14);
				break;	
			case COMMAND_TEST_MODE :

				if(buff[1] == 0x6a&& buff[2] == 0x6b && buff[3] == 0x38)
				{
  					set_led_mode(MODE_LIGHT_TEST);
				}
				break;
			default:
				break;
		}		
   	}
   free(send_data);
}
