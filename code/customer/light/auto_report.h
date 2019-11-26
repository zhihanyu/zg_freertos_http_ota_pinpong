#ifndef __AUTO_REPORT_H__
#define __AUTO_REPORT_H__


#ifdef __cplusplus
extern "C" {
#endif

typedef struct{

unsigned char switch_status;
unsigned char light_model;
unsigned char speed;
unsigned char r;
unsigned char g;
unsigned char b;
unsigned char w;
unsigned char c;
unsigned char color_flag;
unsigned char wiring_type;
unsigned char ver;
unsigned char reason;


}report_type_t;

typedef enum{

	ACTIVE_QUERY,
	LOCAL_CONTROL,
	CLOUD_CONTROL,
	TIMER_CONTROL,
	HARDWARE_CONTROL,
	RESON_NULL

}report_reson_t;

void set_color_updated_flag(bool val);
void set_report_reson(report_reson_t reson);
void auto_report_init();

#ifdef __cplusplus
}
#endif


#endif 
