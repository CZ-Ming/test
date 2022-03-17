#include "tim3.h"

/*********************************************************
引脚说明：
TIM3 -- APB1 -- 84MHZ     分频系数范围  1 到 65536 之间。
TIM3 -- 16位定时器         16位计数范围 0 到 65535

函数使用说明：
1.在主函数初始化TIM3		Tim3_Init(8400,10000);
84M / 8400 = 10000  T = 0.0001s = 0.1ms 意思是计1个数用0.1ms 计10000个数为1s
2.其余时间在TIM3中断服务函数中设置
3.由于中断服务函数有LED0 ，因此在主函数初始化LED	Init_Led();
********************************************/

extern u8 step_count_open_flag ;
//extern unsigned int step_count;

void Tim3_Init(u16 Prescaler,u16 TIM_Period)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
	NVIC_InitTypeDef NVIC_InitStruct;

//	能定时器时钟。
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);

//	初始化定时器，配置ARR,PSC。
	TIM_TimeBaseInitStruct.TIM_Prescaler     = Prescaler-1;   
	TIM_TimeBaseInitStruct.TIM_CounterMode   = TIM_CounterMode_Up;
	TIM_TimeBaseInitStruct.TIM_Period        =TIM_Period-1;              
	TIM_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseInit(TIM3,&TIM_TimeBaseInitStruct);
	
//	启定时器中断，配置NVIC。
	NVIC_InitStruct.NVIC_IRQChannel                     =TIM3_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority   = 0x01;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority          = 0x01;
	NVIC_InitStruct.NVIC_IRQChannelCmd			    = ENABLE;
	NVIC_Init(&NVIC_InitStruct);
	
//	设置 TIM3_DIER  允许更新中断
	TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE);	

//	使能定时器。
	TIM_Cmd(TIM3,DISABLE);
}


void TIM3_IRQHandler(void)
{
	RTC_TimeTypeDef 	RTC_TimeStruct;
	RTC_DateTypeDef 	RTC_DateStruct;
	u8 str[100] = {0},i;
	if(TIM_GetITStatus(TIM3, TIM_IT_Update) == SET)//判断中断标志位是否置为1
	{
		
		RTC_GetTime(RTC_Format_BIN, &RTC_TimeStruct);	//获取时间
		RTC_GetDate(RTC_Format_BIN, &RTC_DateStruct);	//获取日期	
		
		printf("日期：20%d-%d-%d  星期:%d\n",RTC_DateStruct.RTC_Year, RTC_DateStruct.RTC_Month,
				RTC_DateStruct.RTC_Date, RTC_DateStruct.RTC_WeekDay);
		printf("时间：%d:%d:%d\n\n",RTC_TimeStruct.RTC_Hours, RTC_TimeStruct.RTC_Minutes,
				RTC_TimeStruct.RTC_Seconds);
		
		sprintf((char *)str,"20%d-%d-%d",RTC_DateStruct.RTC_Year, RTC_DateStruct.RTC_Month,
				RTC_DateStruct.RTC_Date);//**20%d-%d-%d	
		OLED_ShowStr(28,0,str,2);
		
		sprintf((char *)str,"%d:%d:%d ",RTC_TimeStruct.RTC_Hours, RTC_TimeStruct.RTC_Minutes,
				RTC_TimeStruct.RTC_Seconds);//**%d:%d:%d
		OLED_ShowStr(36,3,str,2);
		
		for(i=0;i<2;i++)
		{
			OLED_ShowCN(44+i*16,6,i);//显示中文 （星期）
		}
		
		sprintf((char *)str,"%d ", RTC_DateStruct.RTC_WeekDay);//**%d
		OLED_ShowStr(80,6,str,2);
		
		TIM_ClearITPendingBit(TIM3, TIM_IT_Update);//清除中断标志位
	}

}


