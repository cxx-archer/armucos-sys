//********************************************************************************************************
//                                               uC/OS-II
//                                         The Real-Time Kernel
//
//                          (c) Copyright 1992-2002, Jean J. Labrosse, Weston, FL
//                                          All Rights Reserved
//
//
//                                        ARM9 Specific code
//
//                                        	GNUGCC
//                                       (IBM/PC Compatible Target)
//
// File         : OS_CPU_A.S
// By           : Songhua.he
//********************************************************************************************************

//********************************************************************************************************
//                                    PUBLIC and EXTERNAL REFERENCES
//********************************************************************************************************

.global OSTickISR
.global OSStartHighRdy
.global OSCtxSw
.global OSIntCtxSw
.global OSCPUSaveSR
.global OSCPURestoreSR
.global ARMEnableInt
.global ARMDisableInt
.global ISR__entry
.global OS_TASK_SW
.global os_exit_critical
.global os_enter_critical
	
.extern OSIntExit
.extern OSTimeTick
.extern OSTaskSwHook
.extern OSIntNesting
.extern OSTickDOSCtr
.extern OSPrioHighRdy
.extern OSPrioCur
.extern OSRunning
.extern OSTCBCur
.extern OSTCBHighRdy
.extern OSRunning

.section .text,"ax"
//*********************************************************************************************************
//                                          START MULTITASKING
//                                       void OSStartHighRdy(void)
//
// The stack frame is assumed to look as follows:
//
// OSTCBHighRdy->OSTCBStkPtr --> SPSR                               (Low memory)
//                               CPSR
//                               R0
//                               R1
//                               R2
//                               R3
//                               R4
//                               R5
//                               R6
//                               R7
//				 R8
//				 R9
//				 R10
//				 R11
//				 R12
//				 R14(LR)
//				 R15(PC)
//
// Note : OSStartHighRdy() MUST:
//           a) Call OSTaskSwHook() then,
//           b) Set OSRunning to TRUE,
//           c) Switch to the highest priority task.
//*********************************************************************************************************

OSStartHighRdy:
	/* set OSRunning = TRUE */
	ldr r0,=OSRunning
	mov r1,#0x1
	strb r1,[r0]
	
	/* OSPrioCur = OSPrioHighRdy */
	ldr r0,=OSPrioCur
	ldr r1,=OSPrioHighRdy
	ldr r1,[r1]
	strb r1,[r0]
	
	/* OSTCBCur = OSTCBHighRdy */
	ldr r0,=OSTCBCur
	ldr r1,=OSTCBHighRdy
	ldr r1,[r1]
	str r1,[r0]
	
	/* load sp */
	ldr r0,[r0]
	ldr sp,[r0]
	
	/* load task context */
	ldmfd sp!,{r0}		/* SPSR */
	msr spsr_cxsf,r0
	ldmfd sp!,{r0}		/* CPSR */
	msr cpsr_cxsf,r0	/* CPSR must be SVCMode */
	ldmfd sp!,{r0-r12,lr,pc}/* goto task */
	
	
//*********************************************************************************************************
//                                PERFORM A CONTEXT SWITCH (From task level)
//                                           void OSCtxSw(void)
//
// Note(s): 1) Upon entry,
//             OSTCBCur     points to the OS_TCB of the task to suspend
//             OSTCBHighRdy points to the OS_TCB of the task to resume
//
//          2) The stack frame of the task to suspend looks as follows:
//
//                 SP -> OFFSET  of task to suspend    (Low memory)
//                       SEGMENT of task to suspend
//                       PSW     of task to suspend    (High memory)
//
//          3) The stack frame of the task to resume looks as follows:
//
//                 OSTCBHighRdy->OSTCBStkPtr --> SPSR                               (Low memory)
//                               		CPSR
//                               		R0
//                               		R1
//                               		R2
//                               		R3
//                               		R4
//                               		R5
//                               		R6
//                               		R7
//				 		R8
//				 		R9
//				 		R10
//				 		R11
//				 		R12
//				 		R14(LR)
//						R15(PC)
//*********************************************************************************************************


OS_TASK_SW:
	/* 1. save register */
	stmfd sp!,{lr}
	stmfd sp!,{r0-r12,lr}
	mrs r4,cpsr
	stmfd sp!,{r4}
	mrs r4,spsr
	stmfd sp!,{r4}

_OSCtxSw:	/* this routine must be run in SVCMOde,and IRQ must be disabled */
	/* 2. OSPrioCur = OSPrioHighRdy */
	ldr r4,=OSPrioCur
	ldr r5,=OSPrioHighRdy
	ldrb r6,[r5]
	strb r6,[r4]
	
	/* 3. Save SP to TCB */
	ldr r4,=OSTCBCur
	ldr r5,[r4]
	str sp,[r5]
	
	/* 4. Load new SP */
	ldr r6,=OSTCBHighRdy
	ldr r6,[r6]
	ldr sp,[r6]
	
	/* 5. OSTCBCur = OSTCBHighRdy */
	str r6,[r4]
	
	/* 6. Restore new context */
	ldmfd sp!,{r4}		/* SPSR */
	msr spsr_cxsf,r4
	ldmfd sp!,{r4}		/* CPSR */
	msr cpsr_cxsf,r4	/* CPSR must be SVCMode */
	ldmfd sp!,{r0-r12,lr,pc}	/* goto new task */
	
need_ctx_sw:
	.word 0
	
//*********************************************************************************************************
//                                PERFORM A CONTEXT SWITCH (From an ISR)
//                                        void OSIntCtxSw(void)
//
// Note(s): 1) Upon entry,
//             OSTCBCur     points to the OS_TCB of the task to suspend
//             OSTCBHighRdy points to the OS_TCB of the task to resume
//
//          2) The stack frame of the task to suspend looks as follows:
//
//             OSTCBCur->OSTCBStkPtr ------>  SPSR                              (Low memory)
//                               		CPSR
//                               		R0
//                               		R1
//                               		R2
//                               		R3
//                               		R4
//                               		R5
//                               		R6
//                               		R7
//				 		R8
//				 		R9
//				 		R10
//				 		R11
//				 		R12
//				 		R14(LR)
//						R15(PC)
//
//          3) The stack frame of the task to resume looks as follows:
//
//             OSTCBHighRdy->OSTCBStkPtr -->    SPSR                               (Low memory)
//                               		CPSR
//                               		R0
//                               		R1
//                               		R2
//                               		R3
//                               		R4
//                               		R5
//                               		R6
//                               		R7
//				 		R8
//				 		R9
//				 		R10
//				 		R11
//				 		R12
//				 		R14(LR)
//						R15(PC)
// only set need_ctx_sw flag to 1
//*********************************************************************************************************

OSIntCtxSw:
	ldr r0,=need_ctx_sw
	mov r1,#1
	str r1,[r0]
	mov pc,lr
	
// really context switch

SAVED_LR_SVC:
	.long 0
SAVED_LR_IRQ:
	.long 0
SAVED_SPSR:
	.long 0

OS_Int_Ctx_SW:
	/* 1. set need_ctx_sw to 0 */
	mov r1,#0
	str r1,[r0]
	
	ldmfd sp!,{r4}	/* restore spsr_irq */
	msr spsr,r4
	
	str r4,SAVED_SPSR
	
	/* 2. clear irq's stack */
	ldmfd sp!,{r0-r12,lr}
	str lr,SAVED_LR_IRQ
	mov lr,#0xd3
	msr cpsr,lr
	
	str lr,SAVED_LR_SVC
	ldr lr,SAVED_LR_IRQ
	stmfd sp!,{lr}		/* LR_irq saved in PC place */
	
	ldr lr,SAVED_LR_SVC
	stmfd sp!,{r0-r12,lr}
	
	ldr r4,SAVED_SPSR
	stmfd sp!,{r4}		/* SVCMODE's spsr seems no used */
	stmfd sp!,{r4}		/* cpsr */

	b _OSCtxSw
	
ISR__entry:
	sub lr,lr,#4
	stmfd sp!,{r0-r12,lr}
	
	mrs r4,spsr
	stmfd sp!,{r4}
	
	/* 1. OSIntNesting++ */
	ldr r0,=OSIntNesting
	ldr r1,[r0]
	add r1,r1,#1
	strb r1,[r0]
	
	/* 2. call user's interrupt service */
	bl do_irq
	bl OSIntExit
	
	/* 3. if(need_ctx_sw) then call OS_Int_Ctx_SW and nerver return*/
	ldr r0,=need_ctx_sw
	ldr r1,[r0]
	cmp r1,#1
	beq OS_Int_Ctx_SW
	
	/* 4. else return irq */
	ldmfd sp!,{r4}
	msr spsr_cxsf,r4
	ldmfd sp!,{r0-r12,pc}^
	
/* hesonghua add */

OSCPUSaveSR:
	stmfd sp!,{r1}
	mrs r0,cpsr
	orr r1,r0,#0x80
	msr cpsr_cxsf,r1
	ldmfd sp!,{r1}
	mov pc,lr

OSCPURestoreSR:
	msr cpsr_cxsf,r0
	mov pc,lr

ARMDisableInt:
	stmfd sp!,{r0}
	mrs r0,cpsr
	orr r0,r0,#0x80
	msr cpsr_cxsf,r0
	ldmfd sp!,{r0}
	mov pc,lr
	
ARMEnableInt:
	stmfd sp!,{r0}
	mrs r0,cpsr
	bic r0,r0,#0x80
	msr cpsr_cxsf,r0
	ldmfd sp!,{r0}
	mov pc,lr

os_exit_critical:
	ldmfd sp!,{r0}
	msr cpsr_c,r0
	mov pc,lr
	
os_enter_critical:
	mrs r0,cpsr
	stmfd sp!,{r0}
	orr r0,r0,#0x80
	msr cpsr_cxsf,r0
	mov pc,lr