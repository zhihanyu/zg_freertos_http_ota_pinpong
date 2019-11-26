
#include "sys/ZG_system.h"
#if 1

static uint32_t connect_count;
static uint32_t heartbeat_timeout = 0;
static uint32_t login_timeout = 0;
static int connect_code = origin_code;
static int dns_code = origin_code;


void debug_record_connect_code(int code )
{
	connect_code = code;
}

void debug_record_dns_code(int code)
{

	printf("dns code ->  %d\n",dns_code);
	dns_code = code;
}

void debug_record_heartbeat_timeout()
{
	heartbeat_timeout++;
	printf("heartbeat_timeout ->  %d\n",heartbeat_timeout);
}

void debug_record_login_timeout()
{
	login_timeout++;
    printf("login timeout ->  %d\n",login_timeout);
}


void debug_record_connect_count()
{
	connect_count++;
	 printf("connect count->  %d\n",connect_count);
}


static int debug_data_pack(char *input,char *body,unsigned short length)
{

	  int i;
	  body[0] = 0xa0;
	  body[1] = 0xa1;
	  body[2] = 0xa2;
	  body[3] = 0xa3;
	  body[4] = 0xa4;
	  body[5] = 0;
	  body[6] = 0x01;
	  body[7] = 3; //
	  if(length <= 255){
		  body[8]=0x00;
		  body[9]=length;
	  }else{
		  body[8]=length >> 8;
		  body[9]=length;
	  }

	  memcpy(body+10,input,length);
	  body[10+length]  = 0;
	  for(i=0;i<10+length;i++){
	     body[10+length] = body[10+length] + body[i];
	  }

	  printf("\n(protocol pack):head ");
		for(i=0;i<10;i++){
			printf("%02x ",body[i]);
		 }

	  printf("\n(protocol pack):body pack:%s ",input);
	  printf("\n(protocol pack):crc:%x\n\n",body[10+length]);

	  return length + 11;
}

/**
 *
   {
	 "SW":1,                        //remote switch
	 "Conn":1,                      //status of connection
	 "R":-38,                       //rssi
	 "AK":"ff234567bbabcdaa",       //aes key
	 "P":8815,                      //remote port
	 "DN":"ra8815us.magichue.net",  //host name
	 "L":21,                        //length of host
	 "F":43384,                     // free heap
	 "st1":2                        //power on count
	 "st2"                          //login count
	 "LIC"                          //login timeout count
         "HBC"                          //heartbeat timeout count
	 "WFDC"                         //The number of WiFi Dropouts
         "dns"                          //dns code
         "cc"                           //connect count
         "code"                         //connect code
	}
 *
 */
extern uint32_t PowerOn_amount,Login_amount;
int send_debug_info(int opt)
{
	uint8_t aes_key[17]={0};

	char *packet = malloc(300);
	char *json = malloc(300);
	int len;

	Get_AES_key(aes_key);
	char hostname[64] = {0};
	uint32_t port = 0;
	int host_len = 0;

	get_tcp_Client_conf(hostname,&host_len,&port);

	sprintf(json,"{\"SW\":%d,\"Conn\":%d,\"R\":%d,\"AK\":\"%s\",P\":%d,DN\":\"%s\",\"L\":%d,\"F\":%d,\"st1\":%d,\"st2\":%d,\"LIC\":%d,\"HBC\":%d,\"WFDC\":%d,\"dns\":%d,\"cc\":%d,\"code\":%d}"
			,get_tcp_Client_switch()
			,get_tcp_Client_connect_status(0)
			,get_ap_rssi()
			,aes_key
			,port
			,hostname
			,host_len
			,xPortGetFreeHeapSize()
			,PowerOn_amount
			,Login_amount
			,login_timeout
			,heartbeat_timeout
            ,WIFI_disconnect_count_get()
			,dns_code
			,connect_count
			,connect_code
			);


    len = debug_data_pack(json,packet,strlen(json));
    printf("debug packet len : %d\n",len);
    if(opt == 0)
	{
    	tcp_server_multi_send((uint8_t *)packet,len);
    }
	else
	{
    	tcp_Client_send((uint8_t *)packet,len);
    }

	free(packet);
	free(json);
	return 0;
}
#endif