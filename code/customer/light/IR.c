
#include "sys/ZG_system.h"




static xQueueHandle key_queue;
static unsigned int key_addr;
static uint8_t rep_flg;
uint8_t ir_repeat =  0;
uint8_t repeat_num;

static queue_t  cap_queue;

typedef struct ir_st_
{
    int capture_stop;  
    u8  buf[4];
    u8  buf_id;
    u16 capture_idx;
    u16 state;
    int rep_cnt;
    int repeat_state;
    u8 repeat_key;
}ir_t;

static ir_t g_ir;

#define TEST_LOG   FALSE


#include "hal_gpio.h"
#include "hal_timer.h"

#define TIM_PRESCALER_NEC       19
#define TIM_PERIOD_NEC          65535

 
#define CAP_FIRST_RISING    0
#define CAP_FIRST_FALLING   1
#define CAP_FIRST           CAP_FIRST_RISING
 
#define CAP_PERIOD          33

static  timer_hdl_t tim_hdl;


//500k = 0.5us 
#define IR_DATA(x)    (x*5ul)

#if TEST_LOG
u16 test_cap[256];
u8  cap_zero_map[32];
u8  cap_zero_pos[32];
#endif


unsigned int get_remote_addr()
{
    return key_addr;
}

unsigned char GET_IR_Repeat()
{
    return ir_repeat;
}

int get_remote_key(unsigned char *key,unsigned int time)
{
    return (xQueueReceive(key_queue, key, time));
}

static void ir_recv_data(u8 zero)
{
    int pos  = g_ir.buf_id / IR_NEC_BIT_NUM;    
    //USER_DBG("[%d %d %d]\n", g_ir.buf_id, zero, g_ir.buf[pos]);
    if(pos >= sizeof(g_ir.buf)) 
    {
        return;
    }
    g_ir.buf[pos] = g_ir.buf[pos] << 1;
    g_ir.buf[pos] += zero;
   
    
#if TEST_CAP
    cap_zero_map[g_ir.buf_id] = zero;
    cap_zero_pos[g_ir.buf_id] = g_ir.buf[pos];
#endif    
    g_ir.buf_id++;  
    
}

static u8 rebuild_pos(u8 data)
{
    u8 ret = 0;
    u8 temp;
   
    ret = data & 0x01;// 0000 0001
    ret = ret >> 0;
    temp |= ret;
    temp = temp << 1;
   
    ret = data & 0x02;// 0000 0010
    ret = ret >> 1;
    temp |= ret;
    temp = temp << 1;
   
    ret = data & 0x04;//0000 0100
    ret = ret >> 2;
    temp |= ret;
    temp = temp << 1;
   
    ret = data & 0x08; //0000 1000
    ret = ret >> 3;
    temp |= ret;
    temp = temp << 1;
   
    ret = data & 0x10; //0001 0000
    ret = ret >> 4;
    temp |= ret;
    temp = temp << 1;
   
    ret = data & 0x20;//0010 0000
    ret = ret >> 5;
    temp |= ret;
    temp = temp << 1;
   
    ret = data & 0x40;//0100 0000
    ret = ret >> 6;
    temp |= ret;
    temp = temp << 1;
   
    ret = data & 0x80;//1000 0000
    ret = ret >> 7;
    temp |= ret;
   
    return temp;
   
}

static void ir_capture_restart(void)
{
#if TEST_LOG
    USER_DBG("IR RESTART.\n");
#endif
    
    g_ir.capture_idx = 0;
    g_ir.buf_id = 0;
    g_ir.rep_cnt = 0;
    g_ir.repeat_state = -1;
    g_ir.state = IR_NEC_STATE_IDLE;
    g_ir.capture_stop = FALSE;
    g_ir.rep_cnt = 0;
    ir_repeat = 0;
    
    
}

static void ir_op_init(void)
{
    g_ir.capture_idx = 0;
    g_ir.capture_stop = FALSE;
}

static void ir_hsm(u16 temp)
{
    u32 val = IR_DATA(temp);

#if TEST_LOG
    USER_DBG("state = %d ir_hsm val %d\n", g_ir.state, temp);  
#endif
    switch(g_ir.state) 
    {
        case IR_NEC_STATE_IDLE:
            if(val > (IR_NEC_TM_PRE_US - IR_NEC_MAGIN_US * 2) && val < (IR_NEC_TM_PRE_US + IR_NEC_MAGIN_US * 2)) 
            {
                g_ir.state = IR_NEC_STATE_CMD;
            }
            else
            {
            #if TEST_LOG
                USER_DBG("head error.\n"); 
            #endif
                g_ir.buf_id = 0;
            }
            break;
        case IR_NEC_STATE_CMD:
           
            if(val > (IR_NEC_D0_TM_US - IR_NEC_MAGIN_US) && val < (IR_NEC_D0_TM_US + IR_NEC_MAGIN_US)) 
            {
                ir_recv_data(0);
            } 
            else if(val > (IR_NEC_D1_TM_US - IR_NEC_MAGIN_US) && val < (IR_NEC_D1_TM_US + IR_NEC_MAGIN_US)) 
            {
                ir_recv_data(1);
            } 
            else 
            {
               #if TEST_LOG
                USER_DBG("g_ir.capture_idx:%d\ng_ir.buf_id:%d\nval:%d\n",g_ir.capture_idx,g_ir.buf_id,val);
               #endif
            }
            if(g_ir.buf_id >= 32) 
            {
                g_ir.capture_stop = TRUE;
                g_ir.state = IR_NEC_STATE_REPEAT;
                //g_ir.state = IR_NEC_STATE_IDLE;
                
                #if TEST_LOG
                printf_arrary(cap_zero_map, sizeof(cap_zero_map), ARY_U8, 0);
                printf_arrary(cap_zero_pos, sizeof(cap_zero_pos), ARY_U8, 0);
                printf_arrary((u8*)test_cap, CAP_PERIOD*2, ARY_U16, 0);
              
                #endif    
                //store key value
                g_ir.buf[0] = rebuild_pos(g_ir.buf[0]);
                g_ir.buf[1] = rebuild_pos(g_ir.buf[1]);
                g_ir.buf[2] = rebuild_pos(g_ir.buf[2]);
                g_ir.buf[3] = rebuild_pos(g_ir.buf[3]);
                
                key_addr = g_ir.buf[0];
                key_addr = key_addr << 8;
                key_addr |= g_ir.buf[1];
                g_ir.repeat_key = g_ir.buf[2];
                g_ir.capture_stop = FALSE;

                #if TEST_LOG
                USER_DBG("addr:%x, faddr:%x, cmd:%x, fcmd:%x\n",g_ir.buf[0],g_ir.buf[1],g_ir.buf[2],g_ir.buf[3]);
                #endif

                g_ir.repeat_state = 0;
                xQueueGenericSend(key_queue,&g_ir.repeat_key,0, queueSEND_TO_BACK);
            }
            break;
        case IR_NEC_STATE_REPEAT:   
            #if TEST_LOG
            USER_DBG("VAL:%d\n",val);
            #endif
            g_ir.rep_cnt++;
            switch(g_ir.repeat_state)
            {
                case 0:
                    if(val > IR_NEC_REP_TM1_US  &&  val < IR_NEC_REP_TM1_US * 20)
                    {
                        g_ir.repeat_state = 1;
                    }
                    else
                    {
                        goto ir_start;
                    }
                    break;

                case 1:
                    if(val < IR_NEC_REP_TM1_US *  6 + IR_NEC_MAGIN_US &&  val > IR_NEC_REP_TM2_US * 5 - IR_NEC_MAGIN_US)
                    {
                        repeat_num = ir_repeat;
                        ir_repeat++;
                    #if TEST_LOG
                        USER_DBG("REPEAT:%d\n",ir_repeat);
                    #endif
                        xQueueGenericSend(key_queue,&g_ir.repeat_key,0, queueSEND_TO_BACK);
                        g_ir.repeat_state = 0;
                    }
                    else
                    {
                        goto ir_start;
                    }
                    break;

                case 2:

                    break;

                default:

                    break;
                
                ir_start:
                    g_ir.repeat_state = -1;
                    g_ir.state = IR_NEC_STATE_IDLE;
                    g_ir.buf_id = 0;
                    g_ir.capture_idx = 0;          
            }
          
    } 
}

static int wl_send_queue_fromisr(queue_t* queue, void* message)
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    return xQueueSendFromISR((QueueHandle_t)*queue, message, &xHigherPriorityTaskWoken);
}
static void timer_capture_gpio7(void *context)
{
    timer_hdl_t *tim = context;
    static u16 cap;
    if(g_ir.capture_stop) 
    {
        return; 
    }
    cap = s907x_hal_timer_get_counter(tim);
    wl_send_queue(&cap_queue, (void*)&cap, 10);
    //wl_send_queue_fromisr(&cap_queue, (void*)&cap);
}


void ir_init(void)
{
    timer_hdl_t *tim = &tim_hdl;
    tim->config.idx = TIM_CAP;

    memset(&g_ir, 0, sizeof(g_ir));

    s907x_hal_timer_stop(tim);

    tim->config.prescaler = TIM_PRESCALER_NEC;
    tim->config.period = TIM_PERIOD_NEC;
    tim->config.int_enable = FALSE;
 
    tim->it.basic_user_cb.func = NULL;
    tim->it.basic_user_cb.context = tim;

    s907x_hal_timer_base_init(tim);
    s907x_hal_timer_start_base(tim);

    gpio_init_t init;
    init.mode = GPIO_MODE_INT_FALLING;
    init.pull = GPIO_PULLUP;
    
    s907x_hal_gpio_init(GPIO7, &init);
    s907x_hal_gpio_it_start( GPIO7, timer_capture_gpio7, tim);   
}
 


static u8 inverse(u8 data)
{
    u8 t_h,t_l;
    
    t_h = HI_UINT8(data);
    t_l = LO_UINT8(data);   
  
    return BUILD_UINT8(t_h, t_l);
}


static void ir_task(void *context)
{  
    u16 cap_val;
    static u16 cap_0;
    static u16 cap_1;
    int ret = -1;
    u16 temp;
    ir_init();
    ir_op_init();
    wl_init_queue(&cap_queue, sizeof(void *), 0xff); 
    
    while(1)
    {
        ret = wl_wait_queue(&cap_queue, (void*)&cap_val, 200);   
        if(!ret)
        {
            if(g_ir.state == IR_NEC_STATE_REPEAT)
            {
                if(g_ir.rep_cnt > (ir_repeat + 1))
                {
                    ir_capture_restart();
                }
              
            }
        }
        if(g_ir.capture_idx % 2 == 0)
        {
            cap_0 = cap_val;
            if(g_ir.capture_idx > 1)
            {
                temp = ((cap_0 >= cap_1) ? (cap_0 - cap_1) : (0xFFFF - cap_1 + cap_0));
            }
        }
        else
        {
            cap_1 = cap_val;
            temp = ((cap_1 >= cap_0) ? (cap_1 - cap_0) : (0xFFFF - cap_0 + cap_1));
        }
        ir_hsm(temp); 
        g_ir.capture_idx++;
    }
}



void ir_entry(void)
{
    key_queue = xQueueCreate(1, 1);
    wl_create_thread("ir thread", 512, MQTT_TEST_PRIO, (thread_func_t)ir_task, NULL);   
}

void IR_Init()
{
    ir_entry();
}


