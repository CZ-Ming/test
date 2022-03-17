#ifndef __EXTI_H__
#define __EXTI_H__

#include "stm32f4xx.h"
#include "delay.h"
#include "usart.h"
#include "rtc.h"
#include "OLED_I2C.h"

#define AHB1_KEY_AE  (RCC_AHB1Periph_GPIOA | RCC_AHB1Periph_GPIOE)

#define KEY0  GPIO_Pin_0	
#define KEY1  GPIO_Pin_2
#define KEY2  GPIO_Pin_3
#define KEY3  GPIO_Pin_4

void Init_Exti(void);




#endif

