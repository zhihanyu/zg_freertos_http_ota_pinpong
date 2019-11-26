/**
  
******************************************************************************
  * @file    Light_fun.c
  * @author  HingreenStore
  * @date    2018-7-31
  * @brief   light funtion
  
******************************************************************************
	*/ 


#include "sys/ZG_system.h"


#define MIN_CAL(a,b) (((a)<(b))?(a):(b))
#define MIN3(a,b,c)       MIN_CAL(MIN_CAL(a,b),c)
#define MIN4(a,b,c,d)     MIN_CAL(MIN_CAL(MIN_CAL(a,b),c),d)
#define MIN5(a,b,c,d,e)   MIN_CAL(MIN_CAL(MIN_CAL(MIN_CAL(a,b),c),d),e)

static unsigned char CustomData[ADDRESS_custom_LEN];

static colort_struct_t CurrentColor,SetColor,colorTmp;

 unsigned char ledMode = 0;
static unsigned char ledSpeed = 0;
static unsigned char ledSwitch = 0x23;

static unsigned char CustomMode;

static unsigned char colorStep,colordix,RT,GT,BT;
static int speedGrade;

static unsigned char redStore,greenStore,blueStore,warmStore,coldStore,ledModeStore,ledSpeedStore,ledSwitchStore;//for store

static int StoreStartT,StoreStartE,StoreStartA; //time count

static unsigned char paramStoreSwitchFlag = 0;

static bool rgbRunningFinish,cctRunningFinish;

static color_channel_t channel = {R_Channel,G_Channel,B_Channel,W_Channel,C_Channel};
static sort_t sort;

static bool Is_color_delay_time_refresh = true;
static int color_delay_time;

static uint8_t mode_record,speed_record;
static colort_struct_t recordColor;

static TaskHandle_t g_light_task_hdl;
#include "s907x_pwm.h"
timer_hdl_t zg_timer_pwm_hdl;


 /**
 * @brief 设置灯输出排序
 * @author hingreenStore
 * @note 
 * @param[in] val : 排序 
 *                  - RGB_SORT
 *                  - GRB_SORT
 *                  - BRG_SORT                
 * @return none
 */
void set_color_output_sort(sort_t val)
{
 	bool Is_setting = 0;
 	printf("sort: -> %d\n",val);
 	if(val > 3)
 	{
  		return;
 	}
  	if(val != sort)
  	{
     	sort = val;
   		Is_setting = 1;

 	}
 	switch(sort)
 	{
  		case RGB_SORT:
   			channel.R_ch = R_Channel;
   			channel.G_ch = G_Channel;
   			channel.B_ch = B_Channel;
  			break;
  		case GRB_SORT:
   			channel.G_ch = R_Channel;
   			channel.R_ch = G_Channel;
   			channel.B_ch = B_Channel;
  			break;  
  		case BRG_SORT:
   			channel.B_ch = G_Channel;
   			channel.R_ch = B_Channel;
   			channel.G_ch = R_Channel;
  			break;   
 	}
 	if(Is_setting == true)
 	{
   		colort_struct_t fetchColor;   
      	mode_record = ledMode;
   		speed_record = ledSpeed;
   		if(mode_record == 0x61)
   		{
     		fetchColor = CurrentColor;
     		printf("record r g b %d %d %d\n",CurrentColor.R,CurrentColor.G,CurrentColor.B);
   		}
		set_led_mode(0x37);
		set_led_speed(1);
		ZG_data_save(LED_SORT_STORE,&val);
		vTaskDelay(1000 / portTICK_PERIOD_MS); 
		set_led_mode(mode_record);
		set_led_speed(speed_record);
   		if(mode_record == 0x61)
   		{
      		Set_color(&fetchColor);
   		}
 	}
}

sort_t get_color_output_sort()
{
	return sort;
}
/**
 * @brief 用户数据保存标志
 * @author hingreenStore
 * @note 
 * @param[in]  type  : 保存类型
 *                   - CUSTOM_STORE
 *                   - TIMER_STORE
 * @return none
 */
void param_store(unsigned char type)
{
	paramStoreSwitchFlag = type;
}
/**
 * @brief 数据保存任务
 * @author hingreenStore
 * @note 
 * @return none
 */
static void  light_param_store_handle()
{
    StoreStartE = ZG_system_time() - StoreStartT;
	StoreStartT = ZG_system_time();
	StoreStartA += StoreStartE;
	//per 3s
	if(StoreStartA / 1000 >= STORE_PER_TIME)
	{
		StoreStartA = 0;
		if(ledModeStore != ledMode  && ledMode != MODE_MUSIC && ledMode != MODE_SUNRISE && ledMode != MODE_SUNSET)
		{
			ZG_data_save(LED_MODE_STORE,&ledMode);
			ledModeStore = ledMode;
		}
		if(ledSpeedStore != ledSpeed)
		{
			ledSpeedStore = ledSpeed;
			ZG_data_save(LED_SPEED_STORE,&ledSpeed);
		}
		if(ledSwitchStore != ledSwitch)
		{
			ledSwitchStore = ledSwitch;
			ZG_data_save(LED_SWITCH_STORE,&ledSwitch);
		}
		if(ledMode == MODE_STATIC && ledSwitch == MODE_ON)
		{
			uint8_t store[3] = {0};
			if(redStore != CurrentColor.R || greenStore != CurrentColor.G || blueStore != CurrentColor.B)
			{
				if(rgbRunningFinish)
				{
					redStore = CurrentColor.R;
					greenStore = CurrentColor.G;
					blueStore = CurrentColor.B;
					printf("(store)-%d: R[%d] G[%d] B[%d]\n",ZG_system_time(),CurrentColor.R,CurrentColor.G,CurrentColor.B);
					set_color_updated_flag(true);
					store[0] = CurrentColor.R;
					store[1] = CurrentColor.G;
					store[2] = CurrentColor.B;
					ZG_data_save(COLOR_STORE,store);
				}
				else
				{
			     	printf("(store): Color is running\n");
				}
			}
			if(warmStore != CurrentColor.W || coldStore != CurrentColor.C)
			{
				if(cctRunningFinish)
				{               
                    uint8_t store[2] = {0};
					warmStore = CurrentColor.W ;
					coldStore = CurrentColor.C;
					store[0] = CurrentColor.W;
					store[1] = CurrentColor.C;
					printf("(store)-%d: W[%d] C[%d]\n",ZG_system_time(),CurrentColor.W,CurrentColor.C);
					set_color_updated_flag(true);
					ZG_data_save(CCT_STORE,store);
				}
				else
				{
				   	printf("(store): CCT is running\n");
				}
			}
		}								
	}
}

static void Light_ReadConf()
{
    ZG_data_read(LED_MODE_STORE,&ledMode);
    if(ledMode == 0xff || ledMode == 0x00)
    {
		ledMode = MODE_LIGHT_TEST;
    }
    ZG_data_read(LED_SORT_STORE,&sort);
    ledModeStore = ledMode;

	if(sort == 0 || sort > 3)
	{
		sort = default_sort_type;
	}
	set_color_output_sort(sort);

	ZG_data_read(LED_SPEED_STORE,&ledSpeed);
	if(ledSpeed > 31 || ledSpeed == 0)
	{
		ledSpeed = 1;
	}
	ledSpeedStore = ledSpeed;

  	printf("\n(read light information):| Light mode:%X | SW : %X | speed:%d | custom mode:%d |\n",ledMode,ledSwitch,ledSpeed,CustomMode);
	if(ledMode == MODE_STATIC)
	{
		uint8_t RGB[3] = {0};
		uint8_t CCT[3] = {0};
        ZG_data_read(COLOR_STORE,RGB);
	    ZG_data_read(CCT_STORE,CCT);
      
		SetColor.R = RGB[0];
		SetColor.G = RGB[1];
		SetColor.B = RGB[2];
		SetColor.W = CCT[0];
		SetColor.C = CCT[1];

		if(SetColor.R != 0 || SetColor.G != 0 ||  SetColor.B != 0)
		{
			SetColor.W = 0;
			SetColor.C = 0;
			CurrentColor.ColorFlag = Dsp_OnlyRGB;
		}
		else
		{
			/*turn on the light if all color val is zero*/
			if(SetColor.W == 0 && SetColor.C == 0)
			{
				SetColor.R = 0xFF;
				SetColor.G = 0xFF;
				SetColor.B = 0xFF;
				CurrentColor.ColorFlag = Dsp_OnlyRGB;
			}
			else
			{
			    CurrentColor.ColorFlag = Dsp_OnlyCCT;
			}
		}
		redStore 	= SetColor.R;
		greenStore 	= SetColor.G;
		blueStore	= SetColor.B;
		warmStore 	= SetColor.W;
		coldStore 	= SetColor.C;

		recordColor = SetColor;		
		printf("(read light information):| R:%d | G:%d | B:%d | W:%d | C:%d |\n\n",SetColor.R,SetColor.G,SetColor.B,SetColor.W,SetColor.C);
	}
	else if(ledMode == MODE_USER)
	{
		ZG_data_read(LED_CUSTOM_MODE_STORE,CustomData);
		set_custom_mode(CustomData[65]);
		set_led_speed(CustomData[64]);
	}
  	//ZG_data_read( LED_SWITCH_STORE,&ledSwitch);
  	//  if(ledSwitch != 0x23 && ledSwitch != 0x24){
		ledSwitch = 0x23;
 	//  }
  	ledSwitchStore = ledSwitch;
 	// set_led_switch(ledSwitch);
}

unsigned char get_red_value()
{
	if(ledSwitch == MODE_ON)
	{
     	return CurrentColor.R;
	}
	else
	{
     	return recordColor.R;
	}
}

unsigned char get_green_value()
{
	if(ledSwitch == MODE_ON)
	{
     	return CurrentColor.G;
	}
	else
	{
     	return recordColor.G;
	}
}

unsigned char get_blue_value()
{
	if(ledSwitch == MODE_ON)
	{
     	return CurrentColor.B;
	}
	else
	{
     	return recordColor.B;
	}
}

unsigned char get_warm_value()
{
	if(ledSwitch == MODE_ON)
	{
     	return CurrentColor.W;
	}
	else
	{
     	return recordColor.W;
	}
}

unsigned char get_cold_value()
{
	if(ledSwitch == MODE_ON)
	{
     	return CurrentColor.C;
	}
	else
	{
     	return recordColor.C;
	}
}

unsigned char get_Color_flag()
{
  return CurrentColor.ColorFlag;
}

colort_struct_t *GET_ColorValue()
{
  	if(ledSwitch == MODE_ON)
  	{  
     	return &CurrentColor;
	}
	else
	{
     	return &recordColor;
	}
}

void cmd_dim_type()
{

}

void cmd_rgb_type()
{
	bool R_running_finish = 0,G_running_finish = 0,B_running_finish = 0;

  	if(ledMode == MODE_STATIC)
  	{
		if(colorTmp.R != CurrentColor.R)
		{
			R_running_finish = 0;
		}
		else
		{
			R_running_finish = 1;
		}
		if(colorTmp.G != CurrentColor.G)
		{
			G_running_finish = 0;
		}
		else
		{
			G_running_finish = 1;
		}

		if(colorTmp.B != CurrentColor.B)
		{

			B_running_finish = 0;
		}
		else
		{
			B_running_finish = 1;
		} 
        /*RGB颜色如果一直在调色，则不进行保存操作*/
		if(R_running_finish == 1 && G_running_finish == 1 && B_running_finish == 1)
		{
			rgbRunningFinish = 1;
			if(colorTmp.R == 0 && colorTmp.G == 0 && colorTmp.B == 0 && SetColor.R == 0x00 && SetColor.G == 0x00 && SetColor.B == 0x00
			  && colorTmp.W == 0 && colorTmp.C == 0 && SetColor.W == 0 && SetColor.C == 0&&  (ledMode == MODE_MUSIC || ledMode == MODE_STATIC))
			{
			    ledSwitch = MODE_OFF;
				//	printf("Light:power down\n\n");
			}
		}
		else
		{
			rgbRunningFinish = 0;
		}
		colorTmp.R = CurrentColor.R;
		colorTmp.G = CurrentColor.G;
		colorTmp.B = CurrentColor.B;
	}
    dimming(channel.R_ch,CurrentColor.R);
	dimming(channel.G_ch,CurrentColor.G);
	dimming(channel.B_ch,CurrentColor.B);
    // USER_DBG("R:%d,G:%d,B:%d\n",CurrentColor.R,CurrentColor.G,CurrentColor.B);
}

void set_cct_zero()
{
	//dev_led_tmp.wDuty = 0;
	//dev_led_tmp.cDuty = 0;
}

void cmd_cct_type()
{
  	bool C_running_finish = 0,W_running_finish = 0;

  	if((colorTmp.W + colorTmp.C) <= 255)
  	{
		if(colorTmp.W != CurrentColor.W)
		{
			W_running_finish = 0;
		}
		else
		{
			W_running_finish = 1;
		}
		if(colorTmp.C != CurrentColor.C)
		{
			C_running_finish = 0;
		}
		else
		{
			C_running_finish = 1;
		}
		if(W_running_finish == 1 && C_running_finish == 1)
		{
			cctRunningFinish = 1;
			//printf("%d %d\n",CurrentColor.W,CurrentColor.C);
		    if(colorTmp.R == 0 && colorTmp.G == 0 && colorTmp.B == 0 && SetColor.R == 0x00 && SetColor.G == 0x00 && SetColor.B == 0x00
			  	&& colorTmp.W == 0 && colorTmp.C == 0 && SetColor.W == 0 && SetColor.C == 0&&  (ledMode == MODE_MUSIC || ledMode == MODE_STATIC))
			{
			    ledSwitch = MODE_OFF;
				//	printf("Light:power down\n\n");
			}
		}
		else
		{
			cctRunningFinish = 0;
		}
		colorTmp.W = CurrentColor.W;
		colorTmp.C = CurrentColor.C;	
	}
	dimming(channel.W_ch,CurrentColor.W);
	dimming(channel.C_ch,CurrentColor.C);
}

void cmd_grb_or_ww_type()
{
/*
	if(dev_led_tmp.tmp_wDuty > 0)
	{
		if(dev_led_tmp.tmp_wDuty != dev_led_tmp.wDuty)
		{
			hfpwm_raw_pulsewidth_ticks(HFGPIO_F_USER_WHITE,dev_led_tmp.wDuty*31);
			//u_printf("System Time:%dms dev_led_tmp.wDuty:%d\r\n", ZG_system_time(), dev_led_tmp.wDuty);
		}
		dev_led_tmp.tmp_wDuty = dev_led_tmp.wDuty;
	}
	else
	{
		cmd_rgb_type();
	}
	*/
}

void cmd_rgb_w_type()
{
/*
	if(dev_led_tmp.tmp_rDuty != dev_led_tmp.rDuty)
	{
		hfpwm_raw_pulsewidth_ticks(HFGPIO_F_USER_RED, dev_led_tmp.rDuty*31);
	}
	if(dev_led_tmp.tmp_gDuty != dev_led_tmp.gDuty)
	{
		hfpwm_raw_pulsewidth_ticks(HFGPIO_F_USER_GREEN, dev_led_tmp.gDuty*31);
	}
	if(dev_led_tmp.tmp_bDuty != dev_led_tmp.bDuty)
	{
		hfpwm_raw_pulsewidth_ticks(HFGPIO_F_USER_BLUE,dev_led_tmp.bDuty*31);
	}
	if(dev_led_tmp.tmp_wDuty != dev_led_tmp.wDuty)
	{
		hfpwm_raw_pulsewidth_ticks(HFGPIO_F_USER_WHITE,dev_led_tmp.wDuty*31);
		u_printf("System Time:%dms dev_led_tmp.wDuty:%d\r\n", ZG_system_time(), dev_led_tmp.wDuty);
	}
	dev_led_tmp.tmp_rDuty = dev_led_tmp.rDuty;
	dev_led_tmp.tmp_gDuty = dev_led_tmp.gDuty;
	dev_led_tmp.tmp_bDuty = dev_led_tmp.bDuty;
	dev_led_tmp.tmp_wDuty = dev_led_tmp.wDuty;
	*/
}

static unsigned char set_pin_step_change(unsigned char inputData, unsigned char *outputData)
{
	if(*outputData > inputData)
	{
		(*outputData)--;
		return 1;
	}
	else if(*outputData  < inputData)
	{
		(*outputData)++;
		return 1;
	}
	return 0;
}

/**
 * @brief 设置灯的颜色
 * @author hingreenStore
 * @note 
 * @param[in]  color  : RGBWC值
 * @return none
 */
void Set_color(colort_struct_t *color)
{
 	SetColor = *color;
 	Is_color_delay_time_refresh = true;

 	if(color->R != 0 || color->G != 0 || color->B != 0 || color->W != 0 || color->C != 0)
 	{
		// if(SW == MODE_OFF)
	   	//  printf("Light:force light SW to ON\n\n");
	 	ledSwitch = MODE_ON;		
 	}
 	printf("Light:set R -> %d G -> %d B -> %d W -> %d C -> %d\n",SetColor.R,SetColor.G,SetColor.B,SetColor.W,SetColor.C);
 	if(SetColor.W + SetColor.C > 255)
 	{
	 	return;
 	}
	CurrentColor.ColorFlag = SetColor.ColorFlag;
 	CurrentColor.music_flag = SetColor.music_flag;
}

void record_cct(uint8_t W,uint8_t C)
{
  recordColor.W = W;
  recordColor.C = C;
}

#define ACCURACY 0.001
double newarmStoreqrt(double n)   
{
	double low, high, mid, tmp;         // 获取上下界
	if (n > 1)  
	{
		low = 1;
		high = n;
	} 
	else 
	{
		low = n;
		high = 1;
	}
	// 二分法求开方
	while (low <= high) 
	{
		mid = (low + high) / 2.000;
		tmp = mid * mid;
		if (tmp - n <= ACCURACY && tmp -n >= ACCURACY * -1) 
		{
			return mid;
		} 
		else if (tmp > n) 
		{
			high = mid;
		} 
		else 
		{
			low = mid;
		}
	}       
  	return -1.000;
}
/**
 * @brief  调光任务
 * @author hingreenStore
 * @note 
 * @return none
 */
static void color_dimmer_handle( )
{
  	int t1 = 0,t2 = 0, t3 = 0;

	if(ledMode == MODE_STATIC || (ledMode == MODE_MUSIC))
	{//RGB����
		if((SetColor.R != CurrentColor.R || SetColor.G != CurrentColor.G || SetColor.B != CurrentColor.B)
			&&CurrentColor.W ==0 && CurrentColor.C == 0)
		{
			set_pin_step_change(SetColor.R, &CurrentColor.R);
			set_pin_step_change(SetColor.G, &CurrentColor.G);
			set_pin_step_change(SetColor.B, &CurrentColor.B);

            if(Is_color_delay_time_refresh == true && ledMode == MODE_STATIC)
            {
				Is_color_delay_time_refresh = false;
				t1 = 9 - newarmStoreqrt(abs(SetColor.R - CurrentColor.R)) / 2;
				t2 = 9 - newarmStoreqrt(abs(SetColor.G - CurrentColor.G)) / 2;
				t3 = 9 - newarmStoreqrt(abs(SetColor.B - CurrentColor.B)) / 2;

				color_delay_time = MIN3(t1,t2,t3) + 1;
			}
			//	printf("color_delay_time %d\n",color_delay_time);
			CurrentColor.ColorFlag = Dsp_OnlyRGB;
		}
		if((SetColor.W != CurrentColor.W || SetColor.C != CurrentColor.C)
			 &&CurrentColor.R ==0 && CurrentColor.G == 0 && CurrentColor.B == 0)
		{			
			set_pin_step_change(SetColor.W, &CurrentColor.W);
			set_pin_step_change(SetColor.C, &CurrentColor.C);

			if(Is_color_delay_time_refresh == true && ledMode == MODE_STATIC)
			{
				Is_color_delay_time_refresh = false;
				t1 = 9 - newarmStoreqrt(abs(SetColor.W - CurrentColor.W)) / 2;
				t2 = 9 - newarmStoreqrt(abs(SetColor.C - CurrentColor.C)) / 2;
				color_delay_time = MIN_CAL(t1,t2) + 1;
			}			
			CurrentColor.ColorFlag = Dsp_OnlyCCT;	
		}
		if(ledSwitch == MODE_ON)
		{
            recordColor = CurrentColor;
		}
		if(color_delay_time != 0 && (ledMode == MODE_STATIC ))
		{
            vTaskDelay(color_delay_time / portTICK_PERIOD_MS); 								
		}
	}
	else
	{
	  	if(ledSwitch == MODE_ON)
	  	{
			if(SetColor.R != 0 || SetColor.G != 0 || SetColor.B != 0)
			{
				SetColor.W = 0;
				SetColor.C = 0;
			}
			CurrentColor.R = SetColor.R;
			CurrentColor.G = SetColor.G;
			CurrentColor.B = SetColor.B;
			CurrentColor.W = SetColor.W;
			CurrentColor.C = SetColor.C;

			recordColor = CurrentColor;
	  	}
	  	else
	  	{
			CurrentColor.R = 0x00;
			CurrentColor.G = 0x00;
			CurrentColor.B = 0x00;
			CurrentColor.W = 0x00;
			CurrentColor.C = 0x00;
	  	}
    }
    cmd_cct_type();
	cmd_rgb_type();
}

void set_led_mode(unsigned char mode)
{
 	if(ledMode != mode)
 	{
    	ledMode = mode;
    	printf("Light:LED mode -> %x\n\n",mode);
	}
 	press_count_clear();
 	//printf("(dimmer):set light mode [%d]\n",mode);
 	/*reset dynamci parameter*/
 	speedGrade = 100;
 	colorStep = 0;
 	colordix = 0;
 	RT = 0;
 	GT = 0;
 	BT = 0;
}

unsigned char get_led_mode()
{
  	return ledMode;
}

void set_led_speed(unsigned char speed)
{
 	if(speed > STATIC_VALUE_SPEED_MAX)
 		speed = STATIC_VALUE_SPEED_MAX;
 	else if(speed ==0)
 		speed = STATIC_VALUE_SPEED_MIN;
  	ledSpeed = speed;
  	//printf("(dimmer):speed %d\n",speed);
}

unsigned char get_led_speed()
{
 	return ledSpeed;
}

void set_led_switch(unsigned char sw)
{
 	if(sw != MODE_ON && sw != MODE_OFF)
 	{
  		sw = MODE_ON;
 	}
  	if(sw == MODE_ON)
  	{
  		if(ledMode == MODE_STATIC || ledMode == MODE_MUSIC)
  		{
   			Is_color_delay_time_refresh = true;
   			if(recordColor.R == 0 && recordColor.G == 0 && recordColor.B == 0 && recordColor.W == 0 && recordColor.C ==0 )
   			{
    			printf("Light:force turn on the light\n");
    			SetColor.R = 0xff;
    			SetColor.G = 0xff;
    			SetColor.B = 0xff;
   			}
   			else
   			{
    			SetColor = recordColor;
   			}
  		}
 	}
 	else
 	{
 	 	SetColor.R = 0x00;
  		SetColor.G = 0x00;
  		SetColor.B = 0x00;
  		SetColor.W = 0x00;
  		SetColor.C = 0x00;
 	}  
  	ledSwitch = sw;
  	printf("Light: SW -> %X\n",ledSwitch);
}

unsigned char get_led_switch()
{
 	return ledSwitch;
}

void set_custom_mode(unsigned char mode)
{
   	CustomMode = mode;
   	printf("Light: custom mode -> %x\n\n",mode);
}

unsigned char get_custom_mode()
{
 	return CustomMode;
}

void custom_mode_data_conf(uint8_t *dat)
{
  	memcpy(CustomData,dat,66);
  	set_custom_mode(CustomData[65]);
  	set_led_speed(CustomData[64]);
  	ZG_data_save(LED_CUSTOM_MODE_STORE,CustomData);
}

uint8_t *get_custom_mode_data()
{
	return CustomData;
}

static void dynamic_rgb_conf(unsigned char r,unsigned char g,unsigned char b)
{
	SetColor.R = r;
	SetColor.G = g;
	SetColor.B = b;
}

void SevenColorFade(void) //内置七彩渐变
{
	if (colordix < 0xff)
		colordix++;
	else 
	{
		colordix = 0;
		colorStep++;
	}
	switch (colorStep) 
	{
		case 0: 
			dynamic_rgb_conf(0xFF,0x00,0x00);
			break;
		case 1: 
			dynamic_rgb_conf(0xFF,colordix,0x00);
			break;
		case 2: 
	        dynamic_rgb_conf(0xFF,0xFF,0x00);
			break;
		case 3:
		    dynamic_rgb_conf(0xFF-colordix,0xFF,0x00);
			break;
		case 4:
	        dynamic_rgb_conf(0x00,0xFF,0x00);
			break;
		case 5: 
			dynamic_rgb_conf(0x00,0xFF,colordix);
			break;
		case 6: 
			dynamic_rgb_conf(0x00,0xFF,0xFF);
			break;
		case 7: 
			dynamic_rgb_conf(0x00,0xFF-colordix,0xFF);
			break;
		case 8: 
	        dynamic_rgb_conf(0x00,0x00,0xFF);
			break;
		case 9: 
			dynamic_rgb_conf(colordix,0x00,0xFF);
			break;
		case 10: 
	        dynamic_rgb_conf(0xFF,0x00,0xFF);
			break;
		case 11: 
	        dynamic_rgb_conf(0xFF,colordix,0xFF);
			break;
		case 12: 
	        dynamic_rgb_conf(0xFF,0xFF,0xFF);
			break;
		case 13:
	        dynamic_rgb_conf(0xFF,0xFF-colordix,0xFF-colordix);		
			break;
		default:
			colorStep = 0;
	}
}

void RGBJump(void) 		//内置七彩跳变
{
	if (colordix < 0xB0) 
	{
		colordix++;
	}
	else 
	{
		colordix = 0;
		colorStep++;
	}
	switch (colorStep) 
	{
		case 0:					
			dynamic_rgb_conf(0xFF,0x00,0x00);
			break;
		case 1:					
			dynamic_rgb_conf(0x00,0xFF,0x00);
			break;
		case 2:					
			dynamic_rgb_conf(0x00,0x00,0xFF);
			break;
		default:
			colorStep = 0;
	}
}

void SevenColorJump(void) 		
{
	if (colordix < 0xB0) 
	{
		colordix++;
	} 
	else 
	{
		colordix = 0;
		colorStep++;
	}
	switch (colorStep) 
	{
		case 0:					
			dynamic_rgb_conf(0xFF,0x00,0x00);
			break;
		case 1:					
			dynamic_rgb_conf(0xFF,0xFF,0x00);
			break;
		case 2:					
			dynamic_rgb_conf(0x00,0xFF,0x00);
			break;
		case 3:					
			dynamic_rgb_conf(0x00,0xFF,0xFF);
			break;
		case 4:					
			dynamic_rgb_conf(0x00,0x00,0xFF);
			break;
		case 5:					
			dynamic_rgb_conf(0xFF,0x00,0xFF);
			break;
		case 6:					
			dynamic_rgb_conf(0xFF,0xFF,0xFF);
			break;
		default:
			colorStep = 0;
	}
}

static int MODE_TEST_startT = 0,MODE_TEST_EndT = 0,MODE_TESTADD = 0;
void Aging_mode(void) //老化模式
{
	MODE_TEST_EndT = ZG_system_time() - MODE_TEST_startT;
	MODE_TEST_startT = ZG_system_time();
	MODE_TESTADD += MODE_TEST_EndT;
	if((MODE_TESTADD)/1000 >= 60)
	{
		MODE_TESTADD = 0;
		colorStep++;
		if(colorStep > 3)
		{
			colorStep = 0;
		}
	}

	switch (colorStep) 
	{
		case 0: 
    		dynamic_rgb_conf(0xFF,0xFF,0xFF);
			break;
		case 1: 
    		dynamic_rgb_conf(0x00,0x00,0x00);
	  		SetColor.W = 0xFF;
	  		SetColor.C = 0;
			break;
		case 2: 
    		dynamic_rgb_conf(0x00,0x00,0x00);
	  		SetColor.W = 0x00;
	  		SetColor.C = 0xFF;
			break;
		default:
			colorStep = 0;
	}
}

void TestMode(void) //测试模式
{
	if (colordix < 0xff)
		colordix++;
	else
	{
		colordix = 0;
		colorStep++;
		if(colorStep >=9 )
		{
			colorStep = 0;
		}
	}
	switch (colorStep) 
	{
		case 0: 
    		dynamic_rgb_conf(0x00,0x00,0x00);
			SetColor.W = 0XFF;
			SetColor.C = 0X00;
			break;
		case 3: 
    		dynamic_rgb_conf(0x00,0x00,0x00);
			SetColor.W = 0X00;
			SetColor.C = 0XFF;
			break;
		case 4: 
    		dynamic_rgb_conf(0xFF,0x00,0x00);
			SetColor.W = 0X00;
			SetColor.C = 0X00;
			break;
		case 5: 
    		dynamic_rgb_conf(0x00,0xFF,0x00);
			SetColor.W = 0X00;
			SetColor.C = 0X00;
			break;
		case 6: 
    		dynamic_rgb_conf(0x00,0x00,0xFF);
			SetColor.W = 0X00;
			SetColor.C = 0X00;
			break;
		case 7: 
    		dynamic_rgb_conf(0x00,0x00,0x00);
			SetColor.W = 0X00;
			SetColor.C = 0X00;
			break;
	}
}

void CustomJump(void)			
{
	if (colordix < 0xB0)
	{
		colordix++;
	} 
	else 
	{
		colordix = 0;
		colorStep++;
	}
	switch (colorStep) 
	{
		case 0:
			dynamic_rgb_conf(CustomData[0],CustomData[1],CustomData[2]);
			break;
		case 1:
			dynamic_rgb_conf(CustomData[4],CustomData[5],CustomData[6]);
			if ((CustomData[4] == 1) & (CustomData[5] == 2) & (CustomData[6] == 3))
				colorStep = 0;
			break;
		case 2:
			dynamic_rgb_conf(CustomData[8],CustomData[9],CustomData[10]);
			if ((CustomData[8] == 1) & (CustomData[9] == 2) & (CustomData[10] == 3))
				colorStep = 0;
			break;
		case 3: 
			dynamic_rgb_conf(CustomData[12],CustomData[13],CustomData[14]);
			if ((CustomData[12] == 1) & (CustomData[13] == 2) & (CustomData[14] == 3))
				colorStep = 0;
			break;
		case 4: 
			dynamic_rgb_conf(CustomData[16],CustomData[17],CustomData[18]);
			if ((CustomData[16] == 1) & (CustomData[17] == 2) & (CustomData[18] == 3))
				colorStep = 0;
			break;
		case 5: 
			dynamic_rgb_conf(CustomData[20],CustomData[21],CustomData[22]);
			if ((CustomData[20] == 1) & (CustomData[21] == 2) & (CustomData[22] == 3))
				colorStep = 0;
			break;
		case 6:
			dynamic_rgb_conf(CustomData[24],CustomData[25],CustomData[26]);
			if ((CustomData[24] == 1) & (CustomData[25] == 2) & (CustomData[26] == 3))
				colorStep = 0;
			break;
		case 7:
			dynamic_rgb_conf(CustomData[28],CustomData[29],CustomData[30]);
			if ((CustomData[28] == 1) & (CustomData[29] == 2) & (CustomData[30] == 3))
				colorStep = 0;
			break;
		case 8: 
			dynamic_rgb_conf(CustomData[32],CustomData[33],CustomData[34]);
			if ((CustomData[32] == 1) & (CustomData[33] == 2) & (CustomData[34] == 3))
				colorStep = 0;
			break;
		case 9: 
			dynamic_rgb_conf(CustomData[36],CustomData[37],CustomData[38]);
			if ((CustomData[36] == 1) & (CustomData[37] == 2) & (CustomData[38] == 3))
				colorStep = 0;
			break;
		case 10: 
			dynamic_rgb_conf(CustomData[40],CustomData[41],CustomData[42]);
			if ((CustomData[40] == 1) & (CustomData[41] == 2) & (CustomData[42] == 3))
				colorStep = 0;
			break;
		case 11: 
			dynamic_rgb_conf(CustomData[44],CustomData[45],CustomData[46]);
			if ((CustomData[44] == 1) & (CustomData[45] == 2) & (CustomData[46] == 3))
				colorStep = 0;
			break;
		case 12: 
			dynamic_rgb_conf(CustomData[48],CustomData[49],CustomData[50]);
			if ((CustomData[48] == 1) & (CustomData[49] == 2) & (CustomData[50] == 3))
				colorStep = 0;
			break;
		case 13: 
			dynamic_rgb_conf(CustomData[52],CustomData[53],CustomData[54]);
			if ((CustomData[53] == 1) & (CustomData[53] == 2) & (CustomData[54] == 3))
				colorStep = 0;
			break;
		case 14: 
			dynamic_rgb_conf(CustomData[56],CustomData[57],CustomData[58]);
			if ((CustomData[56] == 1) & (CustomData[57] == 2) & (CustomData[58] == 3))
				colorStep = 0;
			break;
		case 15:
			dynamic_rgb_conf(CustomData[60],CustomData[61],CustomData[62]);
			if ((CustomData[60] == 1) & (CustomData[61] == 2) & (CustomData[62] == 3))
				colorStep = 0;
			break;
		default:
			colorStep = 0;
	}
}

void CustomFade(void)	//自定义七彩渐变		
{
	if (SetColor.R < CustomData[colordix * 4 ]) 
	{
		SetColor.R++;
		RT = 0;
	}
	else if (SetColor.R > CustomData[colordix * 4])
	{
		SetColor.R--;
		RT = 0;
	} 
	else
	{
		RT = 1;
	}
	if (SetColor.G < CustomData[colordix * 4 + 1]) 
	{
		SetColor.G++;
		GT = 0;
	} 
	else if (SetColor.G > CustomData[colordix * 4 + 1]) 
	{
		SetColor.G--;
		GT = 0;
	} 
	else
	{
		GT = 1;
	}
	if (SetColor.B < CustomData[colordix * 4 + 2]) 
	{
		SetColor.B++;
		BT = 0;
	} 
	else if (SetColor.B > CustomData[colordix * 4 + 2]) 
	{
		SetColor.B--;
		BT = 0;
	} 
	else
	{
		BT = 1;
	}
	if ((RT == 1) && (BT == 1) && (GT == 1))
	{
		colordix++;
		if ((colordix > 15)|| ((CustomData[colordix * 4 ] == 1) && (CustomData[colordix * 4 + 1] == 2)
			&& (CustomData[colordix * 4 + 2] == 3)))
		{
			colordix = 0;
		}
	}
}

void CustomStrobe(void)			//自定义七彩频闪
{
	if (colordix < 0xE0)
	{
		colordix++;
	}
	else 
	{
		colordix = 0;
		colorStep++;
	}
	if ((colorStep > 15)|| ((CustomData[colorStep * 4 ] == 1) && (CustomData[colorStep * 4 + 1] == 2)
		&&(CustomData[colorStep * 4 + 2] == 3)))
	{
		colorStep = 0;
	}
	if (colordix>=50 && colordix<=51) 
	{
		SetColor.R = CustomData[colorStep * 4 ];
		SetColor.G = CustomData[colorStep * 4 + 1];
		SetColor.B = CustomData[colorStep * 4 + 2];
	} 
	else 
	{
		SetColor.R = 0;
		SetColor.G = 0;
		SetColor.B = 0;
	}
}

static void light_mode_factory()
{
	if (colordix < 0xB4)
		colordix++;
	else 
	{
		colordix = 0;
	}
	if (colordix <= 1)
	{
		dynamic_rgb_conf(0xFF,0xFF,0xFF);
	} 
	else 
	{
		dynamic_rgb_conf(0x00,0x00,0x00);
	}
	MODE_TEST_EndT = ZG_system_time() - MODE_TEST_startT;
	MODE_TEST_startT = ZG_system_time();
	MODE_TESTADD += MODE_TEST_EndT;
	if((MODE_TESTADD)/1000 > 5)
	{
		MODE_TESTADD = 0;
		printf("(system):reset to factory mode\n");
		set_led_mode(MODE_FACTORY);	
	}

}

void  light_func_handle()
{
	color_dimmer_handle();
	if(ledMode != MODE_LIGHT_TEST  && ledMode !=MODE_SUNRISE && ledMode != MODE_SUNSET )
	{
	  	speedGrade++;
	}
	else
	{
	  	speedGrade = 100;
	}
   	if(speedGrade >= ledSpeed)
   	{
		speedGrade = 0;
		switch (ledMode)
		{
			case 0x60:
				switch (CustomMode)
				{
					case 0X3B:
						CustomJump();
						break;
					case 0X3A:
						CustomFade();
						break;
					case 0X3c:
						CustomStrobe();
						break;
				}
				break;
			case 0x25:
				SevenColorFade();
				break;
			case 0x26://红色渐变
				if (colordix < 0xff)
					colordix++;
				else 
				{
					colordix = 0;
					colorStep++;
				}
				switch (colorStep)
				{
					case 0:
						dynamic_rgb_conf(0XFF-colordix,0x00,0x00);
						break;
					case 1:
						dynamic_rgb_conf(colordix,0x00,0x00);
						break;
					default:
						colorStep = 0;
				}
				break;
			case 0x27:
				if (colordix < 0xff)
					colordix++;
				else 
				{
					colordix = 0;
					colorStep++;
				}
				switch (colorStep)
				{
					case 0:
						dynamic_rgb_conf(0x00,0xFF-colordix,0x00);
						break;
					case 1:
						dynamic_rgb_conf(0x00,colordix,0x00);
						break;
					default:
						colorStep = 0;
				}
				break;
			case 0x28:
				if (colordix < 0xff)
					colordix++;
				else 
				{
					colordix = 0;
					colorStep++;
				}
				switch (colorStep)
				{
					case 0:
						dynamic_rgb_conf(0x00,0x00,0XFF-colordix);
						break;
					case 1:
						dynamic_rgb_conf(0x00,0x00,colordix);
						break;
					default:
						colorStep = 0;
				}
				break;
			case 0x29:
				if (colordix < 0xff)
					colordix++;
				else 
				{

					colordix = 0;
					colorStep++;
				}
				switch (colorStep){
				case 0:
					dynamic_rgb_conf(0XFF-colordix,0XFF-colordix,0x00);
					break;
				case 1:
					dynamic_rgb_conf(colordix,colordix,0x00);
					break;
				default:
					colorStep = 0;
				}
				break;
			case 0x2A:
				if (colordix < 0xff)
					colordix++;
				else 
				{
					colordix = 0;
					colorStep++;
				}
				switch (colorStep)
				{
					case 0:
						dynamic_rgb_conf(0x00,0xFF-colordix,0xFF-colordix);
						break;
					case 1:
						dynamic_rgb_conf(0x00,colordix,colordix);
						break;
					default:
						colorStep = 0;
				}
				break;
			case 0x2B:
				if (colordix < 0xff)
					colordix++;
				else 
				{
					colordix = 0;
					colorStep++;
				}
				switch (colorStep)
				{
					case 0:
						dynamic_rgb_conf(0xFF-colordix,0x00,0xFF-colordix);
						break;
					case 1:
						dynamic_rgb_conf(colordix,0x00,colordix);
						break;
					default:
						colorStep = 0;
				}
				break;
			case 0x2C:
				if (colordix < 0xff)
					colordix++;
				else 
				{
					colordix = 0;
					colorStep++;
				}
				switch (colorStep)
				{
					case 0:
						dynamic_rgb_conf(0xFF-colordix,0xFF-colordix,0xFF-colordix);
						break;
					case 1:
						dynamic_rgb_conf(colordix,colordix,colordix);
						break;
					default:
						colorStep = 0;
				}
				break;
			case 0x2d:
				if (colordix < 0xff)
					colordix++;
				else 
				{
					colordix = 0;
					colorStep++;
				}
				switch (colorStep)
				{
					case 0:
						dynamic_rgb_conf(colordix,0x00,0x00);
						break;
					case 1:
						dynamic_rgb_conf(0xFF-colordix,0x00,0x00);
						break;
					case 2:
						dynamic_rgb_conf(0x00,colordix,0x00);
						break;
					case 3:
						dynamic_rgb_conf(0x00,0XFF-colordix,0x00);
						break;
					default:
						colorStep = 0;
				}
				break;
			case 0x2e:
				if (colordix < 0xff)
					colordix++;
				else 
				{
					colordix = 0;
					colorStep++;
				}
				switch (colorStep)
				{
					case 0:
						dynamic_rgb_conf(colordix,0x00,0x00);
						break;
					case 1:
						dynamic_rgb_conf(0xFF-colordix,0x00,0x00);
						break;
					case 2:
						dynamic_rgb_conf(0x00,0x00,colordix);
						break;
					case 3:
						dynamic_rgb_conf(0x00,0x00,0xFF-colordix);
						break;
					default:
						colorStep = 0;
				}
				break;
			case 0x2f:
				if (colordix < 0xff)
					colordix++;
				else 
				{
					colordix = 0;
					colorStep++;
				}
				switch (colorStep)
				{
					case 0:
						dynamic_rgb_conf(0x00,0x00,colordix);
						break;
					case 1:
						dynamic_rgb_conf(0x00,0x00,0XFF-colordix);
						break;
					case 2:
						dynamic_rgb_conf(0x00,colordix,0x00);
						break;
			  		case 3:
						dynamic_rgb_conf(0x00,0xFF-colordix,0x00);
						break;
					default:
						colorStep = 0;
				}
				break;
			case 0x30:
				if (colordix < 0xE0)
					colordix++;
				else 
				{
					colordix = 0;
					colorStep++;
				}
				if (colorStep > 6)
					colorStep = 0;
				switch (colorStep)
				{
					case 0:
						if (colordix <= 2) 
						{
							dynamic_rgb_conf(0xFF,0x00,0x00);
						} 
						else 
						{
							dynamic_rgb_conf(0x00,0x00,0x00);
						}
						break;
					case 1:
						if (colordix <= 2) 
						{
							dynamic_rgb_conf(0x00,0xFF,0x00);
						} 
						else 
						{
							dynamic_rgb_conf(0x00,0x00,0x00);
						}
						break;
					case 2:
						if (colordix <= 2) 
						{
							dynamic_rgb_conf(0x00,0x00,0xFF);
						}
						else 
						{
							dynamic_rgb_conf(0x00,0x00,0x00);
						}
						break;
					case 3:
						if (colordix <= 2) 
						{
							dynamic_rgb_conf(0xFF,0xFF,0x00);
						} 
						else 
						{
							dynamic_rgb_conf(0x00,0x00,0x00);
						}
						break;
					case 4:
						if (colordix <= 2) 
						{
							dynamic_rgb_conf(0x00,0xFF,0xFF);
						} 
						else 
						{
							dynamic_rgb_conf(0x00,0x00,0x00);
						}
						break;
					case 5:
						if (colordix <= 2) 
						{
							dynamic_rgb_conf(0xFF,0x00,0xFF);
						} 
						else 
						{
							dynamic_rgb_conf(0x00,0x00,0x00);
						}
						break;
					case 6:
						if (colordix <= 2) 
						{
							dynamic_rgb_conf(0xFF,0xFF,0xFF);
						} 
						else 
						{
							dynamic_rgb_conf(0x00,0x00,0x00);
						}
						break;
					}
					break;
			case 0x31:
				if (colordix < 0xE0)
					colordix++;
				else 
				{
					colordix = 0;
				}
				if (colordix <= 1) 
				{
					dynamic_rgb_conf(0xFF,0x00,0x00);
				} 
				else 
				{
					dynamic_rgb_conf(0x00,0x00,0x00);
				}
				break;
			case 0x32:
				if (colordix < 0xE0)
					colordix++;
				else 
				{
					colordix = 0;
				}
				if (colordix <= 1) 
				{
					dynamic_rgb_conf(0x00,0xFF,0x00);
				} 
				else 
				{
					dynamic_rgb_conf(0x00,0x00,0x00);
				}
				break;
			case 0x33:
				if (colordix < 0xE0)
					colordix++;
				else 
				{
					colordix = 0;
				}
				if (colordix <= 1) 
				{
					dynamic_rgb_conf(0x00,0x00,0xFF);
				} 
				else 
				{
					dynamic_rgb_conf(0x00,0x00,0x00);
				}
				break;
			case 0x34:
				if (colordix < 0xE0)
					colordix++;
				else 
				{
					colordix = 0;
				}
				if (colordix <= 1) 
				{
					dynamic_rgb_conf(0xFF,0xFF,0x00);
				} 
				else 
				{
					dynamic_rgb_conf(0x00,0x00,0x00);
				}
				break;
			case 0x35:
				if (colordix < 0xE0)
					colordix++;
				else 
				{
					colordix = 0;
				}
				if (colordix <= 1) 
				{
					dynamic_rgb_conf(0x00,0xFF,0xFF);
				} 
				else 
				{
					dynamic_rgb_conf(0x00,0x00,0x00);
				}
				break;
			case 0x36:
				if (colordix < 0xE0)
					colordix++;
				else 
				{
					colordix = 0;
				}
				if (colordix <= 1) 
				{
					dynamic_rgb_conf(0xFF,0x00,0xFF);
				} 
				else 
				{
					dynamic_rgb_conf(0x00,0x00,0x00);
				}
				break;
			case 0x37:
				if (colordix < 0xE0)
					colordix++;
				else 
				{
					colordix = 0;
				}
				if (colordix <= 1) 
				{
					dynamic_rgb_conf(0xFF,0xFF,0xFF);
				} 
				else 
				{
					dynamic_rgb_conf(0x00,0x00,0x00);
				}
				break;
			case 0x38:
				SevenColorJump();
				break;
			case MODE_FACTORY:
				Aging_mode();
				break;
			case MODE_LIGHT_TEST:
				TestMode();
				break;
	    	case MODE_RESET:
         		light_mode_factory();
				break;
			case 0xA1:
				CCT_sunrise_func(&SetColor);
				break;
			case 0xA2:
				CCT_sunset_func(&SetColor);
				break;
		}
	}
}


void PWM_init()
{
    timer_hdl_t *p = &zg_timer_pwm_hdl;
    u32 ccr;
    
    p->config.idx = TIM_PWM;
    p->config.prescaler = ZG_PWM_PRESCALER_SET;
    p->config.period = ZG_PWM_PERIOD_SET;      
    USER_DBG("ptimer->config.period:%d\n",p->config.period);
    //basic user callback
    p->it.basic_user_cb.func = NULL;
    p->it.basic_user_cb.context = NULL;

    s907x_hal_timer_base_init(p);
    s907x_hal_timer_start(p); 

    s907x_hal_pinmux_swd_off();

    ccr = 0;
    //channel r
    s907x_hal_timer_pwm_init(p, ZG_PWM_POLARITY_ZERO, ccr, ZG_PWM_CHANNEL_R);
    s907x_hal_timer_start_pwm(p, ZG_PWM_CHANNEL_R);

    s907x_hal_timer_pwm_init(p, ZG_PWM_POLARITY_ZERO, ccr, ZG_PWM_CHANNEL_G);
    s907x_hal_timer_start_pwm(p, ZG_PWM_CHANNEL_G);

    s907x_hal_timer_pwm_init(p, ZG_PWM_POLARITY_ZERO, ccr, ZG_PWM_CHANNEL_B);
    s907x_hal_timer_start_pwm(p, ZG_PWM_CHANNEL_B); 
    
    s907x_hal_timer_pwm_init(p, ZG_PWM_POLARITY_ZERO, ccr, ZG_PWM_CHANNEL_W);
    s907x_hal_timer_start_pwm(p, ZG_PWM_CHANNEL_W); 

    s907x_hal_timer_pwm_init(p, ZG_PWM_POLARITY_ZERO, ccr, ZG_PWM_CHANNEL_C);
    s907x_hal_timer_start_pwm(p, ZG_PWM_CHANNEL_C);    

}

void light_task(void *arg)
{
  	printf("\nZG system:Light init\n");
  	PWM_init();
 	// remote_init();
  	Light_ReadConf();
  	timer_clock_init();  
  	while(1)
  	{
      	vTaskDelay(2 / portTICK_PERIOD_MS);
      	light_param_store_handle();
      	light_func_handle();
  	}
  	vTaskDelete(NULL);
}

void light_init()
{
  xTaskCreate(&light_task, "light_task", LIGHT_TASK_STACK_SZ, NULL, LIGHT_TASK_PRIO, &g_light_task_hdl);
}

TaskHandle_t light_get_task_handler(void)
{
    return g_light_task_hdl;
}

void led_system_init()
{
    light_init();
}
