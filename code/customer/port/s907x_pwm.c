#include "s907x.h"
#include "s907x_pwm.h"
#include "s907x_zg_config.h"


extern timer_hdl_t zg_timer_pwm_hdl;
#if 1
void s907x_hal_time_pwm_msp_init(void *tim, u8 channel) 
{
	switch(channel)
	{
		case 0:
			//PWM0_SEL0(HAL_ON);
			PWM0_SEL1(HAL_ON); 
			//PWM0_SEL2(HAL_ON); 
			// PWM0_SEL4(HAL_ON); 
		break;
		case 1:
			//PWM1_SEL0(HAL_ON);
			//PWM1_SEL1(HAL_ON); 
			PWM1_SEL2(HAL_ON); 
			//PWM1_SEL3(HAL_ON);  
			//PWM1_SEL4(HAL_ON); 
		break;
		case 2:
			PWM2_SEL0(HAL_ON);
			//PWM2_SEL1(HAL_ON); 
			// PWM2_SEL2(HAL_ON);
		break;
		case 3:
			PWM3_SEL0(HAL_ON);
			//PWM3_SEL2(HAL_ON);
		break;
		case 4:
			//PWM4_SEL1(HAL_ON); 
			PWM4_SEL2(HAL_ON);
			//PWM4_SEL3(HAL_ON); 
		break;
		case 5:
			PWM5_SEL2(HAL_ON);
			//PWM5_SEL3(HAL_ON);   
		break;
		#if 1
		case 6:
			PWM6_SEL1(HAL_ON);
			//PWM6_SEL2(HAL_ON);  
		break;
		case 7:
			PWM7_SEL1(HAL_ON);
			//PWM7_SEL2(HAL_ON); 
		break;
		#endif
	}
}

void s907x_hal_time_pwm_msp_deinit(void *tim, u8 channel) 
{

	switch(channel)
	{
		case 0:
			//PWM0_SEL0(HAL_ON);
			PWM0_SEL1(HAL_ON); 
			//PWM0_SEL2(HAL_ON); 
			// PWM0_SEL4(HAL_ON); 
			break;
		case 1:
			//PWM1_SEL0(HAL_ON);
			//PWM1_SEL1(HAL_ON); 
			PWM1_SEL2(HAL_ON); 
			//PWM1_SEL3(HAL_ON);  
			//PWM1_SEL4(HAL_ON); 
			break;
		case 2:
			PWM2_SEL0(HAL_ON);
			//PWM2_SEL1(HAL_ON); 
			// PWM2_SEL2(HAL_ON);
			break;
		case 3:
			PWM3_SEL0(HAL_ON);
			//PWM3_SEL2(HAL_ON);
			break;
		case 4:
			//PWM4_SEL1(HAL_ON); 
			PWM4_SEL2(HAL_ON);
			//PWM4_SEL3(HAL_ON); 
			break;
		case 5:
			PWM5_SEL2(HAL_ON);
			//PWM5_SEL3(HAL_ON);   
			break;

		case 6:
			PWM6_SEL1(HAL_ON);
			//PWM6_SEL2(HAL_ON);  
			break;
		case 7:
			PWM7_SEL1(HAL_ON);
			//PWM7_SEL2(HAL_ON); 
			break;

	}
}

#endif
void HAL_PWM_ChSetDutyRatio(u8 channel, int pusle_p)
{
  float pusle_k;
  u32 pusle;
  
  pusle_k = pusle_p*1.0/(156*255);
  pusle = (int)(zg_timer_pwm_hdl.config.period * pusle_k);
  s907x_hal_timer_pwm_set_ccr(&zg_timer_pwm_hdl,  pusle, channel);
}