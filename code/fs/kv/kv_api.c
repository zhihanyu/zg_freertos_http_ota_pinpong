#include "s907x.h"
#include "kv_api.h"
#include "kv_config.h"


const char AT_WLAN_STR_HEADER[] = "WL";
const char CUS_ZG_STR_HEADER[] = "ZG";

#define CHAR_B3_BCD( data )             (((data >> 16) & 0xf) < 10) ? (((data >> 16) & 0xf) + '0') : ((((data >> 16) & 0xf) - 10) + 'A')
#define CHAR_B2_BCD( data )             (((data >> 8) & 0xf) < 10) ? (((data >> 8) & 0xf) + '0') : ((((data >> 8) & 0xf) - 10) + 'A')
#define CHAR_B1_BCD( data )             (((data >> 4) & 0xf) < 10) ? (((data >> 4) & 0xf) + '0') : ((((data >> 4) & 0xf) - 10) + 'A')
#define CHAR_B0_BCD( data )             ((data & 0xf) < 10) ? ((data & 0xf) + '0') : (((data & 0xf) - 10) + 'A')

#define BCD_2_INT( B3,B2,B1,B0 )        ((B0 <= '9') ? (B0 - '0') : ((B0 - 'A') + 10) ) *1 + \
                                        ((B1 <= '9') ? (B1 - '0') : ((B1 - 'A') + 10) ) *0x10 + \
                                        ((B2 <= '9') ? (B2 - '0') : ((B2 - 'A') + 10) ) *0x10*0x10 + \
                                        ((B3 <= '9') ? (B3 - '0') : ((B3 - 'A') + 10) ) *0x10*0x10*0x10

#define BCD_2_CHAR( B1,B0 )             ((B0 <= '9') ? (B0 - '0') : ((B0 - 'A') + 10) ) *1 + \
                                        ((B1 <= '9') ? (B1 - '0') : ((B1 - 'A') + 10) ) *0x10

#define FLASH_LOG_ENABLE        ( 0 )

#ifndef FLASH_LOG_ENABLE
#define FLASH_LOG_ENABLE        ( 0 )
#endif

#if ( FLASH_LOG_ENABLE )
#define flash_log   printf
#else
#define flash_log
#endif

static int flash_set_with_kv(const char *module_str, u32 addr, u8 *pbuf, int len)
{
    char *key = NULL;
    char *value = NULL;
    int i = 0;
    int err = -1;

    uint32_t index = addr - FLASH_DATA_USER_START;
    uint8_t index_bcd[5];

    memset( index_bcd, 0, 5 );

    index_bcd[0] = CHAR_B3_BCD(index);
    index_bcd[1] = CHAR_B2_BCD(index);
    index_bcd[2] = CHAR_B1_BCD(index);
    index_bcd[3] = CHAR_B0_BCD(index);

    key = wl_zmalloc( KV_MODULE_KEY_LENGTH );
    if( key == NULL )
    {
        flash_log("malloc failed \r\n");
        goto exit;
    }
    value = wl_zmalloc( len * 2 + 1 );
    if( key == NULL )
    {
        flash_log("malloc failed \r\n");
        goto exit;
    }

    for( i=0; i<len; i++ )
    {
        value[i*2] = CHAR_B1_BCD(pbuf[i]);
        value[i*2+1] = CHAR_B0_BCD(pbuf[i]);
    }

    sprintf( key, "%s_%s", module_str, index_bcd );

    flash_log("flash set key : %s, value : %s\r\n", key, value);
    err = kv_item_set( key, value, strlen( value ) );
    if( err )
    {
        flash_log("error :%d", err );
    }
    exit:
    if( key )
    {
        wl_free( key );
        key = NULL;
    }
    if( value )
    {
        wl_free( value );
        value = NULL;
    } 

    return err;   
}

static int flash_get_with_kv(const char *module_str, u32 addr, u8 *pbuf, int len)
{
    char *key = NULL;
    char *value = NULL;
    int i = 0;
    int err = -1;
    int32_t value_len = 0;

    uint32_t index = addr - FLASH_DATA_USER_START;
    uint8_t index_bcd[5];

    memset( index_bcd, 0, 5 );

    index_bcd[0] = CHAR_B3_BCD(index);
    index_bcd[1] = CHAR_B2_BCD(index);
    index_bcd[2] = CHAR_B1_BCD(index);
    index_bcd[3] = CHAR_B0_BCD(index);

    key = wl_zmalloc( KV_MODULE_KEY_LENGTH );
    if( key == NULL )
    {
        flash_log("malloc failed \r\n");
        goto exit;
    }

    value_len = len * 2 + 1;
    value = wl_zmalloc( value_len );
    if( key == NULL )
    {
        flash_log("malloc failed \r\n");
        goto exit;
    }

    sprintf( key, "%s_%s", module_str, index_bcd );

    flash_log("flash get key : %s, ", key);
    err = kv_item_get( key, value, &value_len );
    
    if( !err )
    {
        flash_log( "value : %s, real_len:%d, in_len:%d\r\n", value, value_len, len );
        for( i=0; i < value_len / 2 && i < len; i++ )   
        {
            if( value[i*2] >= '0' && value[i*2+1] >= '0' )
            {
                pbuf[i] = BCD_2_CHAR( value[i*2], value[i*2+1] );
                flash_log(" 0x%02X", pbuf[i]);
            }
        }
        flash_log("\r\n");
    }
    else if( err == -10004 )
    {
        flash_log("not found this key\r\n" );
    }
    else
    {
        flash_log("other error\r\n", err );
    }

    exit:
    if( key )
    {
        wl_free( key );
        key = NULL;
    }
    if( value )
    {
        wl_free( value );
        value = NULL;
    } 

    return err;   
}

int at_wlan_flash_write_kv(u32 addr, u8 *pbuf, int len)
{
    return flash_set_with_kv(AT_WLAN_STR_HEADER, addr, pbuf, len);
}

int at_wlan_flash_read_kv(u32 addr, u8 *pbuf, int len)
{
    return flash_get_with_kv(AT_WLAN_STR_HEADER, addr, pbuf, len);
}


int s907x_write_kv(char *key, u8 *pbuf, int len)
{
	int err;
	
	err = kv_item_set( key, pbuf, len);
	
    return err;
}

int s907x_read_kv(char *key, u8 *pbuf, int len)
{	
	int err;

    err = kv_item_get( key, pbuf, &len );
	
	return err;
}




