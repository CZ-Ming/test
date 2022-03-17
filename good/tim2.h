#ifndef __TIM2_H
#define __TIM2_H

#include "stm32f4xx.h"
#include "usart3.h"
#include "OLED_I2C.h"

void Tim2_Init(u16 Prescaler,u32 TIM_Period);

#endif
