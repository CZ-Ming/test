#ifndef __DHT11_H_
#define __DHT11_H_

#include "stm32f4xx.h"
#include "delay.h"
#include "sys.h"
#include "usart.h"
#include "OLED_I2C.h"


void Dht11_Init(void);
void Dht11_Pin_Mode(GPIOMode_TypeDef mode);
int  Dht11_Start(void);
uint8_t Dht11_Read_Byte(void);
int32_t Dht11_Read(u8 *data);
void show_dh11(void);


#endif

