#ifndef _H_CONFIG_H
#define _H_CONFIG_H

#define FIQ_STK_TOP                     (0x30000000 + 64 * 1024 * 1024)
#define IRQ_STK_TOP                     (FIQ_STK_TOP - 1024)
#define SVC_STK_TOP                     (IRQ_STK_TOP - 1024 * 1024)
#define SYS_STK_BASE                    (SVC_STK_TOP - 4 * 1024 * 1024)

#define PCLK				(50000000)
#define UART_CLK			(PCLK)
#define UART_BAUD_RATE			(115200)

#define vLOCKTIME			(~0xff000000)
#define vCLKDIVN			(0x5)	/* 1:4:8 */
#define vMDIV	 			(0x38)
#define vPDIV				(0x02)
#define vSDIV				(0x02)

/* UCLK = 48MHZ */
#define vUPLLCON			((vMDIV << 12) | (vPDIV << 4) | (vSDIV))

/* FCLK = 405MHZ */
#define vMPLLCON			((0x7f << 12) | (0x2 << 4) | (0x1))

#define UCLKFREQ			(48000000)		/* 48MHZ */

#define FCLKFREQ			(405000000)		/* 405MHZ */
#define HCLKFREQ			(FCLKFREQ / 4)		/* 101MHZ */
#define PCLKFREQ			(HCLKFREQ / 2)		/* 50.6MHZ */

#define vULCON0				(0x3)
#define vUCON0				(0x245)
#define vUFCON0				(0x0)
#define vUMCON0				(0x0)
#define vUBRDIV0			((UART_CLK / (UART_BAUD_RATE * 16)) - 1)

#define vBWSCON				(0x22111110)
#define vBANKCON0			(0x00000700)
#define vBANKCON1			(0x00000700)
#define vBANKCON2			(0x00000700)
#define vBANKCON3			(0x00000700)
#define vBANKCON4			(0x00000700)
#define vBANKCON5			(0x00000700)
#define vBANKCON6			(0x00018009)
#define vBANKCON7			(0x00018009)
#define vREFRESH			(0x00ac03f4)
#define vBANKSIZE			(0x000000b1)
#define vMRSRB6				(0x00000030)
#define vMRSRB7				(0x00000030)

#define vGPACON				(0x007fffff)
#define vGPBCON				(0x00044555)
#define vGPBUP				(0x000007ff)
#define vGPCCON				(0xaaaaaaaa)
#define vGPCUP				(0x0000ffff)
#define vGPDCON				(0xaaaaaaaa)
#define vGPDUP				(0x0000ffff)
#define vGPECON				(0xaaaaaaaa)
#define vGPEUP				(0x0000ffff)
#define vGPFCON				(0x000055aa)
#define vGPFUP				(0x000000ff)
#define vGPGCON				(0xff95ffba)
#define vGPGUP				(0x0000ffff)
#define vGPHCON				(0x0016faaa)
#define vGPHUP				(0x000007ff)

#define vEXTINT0			(0x22222222)
#define vEXTINT1			(0x22222222)
#define vEXTINT2			(0x22222222)

#endif
