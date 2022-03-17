#include "rtc.h"

/*******************************************************
函数使用说明：
0.在主函数初始化延迟函数	   Init_Delay();
1.在主函数初始化RTC 		  Rtc_MyInit();
2.在主函数初始化闹钟A 		  RTC_AlarmA_Init();
3.在主函数初始化闹钟B		  RTC_AlarmB_Init();
**********************************************************/

void RTC_MyInit(void)
{
	RTC_InitTypeDef 	RTC_InitStruct;
	RTC_TimeTypeDef 	RTC_TimeStruct;
	RTC_DateTypeDef 	RTC_DateStruct;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);	//使能PWR时钟   
	PWR_BackupAccessCmd(ENABLE);					//使能后备寄存器访问
	
	if(RTC_ReadBackupRegister(RTC_BKP_DR0) != BKP_VALUE)  //判断后备寄存器值是否等待某值
	{
		RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);		// 配置RTC时钟源，使能RTC时钟
		RCC_RTCCLKCmd(ENABLE);
		
		RCC_LSEConfig(RCC_LSE_ON);				//如果使用LSE，要打开LSE
		delay_ms(50);  						//延时，等待时钟稳定
		
		RTC_InitStruct.RTC_HourFormat	= RTC_HourFormat_24;//24小时制
		RTC_InitStruct.RTC_AsynchPrediv	= 0x7F;	  //异步通道分频器 	128分频
		RTC_InitStruct.RTC_SynchPrediv	= 0xFF;	  //同步通道分频器 	256分频	
		RTC_Init(&RTC_InitStruct);				//初始化RTC(同步/异步分频系数和时钟格式)
		
		RTC_TimeStruct.RTC_H12		= RTC_H12_PM;	//下午
		RTC_TimeStruct.RTC_Hours	= 16;			//时
		RTC_TimeStruct.RTC_Minutes	= 31;			//分
		RTC_TimeStruct.RTC_Seconds	= 0;			//ADC+
		RTC_SetTime(RTC_Format_BIN, &RTC_TimeStruct);	//设置时间
		
		RTC_DateStruct.RTC_Year		= 21;				//年
		RTC_DateStruct.RTC_Month	= 7;				//月
		RTC_DateStruct.RTC_Date		= 15;				//日
		RTC_DateStruct.RTC_WeekDay	= RTC_Weekday_Thursday; //星期四	
		RTC_SetDate(RTC_Format_BIN, &RTC_DateStruct);		//设置日期
	
		RTC_WriteBackupRegister(RTC_BKP_DR0, BKP_VALUE);	//值写入后备寄存器
	}
}

//设置闹钟A
void RTC_AlarmA_Init(void)
{
	RTC_AlarmTypeDef  	RTC_AlarmStruct;
	RTC_TimeTypeDef 		RTC_AlarmTime;
	EXTI_InitTypeDef		EXTI_InitStruct;
	NVIC_InitTypeDef  	NVIC_InitStruct;
	
	RTC_AlarmCmd(RTC_Alarm_A,DISABLE); //关闭闹钟
	
	//时间设置
	RTC_AlarmTime.RTC_H12		= RTC_H12_PM;	//下午
	RTC_AlarmTime.RTC_Hours		= 16;			//时
	RTC_AlarmTime.RTC_Minutes	= 31;			//分
	RTC_AlarmTime.RTC_Seconds	= 5;			//ADC+
	
	RTC_AlarmStruct.RTC_AlarmTime			= RTC_AlarmTime;			 //设置闹钟时间
	RTC_AlarmStruct.RTC_AlarmMask			= RTC_AlarmMask_None;		 //无掩码位，按实际时间响应
	RTC_AlarmStruct.RTC_AlarmDateWeekDaySel 	= RTC_AlarmDateWeekDaySel_Date;//按日期响应
	RTC_AlarmStruct.RTC_AlarmDateWeekDay	= 15;					 //日期
	RTC_SetAlarm(RTC_Format_BIN, RTC_Alarm_A, &RTC_AlarmStruct);		 //配置闹钟参数
	
//	RTC_AlarmCmd(RTC_Alarm_A,ENABLE);//开启闹钟	
//	RTC_ITConfig(RTC_IT_ALRA, ENABLE);//开启配置闹钟中断
	
	EXTI_InitStruct.EXTI_Line	= EXTI_Line17;			//中断线17
	EXTI_InitStruct.EXTI_Mode	= EXTI_Mode_Interrupt;		//中断
	EXTI_InitStruct.EXTI_Trigger	= EXTI_Trigger_Rising;  	//上升沿
	EXTI_InitStruct.EXTI_LineCmd	= ENABLE;				//使能
	EXTI_Init(&EXTI_InitStruct);		//初始化线上中断，设置触发条件等。
		
	NVIC_InitStruct.NVIC_IRQChannel				= RTC_Alarm_IRQn;   //中断通道
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority	= 1;			  //抢占优先级
	NVIC_InitStruct.NVIC_IRQChannelSubPriority		= 1;			  //响应优先级
	NVIC_InitStruct.NVIC_IRQChannelCmd				= ENABLE;		  //使能
	NVIC_Init(&NVIC_InitStruct);		//配置中断分组（NVIC），并使能中断。
}

//设置闹钟B
void RTC_AlarmB_Init(void)
{
	RTC_AlarmTypeDef  	RTC_AlarmStruct;
	RTC_TimeTypeDef 		RTC_AlarmTime;
	EXTI_InitTypeDef		EXTI_InitStruct;
	NVIC_InitTypeDef  	NVIC_InitStruct;	
	
	RTC_AlarmCmd(RTC_Alarm_B,DISABLE); //关闭闹钟
	
	//时间设置
	RTC_AlarmTime.RTC_H12		= RTC_H12_PM;	//下午
	RTC_AlarmTime.RTC_Hours		= 16;			//时
	RTC_AlarmTime.RTC_Minutes	= 31;			//分
	RTC_AlarmTime.RTC_Seconds	= 10;			//ADC+
	
	RTC_AlarmStruct.RTC_AlarmTime			= RTC_AlarmTime;			 //设置闹钟时间
	RTC_AlarmStruct.RTC_AlarmMask			= RTC_AlarmMask_None;		 //无掩码位，按实际时间响应
	RTC_AlarmStruct.RTC_AlarmDateWeekDaySel 	= RTC_AlarmDateWeekDaySel_Date;//按日期响应
	RTC_AlarmStruct.RTC_AlarmDateWeekDay	= 15;					 //日期
	RTC_SetAlarm(RTC_Format_BIN, RTC_Alarm_B, &RTC_AlarmStruct);//配置闹钟参数
	
//	RTC_AlarmCmd(RTC_Alarm_B,ENABLE);//开启闹钟	
//	RTC_ITConfig(RTC_IT_ALRB, ENABLE);//开启配置闹钟中断
		
	EXTI_InitStruct.EXTI_Line	= EXTI_Line17;			//中断线17
	EXTI_InitStruct.EXTI_Mode	= EXTI_Mode_Interrupt;		//中断
	EXTI_InitStruct.EXTI_Trigger	= EXTI_Trigger_Rising;  	//上升沿
	EXTI_InitStruct.EXTI_LineCmd	= ENABLE;				//使能	
	EXTI_Init(&EXTI_InitStruct);		//初始化线上中断，设置触发条件等。
		
	NVIC_InitStruct.NVIC_IRQChannel				= RTC_Alarm_IRQn;  //中断通道
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority	= 1;			 //抢占优先级
	NVIC_InitStruct.NVIC_IRQChannelSubPriority		= 1;			 //响应优先级
	NVIC_InitStruct.NVIC_IRQChannelCmd				= ENABLE;		 //使能	
	NVIC_Init(&NVIC_InitStruct);		//配置中断分组（NVIC），并使能中断。

}


//编写闹钟中断服务函数：
void RTC_Alarm_IRQHandler(void)
{
	//判断中断标志位是否置为1
	if(EXTI_GetITStatus(EXTI_Line17) == SET)
	{
		if(RTC_GetFlagStatus(RTC_FLAG_ALRAF) == SET)//闹钟A
		{

			RTC_ClearFlag(RTC_FLAG_ALRAF);	//清空标志位
		}
		if(RTC_GetFlagStatus(RTC_FLAG_ALRBF) == SET)//闹钟B
		{

			RTC_ClearFlag(RTC_FLAG_ALRBF);	//清空标志位
		}			
		EXTI_ClearITPendingBit(EXTI_Line17);	//清除中断标志位
	}
}

//设置闹钟A时间函数
void Set_Clock1(u8 hour,u8 min,u8 sec,u8 day)
{
	RTC_AlarmTypeDef  	RTC_AlarmStruct;
	RTC_TimeTypeDef 		RTC_AlarmTime;
	
	RTC_AlarmCmd(RTC_Alarm_A,DISABLE);		 //关闭闹钟
	RTC_AlarmTime.RTC_H12		= RTC_H12_PM;//下午
	RTC_AlarmTime.RTC_Hours		= hour;	 //时
	RTC_AlarmTime.RTC_Minutes	= min;	 //分
	RTC_AlarmTime.RTC_Seconds	= sec;	 //ADC+
	
	RTC_AlarmStruct.RTC_AlarmTime			= RTC_AlarmTime;			 //设置闹钟时间
	RTC_AlarmStruct.RTC_AlarmMask			= RTC_AlarmMask_None;		 //无掩码位，按实际时间响应
	RTC_AlarmStruct.RTC_AlarmDateWeekDaySel 	= RTC_AlarmDateWeekDaySel_Date;//按日期响应
	RTC_AlarmStruct.RTC_AlarmDateWeekDay	= day;				 //日期	
	RTC_SetAlarm(RTC_Format_BIN, RTC_Alarm_A, &RTC_AlarmStruct);//配置闹钟参数
	RTC_AlarmCmd(RTC_Alarm_A,ENABLE);					//开启闹钟
		
	RTC_ITConfig(RTC_IT_ALRA, ENABLE);	//开启配置闹钟中断
}

//设置闹钟B时间函数
void Set_Clock2(u8 hour,u8 min,u8 sec,u8 day)
{
	RTC_AlarmTypeDef  	RTC_AlarmStruct;
	RTC_TimeTypeDef 		RTC_AlarmTime;
	
	RTC_AlarmCmd(RTC_Alarm_B,DISABLE);			//关闭闹钟
	RTC_AlarmTime.RTC_H12		= RTC_H12_PM;	//下午
	RTC_AlarmTime.RTC_Hours		= hour;		//时
	RTC_AlarmTime.RTC_Minutes	= min;		//分
	RTC_AlarmTime.RTC_Seconds	= sec;		//ADC+
	
	RTC_AlarmStruct.RTC_AlarmTime			= RTC_AlarmTime;		 	 //设置闹钟时间
	RTC_AlarmStruct.RTC_AlarmMask			= RTC_AlarmMask_None;	 	 //无掩码位，按实际时间响应
	RTC_AlarmStruct.RTC_AlarmDateWeekDaySel 	= RTC_AlarmDateWeekDaySel_Date;//按日期响应
	RTC_AlarmStruct.RTC_AlarmDateWeekDay	= day;				 //日期
	RTC_SetAlarm(RTC_Format_BIN, RTC_Alarm_B, &RTC_AlarmStruct);//配置闹钟参数	
	RTC_AlarmCmd(RTC_Alarm_B,ENABLE);//开启闹钟
	
	RTC_ITConfig(RTC_IT_ALRB, ENABLE);//开启配置闹钟中断

}

void Set_Date(u8 year,u8 mon,u8 day,u8 week)
{
	RTC_DateTypeDef 	RTC_DateStruct;
	
	RTC_DateStruct.RTC_Year		= year;		//年
	RTC_DateStruct.RTC_Month	= mon;		//月
	RTC_DateStruct.RTC_Date		= day;		//日
	RTC_DateStruct.RTC_WeekDay	= week; 		//星期
	RTC_SetDate(RTC_Format_BIN, &RTC_DateStruct);   //设置日期
}
	
void Set_Time(u8 hour,u8 min,u8 sec)
{
	RTC_TimeTypeDef 	RTC_TimeStruct;

	RTC_TimeStruct.RTC_H12		= RTC_H12_PM;	//下午
	RTC_TimeStruct.RTC_Hours	= hour;		//时
	RTC_TimeStruct.RTC_Minutes	= min;		//分
	RTC_TimeStruct.RTC_Seconds	= sec;		//ADC+
	RTC_SetTime(RTC_Format_BIN, &RTC_TimeStruct);	//设置时间

}	

void Set_Date_Time(u8 year,u8 mon,u8 day,u8 hour,u8 min,u8 sec,u8 week)
{
	RTC_TimeTypeDef 	RTC_TimeStruct;
	RTC_DateTypeDef 	RTC_DateStruct;

	RTC_TimeStruct.RTC_H12		= RTC_H12_PM;	//下午
	RTC_TimeStruct.RTC_Hours	= hour;		//时
	RTC_TimeStruct.RTC_Minutes	= min;		//分
	RTC_TimeStruct.RTC_Seconds	= sec;		//ADC+
	RTC_SetTime(RTC_Format_BIN, &RTC_TimeStruct);	//设置时间

	RTC_DateStruct.RTC_Year		= year;		//年
	RTC_DateStruct.RTC_Month	= mon;		//月
	RTC_DateStruct.RTC_Date		= day;		//日
	RTC_DateStruct.RTC_WeekDay	= week; 		//星期
	RTC_SetDate(RTC_Format_BIN, &RTC_DateStruct);	//设置日期
}


