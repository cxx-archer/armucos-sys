#ifndef _H_IRQ_H
#define _H_IRQ_H
#include "common.h"

#define bINT_ADC	BIT(31)
#define bINT_RTC	BIT(30)
#define bINT_SPL1	BIT(29)
#define bINT_UART0	BIT(28)
#define bINT_IIC	BIT(27)
#define bINT_USBH	BIT(26)
#define bINT_USBD	BIT(25)
#define bINT_NFCON	BIT(24)
#define bINT_URRT1	BIT(23)
#define bINT_SPI0	BIT(22)
#define bINT_SDI	BIT(21)
#define bINT_DMA3	BIT(20)
#define bINT_DMA2	BIT(19)
#define bINT_DMA1	BIT(18)
#define bINT_DMA0	BIT(17)
#define bINT_LCD	BIT(16)
#define bINT_UART2	BIT(15)
#define bINT_TIMER4	BIT(14)
#define bINT_TIMER3	BIT(13)
#define bINT_TIMER2	BIT(12)
#define bINT_TIMER1	BIT(11)
#define bINT_TIMER0	BIT(10)
#define bINT_WDT_AC97	BIT(9)
#define bINT_TICK	BIT(8)
#define bINT_nBATT_FLT	BIT(7)
#define bINT_CAM	BIT(6)
#define bEINT8_23	BIT(5)
#define bEINT4_7	BIT(4)
#define bEINT3		BIT(3)
#define bEINT2		BIT(2)
#define bEINT1		BIT(1)
#define bEINT0		BIT(0)

#define oINT_ADC	(31)
#define oINT_RTC	(30)
#define oINT_SPL1	(29)
#define oINT_UART0	(28)
#define oINT_IIC	(27)
#define oINT_USBH	(26)
#define oINT_USBD	(25)
#define oINT_NFCON	(24)
#define oINT_URRT1	(23)
#define oINT_SPI0	(22)
#define oINT_SDI	(21)
#define oINT_DMA3	(20)
#define oINT_DMA2	(19)
#define oINT_DMA1	(18)
#define oINT_DMA0	(17)
#define oINT_LCD	(16)
#define oINT_UART2	(15)
#define oINT_TIMER4	(14)
#define oINT_TIMER3	(13)
#define oINT_TIMER2	(12)
#define oINT_TIMER1	(11)
#define oINT_TIMER0	(10)
#define oINT_WDT_AC97	(9)
#define oINT_TICK	(8)
#define oINT_nBATT_FLT	(7)
#define oINT_CAM	(6)
#define oEINT8_23	(5)
#define oEINT4_7	(4)
#define oEINT3		(3)
#define oEINT2		(2)
#define oEINT1		(1)
#define oEINT0		(0)


typedef void (*irq_handler)(uint32_t vector, void *param);
void intc_init();
void do_irq();
void int_enable(uint32_t vector);
void int_disalble(uint32_t vector);
int int_connect(uint32_t vector, irq_handler handler, void *param);
int int_disconnect(uint32_t vector);

#endif
