#ifndef __PROTOCOL_HANDLE_H__
#define __PROTOCOL_HANDLE_H__



#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    LOCAL_DATA_EVENT = 0,
    CLOUD_DATA_EVENT
} protocol_event_t;

#define COMMAND_TIME_SET         0x10
#define COMMAND_TIME_QUERY       0x11
#define COMMAND_CLOCK_SET        0x21
#define COMMAND_CLOCK_QUERY      0x22
#define COMMAND_COLOR_SET        0x31
#define COMMAND_MUSIC_SET_FAST   0x41
#define COMMAND_MUSIC_SET_SLOW   0x42
#define COMMAND_USERDEFINE_SET   0x51
#define COMMAND_USERDEFINE_QUERY 0x52
#define COMMAND_DYNAMIC_SET      0x61
#define COMMAND_SET_SORT         0x62
#define COMMAND_TEST_MODE        0x63
#define COMMAND_SWTICH           0x71
#define COMMAND_DEVICE_QUERY     0x81



void protocol_data_parser(uint8_t *buff ,uint16_t len,protocol_event_t type);

#ifdef __cplusplus
}
#endif


#endif 
