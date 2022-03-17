#ifndef __TIM3_H
#define __TIM3_H

#include "stm32f4xx.h"

#include "rtc.h"
#include "usart3.h"
#include "usart.h"
#include "OLED_I2C.h"

void Tim3_Init(u16 Prescaler,u16 TIM_Period);

#endif
