#include "timer.h"
#include "board_regs.h"
#include "board_config.h"

void timer_init(uint8_t id)
{
    REG(rTCFG0) &= ~0xffff00ff;
    REG(rTCFG0) |= (49 << 8);
    REG(rTCFG1) &= ~0xfff0ffff;
    REG(rTCFG1) |= (2 << 16);
}

void timer_start(uint8_t id)
{
    REG(rTCNTB4) = 2500;
    REG(rTCON) &= 0xff0fffff;
    REG(rTCON) |= 0x00700000;
    REG(rTCON) &= 0xffdfffff;
}

void timer_stop(uint8_t id)
{
    
}

