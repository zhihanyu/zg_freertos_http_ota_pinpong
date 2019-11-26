#include "s907x.h"
#include "kv_api.h"
#include "utils.h"

#if defined (__CC_ARM)

int atoi(const char *src)
{
		int s = 0;
		bool isMinus = false;

		while(*src == ' ') 
		{
				src++; 
		}

		if(*src == '+' || *src == '-')
		{
				if(*src == '-')
				{
						isMinus = true;
				}
				src++;
		}
		else if(*src < '0' || *src > '9')  
		{
				s = 2147483647;
				return s;
		}

		while(*src != '\0' && *src >= '0' && *src <= '9')
		{
				s = s * 10 + *src - '0';
				src++;
		}
		return s * (isMinus ? -1 : 1);
 }
 
 #endif


static inline char *_strpbrk(const char *cs, const char *ct)
{
    const char *sc1 = cs, *sc2;

    for (; *sc1 != '\0'; ++sc1) {
        for (sc2 = ct; *sc2 != '\0'; ++sc2) {
            if (*sc1 == *sc2)
                return (char *)sc1;
        }
    }
    return NULL;
}

char *strsep(char **s, const char *ct)
{
    char *sbegin = *s;
    char *end;
    if (sbegin == NULL)
        return NULL;
    end = _strpbrk(sbegin, ct);
    if (end)
        *end++ = '\0';
    *s = end;
    return sbegin;
}

static int flash_write_core(u8 * const flash_buf, u32 addr, u8 *pbuf, int len)
{
    u32 secpos = 0;
    u32 secoff = 0;
    u32 wr_addr;
    int wr_len=0;
    int ret = AT_RET_ERR;
    u8 *ptr = flash_buf;

    if (NULL == flash_buf) {
        ptr = wl_malloc(SECTOR_SIZE);
        if(!ptr) {
            return ret;
        }
    }
    
    ret = AT_RET_OK;
    while(len){
        secpos = addr / SECTOR_SIZE;
        secoff = addr % SECTOR_SIZE;
        wr_addr = secpos * SECTOR_SIZE ;
        
        if(len <= (SECTOR_SIZE - secoff)){
            wr_len = len;
        }
        else{
            wr_len = SECTOR_SIZE - secoff;
        }
        
        s907x_hal_flash_read_resume_tick(secpos * SECTOR_SIZE, ptr, SECTOR_SIZE);
        memcpy(ptr + secoff, pbuf, wr_len);
        s907x_hal_flash_erase_resume_tick(EraseSector, secpos * SECTOR_SIZE); 
        s907x_hal_flash_write_resume_tick(wr_addr, ptr, SECTOR_SIZE);
        
        addr += wr_len;
        pbuf += wr_len;
        len -= wr_len;
    }

    if (NULL == flash_buf) {
        wl_free(ptr);
    }
    
    return ret;
}

//flash read
void s907x_flash_read(u32 addr, u8 *pbuf, int len)
{
    s907x_hal_flash_read_resume_tick(addr,pbuf,len);
}

//flash write with erase
int s907x_flash_write(u32 addr, u8 *pbuf, int len)
{
    if(addr < FLASH_DATA_USER_START || addr > FLASH_DATA_USER_END) {
        return AT_RET_ERR;
    }

    return flash_write_core(NULL, addr, pbuf, len);
}

//start a task 
int s907x_start_action_task(thread_func_t func, void *ctx)
{
	char name[32];

	sprintf(name, "action %x\n", func);
	wl_create_thread(name, ACTION_TASK_STACK_SZ, ACTION_TASK_PRIO,func, ctx);

	return 0;
}

run_mode_e get_s907_run_mode(void)
{
	u32 result;
	int err;
    run_mode_e ret;

	err = s907x_read_kv(S907X_RSVD_KEY_RUNMODE, (u8*)&result, sizeof(result));
	if(err) {
		result = RUN_MODE_NORMAL_MASK;
		printf("s907x set default run mode...\n");
		s907x_write_kv(S907X_RSVD_KEY_RUNMODE, (u8*)&result, sizeof(result));
	}
    if(result == RUN_MODE_MP_MASK) {
        ret = s907x_mode_mp;
        printf("s907x mp mode...\n");
    } else if(result == RUN_MODE_NORMAL_MASK) {
        ret = s907x_mode_normal;
        printf("s907x normal mode...\n");
    } else if(result == RUN_MODE_TEST_MASK) {
        ret = s907x_mode_test;
        printf("s907x test mode...\n");
    } else {
        ret = s907x_mode_normal;
        printf("s907x normal mode...\n");
    }

	return ret;
}

int set_s907_run_mode(run_mode_e ret)
{
	u32 result;
	int err;
    run_mode_e mode; 

    printf(" ready to set mode %d", ret);

    mode = get_s907_run_mode();
    
    //same mode no switch
    if(mode == ret) {
        return AT_RET_ERR;
    }

    if(ret == s907x_mode_mp) {
        result = RUN_MODE_MP_MASK;
    } else if(ret == s907x_mode_normal) {
        result = RUN_MODE_NORMAL_MASK;
    } else if(ret == s907x_mode_test) {
        result = RUN_MODE_TEST_MASK;
    } else {
        result = RUN_MODE_NORMAL_MASK;
    }

	err = s907x_write_kv(S907X_RSVD_KEY_RUNMODE, (u8*)&result, sizeof(result));

    return err;
}




//support commad
//test start
//test stop
//mp start
//mp stop
int mode_switch_hdl(void *context)
{
	char *rxbuf = (char*)context;  
    int ret;

    if(strstr(rxbuf, MP_START_CMD)) {
        ret = set_s907_run_mode(s907x_mode_mp);
    } else if(strstr(rxbuf, TEST_START_CMD)) {
        ret = set_s907_run_mode(s907x_mode_test);
    } else if(strstr(rxbuf, MP_STOP_CMD) || 
              strstr(rxbuf, TEST_STOP_CMD)) {
        ret = set_s907_run_mode(s907x_mode_normal);
    } else {
       //other commad
       ret = AT_RET_ERR;
    }
    if(!ret) {
        wl_os_mdelay(50);
        NVIC_SystemReset();
	}
	return ret;
}




