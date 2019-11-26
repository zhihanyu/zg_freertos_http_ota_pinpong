
#include "sys/ZG_system.h"
#include "Sun_func.h"

static unsigned char sun_flag = 0;

static unsigned char Val_begin, Val_end;
static colort_struct_t Color_Val;
static unsigned int riseT, downT;
static unsigned int sunST, sunET, sunAT;

void CCT_sunset_func(colort_struct_t* color)
{
    sunET = GET_SYSTEM_TIME() - sunST;
    sunST = GET_SYSTEM_TIME();
    sunAT += sunET;

    if (sunAT >= downT) {
        sunAT = 0;
        if (get_led_mode() == MODE_SUNSET && get_led_switch() == 0x23) { //sun rise
            if (sun_flag == WARM_SUN_FUNC || sun_flag == CCT_SUN_FUNC) { // W Val or both
                if (Color_Val.W > Val_end) {
                    Color_Val.W--;
                }
            }
            if (sun_flag == COLD_SUN_FUNC || sun_flag == CCT_SUN_FUNC) { //C Val or both
                if (Color_Val.C > Val_end) {
                    Color_Val.C--;
                }
            }
            printf("(sun):sun down:%d W-C %d-%d\n", GET_SYSTEM_TIME(), Color_Val.W, Color_Val.C);
            if (sun_flag == WARM_SUN_FUNC || sun_flag == CCT_SUN_FUNC) { // W Val or both
                if (Color_Val.W == 0) { //Val is 0 when sun funtion end.
				   
                    set_led_switch(0x24);
                    //struct rtc_time RTCtime;
                    //get_RTC_time(&RTCtime);
                    //printf("(sun):[%d:%d:%d]sun down done\n",RTCtime.nHour,RTCtime.nMin,RTCtime.nSec);
                    if (sun_flag == CCT_SUN_FUNC) //色温
                    {
                        record_cct(Val_begin / 2,Val_begin / 2);
                    } else {
                        record_cct(Val_begin / 2,0);
                    }
                    set_led_mode(MODE_STATIC);
                } else if (Color_Val.W <= Val_end) { //Val is not 0 when sun funtion end.
                    //struct rtc_time RTCtime;
                    //get_RTC_time(&RTCtime);
                    //printf("(sun):[%d:%d:%d]sun down done\n",RTCtime.nHour,RTCtime.nMin,RTCtime.nSec);
                    set_led_mode(MODE_STATIC);
                    if (sun_flag == WARM_SUN_FUNC) {
                        Color_Val.C = 0;
                    }
                }
            } else if (sun_flag == COLD_SUN_FUNC) //冷白
            { //C Val
                if (Color_Val.C == 0) //结束时亮度值为0
                {
				    
                    set_led_switch(0x24);
                    record_cct(0,Val_begin / 2);
                    set_led_mode(MODE_STATIC);
                } else if (Color_Val.C <= Val_end) {
                    set_led_mode(MODE_STATIC);
                }
            }
        } else if (get_led_switch() == 0x24) {
            set_led_mode(MODE_STATIC);
        } else {
        }
    }
    memcpy(color, &Color_Val, sizeof(colort_struct_t));
}
//Sunf_store_flag(bool flag);
void CCT_sunrise_func(colort_struct_t* color)
{
    sunET = GET_SYSTEM_TIME() - sunST;
    sunST = GET_SYSTEM_TIME();
    sunAT += sunET;

    if (sunAT >= riseT) {
        sunAT = 0;
        if (get_led_mode() == MODE_SUNRISE && get_led_switch() == 0x23) { //sun up
            if (sun_flag == WARM_SUN_FUNC || sun_flag == CCT_SUN_FUNC) {
                if (Color_Val.W < Val_end) {
                    Color_Val.W++;
                }
            }
            if (sun_flag == COLD_SUN_FUNC || sun_flag == CCT_SUN_FUNC) {
                if (Color_Val.C < Val_end) {
                    Color_Val.C++;
                }
            }
            //printf("(sun):sun rise:%d W-C %d-%d\n",GET_SYSTEM_TIME(),Color_Val.W,Color_Val.C);
            if (sun_flag == WARM_SUN_FUNC || sun_flag == CCT_SUN_FUNC) {
                if (Color_Val.W >= Val_end) {
                    // struct rtc_time RTCtime;
                    //get_RTC_time(&RTCtime);
                    //printf("(sun):[%d:%d:%d]sun rise done\n",RTCtime.nHour,RTCtime.nMin,RTCtime.nSec);
                    set_led_mode(MODE_STATIC);
                    if (sun_flag == WARM_SUN_FUNC) {
                        Color_Val.C = 0;
                    }
                }
            } else if (sun_flag == COLD_SUN_FUNC) {
                if (Color_Val.C >= Val_end) {
                    //struct rtc_time RTCtime;
                    //get_RTC_time(&RTCtime);
                    //printf("(sun):[%d:%d:%d]sun rise done\n",RTCtime.nHour,RTCtime.nMin,RTCtime.nSec);
                    set_led_mode(MODE_STATIC);
                    Color_Val.W = 0;
                }
            }
        } else if (get_led_switch() == 0x24) {
            set_led_mode(MODE_STATIC);
        } else {
        }
    }
    memcpy(color, &Color_Val, sizeof(colort_struct_t));
}

void CCT_sunrise_init(unsigned char Val_flag, unsigned char delay_t, unsigned char begin_buf, unsigned char end_buf)
{

    if (begin_buf - end_buf == 0)
        return;

    //struct rtc_time RTCtime;
    //get_RTC_time(&RTCtime);
    //printf("(sun):%d:%d:%d sun rise start\n",RTCtime.nHour,RTCtime.nMin,RTCtime.nSec);

    set_led_switch(0x23);
    sun_flag = Val_flag;
    Val_begin = begin_buf;
    Color_Val.ColorFlag = Dsp_OnlyCCT;

    printf("(sun):sun_flag:%d\n", sun_flag);

    switch (sun_flag) {

    case WARM_SUN_FUNC:
        Color_Val.W = Val_begin;
        Color_Val.C = 0;
        Val_end = end_buf;
        break;
    case COLD_SUN_FUNC:
        Color_Val.C = Val_begin;
        Color_Val.W = 0;
        Val_end = end_buf;
        break;
    case CCT_SUN_FUNC:
        Color_Val.W = Val_begin / 2;
        Color_Val.C = Val_begin / 2;
        Val_end = end_buf / 2;
        break;
    }
    if (sun_flag == CCT_SUN_FUNC) {
        riseT = delay_t * 120000 / (end_buf - Val_begin);
    } else {
        riseT = delay_t * 60000 / (end_buf - Val_begin);
    }
    set_led_mode(MODE_SUNRISE);
    printf("(sun):delay time:%d\n", riseT);
    printf("(sun):begin_val:%d\n", Val_begin);
    printf("(sun):end_val:%d\n", Val_end);
}

void CCT_sunset_init(unsigned char Val_flag, unsigned char delay_t, unsigned char begin_buf, unsigned char end_buf)
{
    if (begin_buf - end_buf == 0)
        return;

    //struct rtc_time RTCtime;
   // get_RTC_time(&RTCtime);
    //printf("(sun):[%d:%d:%d]sun down start\n",RTCtime.nHour,RTCtime.nMin,RTCtime.nSec);

    set_led_switch(0x23);

    Val_begin = begin_buf;
    sun_flag = Val_flag;
    Color_Val.ColorFlag = Dsp_OnlyCCT;

    switch (sun_flag) {
    case WARM_SUN_FUNC:
        Color_Val.W = Val_begin;
        Color_Val.C = 0;
        Val_end = end_buf;
        break;
    case COLD_SUN_FUNC:
        Color_Val.C = Val_begin;
        Color_Val.W = 0;
        Val_end = end_buf;
        break;
    case CCT_SUN_FUNC:
        Color_Val.W = Val_begin / 2;
        Color_Val.C = Val_begin / 2;
        Val_end = end_buf / 2;
        break;
    }
    if (sun_flag == CCT_SUN_FUNC) {
        downT = delay_t * 120000 / (Val_begin - end_buf);
    } else {
        downT = delay_t * 60000 / (Val_begin - end_buf);
    }
    printf("(sun):delay time:%d\n", downT);
    printf("(sun):begin_val:%d\n", Val_begin);
    printf("(sun):end_val:%d\n", Val_end);
    set_led_mode(MODE_SUNSET);
}
