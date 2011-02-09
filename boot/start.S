#include "board_regs.h"
#include "board_config.h"

.section .bootstone.text,"ax"

.global _start
.extern C_Entry
.extern ISR__entry

.macro ChangeModeWithNoInt newmode
	mrs r0,cpsr
	bic r0,r0,#MODE_MASK
	orr r0,r0,#\newmode | NOINT
	msr cpsr,r0
.endm

.macro REGWRITE address,value
        ldr r0, =\value
        ldr r1, =\address
        str r0, [r1]
.endm

_start:
	b 	ResetHandler
	b 	UndefHandler
	b .		    
	b .		    
	b .		    
	b .		   
	b 	IRQHandler
	b .		    

UndefHandler:
	mov r0,lr
	b .
	
ResetHandler:
        /* watch dog disable */
        REGWRITE rWTCON,0;

        /* disable all interrupt */
        REGWRITE rINTMSK, 0xffffffff
        REGWRITE rINTSUBMSK, 0x7ff

        /* init system clock */
        REGWRITE rLOCKTIME, vLOCKTIME
        REGWRITE rCLKDIVN, vCLKDIVN

        mrc p15, 0, r1, c1, c0, 0
        orr r1, r1, #0xc0000000
        mcr p15, 0, r1, c1, c0, 0

        /* UPLL setup */
        REGWRITE rUPLLCON, vUPLLCON
        nop;nop;nop;nop;nop;nop;nop;nop;

        REGWRITE rMPLLCON, vMPLLCON

        /* sdram init */
        REGWRITE rBWSCON, vBWSCON
        REGWRITE rBANKCON0, vBANKCON0
        REGWRITE rBANKCON1, vBANKCON1
        REGWRITE rBANKCON2, vBANKCON2
        REGWRITE rBANKCON3, vBANKCON3
        REGWRITE rBANKCON4, vBANKCON4
        REGWRITE rBANKCON5, vBANKCON5
        REGWRITE rBANKCON6, vBANKCON6
        REGWRITE rBANKCON7, vBANKCON7
        REGWRITE rREFRESH, vREFRESH
        REGWRITE rBANKSIZE, vBANKSIZE
        REGWRITE rMRSRB6, vMRSRB6
        REGWRITE rMRSRB7, vMRSRB7

        /* init stack */
        ChangeModeWithNoInt MODE_IRQ
	ldr sp, =IRQ_STK_TOP

	ChangeModeWithNoInt MODE_FIQ
        ldr sp, =FIQ_STK_TOP

        ChangeModeWithNoInt MODE_SVC
        ldr sp, =SVC_STK_TOP

	/* Init Stack,prepare to jump to ram */
	b C_Entry

IRQHandler:
	ldr pc, =ISR__entry
