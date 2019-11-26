#ifndef __CLOCK_H__
#define __CLOCK_H__


#ifdef __cplusplus
extern "C" {
#endif

void  timer_clock_init();
void timer_clock_Conf(uint8_t *dat,int len);
uint8_t * check_timer_clock_data();
void timer_clock_start();

#ifdef __cplusplus
}
#endif


#endif 
