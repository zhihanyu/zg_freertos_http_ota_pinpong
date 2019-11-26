

#include "sys/ZG_system.h"

#define FEBRUARY		2
#define	STARTOFTIME		1970
#define SECDAY			86400L         
#define SECYR			(SECDAY * 365)
#define	leapyear(year)		((year) % 4 == 0)
#define	days_in_year(a) 	(leapyear(a) ? 366 : 365)
#define	days_in_month(a) 	(month_days[(a) - 1])

int month_days[12] = {	31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

static uint64_t cla_time;
static uint64_t basic_time;

#define RTC_MAGIC 0x55aaaa55
typedef struct {
	uint64_t time_acc;
	uint32_t magic ;
	uint32_t time_base;
}RTC_TIMER_DEMO;


struct rtc_time systmtime;
/*
 *
 * 闁荤姳绶ょ槐鏇㈡偩婵犳艾绀傛い鎺炴嫹鐎归潻鎷�
 */
void  GregorianDay(struct rtc_time * tm)
{
	int leapsToDate;
	int lastYear;
	int day2;
	int MonthOffset[] = { 0,31,59,90,120,151,181,212,243,273,304,334 };

	lastYear=tm->tm_year-1;


	leapsToDate = lastYear/4 - lastYear/100 + lastYear/400;


	if((tm->tm_year%4==0) &&
	   ((tm->tm_year%100!=0) || (tm->tm_year%400==0)) &&
	   (tm->tm_mon>2)) {
		/*
		 * We are past Feb. 29 in a leap year
		 */
		day2=1;
	} else {
		day2=0;
	}

	day2 += lastYear*365 + leapsToDate + MonthOffset[tm->tm_mon-1] + tm->tm_mday;

	tm->tm_wday=day2%7;
}

/*
 * 闂佺娉涢敃銉ヮ渻閸岀偞鈷掔痪鎯ь儐缁侇喖鈽夐幘鍓佹瘒inx闂佸搫鍟悥鐓幬涢崸妤�绠ｉ柨鐕傛嫹
 */
uint32_t  mktimev(struct rtc_time *tm)
{
	if (0 >= (int) (tm->tm_mon -= 2)) {	/* 1..12 -> 11,12,1..10 */
		tm->tm_mon += 12;		/* Puts Feb last since it has leap day */
		tm->tm_year -= 1;
	}

	return (((
		(uint32_t) (tm->tm_year/4 - tm->tm_year/100 + tm->tm_year/400 + 367*tm->tm_mon/12 + tm->tm_mday) +
			tm->tm_year*365 - 719499
	    )*24 + tm->tm_hour /* now have hours */
	  )*60 + tm->tm_min /* now have minutes */
	)*60 + tm->tm_sec-8*60*60; /* finally seconds */


}

/*
 * 闂佺娉涢〃顥篊闁诲酣娼х�氼剟鎮洪妸鈺侀棷闁靛ě鍕殸闂佽桨鐒﹀姗�鏁撻崐鐔奉棈婵炴潙妫欑粙澶愭倻濡椿妲梻鍌氬亰閹凤拷
 *
 */
void  to_tm(uint32_t tim, struct rtc_time * tm)
{
	register uint32_t    ri;
	register long   hms, day1;

	day1 = tim / SECDAY;
	hms = tim % SECDAY;

	/* Hours, minutes, seconds are easy */
	tm->tm_hour = hms / 3600;
	tm->tm_min = (hms % 3600) / 60;
	tm->tm_sec = (hms % 3600) % 60;

	/* Number of years in days */
	for (ri = STARTOFTIME; day1 >= days_in_year(ri); ri++) {
		day1 -= days_in_year(ri);
	}
	tm->tm_year = ri;

	/* Number of months in days left */ /*鐟滅増娲濋崝鍐嚇閿濆牆鐨哥痪鏉胯嫰瀹曠敻鎳橀悢绋跨疇缁炬澘鐭侀崜楣冩嚇椤撯�冲閻犱警鍨甸崝锟�*/
	if (leapyear(tm->tm_year)) {
		days_in_month(FEBRUARY) = 29;
	}
	for (ri = 1; day1 >= days_in_month(ri); ri++) {
		day1 -= days_in_month(ri);
	}
	days_in_month(FEBRUARY) = 28;
	tm->tm_mon = ri;

	/* Days are what is left over (+1) from all that. *//*鐟滅増娲濋崝鍐嚇閿濆牆鐨哥痪鏉胯嫰瀹曠敻鎳橀悢绋跨疇闁煎濮鹃崝妤呮嚇濠靛﹤濮�*/
	tm->tm_mday = day1 + 1;

	/*
	 * Determine the day of week
	 */
	GregorianDay(tm);

}
static uint64_t get_time;
rtc_hdl_t g_zg_rtc_hdl;
void rtc_basic_init()
{
	
	memset((void*)&g_zg_rtc_hdl, 0, sizeof(g_zg_rtc_hdl));
    //init clk_sel & zone
    g_zg_rtc_hdl.config.clk_sel = RTC_CLOCKSEL_I32K;
    g_zg_rtc_hdl.config.zone = 8;
    s907x_hal_rtc_init(&g_zg_rtc_hdl);
    //set init_time use basic or unix time
    g_zg_rtc_hdl.config.init_time.year 	= 1900;
    g_zg_rtc_hdl.config.init_time.month 	= 0;
    g_zg_rtc_hdl.config.init_time.day 	= 0;
    g_zg_rtc_hdl.config.init_time.hour 	= 0;
    g_zg_rtc_hdl.config.init_time.min 	= 0;
    g_zg_rtc_hdl.config.init_time.sec 	= 0;
    s907x_hal_rtc_set_basictime(&g_zg_rtc_hdl);
   // s907x_hal_rtc_get_time(&rtc_hdl);
}
void  RTC_time_Conf(struct rtc_time *time_set)
{
	rtc_basic_init();
	s907x_hal_rtc_get_time(&g_zg_rtc_hdl);
	cla_time = g_zg_rtc_hdl.real_time.hw_time;
  	//cla_time = ZG_system_time();
  	GregorianDay(time_set);
  	basic_time = mktimev(time_set);
  	printf("rtc init : %d : %d : %d\n", time_set->tm_hour,time_set->tm_min,time_set->tm_sec);
}

void  get_RTC_time(struct rtc_time *RTC_return)
{
	s907x_hal_rtc_get_time(&g_zg_rtc_hdl);
	get_time=g_zg_rtc_hdl.real_time.hw_time;
   	//uint64_t current_time = basic_time + (ZG_system_time() - cla_time) / 1000;
   	uint64_t current_time = basic_time + (get_time - cla_time) ;
  	to_tm(current_time + 8 * 60 * 60, RTC_return);
  
  	if(RTC_return->tm_wday == 0)
  	{
	   	RTC_return->tm_wday = 7;
	}
}




