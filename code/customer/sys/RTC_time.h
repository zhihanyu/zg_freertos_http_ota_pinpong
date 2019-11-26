
#ifndef __RTC_TIME_H__
#define __RTC_TIME_H__



#ifdef __cplusplus
extern "C" {
#endif


struct rtc_time {
	int tm_sec;
	int tm_min;
	int tm_hour;
	int tm_mday;
	int tm_mon;
	int tm_year;
	int tm_wday;
};

void RTC_time_Conf(struct rtc_time *time_set);
void get_RTC_time(struct rtc_time *RTC_return);


#ifdef __cplusplus
}
#endif


#endif /*#ifndef __TCP_PERF_H__*/


