#ifndef FOTA_H_
#define FOTA_H_


#ifdef __cplusplus
extern "C" {
#endif

void fota_progress_send(uint8_t val);
uint8_t get_fota_progress();
bool Is_ota_start();

int fota_conf(char *host,int host_len,uint16_t port,char *path,int path_len,char *code,int code_len);
int zg_ota_start();

#ifdef __cplusplus
}
#endif



#endif 
