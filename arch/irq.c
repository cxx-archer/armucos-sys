#include "irq.h"
#include "utils.h"
#include "board_regs.h"
#include "board_config.h"

static struct
{
    int enable;
    irq_handler handler;
    void *param;
}isr_tbl[64] = {
    0,
};

void do_irq(void)
{
    uint32_t offset;
    uint32_t srcpnd, intpnd;

    srcpnd = REG(rSRCPND);
    intpnd = REG(rINTPND);
    offset = REG(rINTOFFSET);

    REG(rINTPND) = intpnd;
    REG(rSRCPND) = intpnd;

    if(isr_tbl[offset].enable){
	isr_tbl[offset].handler(offset, isr_tbl[offset].param);
    }
}

void intc_init()
{
    REG(rINTSUBMSK) = 0xffff;
    REG(rINTMSK) = 0xffffffff;
    REG(rPRIORITY) = 0x7f;
    REG(rINTMOD) = 0x0;
}

void int_enable(uint32_t vector)
{
    if(vector < 32){
	REG(rINTMSK) &= ~(1 << vector);
    }
    else
    {
	panic("Unspoort!\n");
    }
}

void int_disable(uint32_t vector)
{
    if(vector < 32){
	REG(rINTMSK) |= (1 << vector);
    }
    else
    {
	panic("Unspoort!\n");
    }    
}

int int_connect(uint32_t vector, irq_handler handler, void *param)
{
    if(vector < 32){
	isr_tbl[vector].handler = handler;
	isr_tbl[vector].param = param;
	isr_tbl[vector].enable = 1;
    }
    else
    {
	panic("Unspoort!\n");
    }
}

int int_disconnect(uint32_t vector)
{
    if(vector < 32){
	isr_tbl[vector].enable = 0;
	isr_tbl[vector].handler = 0;
	isr_tbl[vector].param = 0;
    }
}
