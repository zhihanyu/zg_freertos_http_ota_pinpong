
#include "sys/ZG_system.h"


static void remote_thread(void *arg)
{
    uint8_t key = 0xff;
    float h = 0,s = 0,v = 0,s_r = 0,h_r = 0;
    sort_t sort_step = 0xff;
    while(1)
    { 
        get_remote_key(&key,portMAX_DELAY);

        //printf("remote:key:%02X addr %04X\n",key,get_remote_addr());
        if(get_remote_addr() == 0x00ff)
        {
            colort_struct_t color;
            memset(&color,0,sizeof(colort_struct_t));
            if(ZG_system_time()/1000 <= 8 && sort_step != 0xff)
            {
                printf("remote:learn sort...\n");
                if( GET_IR_Repeat() >= 15 &&  GET_IR_Repeat() <= 50)
                {//对排序成功
                    printf("remote:learn sort success\n");
                    set_led_mode(0x37);
                    set_led_speed(1);
                    set_color_output_sort(sort_step);
                    vTaskDelay(3000 / portTICK_PERIOD_MS); 
                    color.R = 0xff;
                    color.G = 0xff;
                    color.B = 0xff;
                    Set_color(&color);  
                    sort_step = 0xff;
                }
            }
            set_report_reson(HARDWARE_CONTROL);
            if(key == 0x0d)
            {
                if(get_led_switch() == MODE_OFF)
                {
                    set_led_switch(MODE_ON);
                }
            }
            else
            {
                if(get_led_switch() == MODE_ON)
                {
                    switch(key)
                    {
                        case 0x09:
                            if(get_led_mode() == MODE_MUSIC)
                            {
                                set_led_mode(MODE_STATIC);
                            }
                            if(get_led_mode() == MODE_STATIC)
                            {
                                convertRGBtoHSV(get_red_value(),get_green_value(),get_blue_value(), &h,  &s, &v);
                                if(press_count() == 1)
                                {
                                    s_r = s;
                                    h_r = h;
                                }
                                if(v < 0.9) 
                                    v += 0.1;
                                else     
                                    v = 1;
                                convertHSVtoRGB(h_r, s_r, v, &color.R, &color.G, &color.B);
                                Set_color(&color);

                            }
                            else
                            {
                                if(get_led_speed() > 3)
                                {       
                                    set_led_speed(get_led_speed() - 3);
                                }
                                else if(get_led_speed() == 3 || get_led_speed() == 2) 
                                {
                                    set_led_speed(0x01);
                                }
                            }
                            break;
                        case 0x1d:
                            if(get_led_mode() == MODE_MUSIC)
                            {
                                set_led_mode(MODE_STATIC);
                            }
                            if(get_led_mode() == MODE_STATIC)
                            {
                                convertRGBtoHSV(get_red_value(),get_green_value(),get_blue_value(),  &h,  &s, &v);
                                if(press_count() == 1)
                                {
                                    s_r = s;
                                    h_r = h;
                                }
                                if(v > 0.2)
                                    v -= 0.1;
                                else    
                                    v = 0.01;
                                convertHSVtoRGB(h_r, s_r, v, &color.R, &color.G, &color.B);
                                Set_color(&color);
                            }
                            else
                            {
                                if(get_led_speed() <= 28)
                                {                              
                                    set_led_speed(get_led_speed() + 3);
                                }
                                else if(get_led_speed() == 30 || get_led_speed() == 29)
                                {   
                                    set_led_speed(31);
                                }
                            }
                            break;                    
                        case 0x1f:
                            set_led_switch(MODE_OFF);           
                            break;
                        case 0x15:
                            set_led_mode(MODE_STATIC);
                            color.R = 0xff;
                            color.G = 0xff;
                            color.B = 0xff;
                            Set_color(&color);                                
                            break;
                        case 0x19:   
                            set_led_mode(MODE_STATIC);
                            color.R = 0xff;
                            color.G = 0x00;
                            color.B = 0x00;
                            Set_color(&color);
                            if(ZG_system_time() / 1000 <= 8)
                            {
                                sort_step = RGB_SORT;                           
                            }                       
                            break;  
                        case 0x1b:
                            set_led_mode(MODE_STATIC);
                            color.R = 0x00;
                            color.G = 0xff;
                            color.B = 0x00;
                            Set_color(&color);
                            if(ZG_system_time() / 1000 <= 8)
                            {
                                sort_step = GRB_SORT;
                            }
                            break;    
                        case 0x11:
                            set_led_mode(MODE_STATIC);
                            color.R = 0x00;
                            color.G = 0x00;
                            color.B = 0xff;
                            Set_color(&color);
                            if(ZG_system_time() / 1000 <= 8)
                            {
                                sort_step = BRG_SORT;
                            }
                            break;    
                        case 0x17:
                            set_led_mode(MODE_STATIC);
                            color.R = 223;
                            color.G = 32;
                            color.B = 0;
                            Set_color(&color);                    
                            break;       
                        case 0x12:
                            set_led_mode(MODE_STATIC);
                            color.R = 0x00;
                            color.G = 233;
                            color.B = 32;
                            Set_color(&color);                    
                            break;                          
                        case 0x16:
                            set_led_mode(MODE_STATIC);
                            color.R = 32;
                            color.G = 0;
                            color.B = 233;
                            Set_color(&color);                     
                            break;   
                        case 0x40:
                            set_led_mode(MODE_STATIC);
                            color.R = 191;
                            color.G = 64;
                            color.B = 00;
                            Set_color(&color);                  
                            break;   
                        case 0x4c:
                            set_led_mode(MODE_STATIC);
                            color.R = 0;
                            color.G = 191;
                            color.B = 64;
                            Set_color(&color);                      
                            break;  
                        case 0x04:
                            set_led_mode(MODE_STATIC);
                            color.R = 64;
                            color.G = 0;
                            color.B = 191;
                            Set_color(&color);                     
                            break;
                        case 0x0a:
                            set_led_mode(MODE_STATIC);
                            color.R = 159;
                            color.G = 96;
                            color.B = 00;
                            Set_color(&color);                   
                            break;  
                        case 0x1e:
                            set_led_mode(MODE_STATIC);
                            color.R = 0;
                            color.G = 159;
                            color.B = 96;
                            Set_color(&color);                       
                            break;        
                        case 0x0e:
                            set_led_mode(MODE_STATIC);
                            color.R = 159;
                            color.G = 0;
                            color.B = 96;
                            Set_color(&color);                      
                            break;    
                        case 0x1c:
                            set_led_mode(MODE_STATIC);
                            color.R = 127;
                            color.G = 128;
                            color.B = 0;
                            Set_color(&color);                 
                            break; 
                        case 0x14:
                            set_led_mode(MODE_STATIC);
                            color.R = 0;
                            color.G = 127;
                            color.B = 128;
                            Set_color(&color);                 
                            break;    
                        case 0x0f:
                            set_led_mode(MODE_STATIC);
                            color.R = 128;
                            color.G = 0;
                            color.B = 127;
                            Set_color(&color);                 
                            break;  
                        case 0x4d:
                            set_led_mode(0x38);
                            break;  
                        case 0x00:
                            set_led_mode(0x37);
                            break;   
                        case 0x1a:
                            set_led_mode(0x25);
                            break;   
                        case 0x0c:
                            set_led_mode(MODE_RGB);
                            break;                                                                                                                                                                                                 
                    }
                }
            }
        }
    }
    vTaskDelete(NULL);
}

void remote_init()
{
    IR_Init();
    xTaskCreate(&remote_thread, "remote_thread", LIGHT_REMOTE_TASK_STACK_SZ, NULL, LIGHT_REMOTE_TASK_PRIO, NULL);
}