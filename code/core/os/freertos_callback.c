#include "s907x.h"

#include "FreeRTOS.h"
#include "task.h"

  
StackType_t IdleTaskStack[idle_task_size];
StackType_t TimerTaskStack[configTIMER_TASK_STACK_DEPTH];


static StaticTask_t IdleTaskTCB;
static StaticTask_t TimerTaskTCB;

  
void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, 
                                    StackType_t **ppxIdleTaskStackBuffer, 
                                    uint32_t *pulIdleTaskStackSize )
{
    *ppxIdleTaskTCBBuffer = &IdleTaskTCB;
    *ppxIdleTaskStackBuffer = IdleTaskStack;
    *pulIdleTaskStackSize = idle_task_size;
}

void vApplicationGetTimerTaskMemory( StaticTask_t **ppxTimerTaskTCBBuffer, 
                                    StackType_t **ppxTimerTaskStackBuffer, 
                                    uint32_t *pulTimerTaskStackSize )
{
    *ppxTimerTaskTCBBuffer = &TimerTaskTCB;
    *ppxTimerTaskStackBuffer = TimerTaskStack;
    *pulTimerTaskStackSize = configTIMER_TASK_STACK_DEPTH;
}
 

void vApplicationStackOverflowHook( TaskHandle_t xTask, char *pcTaskName )
{
    printf("vApplicationStackOverflowHook \n\r");
    printf("task name : %s, TCB : %x\n\r", pcTaskName, xTask);
  
}

void vApplicationTickHook( void )
{
#if defined (__CC_ARM)
	__asm(" nop");
#else
	asm(" nop");
#endif
}

void vApplicationMallocFailedHook( void )
{
#if defined (__CC_ARM)
	__asm(" nop");
#else
	asm(" nop");
#endif
}


int  freertos_ready_to_sleep()
{
#if S907X_LOW_POWER == 0
	return 0;
#else
	return freertos_ll_ready_to_sleep();
#endif
} 

void freertos_pre_sleep_processing(unsigned int *expected_idle_time)
{
#if S907X_LOW_POWER == 0
	return ;
#else
	freertos_ll_pre_sleep_processing(expected_idle_time);
#endif

}

void vPortSuppressTicksAndSleep( TickType_t xExpectedIdleTime )
{
#if S907X_LOW_POWER == 0
	return ;
#else
	vPort_ll_SuppressTicksAndSleep(xExpectedIdleTime);
#endif
}

void freertos_post_sleep_processing(unsigned int *expected_idle_time)
{
#if S907X_LOW_POWER == 0
	return ;
#else
	freertos_ll_post_sleep_processing(expected_idle_time);
#endif
}


#if defined ( __GNUC__ )
extern uint8_t heap_start[];
#else
HEAP_START_SECTION u8 heap_start[];
#endif


#define MM_ALIGN_BIT     3
#define MM_ALIGN_SIZE    (1 << MM_ALIGN_BIT)
#define MM_ALIGN_MASK    (MM_ALIGN_SIZE - 1)
#define MM_ALIGN_UP(a)   (((a) + MM_ALIGN_MASK) & ~MM_ALIGN_MASK)
#define MM_ALIGN_DOWN(a) ((a) & ~MM_ALIGN_MASK)

#if 0
//heap_start < 0x1002e000

HeapRegion_t xHeapRegions[] =
  {
  	{ ( uint8_t * ) heap_start, 0x1002e000 - MM_ALIGN_UP((u32)heap_start) }, 
  	{ ( uint8_t * ) 0x1002f000, HEAP_DATA_END - 0x1002f000 }, 
  	{ NULL, 0 }                
};
//#else
//heap_start > 0x1002e000
HeapRegion_t xHeapRegions[] =
  {
  	{ ( uint8_t * ) heap_start, HEAP_DATA_END - MM_ALIGN_UP((u32)heap_start) }, 
  	{ NULL, 0 }                
};
#endif



void wl_heap_init(void)
{
//	printf("heap_start  = %x   0x%x \r\n",heap_start,HEAP_DATA_END - MM_ALIGN_UP((u32)heap_start));
HeapRegion_t xHeapRegions[3];

if(((u32)heap_start)<0x1002e000)
{
	printf("heap_start1  = %x   0x%x \r\n",heap_start,0x1002e000 - MM_ALIGN_UP((u32)heap_start));
	xHeapRegions[0].pucStartAddress = ( uint8_t * ) heap_start;
	xHeapRegions[0].xSizeInBytes = 0x1002e000 - MM_ALIGN_UP((u32)heap_start);
	xHeapRegions[1].pucStartAddress = ( uint8_t * ) 0x1002f000;
	xHeapRegions[1].xSizeInBytes = HEAP_DATA_END - 0x1002f000;
	xHeapRegions[2].pucStartAddress =  NULL;
	xHeapRegions[2].xSizeInBytes = 0;
}else
{
	printf("heap_start  = %x   0x%x \r\n",heap_start,HEAP_DATA_END - MM_ALIGN_UP((u32)heap_start));
	xHeapRegions[0].pucStartAddress = ( uint8_t * ) heap_start;
	xHeapRegions[0].xSizeInBytes = HEAP_DATA_END - MM_ALIGN_UP((u32)heap_start);
	xHeapRegions[1].pucStartAddress =  NULL;
	xHeapRegions[1].xSizeInBytes = 0;
}

	vPortDefineHeapRegions(xHeapRegions); 
}

