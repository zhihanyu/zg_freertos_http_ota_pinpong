#include "s907x.h"
#include "kv_test.h"
#include "kv_types.h"
#include "kv_adapt.h"
#include "kv_api.h"

#ifndef AT_TEST_SETTAG
    #define AT_TEST_SETTAG                  ','
#endif
   
extern kv_item_t *kv_item_traverse(item_func func, uint8_t blk_idx, const char *key);

#if M_AT_TEST

//example
/* 
 * how to use:
 * 1, kv set:
 *      send:   AT+KV=0,my_tel,17621157113
 *      resp:       kv set key:my_tel|value:17621157113, return:0
 *      send:   AT+KV=0,my_name,Steven
 *      resp:       kv set key:my_name|value:Steven, return:0
 *      send:   AT+KV=0,my_age,18
 *      resp:       kv set key:my_age|value:18, return:0
 * 2, kv get:
 *      send:   AT+KV=1,my_tel
 *      resp:       kv get success, result: key:my_tel|value:17621157113|len:11
 * 3, kv delete
 *      send:   AT+KV=2,my_tel
 *      resp:       kv del success
 * 4, kv list
 *      send:   AT+KV=3
 *      resp:       kv list:
 *      resp:       my_name = Steven
 *      resp:       my_age = 18
 *      
 * */
hal_test_name_map_t kv_test_map[] = 
{
	{0, "KV_SET"},
	{1, "KV_GET"},
	{2, "KV_DEL"},
    {3, "KV_LIST"},
    {4, "KV_UNITTEST"},
};

static bool assert_para_num( uint32_t real_num, uint32_t wish_num )
{
    if( real_num > wish_num )
    {
        printf(" too many paras\r\n");
        return 1;
    }
    else if( real_num < wish_num )
    {
        printf(" too few paras\r\n");
        return 1;
    }       
    else
    {
        return 0;
    }   
}

static int __item_print_cb(kv_item_t *item, const char *key)
{
    kv_size_t off;

    char *p_key = NULL;
    char *p_val = NULL;

    p_key = (char *)kv_malloc(item->hdr.key_len + 1);
    if (!p_key) {
        return KV_ERR_MALLOC_FAILED;
    }

    memset(p_key, 0, item->hdr.key_len + 1);
    off = item->pos + KV_ITEM_HDR_SIZE;
    kv_flash_read(off, p_key, item->hdr.key_len);

    p_val = (char *)kv_malloc(item->hdr.val_len + 1);
    if (!p_val) {
        kv_free(p_key);
        return KV_ERR_MALLOC_FAILED;
    }

    memset(p_val, 0, item->hdr.val_len + 1);
    off = item->pos + KV_ITEM_HDR_SIZE + item->hdr.key_len;
    kv_flash_read(off, p_val, item->hdr.val_len);

    printf("%s = %s\r\n", p_key, p_val);
    kv_free(p_key);
    kv_free(p_val);

    return KV_LOOP_CONTINUE;
}
 
static void unit_test_task( void* para )
{
    int    ret = 0;
    int32_t i = 0;    

    char *key_header = "key_test_header";
    char *value_header = "value_test_header";
    char *key = NULL;
    char *value = NULL;
    char *value_out = NULL;
    int32_t value_len = 128;

    key = wl_zmalloc( 128 );
    if( !key ) printf("malloc failed \r\n");
    value = wl_zmalloc( 128 );
    if( !value ) printf("malloc failed \r\n");
    value_out = wl_zmalloc( 128 );
    if( !value_out ) printf("malloc failed \r\n");

    while(1)
    {
        memset( key, 0, 128 );
        memset( value, 0, 128 );
        sprintf( key, "%s-%05d", key_header, i);
        sprintf( value, "%s-%05d", value_header, i);

        ret = kv_item_set( key, value, strlen(value) );
        if( ret  )
        {
            if( ret == KV_ERR_NO_SPACE )
            {
                printf(" test set stop, kv partition is full\r\n" );
            }
            else
            {
                printf(" test over, set ret:%d, cnt:%d\r\n", ret, i );
            }

            break;
        }                    
        wl_os_mdelay( 10 );

        memset( value_out, 0, 128 );

        value_len = 128;
        ret = kv_item_get( key, value_out, &value_len );
        if( ret )
        {
            printf(" test over, get ret:%d\r\n", ret );
            break;
        }                    
        if( strcmp( value, value_out ) )
        {
            printf(" test over, str not compare :write:%s, read:%s, cnt:%d\r\n", value, value_out, i );
            break;
        }

        if( ret  || i >= 0xffff )
        {
            printf(" test over, cnt:%d\r\n", i );
            break;
        }

        printf("test:%d compare ok, key:%s, value:%s\r\n", i, key, value_out);

        i++;
        
        wl_os_mdelay( 10 );
    }

    i--;
    for( ; ; i--)
    {
        memset( key, 0, 128 );
        sprintf( key, "%s-%05d", key_header, i);                
        ret = kv_item_delete( key );
        if( ret )
        {
            printf(" test over, delete ret:%d\r\n", ret );
            break;
        }
        printf("delete:%d ok, key:%s\r\n", i, key);
        wl_os_mdelay( 10 );
        if( i == 0 )
        {
            printf(" test over\r\n" );
            break;
        }
    }

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
    if( value_out )
    {
        wl_free( value_out );
        value_out = NULL;
    }   

    wl_destory_threadself();
    
    return;
}

void kv_test( void *context )
{
    char  *argv[AT_SET_MAX_ARGC];
    int    argc;
    int    index;
    int    ret = 0;
    int32_t i = 0;

    at_cmd_t *cmd = (at_cmd_t *)context;

    argc   = at_parse_set(cmd->set, argv, AT_TEST_SETTAG); 

    index = atoi(argv[0]);

    if( index >= (sizeof(kv_test_map)/sizeof(hal_test_name_map_t)) )
    {
        printf( "kv test para 1 error\r\n" );
        return;
    }

	switch( index ) 
	{
		case KV_SET:
            {
                if( assert_para_num( argc, 3 ) )
                    return;

                char *key = argv[1];
                char *value = argv[2];    

                ret = kv_item_set( key, value, strlen( value ) );
                printf( "kv set key:%s|value:%s, return:%d\r\n",key, value, ret );    
            }        
			break;
		case KV_GET:
            {
                if( assert_para_num( argc, 2 ) )
                    return;

                char *key = argv[1];

                int value_len = KV_MAX_VAL_LEN;
                char *value = NULL;
                
                value = wl_malloc( value_len );
                if(!value) {
                    printf( "kv mem alloc failed\r\n" );
                    return;
                }  

                ret = kv_item_get( (const char *)key, (void *)value, (int32_t *)&value_len );
                if( ret == -10004 )
                {
                    printf( "kv get failed, result: not found this '%s' key\r\n", key );
                }
                else if( ret == -10001 )
                {
                    printf( "kv get failed, result: size is too small to storage key '%s''value\r\n", key );
                }
                else if( ret == 0 )
                {
                    printf( "kv get success, result: key:%s|value:%s|len:%d\r\n", key, value, value_len );
                }
                else 
                {
                    printf( "kv get failed, return: %d\r\n", ret );
                }

                if( value )
                {
                    wl_free( value );
                    value = NULL;
                }	
            }		
			break;
		case KV_DEL:    
            {    
                if( assert_para_num( argc, 2 ) )
                    return;

                char *key = argv[1];

                ret = kv_item_delete( key );
                if( ret == -10004 )
                {
                    printf( "kv del failed, result: not found this '%s' key\r\n", key );
                }
                else if( ret == 0 )
                {
                    printf( "kv del success\r\n" );
                }
                else
                {
                    printf( "kv del failed, return: %d\r\n", ret );
                }
            } 
            break;
        case KV_LIST:
            {
                if( assert_para_num( argc, 1 ) )
                    return;
                printf( "kv list:\r\n" );
                for (i = 0; i < KV_BLOCK_NUMS; i++) {
                    kv_item_traverse((item_func)__item_print_cb, i, NULL);
                }                
            }
            break;
        case KV_UNITTEST:
            {
                wl_create_thread( "kv unittest", 2048, 11, unit_test_task, NULL );
            }

            break;     
		default:
			HAL_TEST_DBG("no such case\n");
			break;
	}
    
    
}

#endif