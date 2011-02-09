#include "common.h"
#include "board_regs.h"
#include "board_config.h"

struct s3c2440_uart_reg
{
    volatile uint32_t ulcon;
    volatile uint32_t ucon;
    volatile uint32_t ufcon;
    volatile uint32_t umcon;
    volatile uint32_t utrstat;
    volatile uint32_t uerstat;
    volatile uint32_t ufstat;
    volatile uint32_t umstat;
    volatile uint32_t utxh;
    volatile uint32_t urxh;
    volatile uint32_t ubrdiv;
};

void uart_init()
{
    int i = 0;
    struct s3c2440_uart_reg *reg;

    /* set GPIO for uart */
    REG(rGPHCON) |= 0xa0;
    REG(rGPHUP) = vGPHUP;
    
    reg = (struct s3c2440_uart_reg *)(UART_CTRL0_BASE);
    reg->ulcon = vULCON0;
    reg->ucon = vUCON0;
    reg->ufcon = vUFCON0;
    reg->umcon = vUMCON0;
    reg->ubrdiv = vUBRDIV0;

    reg = (struct s3c2440_uart_reg *)(UART_CTRL1_BASE);
    reg->ulcon = vULCON0;
    reg->ucon = vUCON0;
    reg->ufcon = vUFCON0;
    reg->umcon = vUMCON0;
    reg->ubrdiv = vUBRDIV0;
}

void uart_putch(int id,char ch)
{
    struct s3c2440_uart_reg *reg;

    if(id < 0 || id > 1)
	return;
    
    if(id == 0)
	 reg = (struct s3c2440_uart_reg *)(UART_CTRL0_BASE);
    else if(id == 1)
	 reg = (struct s3c2440_uart_reg *)(UART_CTRL1_BASE);
    
    while(!(reg->utrstat & BIT(2))){
    }
    reg->utxh = ch;
}

char uart_getch(int id)
{
    struct s3c2440_uart_reg *reg;

    if(id < 0 || id > 1)
	return;
    
    if(id == 0)
	 reg = (struct s3c2440_uart_reg *)(UART_CTRL0_BASE);
    else if(id == 1)
	 reg = (struct s3c2440_uart_reg *)(UART_CTRL1_BASE);
    
    while(!(reg->utrstat & BIT(0))){
	
    }
    return reg->urxh;
}
