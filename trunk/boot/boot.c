#include "common.h"
#include "board_regs.h"
#include "board_config.h"

static bool __BOOTSTONE_DATA debug_mode = false;

static void __BOOTSTONE copy_code(void)
{
    
}

static void __BOOTSTONE base_init(void)
{

}

void __BOOTSTONE C_Entry(void) 
{
    base_init();
    if(!debug_mode)
	copy_code();
    
    asm(
	"ldr pc, =gmain;"
	);
}
