#include "s907x.h"
#include "cal_test.h"
 

typedef struct regular_val_
{
    u8 freqcal[2];
    u8 tempcal;
    u8 powercal[21];
    u16 debug;
}regular_val_t;




u8 s907x_char2num(u8 a)
{
    if (a >= '0' && a <='9')
    {
        return (int)(a - '0');
    }
    if (a >= 'a' && a <= 'f')
    {
        return (int)(a - 'a') + 10;
    }
    if (a >= 'A' && a <= 'F')
    {
        return (int)(a - 'A') + 10;
    }
    return 0xff;
}


static u8 char2num_hex(u8 h,u8 l)
{
	return ((s907x_char2num(h)<<4)|(s907x_char2num(l)));
}



void s907x_set_regular(void)
{
	int err;
    u8 freqcal[2] = {0x04,0x97};
    u8 tempcal = 0x1D;
    u8 powercal[21]={0x2C,0x2C,0x2C,0x2C,0x2C,0x2C,0x1E,0x1E,0x1E,0x1E,0x1E,0x23,0x23,0x22,0x23,0x25,0x1F,0x1F,0x1F,0x20,0x20};     


    regular_val_t val;
    
    err = s907x_read_kv(S907X_RSVD_KEY_CALVAL, (u8*)&val, sizeof(regular_val_t));
	if(err) {
        memcpy(val.freqcal,freqcal,2);
        memcpy(&val.tempcal,&tempcal,1);
        memcpy(val.powercal,powercal,21);	
		s907x_write_kv(S907X_RSVD_KEY_CALVAL, (u8*)&val, sizeof(regular_val_t));
	}
	
    s907x_wlan_set_regular_cali(&val);
}


static void s907x_regval_read(void)
{
    u8 i;
	int err;
    regular_val_t regval;


    err = s907x_read_kv(S907X_RSVD_KEY_CALVAL, (u8*)&regval, sizeof(regular_val_t));
	if(err) {
		printf("kv not found please set first!\r\n");
		return;
	}
    printf("freq = 0x%02x 0x%02x \r\n",regval.freqcal[0],regval.freqcal[1]);
    printf("tempcal = 0x%02x\r\n",regval.tempcal);
    printf("powercal:\r\n");
    
    for(i=0;i<21;i++) {
        printf("0x%02x \r\n",regval.powercal[i]);
    }
    printf("\r\n");
}


//2+1+21 = 24
static void s907x_regval_write(int argc,char **argv)
{
    u8 i,cnt;
    u8 *pch;
    regular_val_t regval;

    if ( argc < 1 ) {
        printf("Invalid command\r\n" );
        return;
    }

    pch = (u8 *)argv[1];
    i = 0;
  
    s907x_read_kv(S907X_RSVD_KEY_CALVAL, (u8*)&regval, sizeof(regular_val_t));

    if(strncmp(argv[0], "freqcal",7) == 0){
        for(cnt = 0; cnt < 2; cnt++) {
            regval.freqcal[cnt] = char2num_hex(pch[i],pch[i+1]);
            i+=2;
        }
    }else if(strncmp(argv[0], "tempcal",7) == 0){
        for(cnt = 0; cnt < 1; cnt++) {
            regval.tempcal = char2num_hex(pch[i],pch[i+1]);
            i+=2;
        }
    }else if(strncmp(argv[0], "powercal",8) == 0){
        for(cnt = 0; cnt < 21; cnt++) {
            regval.powercal[cnt] = char2num_hex(pch[i],pch[i+1]);
            i+=2;
        }
    } else if(strncmp(argv[0], "regval",6) == 0){
        for(cnt = 0; cnt < 2; cnt++) {
            regval.freqcal[cnt] = char2num_hex(pch[i],pch[i+1]);
            i+=2;
        }
        for(cnt = 0; cnt < 1; cnt++) {
            regval.tempcal = char2num_hex(pch[i],pch[i+1]);
            i+=2;
        }
        for(cnt = 0; cnt < 21; cnt++) {
            regval.powercal[cnt] = char2num_hex(pch[i],pch[i+1]);
            i+=2;
        }
    }
    regval.debug = 0;
 
    s907x_write_kv(S907X_RSVD_KEY_CALVAL, (u8*)&regval, sizeof(regular_val_t));
}



void regcali_main(int argc, char **argv)
{
    if(strcmp( argv[0], "write" ) == 0 ) {
        s907x_regval_write(argc-1,argv+1);
    }else if ( strcmp( argv[0], "read" ) == 0 ) {
        s907x_regval_read();	
    }
}
