#ifndef ZG_IR_H
#define ZG_IR_H

#define GPIO_EINT_PORT			GPIO_PORT_A
#define GPIO_EINT_PIN			GPIO_PIN_22


#define IR_POTOCOL_NEC          0
#define IR_POTOCOL_RC5          1  //not support yet 

#define IR_NEC_STATE_IDLE       0
#define IR_NEC_STATE_PRE        1
#define IR_NEC_STATE_CMD        2
#define IR_NEC_STATE_REPEAT     3

//500k = 0.5us 
#define IR_DATA(x)              (x*5ul)

#define IR_NEC_BIT_NUM 		    8
#define IR_NEC_MAGIN_US 		800
#define IR_NEC_TM_PRE_US 		135000
#define IR_NEC_D1_TM_US 		22500
#define IR_NEC_D0_TM_US 		11200
#define IR_NEC_REP_TM1_US 	    20000
#define IR_NEC_REP_TM2_US 	    11250
#define IR_NEC_REP_LOW_US 	    2250
#define IR_NEC_REP_CYCLE 	    11000



#define IR_NEC_HEADER_HIGH_US   9000
#define IR_NEC_HEADER_LOW_US 	4500
#define IR_NEC_DATA_HIGH_US 	560
#define IR_NEC_DATA_LOW_1_US 	1690
#define IR_NEC_DATA_LOW_0_US 	560


void IR_Init();
int get_remote_key(unsigned char  *key, unsigned int time);
unsigned char GET_IR_Repeat();
unsigned int  get_remote_addr();
#endif