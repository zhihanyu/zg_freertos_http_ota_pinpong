#ifndef _ENCRYPTION_H_
#define _ENCRYPTION_H_


#ifdef __cplusplus
extern "C" {
#endif
	
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
	
void  AES_Data_Func_init();
void Get_AES_key(uint8_t *key);
uint8_t AES_encrypt(uint8_t *src,uint8_t *dest);
    
#ifdef __cplusplus
}
#endif



#endif 