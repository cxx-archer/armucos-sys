#include "common.h"
#include "board.h"
#include "utils.h"
#include "memory.h"
#include "board_config.h"
#include "includes.h"
#include "irq.h"
#include "timer.h"

static OS_STK inittask_stack[512];
static OS_STK task1_stack[1024];
static OS_STK task2_stack[1024];

static void task1(void *pd)
{
    while(1){
	printk("task1...\n");
	OSTimeDly(10);
    }
}

static void task2(void *pd)
{
    while(1){
	printk("task2...\n");
	OSTimeDly(10);
    }
}

static void timer_handler(uint32_t vector, void *param)
{
    OSTimeTick();
}

void inittask(void *pd)
{
    uint32_t free_begin, free_end;

    bsp_init();
    free_begin = PAGE_ALIGN((uint32_t)_free_mem);
    free_end = SYS_STK_BASE;
    printk("Free Memory region 0x%08x - 0x%08x\n", free_begin, free_end);
    mem_init(free_begin, free_end);
    dump_mem_info();

    OSTaskCreate(task1, 0, task1_stack + 1024, 2);
    OSTaskCreate(task2, 0, task2_stack + 1024, 4);
    timer_init(4);
    int_connect(oINT_TIMER4, timer_handler,NULL);
    int_enable(oINT_TIMER4);
    timer_start(4);
    ARMEnableInt();

    OSTaskDel(OS_PRIO_SELF);
}

void os_main()
{
    /* bss section init */
    memset(_bss_begin, 0, _bss_end - _bss_begin);
    
    OSInit();
    OSTaskCreate(inittask, 0, inittask_stack + 512, OS_LOWEST_PRIO - 2);
    OSStart();
}
