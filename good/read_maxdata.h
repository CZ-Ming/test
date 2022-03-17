#ifndef __READ_MAXDATA_H
#define __READ_MAXDATA_H


#include "stm32f4xx.h"
#include "delay.h"
#include "algorithm.h"
#include "usart.h"
#include "max30102.h"
#include "myiic.h"
#include "OLED_I2C.h"

void read_maxdata(void);
void read_spo2(void);
void read_hr(void);


#endif

