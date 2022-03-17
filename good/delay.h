#ifndef __DELAY_H
#define __DELAY_H


#include "stm32f4xx.h"

void Init_Delay(void);
void delay_us(unsigned int nus);
void delay_ms(unsigned int nms);
void delay_s(unsigned int ns);





#endif
