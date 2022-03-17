#ifndef __RTC_H
#define __RTC_H

#include "stm32f4xx.h"
#include "delay.h"

#define BKP_VALUE 1111

void RTC_MyInit(void);
void RTC_AlarmA_Init(void);
void RTC_AlarmB_Init(void);

void Set_Clock1(u8 hour,u8 min,u8 sec,u8 day);
void Set_Clock2(u8 hour,u8 min,u8 sec,u8 day);



void Set_Date(u8 year,u8 mon,u8 day,u8 week);
void Set_Time(u8 hour,u8 min,u8 sec);
void Set_Date_Time(u8 year,u8 mon,u8 day,u8 hour,u8 min,u8 sec,u8 week);


#endif
