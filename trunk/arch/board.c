#include "common.h"
#include "board.h"
#include "irq.h"
#include "utils.h"
#include "timer.h"


void bsp_init(void)
{
    intc_init();
    uart_init();
}
