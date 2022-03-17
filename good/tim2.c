#include "tim2.h"
/********************************************
引脚说明：
TIM2 -- APB1 -- 84MHZ      分频系数范围  1 到 65536 之间。
TIM2 -- 32位定时器         32位计数范围 0 到 4 294 967 296
************************************************/

extern u8 wait_Init_flag;
extern const unsigned char BMP6[];  //开机图片

void Tim2_Init(u16 Prescaler,u32 TIM_Period)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
	NVIC_InitTypeDef NVIC_InitStruct;

//	能定时器时钟。
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

//	初始化定时器，配置ARR,PSC。
	TIM_TimeBaseInitStruct.TIM_Prescaler     = Prescaler-1;   
	TIM_TimeBaseInitStruct.TIM_CounterMode   = TIM_CounterMode_Up;
	TIM_TimeBaseInitStruct.TIM_Period        =TIM_Period-1;              
	TIM_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseInit(TIM2,&TIM_TimeBaseInitStruct);
	
//	启定时器中断，配置NVIC。
	NVIC_InitStruct.NVIC_IRQChannel                     =TIM2_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority   = 0x01;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority          = 0x01;
	NVIC_InitStruct.NVIC_IRQChannelCmd			    = ENABLE;
	NVIC_Init(&NVIC_InitStruct);
	
//	设置 TIM2_DIER  允许更新中断
	TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE);	
//	使能定时器。
	TIM_Cmd(TIM2,ENABLE);
	
}

void TIM2_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM2, TIM_IT_Update) == SET)//判断中断标志位是否置为1
	{
		if(wait_Init_flag)
		{
			OLED_DrawBMP(0,0,128,8,(unsigned char *)BMP6);//显示开机logo
		}
		else
		{
			Usart3_Deal();			//用于处理蓝牙发送的信息即 解包
		}
		TIM_ClearITPendingBit(TIM2, TIM_IT_Update);//清除中断标志位
	}
}


