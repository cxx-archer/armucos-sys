#ifndef _H_UTILS_H
#define _H_UTILS_H
#include "common.h"
#include <stdarg.h>

void uart_init(void);
void uart_putch(int id, char ch);
char uart_getch(int id);
int printk(const char *fmt,...);
int vsprintk(char *buf, const char *fmt, va_list args);
int sprintk(char * buf, const char *fmt, ...);

#define panic(fmt, args...)                     \
    do{ \
        printk(fmt, ##args); while(1);          \
    }while(0)

#endif
