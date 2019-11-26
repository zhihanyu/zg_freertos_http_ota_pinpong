
#include "sys/ZG_system.h"

static  uint8_t Is_timer_trigger = 0;
unsigned char  timing_buf[94];
unsigned char  timeone[7];
//static TaskHandle_t xHandleClock = NULL;
static void timercolok_thread(void *arg)
{
  	struct rtc_time Real_time;
  	for(;;)
  	{
 		uint8_t week_tem = 0;
 
 		get_RTC_time(&Real_time);
 		//printf("shijian:::%d:%d:%d\n",Real_time.tm_hour,Real_time.tm_min,Real_time.tm_sec);
    	vTaskDelay(1000 / portTICK_PERIOD_MS);
    
    	colort_struct_t color;
		memset(&color,0,sizeof(colort_struct_t));
     	for(int i = 1; i < 7;i++) 
     	{
         	/*hour min sec equal*/
    	 	if(Real_time.tm_hour == timing_buf[i * 15 - 10] &&Real_time.tm_min == timing_buf[i * 15 - 9] &&
				 (Real_time.tm_sec < 3) && timing_buf[i * 15 - 14] == 0xF0)
    	 	{
            /*每周*/
 		    switch (Real_time.tm_wday) 
 		    {
				case 1 :
				   	week_tem = timing_buf[i * 15 - 7] & 0x02;
					break;
				case 2 :
					week_tem = timing_buf[i * 15 - 7] & 0x04;
					break;
				case 3 :
					week_tem = timing_buf[i * 15 - 7] & 0x08;
					break;
				case 4 :
				    week_tem = timing_buf[i * 15 - 7] & 0x10;
					break;
				case 5 :
					week_tem = timing_buf[i * 15 - 7] & 0x20;
					break;
				case 6 :
					week_tem = timing_buf[i * 15 - 7] & 0x40;
					break;
				case 7 :
					week_tem = timing_buf[i * 15 - 7] & 0x80;
					break;
				default	:
					week_tem=0;
 		    }
			if(week_tem != 0 
				||	(timing_buf[i * 15 - 13] == (Real_time.tm_year - 2000) && timing_buf[i * 15 - 12] == Real_time.tm_mon &&
						timing_buf[i * 15 - 11] == Real_time.tm_mday && timing_buf[i * 15 - 7] == 0)
				|| 	(timing_buf[i * 15 - 12] == 0 && timing_buf[i * 15 - 11] == Real_time.tm_mday && timing_buf[i * 15 - 7] == 0)
				|| 	(timing_buf[i * 15 - 11] == 0 && timing_buf[i * 15 - 7] == 0)
				|| 	(timing_buf[i] == 0x01 && timing_buf[i * 15 - 13] == Real_time.tm_year-2000 && timing_buf[i * 15 - 12] == Real_time.tm_mon && 
						timing_buf[i * 15 - 11] == Real_time.tm_mday&& timing_buf[i * 15 - 7] == 0))
			{
				    if(Is_timer_trigger == 0)
				    {
                        printf("clock:trigger\n");
				    	set_report_reson(TIMER_CONTROL);
						Is_timer_trigger = 1;
						
                        /*开关值判断*/
						if(timing_buf[i * 15] == 0x0F )
						{                            
                            set_led_switch(MODE_OFF);
                            printf("(timer):Light off\n");
						}
						else if(timing_buf[i * 15] == 0xF0 )
						{
							set_led_switch(MODE_ON);
							printf("(timer):Light on\n");	
						}
						if (timeone[i] == 0x01 && timing_buf[i * 15 - 13] == Real_time.tm_year-2000 && 
							timing_buf[i * 15 - 12] == Real_time.tm_mon && timing_buf[i * 15 - 11] == Real_time.tm_mday)
						{
							timeone[i] = 0;
						}
						
						if(timing_buf[i * 15 - 6] == 0x61)
						{/*静态颜色*/
							unsigned char r,g,b,w,c;
							printf("(timer):static mode\n");
							r = timing_buf[i * 15 - 5];
							g = timing_buf[i * 15 - 4];
							b = timing_buf[i * 15 - 3];
							w = timing_buf[i * 15 - 2];
							c = timing_buf[i * 15 - 1];
							set_led_mode(MODE_STATIC);
						 	if((r == 0) && (g == 0) && (b == 0))
						 	{
								color.ColorFlag = STATIC_VALUE_MODE_CT;
								color.W = w;
								color.C = c;
							}
							else if((w == 0) && (c == 0))
							{
								color.ColorFlag = STATIC_VALUE_MODE_RGB;
								color.R = r;
								color.G = g;
								color.B = b;
							}
							Set_color(&color); 
						}
						if (timing_buf[i * 15 - 6] == MODE_SUNRISE )
						{
							CCT_sunrise_init(timing_buf[i*15-2],timing_buf[i*15-5],timing_buf[i*15-4],timing_buf[i*15-3]);	
						}
						else if (timing_buf[i * 15 - 6] == MODE_SUNSET)
						{
	                     	CCT_sunset_init(timing_buf[i*15-2],timing_buf[i*15-5],timing_buf[i*15-4],timing_buf[i*15-3]);
						}
						else if (timing_buf[i * 15 - 6] >= 0x25&& timing_buf[i * 15 - 6] <= 0x38)
						{	
							printf("(timer):dynamic mode\n");			
							set_led_mode(timing_buf[i * 15 - 6]);
							set_led_speed(timing_buf[i * 15 - 5]);
						} 
						else if (timing_buf[i * 15 - 6] == 0x3A|| timing_buf[i * 15 - 6] == 0x3B|| timing_buf[i * 15 - 6] == 0x3C) 
						{
							set_led_mode(MODE_USER);
							set_custom_mode(timing_buf[i * 15 - 6]);
							set_led_speed(timing_buf[i * 15 - 5]);
						}
				    }
				}
 			}
		 	if(Real_time.tm_sec > 3){
			 	Is_timer_trigger = 0;
		 	}
     	}
  	}
  	vTaskDelete(NULL);
}

void  timer_clock_init()
{
  	ZG_data_read(CLOCK_STORE,timing_buf);
}

void timer_clock_Conf(uint8_t *dat,int len)
{
  	memcpy(timing_buf,dat,len);
  	ZG_data_save(CLOCK_STORE,dat);
}

uint8_t * check_timer_clock_data()
{
	return timing_buf;
}
//static TaskHandle_t xHandleClock = NULL;
static bool colck_init_flag = 0;
void timer_clock_start()
{
	
 	if(colck_init_flag == 0)
 	{        
		xTaskCreate(&timercolok_thread, "timercolok_thread", TIMER_CLOCK_TASK_STACK_SZ, NULL, TIMER_CLOCK_TASK_PRIO, NULL);
		colck_init_flag=1;
		printf("clock : creat clock thread\n");
 	}

 	/*
    if(xHandleClock == NULL)
    {
      xTaskCreate(&timercolok_thread, "timercolok_thread", TIMER_CLOCK_TASK_STACK_SZ, NULL, TIMER_CLOCK_TASK_PRIO, &xHandleClock);
	  printf("clock : creat clock thread\n");
	  printf("ces+++++++++++++++++++++++++++++++++++++++++++\n");
    }
    */
}