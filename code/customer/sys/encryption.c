
#include "sys/ZG_system.h"

mbedtls_aes_context aes;
uint8_t key_Val[32]={0};
/*
 * PKCS7
 */
static void  PKCS7_Pad(uint8_t * buf,int buflen,int blocksize,uint8_t * paddingBuf)
{
    int i;

    int p = blocksize - buflen % blocksize;
    for(i = 0;i < buflen;i++){

        paddingBuf[i] = buf[i];
    }

    for(i= buflen;i < blocksize;i++){

        paddingBuf[i] = p;
    }

}
void  Set_AES_key_InFlash(uint8_t *key)
{
	uint8_t padding[32];
	if(strlen((char*)key) != 16){
		
		PKCS7_Pad(key,strlen((char*)key),16,padding);
		memcpy(key_Val,padding,16);
	}else{
		memcpy(key_Val,key,16);		
	}

}
void  AES_Data_Func_init()
{

	//if(strlen((char *)key_Val) == 0){
	
		Set_AES_key_InFlash((uint8_t *)"ff234567bbabcdaa");
	//}
   // printf("AES KEY:%s\n",key_Val);

}
void Get_AES_key(uint8_t *key)
{
	memcpy(key,key_Val,16);
}

uint8_t AES_encrypt(uint8_t *src,uint8_t *dest)
{
	uint8_t IV[16] = {0};
	uint8_t padding[32] = {0};
	uint8_t buf[50] = {0};
	if(key_Val == NULL)
		return 1;
	mbedtls_aes_init(&aes );

	PKCS7_Pad(src,strlen((char*)src),16,padding);
	memcpy(IV,key_Val,16);
	
	mbedtls_aes_setkey_enc(&aes, key_Val, 128 );
	mbedtls_aes_crypt_cbc(&aes, MBEDTLS_AES_ENCRYPT,16, IV,padding,buf );
	sprintf((char *)dest,"%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X", 
            buf[0],buf[1],buf[2],buf[3],buf[4],buf[5],buf[6],buf[7],
			buf[8],buf[9],buf[10],buf[11],buf[12],buf[13],buf[14],buf[15]);

	return 0;
}
