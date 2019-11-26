#ifndef _LIGHTFUNC__
#define _LIGHTFUNC__
#include "s907x_zg_config.h"

#include "s907x_pwm.h"

#define R_Channel  PWM_GROUP1_CH2
#define G_Channel  PWM_GROUP1_CH3
#define B_Channel  PWM_GROUP2_CH4
#define W_Channel  PWM_GROUP2_CH5
#define C_Channel  PWM_GROUP3_CH6

#define STORE_PER_TIME  3 
#define  PWM_DEPTH 1000
#define  dimming(channel,val) \
	do{\
	   	 if(val==255){\
	   		 HAL_PWM_ChSetDutyRatio(channel, 40000);\
	   	 }else{\
	   		 HAL_PWM_ChSetDutyRatio(channel, val * 156);\
	   	 }\
	  }while(0);

typedef enum 
{
	RGB_SORT = 0x01,
	GRB_SORT,
	BRG_SORT,
}sort_t;

#define  default_sort_type     RGB_SORT

typedef struct 
{
	unsigned char R_ch;
	unsigned char G_ch;
	unsigned char B_ch;
	unsigned char W_ch;
	unsigned char C_ch;

}color_channel_t;


typedef enum{

	Dsp_RGBCCT = 0x5A,
	Dsp_OnlyRGB = 0xf0,
	Dsp_OnlyCCT = 0x0f

}colorflag;

typedef struct {
   
	unsigned char  W;
	unsigned char  C;
	unsigned char  Delay_time;
	unsigned char  Delay_flag;
	unsigned char  R;
	unsigned char  G;
	unsigned char  B;
	unsigned char  music_flag;
	colorflag  ColorFlag;
}colort_struct_t;

enum{
    MODE_ON           = 0x23,
	MODE_OFF          = 0x24,

    MODE_RGB          = 0x39,
    MODE_RESET        = 0x64,
    MODE_FACTORY      = 0x63,
	MODE_LIGHT_TEST   = 0x65,
	MODE_ROUTER       = 0x66,
	MODE_DELAYSWITCH  = 0x67,

    MODE_SUNRISE      = 0xA1,
	MODE_SUNSET       = 0xA2,
	MODE_USER         = 0x60,
	MODE_STATIC       = 0x61,
	MODE_MUSIC        = 0x62
};

enum{
	CUSTOM_STORE = 0x01,
    TIMER_STORE	
};

#define GET_SYSTEM_TIME() wl_get_systemtick()

#define ADDRESS_COLOR_PATEN     16

#define ADDRESS_custom_LEN		70


#define ADDRESS_TIMER_LEN		88

#define DEVICE_CODE				0x35	     
#define	DEVICE_SOFTWARE_VERSION	0x05	
#define DEVICE_COOL_WHITE_VALUE	0		 

#define STATIC_VALUE_SPEED_MIN	0x01
#define STATIC_VALUE_SPEED_MAX	0x1F
#define STATIC_VALUE_MODE_RGB	0xF0
#define STATIC_VALUE_MODE_CT	0x0F
#define STATIC_VALUE_MODE_RGBCT 0x5A


void param_store(unsigned char type);

void led_system_init();
void TestMode();

void record_cct(uint8_t W,uint8_t C);
void Set_color(colort_struct_t *color);
void light_param_store_handle();

void light_init();

unsigned char get_Color_flag();
unsigned char get_red_value();
unsigned char get_green_value();
unsigned char get_blue_value();
unsigned char get_warm_value();
unsigned char get_cold_value();

void set_led_speed(unsigned char speed);
unsigned char get_led_speed();

void set_led_mode(unsigned char mode);
unsigned char get_led_mode();

void set_led_switch(unsigned char sw);
unsigned char get_led_switch();

void set_custom_mode(unsigned char mode);
unsigned char get_custom_mode();

void custom_mode_data_conf(uint8_t *dat);
uint8_t *get_custom_mode_data();

void set_color_output_sort(sort_t val);
sort_t get_color_output_sort();

TaskHandle_t light_get_task_handler(void);

#endif
