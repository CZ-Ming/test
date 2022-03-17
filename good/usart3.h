#ifndef __USART3_H
#define __USART3_H

#include "stm32f4xx.h"
#include "string.h"
#include "rtc.h"
#include "usart.h"

void Usart3_Init(u32 baudrate);
void Usart3_Deal(void);

void u32tostr(unsigned long dat,char *str) ;
void UART_Send_Str(char *s);


#endif

