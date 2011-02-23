#include "common.h"
#include "board_regs.h"
#include "board_config.h"

extern void _start(void);

void C_Entry(void) 
{
	uint32_t size = 16;
	uint32_t *psrc = (uint32_t*)_start;
	uint32_t *pdst = (uint32_t*)0;

	/* copy interrupt vector to 0x0 */
	while(size--)
		*pdst++ = *psrc++;

    os_main();
}
