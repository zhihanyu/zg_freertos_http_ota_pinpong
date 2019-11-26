
#define GPIO0      BIT(0)
#define GPIO1      BIT(1)
#define GPIO2      BIT(2)
#define GPIO3      BIT(3)
#define GPIO4      BIT(4)
#define GPIO5      BIT(5)
#define GPIO6      BIT(6)
#define GPIO7      BIT(7)

#define GPIO8      BIT(8)
#define GPIO9      BIT(9)
#define GPIO10     BIT(10)
#define GPIO11     BIT(11)


#define GPIO12     BIT(12)
#define GPIO13     BIT(13)
#define GPIO14     BIT(14)
#define GPIO15     BIT(15)
#define GPIO16     BIT(16)
#define GPIO17     BIT(17)
#define GPIO18     BIT(18)
#define GPIO19     BIT(19)
#define GPIO20     BIT(20)
#define GPIO22     BIT(22)
#define GPIO23     BIT(23)


#define GPIO21     BIT(21)

#define GPIO22     BIT(22)
#define GPIO23     BIT(23)
#define GPIONULL   BIT(31)


#define GPIO_AFP_NC	         0xFFFFFFFF

#define GPIOX_AFP_GPIO 		     0



//s9070
//QFN32 19 gpio
//QFN24 11 gpio


#define PIN1      (1)
#define PIN2      (2)
#define PIN3      (3)
#define PIN4      (4)
#define PIN5      (5)
#define PIN6      (6)
#define PIN7      (7)
#define PIN8      (8)
#define PIN9      (9)
#define PIN10     (10)
#define PIN11     (11)
#define PIN12     (12)
#define PIN13     (13)
#define PIN14     (14)
#define PIN15     (15)
#define PIN16     (16)
#define PIN17     (17)
#define PIN18     (18)
#define PIN19     (19)


#define PIN_NO_LAYOUT     (0xFFFFFFFF)


//pin type
#define PIN_POWER    0
#define PIN_GND      1
#define PIN_CHIP_EN  2
#define PIN_GPIO     3
#define PIN_NC       4

#define GPIO_AFP_NULL 0
    

typedef struct
{
    u32 pin;
    u32 type;  //@pin type
    u32 gpio;
    u32 afp;    	
    u32 dir;   //input or output or 
    u32 pupd_normal;
    u32 pupd_sleep;    
} board_pin_t;



#define GPIO0_AFP_GPIO	         0
#define GPIO0_AFP_PWM4           7
#define GPIO0_AFP_WLED		     8
#define GPIO0_AFP_PWM3           16

#define GPIO1_AFP_GPIO           0
#define GPIO1_AFP_LOG_TXD        1
#define GPIO1_AFP_I2C0_SDA       5
#define GPIO1_AFP_PWM6           7
#define GPIO1_AFP_SIC_SCL	     13
#define GPIO1_AFP_I2C1_SDA       15
#define GPIO1_AFP_PWM7		     16

#define GPIO2_AFP_GPIO           0
#define GPIO2_AFP_LOG_RXD        1
#define GPIO2_AFP_I2C0_SCL       5
#define GPIO2_AFP_PWM4    	     7
#define GPIO2_AFP_TIM5_TRIG      11
#define GPIO2_AFP_SIC_SDA        13
#define GPIO2_AFP_I2C1_SCL       15
#define GPIO2_AFP_PWM5           16

#define GPIO3_AFP_GPIO           0
#define GPIO3_AFP_PWM0           7
#define GPIO3_AFP_SWD_CLK        8
#define GPIO3_AFP_I2S_WS         9
#define GPIO3_AFP_TIM_CAP_TRIG   11
#define GPIO3_AFP_PWM1           16

#define GPIO4_AFP_GPIO           0
#define GPIO4_AFP_PWM1           7
#define GPIO4_AFP_SWD_DATA       8
#define GPIO4_AFP_I2S_SD_RX	     9
#define GPIO4_AFP_TIM_CAP_TRIG   11

#define GPIO5_AFP_GPIO     	     0
#define GPIO5_AFP_PWM2		     7
#define GPIO5_AFP_EXT32K  	     8
#define GPIO5_AFP_I2S_MCK	     9
#define GPIO5_AFP_WLED     	     10

#define GPIO6_AFP_GPIO           0
#define GPIO6_AFP_PWM7		     7 	
#define GPIO6_AFP_EXT32K         8
#define GPIO6_AFP_I2S_CLK        9
#define GPIO6_AFP_TIM5_TRIG      11
#define GPIO6_AFP_PWM4           16

#define GPIO7_AFP_GPIO           0
#define GPIO7_AFP_PWM3           7
#define GPIO7_AFP_I2S_SD_TX      9
#define GPIO7_AFP_TIM_CAP_TRIG   11
#define GPIO7_AFP_PWM2		     16
#define GPIO12_AFP_GPIO          0
#define GPIO12_AFP_SPIC_CS_4     4
#define GPIO12_AFP_SPIC_CS_17    17

#define GPIO13_AFP_GPIO          0
#define GPIO13_AFP_SPIC_DATA1    4
#define GPIO13_AFP_SPIC_DATA2    17

#define GPIO14_AFP_GPIO          0
#define GPIO14_AFP_SPIC_DATA2    4
#define GPIO14_AFP_I2C0_SCL      5
#define GPIO14_AFP_PWM0          7
#define GPIO14_AFP_SPIC_DATA1    17

#define GPIO15_AFP_GPIO          0
#define GPIO15_AFP_SPIC_DATA0_4  4
#define GPIO15_AFP_SPIC_DATA0_17 17

#define GPIO16_AFP_GPIO          0
#define GPIO16_AFP_SPIC_CLK	     4
#define GPIO16_AFP_SPIC_DATA3    17

#define GPIO17_AFP_GPIO		     0
#define GPIO17_AFP_SPIC_DATA3    4
#define GPIO17_AFP_I2C0_SDA      5
#define GPIO17_AFP_PWM1		     7
#define GPIO17_AFP_SPIC_CLK      17

#define GPIO18_AFP_GPIO          0
#define GPIO18_AFP_UART0_RXD	 1
#define GPIO18_AFP_SPI1_CLK      2
#define GPIO18_AFP_SPI0_CLK      3
#define GPIO18_AFP_I2C1_SCL      5
#define GPIO18_AFP_I2S_MCK       9

#define GPIO19_AFP_GPIO          0
#define GPIO19_AFP_UART0_CTS	 1
#define GPIO19_AFP_SPI1_CS       2
#define GPIO19_AFP_SPI0_CS       3
#define GPIO19_AFP_I2C0_SDA      5
#define GPIO19_AFP_PWM1          7
#define GPIO19_AFP_I2S_SD_TX     9
#define GPIO19_AFP_TIM5_TRIG     11
#define GPIO19_AFP_PWM2          16

#define GPIO20_AFP_GPIO          0
#define GPIO20_AFP_PWM6          7
#define GPIO20_AFP_EXT32K        8
#define GPIO20_AFP_I2S_SD_RX     9
#define GPIO20_AFP_PWM0          16

#define GPIO22_AFP_GPIO          0
#define GPIO22_AFP_UART0_RTS     1
#define GPIO22_AFP_SPI1_MISO     2
#define GPIO22_AFP_SPI0_MISO     3
#define GPIO22_AFP_I2C0_SCL      5
#define GPIO22_AFP_PWM5          7
#define GPIO22_AFP_I2S_WS        9
#define GPIO22_AFP_PWM1          16

#define GPIO23_AFP_GPIO          0
#define GPIO23_AFP_UART0_TXD     1
#define GPIO23_AFP_SPI1_MOSI     2
#define GPIO23_AFP_SPI0_MOSI     3
#define GPIO23_AFP_I2C1_SDA      5
#define GPIO23_AFP_PWM0	         7 
   
#define GPIO0_AFP_GPIO_EN() 		    s907x_hal_pinmux_config(GPIO0, GPIO0_AFP_GPIO)	  
#define GPIO0_AFP_PWM4_EN() 		    s907x_hal_pinmux_config(GPIO0, GPIO0_AFP_PWM4)      
#define GPIO0_AFP_WLED_EN() 		    s907x_hal_pinmux_config(GPIO0, GPIO0_AFP_WLED)		
#define GPIO0_AFP_PWM3_EN() 		    s907x_hal_pinmux_config(GPIO0, GPIO0_AFP_PWM3)      
    
#define GPIO1_AFP_GPIO_EN() 			s907x_hal_pinmux_config(GPIO1, GPIO1_AFP_GPIO)       
#define GPIO1_AFP_LOG_TXD_EN() 		    s907x_hal_pinmux_config(GPIO1, GPIO1_AFP_LOG_TXD)    
#define GPIO1_AFP_I2C0_SDA_EN() 		s907x_hal_pinmux_config(GPIO1, GPIO1_AFP_I2C0_SDA)   
#define GPIO1_AFP_PWM6_EN() 			s907x_hal_pinmux_config(GPIO1, GPIO1_AFP_PWM6)       
#define GPIO1_AFP_SIC_SCL_EN() 		    s907x_hal_pinmux_config(GPIO1, GPIO1_AFP_SIC_SCL)	 
#define GPIO1_AFP_I2C1_SDA_EN() 		s907x_hal_pinmux_config(GPIO1, GPIO1_AFP_I2C1_SDA)   
#define GPIO1_AFP_PWM7_EN() 			s907x_hal_pinmux_config(GPIO1, GPIO1_AFP_PWM7)		 

#define GPIO2_AFP_GPIO_EN() 			s907x_hal_pinmux_config(GPIO2, GPIO2_AFP_GPIO)       
#define GPIO2_AFP_LOG_RXD_EN() 		    s907x_hal_pinmux_config(GPIO2, GPIO2_AFP_LOG_RXD)    
#define GPIO2_AFP_I2C0_SCL_EN() 		s907x_hal_pinmux_config(GPIO2, GPIO2_AFP_I2C0_SCL)   
#define GPIO2_AFP_PWM4_EN() 			s907x_hal_pinmux_config(GPIO2, GPIO2_AFP_PWM4)    	 
#define GPIO2_AFP_TIM5_TRIG_EN()		s907x_hal_pinmux_config(GPIO2, GPIO2_AFP_TIM5_TRIG)  
#define GPIO2_AFP_SIC_SDA_EN() 		    s907x_hal_pinmux_config(GPIO2, GPIO2_AFP_SIC_SDA)    
#define GPIO2_AFP_I2C1_SCL_EN() 		s907x_hal_pinmux_config(GPIO2, GPIO2_AFP_I2C1_SCL)   
#define GPIO2_AFP_PWM5_EN() 			s907x_hal_pinmux_config(GPIO2, GPIO2_AFP_PWM5) 

#define GPIO3_AFP_GPIO_EN() 			s907x_hal_pinmux_config(GPIO3, GPIO3_AFP_GPIO)       
#define GPIO3_AFP_PWM0_EN() 			s907x_hal_pinmux_config(GPIO3, GPIO3_AFP_PWM0)       
#define GPIO3_AFP_SWD_CLK_EN() 		    s907x_hal_pinmux_config(GPIO3, GPIO3_AFP_SWD_CLK)    
#define GPIO3_AFP_I2S_WS_EN() 		    s907x_hal_pinmux_config(GPIO3, GPIO3_AFP_I2S_WS)     
#define GPIO3_AFP_TIM_CAP_TRIG_EN() 	s907x_hal_pinmux_config(GPIO3, GPIO3_AFP_TIM_CAP_TRIG)  
#define GPIO3_AFP_PWM1_EN() 			s907x_hal_pinmux_config(GPIO3, GPIO3_AFP_PWM1)       
       
#define GPIO4_AFP_GPIO_EN() 			s907x_hal_pinmux_config(GPIO4, GPIO4_AFP_GPIO)       
#define GPIO4_AFP_PWM1_EN() 			s907x_hal_pinmux_config(GPIO4, GPIO4_AFP_PWM1)       
#define GPIO4_AFP_SWD_DATA_EN() 		s907x_hal_pinmux_config(GPIO4, GPIO4_AFP_SWD_DATA)   
#define GPIO4_AFP_I2S_SD_RX_EN() 	    s907x_hal_pinmux_config(GPIO4, GPIO4_AFP_I2S_SD_RX)	 
#define GPIO4_AFP_TIM_CAP_TRIG_EN() 	s907x_hal_pinmux_config(GPIO4, GPIO4_AFP_TIM_CAP_TRIG)  
       

#define GPIO5_AFP_GPIO_EN() 		    s907x_hal_pinmux_config(GPIO5, GPIO5_AFP_GPIO)    	  
#define GPIO5_AFP_PWM2_EN() 		    s907x_hal_pinmux_config(GPIO5, GPIO5_AFP_PWM2)		  
#define GPIO5_AFP_EXT32K_EN() 	        s907x_hal_pinmux_config(GPIO5, GPIO5_AFP_EXT32K)  	  
#define GPIO5_AFP_I2S_MCK_EN() 	        s907x_hal_pinmux_config(GPIO5, GPIO5_AFP_I2S_MCK)		
#define GPIO5_AFP_WLED_EN() 		    s907x_hal_pinmux_config(GPIO5, GPIO5_AFP_WLED)     

#define GPIO6_AFP_GPIO_EN() 			s907x_hal_pinmux_config(GPIO6, GPIO6_AFP_GPIO)       
#define GPIO6_AFP_PWM7_EN() 			s907x_hal_pinmux_config(GPIO6, GPIO6_AFP_PWM7)		 	
#define GPIO6_AFP_EXT32K_EN() 		    s907x_hal_pinmux_config(GPIO6, GPIO6_AFP_EXT32K)     
#define GPIO6_AFP_I2S_CLK_EN() 		    s907x_hal_pinmux_config(GPIO6, GPIO6_AFP_I2S_CLK)    
#define GPIO6_AFP_TIM5_TRIG_EN() 	    s907x_hal_pinmux_config(GPIO6, GPIO6_AFP_TIM5_TRIG)  
#define GPIO6_AFP_PWM4_EN() 			s907x_hal_pinmux_config(GPIO6, GPIO6_AFP_PWM4)     

#define GPIO7_AFP_GPIO_EN() 			s907x_hal_pinmux_config(GPIO7, GPIO7_AFP_GPIO)       
#define GPIO7_AFP_PWM3_EN() 			s907x_hal_pinmux_config(GPIO7, GPIO7_AFP_PWM3)       
#define GPIO7_AFP_I2S_SD_TX_EN() 	    s907x_hal_pinmux_config(GPIO7, GPIO7_AFP_I2S_SD_TX)  
#define GPIO7_AFP_TIM_CAP_TRIG_EN() 	s907x_hal_pinmux_config(GPIO7, GPIO7_AFP_TIM_CAP_TRIG)  
#define GPIO7_AFP_PWM2_EN() 			s907x_hal_pinmux_config(GPIO7, GPIO7_AFP_PWM2)		

#define GPIO12_AFP_GPIO_EN() 		    s907x_hal_pinmux_config(GPIO12, GPIO12_AFP_GPIO)      
#define GPIO12_AFP_SPIC_CS_4_EN()       s907x_hal_pinmux_config(GPIO12, GPIO12_AFP_SPIC_CS_4) 
#define GPIO12_AFP_SPIC_CS_17_EN()      s907x_hal_pinmux_config(GPIO12, GPIO12_AFP_SPIC_CS_17)
       
#define GPIO13_AFP_GPIO_EN() 		    s907x_hal_pinmux_config(GPIO13, GPIO13_AFP_GPIO)      
#define GPIO13_AFP_SPIC_DATA1_EN()      s907x_hal_pinmux_config(GPIO13, GPIO13_AFP_SPIC_DATA1)
#define GPIO13_AFP_SPIC_DATA2_EN()      s907x_hal_pinmux_config(GPIO13, GPIO13_AFP_SPIC_DATA2)
       
#define GPIO14_AFP_GPIO_EN() 		    s907x_hal_pinmux_config(GPIO14, GPIO14_AFP_GPIO)      
#define GPIO14_AFP_SPIC_DATA2_EN()      s907x_hal_pinmux_config(GPIO14, GPIO14_AFP_SPIC_DATA2)
#define GPIO14_AFP_I2C0_SCL_EN() 	    s907x_hal_pinmux_config(GPIO14, GPIO14_AFP_I2C0_SCL)  
#define GPIO14_AFP_PWM0_EN() 		    s907x_hal_pinmux_config(GPIO14, GPIO14_AFP_PWM0)      
#define GPIO14_AFP_SPIC_DATA1_EN()      s907x_hal_pinmux_config(GPIO14, GPIO14_AFP_SPIC_DATA1)
        
#define GPIO15_AFP_GPIO_EN() 			s907x_hal_pinmux_config(GPIO15, GPIO15_AFP_GPIO)          
#define GPIO15_AFP_SPIC_DATA0_4_EN() 	s907x_hal_pinmux_config(GPIO15, GPIO15_AFP_SPIC_DATA0_4)  
#define GPIO15_AFP_SPIC_DATA0_17_EN()   s907x_hal_pinmux_config(GPIO15, GPIO15_AFP_SPIC_DATA0_17) 

#define GPIO16_AFP_GPIO_EN() 			s907x_hal_pinmux_config(GPIO16, GPIO16_AFP_GPIO)       
#define GPIO16_AFP_SPIC_CLK_EN() 		s907x_hal_pinmux_config(GPIO16, GPIO16_AFP_SPIC_CLK)	 
#define GPIO16_AFP_SPIC_DATA3_EN()  	s907x_hal_pinmux_config(GPIO16, GPIO16_AFP_SPIC_DATA3)  

#define GPIO17_AFP_GPIO_EN() 			s907x_hal_pinmux_config(GPIO17, GPIO17_AFP_GPIO)		 
#define GPIO17_AFP_SPIC_DATA3_EN() 	    s907x_hal_pinmux_config(GPIO17, GPIO17_AFP_SPIC_DATA3) 
#define GPIO17_AFP_I2C0_SDA_EN() 		s907x_hal_pinmux_config(GPIO17, GPIO17_AFP_I2C0_SDA)   
#define GPIO17_AFP_PWM1_EN() 			s907x_hal_pinmux_config(GPIO17, GPIO17_AFP_PWM1)		 
#define GPIO17_AFP_SPIC_CLK_EN() 		s907x_hal_pinmux_config(GPIO17, GPIO17_AFP_SPIC_CLK)

#define GPIO18_AFP_GPIO_EN() 			s907x_hal_pinmux_config(GPIO18, GPIO18_AFP_GPIO)       
#define GPIO18_AFP_UART0_RXD_EN() 	    s907x_hal_pinmux_config(GPIO18, GPIO18_AFP_UART0_RXD)	 
#define GPIO18_AFP_SPI1_CLK_EN() 		s907x_hal_pinmux_config(GPIO18, GPIO18_AFP_SPI1_CLK)   
#define GPIO18_AFP_SPI0_CLK_EN() 		s907x_hal_pinmux_config(GPIO18, GPIO18_AFP_SPI0_CLK)   
#define GPIO18_AFP_I2C1_SCL_EN() 		s907x_hal_pinmux_config(GPIO18, GPIO18_AFP_I2C1_SCL)   
#define GPIO18_AFP_I2S_MCK_EN() 		s907x_hal_pinmux_config(GPIO18, GPIO18_AFP_I2S_MCK)    
     
#define GPIO19_AFP_GPIO_EN() 			s907x_hal_pinmux_config(GPIO19, GPIO19_AFP_GPIO)       
#define GPIO19_AFP_UART0_CTS_EN() 	    s907x_hal_pinmux_config(GPIO19, GPIO19_AFP_UART0_CTS)	 
#define GPIO19_AFP_SPI1_CS_EN() 		s907x_hal_pinmux_config(GPIO19, GPIO19_AFP_SPI1_CS)    
#define GPIO19_AFP_SPI0_CS_EN() 		s907x_hal_pinmux_config(GPIO19, GPIO19_AFP_SPI0_CS)    
#define GPIO19_AFP_I2C0_SDA_EN() 		s907x_hal_pinmux_config(GPIO19, GPIO19_AFP_I2C0_SDA)   
#define GPIO19_AFP_PWM1_EN() 			s907x_hal_pinmux_config(GPIO19, GPIO19_AFP_PWM1)       
#define GPIO19_AFP_I2S_SD_TX_EN() 	    s907x_hal_pinmux_config(GPIO19, GPIO19_AFP_I2S_SD_TX)  
#define GPIO19_AFP_TIM5_TRIG_EN() 	    s907x_hal_pinmux_config(GPIO19, GPIO19_AFP_TIM5_TRIG)  
#define GPIO19_AFP_PWM2_EN() 			s907x_hal_pinmux_config(GPIO19, GPIO19_AFP_PWM2)   

#define GPIO20_AFP_GPIO_EN() 			s907x_hal_pinmux_config(GPIO20, GPIO20_AFP_GPIO)       
#define GPIO20_AFP_PWM6_EN() 			s907x_hal_pinmux_config(GPIO20, GPIO20_AFP_PWM6)       
#define GPIO20_AFP_EXT32K_EN() 		    s907x_hal_pinmux_config(GPIO20, GPIO20_AFP_EXT32K)     
#define GPIO20_AFP_I2S_SD_RX_EN() 	    s907x_hal_pinmux_config(GPIO20, GPIO20_AFP_I2S_SD_RX)  
#define GPIO20_AFP_PWM0_EN() 			s907x_hal_pinmux_config(GPIO20, GPIO20_AFP_PWM0)  

#define GPIO22_AFP_GPIO_EN() 			s907x_hal_pinmux_config(GPIO22, GPIO22_AFP_GPIO)       
#define GPIO22_AFP_UART0_RTS_EN() 	    s907x_hal_pinmux_config(GPIO22, GPIO22_AFP_UART0_RTS)  
#define GPIO22_AFP_SPI1_MISO_EN() 	    s907x_hal_pinmux_config(GPIO22, GPIO22_AFP_SPI1_MISO)  
#define GPIO22_AFP_SPI0_MISO_EN() 	    s907x_hal_pinmux_config(GPIO22, GPIO22_AFP_SPI0_MISO)  
#define GPIO22_AFP_I2C0_SCL_EN()		s907x_hal_pinmux_config(GPIO22, GPIO22_AFP_I2C0_SCL)   
#define GPIO22_AFP_PWM5_EN() 			s907x_hal_pinmux_config(GPIO22, GPIO22_AFP_PWM5)       
#define GPIO22_AFP_I2S_WS_EN() 		    s907x_hal_pinmux_config(GPIO22, GPIO22_AFP_I2S_WS)     
#define GPIO22_AFP_PWM1_EN() 			s907x_hal_pinmux_config(GPIO22, GPIO22_AFP_PWM1)

#define GPIO23_AFP_GPIO_EN() 			s907x_hal_pinmux_config(GPIO23, GPIO23_AFP_GPIO)       
#define GPIO23_AFP_UART0_TXD_EN() 	    s907x_hal_pinmux_config(GPIO23, GPIO23_AFP_UART0_TXD)  
#define GPIO23_AFP_SPI1_MOSI_EN() 	    s907x_hal_pinmux_config(GPIO23, GPIO23_AFP_SPI1_MOSI)  
#define GPIO23_AFP_SPI0_MOSI_EN()	 	s907x_hal_pinmux_config(GPIO23, GPIO23_AFP_SPI0_MOSI)  
#define GPIO23_AFP_I2C1_SDA_EN() 		s907x_hal_pinmux_config(GPIO23, GPIO23_AFP_I2C1_SDA)   
#define GPIO23_AFP_PWM0_EN() 			s907x_hal_pinmux_config(GPIO23, GPIO23_AFP_PWM0)	


//AFP OFF set normal gpio
#define GPIO5_AFP_CLR()         GPIO5_AFP_GPIO_EN() 		  	  	  	
#define GPIO0_AFP_CLR()         GPIO0_AFP_GPIO_EN() 		  
#define GPIO12_AFP_CLR()        GPIO12_AFP_GPIO_EN() 		    
#define GPIO13_AFP_CLR()        GPIO13_AFP_GPIO_EN() 		    
#define GPIO14_AFP_CLR()        GPIO14_AFP_GPIO_EN() 		    
#define GPIO15_AFP_CLR()        GPIO15_AFP_GPIO_EN() 		        
#define GPIO16_AFP_CLR()        GPIO16_AFP_GPIO_EN() 		       
#define GPIO17_AFP_CLR()        GPIO17_AFP_GPIO_EN() 				 
#define GPIO7_AFP_CLR()         GPIO7_AFP_GPIO_EN() 		       
#define GPIO3_AFP_CLR()         GPIO3_AFP_GPIO_EN() 		       
#define GPIO4_AFP_CLR()         GPIO4_AFP_GPIO_EN() 		       
#define GPIO18_AFP_CLR()        GPIO18_AFP_GPIO_EN() 		       
#define GPIO19_AFP_CLR()        GPIO19_AFP_GPIO_EN() 		       
#define GPIO20_AFP_CLR()        GPIO20_AFP_GPIO_EN() 		       	       
#define GPIO22_AFP_CLR()        GPIO22_AFP_GPIO_EN() 		       
#define GPIO23_AFP_CLR()        GPIO23_AFP_GPIO_EN() 		       
#define GPIO6_AFP_CLR()         GPIO6_AFP_GPIO_EN() 		       
#define GPIO2_AFP_CLR()         GPIO2_AFP_GPIO_EN() 		       
#define GPIO1_AFP_CLR()         GPIO1_AFP_GPIO_EN() 

#define PINMUX_SWITCH(enable, on_action, off_action) \
{   \
	if(enable) {	\
		on_action;	\
	}else {			\
		off_action; \
	}				\
}while(0)


#define AFP_ON  1
#define AFP_OFF 0

#define SWD_DATA_SEL0(on)	PINMUX_SWITCH(on,GPIO4_AFP_SWD_DATA_EN(), GPIO4_AFP_CLR());
#define SWD_CLK_SEL0(on)	PINMUX_SWITCH(on,GPIO3_AFP_SWD_CLK_EN(),  GPIO3_AFP_CLR());

//UART0
#define UART0_RX_SEL1(on)   PINMUX_SWITCH(on, GPIO18_AFP_UART0_RXD_EN(), GPIO18_AFP_CLR())
                           
#define UART0_TX_SEL1(on)   PINMUX_SWITCH(on, GPIO23_AFP_UART0_TXD_EN(), GPIO23_AFP_CLR())
                           
#define UART0_CTS_SEL1(on)  PINMUX_SWITCH(on, GPIO19_AFP_UART0_CTS_EN(), GPIO19_AFP_CLR())
                           
#define UART0_RTS_SEL1(on)  PINMUX_SWITCH(on, GPIO22_AFP_UART0_RTS_EN(), GPIO22_AFP_CLR())      
                            
//SPI0                      	
#define SPI0_CLK_SEL1(on) 	PINMUX_SWITCH(on, GPIO18_AFP_SPI0_CLK_EN(), GPIO18_AFP_CLR())
                          
#define SPI0_MOSI_SEL1(on)	PINMUX_SWITCH(on, GPIO23_AFP_SPI0_MOSI_EN(), GPIO23_AFP_CLR())
                            
#define SPI0_MISO_SEL1(on)	PINMUX_SWITCH(on, GPIO22_AFP_SPI0_MISO_EN(), GPIO22_AFP_CLR())
           
#define SPI0_CS_SEL1(on)	PINMUX_SWITCH(on, GPIO19_AFP_SPI0_CS_EN(), GPIO19_AFP_CLR())

//SPI1
#define SPI1_CLK_SEL1(on)   PINMUX_SWITCH(on, GPIO18_AFP_SPI1_CLK_EN(), GPIO18_AFP_CLR())

#define SPI1_MOSI_SEL1(on)  PINMUX_SWITCH(on, GPIO23_AFP_SPI1_MOSI_EN(), GPIO23_AFP_CLR())
                       
#define SPI1_MISO_SEL1(on)  PINMUX_SWITCH(on, GPIO22_AFP_SPI1_MISO_EN(), GPIO22_AFP_CLR())

#define SPI1_CS_SEL1(on)	PINMUX_SWITCH(on, GPIO19_AFP_SPI1_CS_EN(),   GPIO19_AFP_CLR())

//I2C0
#define I2C0_SCL_SEL1(on)	PINMUX_SWITCH(on, GPIO14_AFP_I2C0_SCL_EN(), GPIO14_AFP_CLR())
#define I2C0_SCL_SEL2(on)	PINMUX_SWITCH(on, GPIO22_AFP_I2C0_SCL_EN(), GPIO22_AFP_CLR())
#define I2C0_SCL_SEL3(on)	PINMUX_SWITCH(on, GPIO2_AFP_I2C0_SCL_EN(), GPIO2_AFP_CLR())
                      
#define I2C0_SDA_SEL1(on)	PINMUX_SWITCH(on, GPIO17_AFP_I2C0_SDA_EN(), GPIO17_AFP_CLR())
#define I2C0_SDA_SEL2(on)	PINMUX_SWITCH(on, GPIO19_AFP_I2C0_SDA_EN(), GPIO19_AFP_CLR())
#define I2C0_SDA_SEL3(on)	PINMUX_SWITCH(on, GPIO1_AFP_I2C0_SDA_EN(), GPIO1_AFP_CLR())
                      
//I2C1                
#define I2C1_SCL_SEL2(on)	PINMUX_SWITCH(on, GPIO18_AFP_I2C1_SCL_EN(), GPIO18_AFP_CLR())
#define I2C1_SCL_SEL3(on)	PINMUX_SWITCH(on, GPIO2_AFP_I2C1_SCL_EN(), GPIO2_AFP_CLR())
                      
#define I2C1_SDA_SEL2(on)	PINMUX_SWITCH(on, GPIO23_AFP_I2C1_SDA_EN(), GPIO23_AFP_CLR())
#define I2C1_SDA_SEL3(on)	PINMUX_SWITCH(on, GPIO1_AFP_I2C1_SDA_EN(), GPIO1_AFP_CLR())

//EXT32k
#define EXT32K_SEL0(on)		PINMUX_SWITCH(on, GPIO5_AFP_EXT32K_EN(), GPIO5_AFP_CLR())
#define EXT32K_SEL2(on)		PINMUX_SWITCH(on, GPIO20_AFP_EXT32K_EN(), GPIO20_AFP_CLR())
#define EXT32K_SEL3(on)		PINMUX_SWITCH(on, GPIO6_AFP_EXT32K_EN(), GPIO6_AFP_CLR())

//I2S
#define I2S_MCK_SEL0(on)	PINMUX_SWITCH(on, GPIO5_AFP_I2S_MCK_EN(), GPIO5_AFP_CLR())
#define I2S_MCK_SEL1(on)	PINMUX_SWITCH(on, GPIO18_AFP_I2S_MCK_EN(), GPIO18_AFP_CLR())

#define I2S_SD_TX_SEL0(on)	PINMUX_SWITCH(on, GPIO7_AFP_I2S_SD_TX_EN(), GPIO7_AFP_CLR())
#define I2S_SD_TX_SEL1(on)	PINMUX_SWITCH(on, GPIO19_AFP_I2S_SD_TX_EN(), GPIO19_AFP_CLR())
                       
#define I2S_SD_RX_SEL0(on)	PINMUX_SWITCH(on, GPIO4_AFP_I2S_SD_RX_EN(), GPIO4_AFP_CLR())
#define I2S_SD_RX_SEL1(on)	PINMUX_SWITCH(on, GPIO20_AFP_I2S_SD_RX_EN(), GPIO20_AFP_CLR())
                       
#define I2S_SD_WS_SEL0(on)	PINMUX_SWITCH(on, GPIO3_AFP_I2S_WS_EN(), GPIO3_AFP_CLR())
#define I2S_SD_WS_SEL1(on)	PINMUX_SWITCH(on, GPIO22_AFP_I2S_WS_EN(), GPIO22_AFP_CLR())

#define I2S_CLK_SEL0(on)	PINMUX_SWITCH(on, GPIO6_AFP_I2S_CLK_EN(), GPIO6_AFP_CLR())

//LOGUART
#define LOGUART_RX_SEL0(on)	PINMUX_SWITCH(on, GPIO2_AFP_LOG_RXD_EN(), GPIO2_AFP_CLR())
#define LOGUART_TX_SEL0(on)	PINMUX_SWITCH(on, GPIO1_AFP_LOG_TXD_EN(), GPIO1_AFP_CLR())

//SPIC
#define SPIC_CS_SEL0(on)	PINMUX_SWITCH(on, GPIO12_AFP_SPIC_CS_4_EN(), GPIO12_AFP_CLR()) 
#define SPIC_CS_SEL1(on)	PINMUX_SWITCH(on, GPIO12_AFP_SPIC_CS_17_EN(), GPIO12_AFP_CLR())

#define SPIC_CLK_SEL0(on)	PINMUX_SWITCH(on, GPIO16_AFP_SPIC_CLK_EN(), GPIO16_AFP_CLR())
#define SPIC_CLK_SEL1(on)	PINMUX_SWITCH(on, GPIO17_AFP_SPIC_CLK_EN(), GPIO17_AFP_CLR())

#define SPIC_DATA0_SEL0(on)	PINMUX_SWITCH(on, GPIO15_AFP_SPIC_DATA0_4_EN(), GPIO15_AFP_CLR())
#define SPIC_DATA0_SEL1(on)	PINMUX_SWITCH(on, GPIO15_AFP_SPIC_DATA0_17_EN(), GPIO15_AFP_CLR())
                        
#define SPIC_DATA1_SEL0(on)	PINMUX_SWITCH(on, GPIO13_AFP_SPIC_DATA1_EN(), GPIO13_AFP_CLR())
#define SPIC_DATA1_SEL1(on)	PINMUX_SWITCH(on, GPIO14_AFP_SPIC_DATA1_EN(), GPIO14_AFP_CLR())	
                       
#define SPIC_DATA2_SEL0(on)	PINMUX_SWITCH(on, GPIO14_AFP_SPIC_DATA2_EN(), GPIO14_AFP_CLR())
#define SPIC_DATA2_SEL1(on)	PINMUX_SWITCH(on, GPIO13_AFP_SPIC_DATA2_EN(), GPIO13_AFP_CLR()) 
                        
#define SPIC_DATA3_SEL0(on)	PINMUX_SWITCH(on, GPIO17_AFP_SPIC_DATA3_EN(), GPIO17_AFP_CLR())
#define SPIC_DATA3_SEL1(on)	PINMUX_SWITCH(on, GPIO16_AFP_SPIC_DATA3_EN(), GPIO16_AFP_CLR()) 


//PWM
#define PWM0_SEL0(on)	    PINMUX_SWITCH(on, GPIO14_AFP_PWM0_EN(), GPIO14_AFP_CLR())
#define PWM0_SEL1(on)	    PINMUX_SWITCH(on, GPIO3_AFP_PWM0_EN(),  GPIO3_AFP_CLR())
#define PWM0_SEL2(on)	    PINMUX_SWITCH(on, GPIO20_AFP_PWM0_EN(), GPIO20_AFP_CLR())
#define PWM0_SEL4(on)	    PINMUX_SWITCH(on, GPIO23_AFP_PWM0_EN(), GPIO23_AFP_CLR())
                    
#define PWM1_SEL0(on)	    PINMUX_SWITCH(on, GPIO17_AFP_PWM1_EN(), GPIO17_AFP_CLR())
#define PWM1_SEL1(on)	    PINMUX_SWITCH(on, GPIO3_AFP_PWM1_EN(), GPIO3_AFP_CLR())
#define PWM1_SEL2(on)	    PINMUX_SWITCH(on, GPIO4_AFP_PWM1_EN(), GPIO4_AFP_CLR())
#define PWM1_SEL3(on)	    PINMUX_SWITCH(on, GPIO19_AFP_PWM1_EN(), GPIO19_AFP_CLR())
#define PWM1_SEL4(on)	    PINMUX_SWITCH(on, GPIO22_AFP_PWM1_EN(), GPIO22_AFP_CLR())
                    
#define PWM2_SEL0(on)	    PINMUX_SWITCH(on, GPIO5_AFP_PWM2_EN(), GPIO5_AFP_CLR())
#define PWM2_SEL1(on)	    PINMUX_SWITCH(on, GPIO7_AFP_PWM2_EN(), GPIO7_AFP_CLR())
#define PWM2_SEL2(on)	    PINMUX_SWITCH(on, GPIO19_AFP_PWM2_EN(), GPIO19_AFP_CLR())

#define PWM3_SEL0(on)	    PINMUX_SWITCH(on, GPIO7_AFP_PWM3_EN(), GPIO7_AFP_CLR())                                    
#define PWM3_SEL2(on)	    PINMUX_SWITCH(on, GPIO0_AFP_PWM3_EN(), GPIO0_AFP_CLR())
                    
#define PWM4_SEL1(on)	    PINMUX_SWITCH(on, GPIO0_AFP_PWM4_EN(), GPIO0_AFP_CLR())
#define PWM4_SEL2(on)	    PINMUX_SWITCH(on, GPIO6_AFP_PWM4_EN(), GPIO6_AFP_CLR())
#define PWM4_SEL3(on)	    PINMUX_SWITCH(on, GPIO2_AFP_PWM4_EN(), GPIO2_AFP_CLR())
                    
    
#define PWM5_SEL2(on)	    PINMUX_SWITCH(on, GPIO22_AFP_PWM5_EN(), GPIO22_AFP_CLR())
#define PWM5_SEL3(on)	    PINMUX_SWITCH(on, GPIO2_AFP_PWM5_EN(), GPIO2_AFP_CLR())
                    
    
#define PWM6_SEL1(on)	    PINMUX_SWITCH(on, GPIO20_AFP_PWM6_EN(), GPIO20_AFP_CLR())
#define PWM6_SEL2(on)	    PINMUX_SWITCH(on, GPIO1_AFP_PWM6_EN(), GPIO1_AFP_CLR())
                    
    
#define PWM7_SEL1(on)	    PINMUX_SWITCH(on, GPIO6_AFP_PWM7_EN(), GPIO6_AFP_CLR())
#define PWM7_SEL2(on)	    PINMUX_SWITCH(on, GPIO1_AFP_PWM7_EN(), GPIO1_AFP_CLR())

//capture timer 
#define TIM_CAP_SEL0(on)	PINMUX_SWITCH(on, GPIO7_AFP_TIM_CAP_TRIG_EN(), GPIO7_AFP_CLR())
#define TIM_CAP_SEL1(on)	PINMUX_SWITCH(on, GPIO3_AFP_TIM_CAP_TRIG_EN(), GPIO3_AFP_CLR())
#define TIM_CAP_SEL2(on)	PINMUX_SWITCH(on, GPIO4_AFP_TIM_CAP_TRIG_EN(), GPIO4_AFP_CLR())
                       
//wlan led
#define WLED_SEL0(on)		PINMUX_SWITCH(on, GPIO5_AFP_WLED_EN(), GPIO5_AFP_CLR())
#define WLED_SEL1(on)		PINMUX_SWITCH(on, GPIO0_AFP_WLED_EN(), GPIO0_AFP_CLR())


void s907x_hal_pinmux_config(u32 gpio, int func);
void s907x_hal_pinmux_swd_off(void);

