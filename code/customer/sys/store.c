#include "sys/ZG_system.h"


static int ZG_make_key(char* key_buf, char *key)
{
	if(!key_buf || !key) {
		return -1;
	}
	sprintf(key_buf, "%s", key);
	
	return 0;
}


int  ZG_data_save(store_type_t type,unsigned char *dat)
{
    int return_type = ESP_OK;
	char key[32];
	
    switch(type){

        case WIFI_MODE_STORE :
			ZG_make_key(key, "WIFI_MODE_STORE");
			fdcm_write(key, dat, 1);
            printf("store:wifi mode %d\n",*dat);
            break;
        case COLOR_STORE:
			ZG_make_key(key, "COLOR_STORE");
			fdcm_write(key, dat, 3);
            printf("store:R G B %d %d %d\n",dat[0],dat[1],dat[2]);
            break;
        case CCT_STORE:
			ZG_make_key(key, "CCT_STORE");
			fdcm_write(key, dat, 2);
            printf("store:W %d C %d\n",dat[0],dat[1]);
            break;
        case CLOUD_SWITCH_STORE:
			ZG_make_key(key, "CLOUD_SWITCH_STORE");
			fdcm_write(key, dat, 1);
            printf("store:cloud switch %X\n",*dat);
            break;
        case RESTORE_STORE:
			ZG_make_key(key, "RESTORE_STORE");
			fdcm_write(key, dat, 1);
            printf("store:factory count %d\n",*dat);
            break;
        case LED_SWITCH_STORE:
			ZG_make_key(key, "LED_SWITCH_STORE");
			fdcm_write(key, dat, 1);
            printf("store:led switch %X\n",*dat);
            break;
        case LED_MODE_STORE:
			ZG_make_key(key, "LED_MODE_STORE");
			fdcm_write(key, dat, 1);			
            printf("store:led mode %X\n",*dat);
            break;
        case LED_SPEED_STORE:
			ZG_make_key(key, "LED_SPEED_STORE");
			fdcm_write(key, dat, 1);				
            printf("store:led speed %X\n",*dat);    
            break;
        case LED_SORT_STORE:
			ZG_make_key(key, "LED_SORT_STORE");
			fdcm_write(key, dat, 1);				
            printf("store:led wring type %X\n",*dat);   
            break;
        case REMOTE_ADDR_STORE:
			ZG_make_key(key, "REMOTE_ADDR_STORE");
			fdcm_write(key, dat, 2);				
            printf("store:led remote %X %d\n",dat[0],dat[1]);   
            break;
        case PORT_STORE:
			ZG_make_key(key, "PORT_STORE");
			fdcm_write(key, dat, 2);			
            printf("store:port %X %d\n",dat[0],dat[1]);   
            break;
        case LED_CUSTOM_MODE_STORE:
			ZG_make_key(key, "LED_CUSTOM_MODE_STORE");
			fdcm_write(key, dat, 66);			
            break;
        case CLOCK_STORE:
			ZG_make_key(key, "CLOCK_STORE");
			fdcm_write(key, dat, 90);			
            break;
        case HOST_STORE :
            if(dat[0] > 64)
            {
                return -1;
            }
			ZG_make_key(key, "HOST_STORE_LEN");
			fdcm_write(key, dat, 1);
			
			ZG_make_key(key, "HOST_STORE_VAL");
			fdcm_write(key, dat + 1, dat[0]);		
            break;
        case AES_KEY_STORE :
			ZG_make_key(key, "AES_KEY_STORE");
			fdcm_write(key, dat, 16);				
            break;
        case DEV_SSID_STORE :
            if(dat[0] > 32)
            {
                return -1;
            }
			ZG_make_key(key, "DEV_SSID_STORE_LEN");
			fdcm_write(key, dat, 1);	

			ZG_make_key(key, "DEV_SSID_STORE_VAL");
			fdcm_write(key, dat + 1, dat[0]);
			
            break;
        case DEV_PWD_STORE :
            if(dat[0] > 64)
            {
                return -1;
            }
			ZG_make_key(key, "DEV_PWD_STORE_LEN");
			fdcm_write(key, dat, 1);	

			ZG_make_key(key, "DEV_PWD_STORE_VAL");
			fdcm_write(key, dat + 1, dat[0]);	 

            break;
        case PRINTF_EN_STORE:
			ZG_make_key(key, "PRINTF_EN_STORE");
			fdcm_write(key, dat, 1);	
            break;
        case ST1_STORE:
			ZG_make_key(key, "ST1_STORE");
			fdcm_write(key, dat, 2);
            break; 
        case ST2_STORE:
			ZG_make_key(key, "ST2_STORE");
			fdcm_write(key, dat, 2);
            break; 
		case MAGIC:
			ZG_make_key(key, "MAGIC");
			fdcm_write(key, dat, 4);
			break;
    } 
    return return_type;
}

int ZG_data_read(store_type_t type,unsigned char *dat)
{
    int  return_type = ESP_OK;
	char key[32];
	
    switch(type)
    {
        case WIFI_MODE_STORE :
			ZG_make_key(key, "WIFI_MODE_STORE");
			fdcm_read(key, dat, 1);			
            printf("system read:wifi mode %d\n",dat[0]);
            break;
        case COLOR_STORE:
			ZG_make_key(key, "COLOR_STORE");
			fdcm_read(key, dat, 3);	
            printf("system read:R G B %d %d %d\n",dat[0],dat[1],dat[2]);
            break;
        case CCT_STORE:
			ZG_make_key(key, "CCT_STORE");
			fdcm_read(key, dat, 2);	
            printf("system read:W %d C %d\n",dat[0],dat[1]);
            break;
        case CLOUD_SWITCH_STORE:
			ZG_make_key(key, "CLOUD_SWITCH_STORE");
			fdcm_read(key, dat, 1);	
            printf("system read:cloud switch %X\n",dat[0]);
            break;
        case RESTORE_STORE:
			ZG_make_key(key, "RESTORE_STORE");
			fdcm_read(key, dat, 1);				
            printf("system read:factory count %d\n",dat[0]);
            break;
        case LED_SWITCH_STORE:
			ZG_make_key(key, "LED_SWITCH_STORE");
			fdcm_read(key, dat, 1);					
            printf("system read:led switch:%x\n",dat[0]);
            break;
        case LED_MODE_STORE:
			ZG_make_key(key, "LED_MODE_STORE");
			fdcm_read(key, dat, 1);				
            printf("system read:led mode:%x\n",dat[0]);
            break;
        case LED_SPEED_STORE:
			ZG_make_key(key, "LED_SPEED_STORE");
			fdcm_read(key, dat, 1);	
            printf("system read:led speed:%x\n",dat[0]);
            break;
        case LED_SORT_STORE:
			ZG_make_key(key, "LED_SORT_STORE");
			fdcm_read(key, dat, 1);	
            printf("system read:led sort:%x\n",dat[0]);
            break;
        case REMOTE_ADDR_STORE:
			ZG_make_key(key, "REMOTE_ADDR_STORE");
			fdcm_read(key, dat, 2);				
            printf("system read:remote adrress:%x\n",dat[0] << 8 | dat[1]);
            break;
        case PORT_STORE:
			ZG_make_key(key, "PORT_STORE");
			fdcm_read(key, dat, 2);	
            printf("system read:port:%x\n",dat[0] << 8 | dat[1]);
            break;
        case LED_CUSTOM_MODE_STORE:
			ZG_make_key(key, "LED_CUSTOM_MODE_STORE");
			fdcm_read(key, dat, 66);				
            break;
        case CLOCK_STORE:
			ZG_make_key(key, "CLOCK_STORE");
			fdcm_read(key, dat, 90);	
            break;
        case HOST_STORE :
			ZG_make_key(key, "HOST_STORE_LEN");
			fdcm_read(key, dat, 1);	
			if(dat[0] > 64) {
				return_type = ESP_FAIL;
			} else {
				ZG_make_key(key, "HOST_STORE_VAL");
				fdcm_read(key, dat+1, dat[0]);	
			}	
            break;
        case AES_KEY_STORE :
			ZG_make_key(key, "AES_KEY_STORE");
			fdcm_read(key, dat, 16);	
            break;
        case DEV_SSID_STORE :
			ZG_make_key(key, "DEV_SSID_STORE_LEN");
			fdcm_read(key, dat, 1);	
			if(dat[0] > 32) {
				return_type = ESP_FAIL;
			} else {
				ZG_make_key(key, "DEV_SSID_STORE_VAL");
				fdcm_read(key, dat+1, dat[0]);	
			}
            break;
        case DEV_PWD_STORE :
			ZG_make_key(key, "DEV_PWD_STORE_LEN");
			fdcm_read(key, dat, 1);	
			if(dat[0] > 64) {
				return_type = ESP_FAIL;
			} else {
				ZG_make_key(key, "DEV_PWD_STORE_VAL");
				fdcm_read(key, dat+1, dat[0]);	
			}
            break;
      case PRINTF_EN_STORE:
		    ZG_make_key(key, "PRINTF_EN_STORE");
		    fdcm_read(key, dat, 1); 
            break;
      case ST1_STORE:
			ZG_make_key(key, "ST1_STORE");
			fdcm_read(key, dat, 2); 
			printf("system1 read:%d\n",dat[0] << 8 | dat[1] );
            break; 
      case ST2_STORE:
		  	ZG_make_key(key, "ST2_STORE");
		  	fdcm_read(key, dat, 2); 
            printf("system2 read:%d\n",dat[0] << 8 | dat[1] );
            break; 
		case MAGIC:
			ZG_make_key(key, "MAGIC");
			fdcm_read(key, dat, 4); 
			break;			
      default :
            return_type = ESP_FAIL;
            break;
    }
    return return_type;
}

//system first power on set factory value
static int ZG_default_config()
{

	u8 *zero_buf;
	int i;
	
	zero_buf = wl_zmalloc(512);
	if(!zero_buf) {
		return -1;
	}
	//
	for(i = HOST_STORE; i <ZG_STORE_MAX; i++) {
		ZG_data_save(i, zero_buf);
	}

	wl_free(zero_buf);

	return 0;
}

void ZG_store_init(void)
{
	u32 magic = 0;
	int ret;

	//read magic key
    ret = ZG_data_read(MAGIC, (u8*)&magic);
	if(ret || (magic != MAGIC_VAL)) {
		ZG_default_config();
		magic = MAGIC_VAL;
		ZG_data_save(MAGIC, (u8*)&magic);
	}
}

void ZG_store_deinit(void)
{
	u32 magic = 0;
	
	//destory magic key
	ZG_data_save(MAGIC, (u8*)&magic);
}
