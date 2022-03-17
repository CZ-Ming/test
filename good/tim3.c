#include "tim3.h"

/*********************************************************
����˵����
TIM3 -- APB1 -- 84MHZ     ��Ƶϵ����Χ  1 �� 65536 ֮�䡣
TIM3 -- 16λ��ʱ��         16λ������Χ 0 �� 65535

����ʹ��˵����
1.����������ʼ��TIM3		Tim3_Init(8400,10000);
84M / 8400 = 10000  T = 0.0001s = 0.1ms ��˼�Ǽ�1������0.1ms ��10000����Ϊ1s
2.����ʱ����TIM3�жϷ�����������
3.�����жϷ�������LED0 ���������������ʼ��LED	Init_Led();
********************************************/

extern u8 step_count_open_flag ;
//extern unsigned int step_count;

void Tim3_Init(u16 Prescaler,u16 TIM_Period)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
	NVIC_InitTypeDef NVIC_InitStruct;

//	�ܶ�ʱ��ʱ�ӡ�
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);

//	��ʼ����ʱ��������ARR,PSC��
	TIM_TimeBaseInitStruct.TIM_Prescaler     = Prescaler-1;   
	TIM_TimeBaseInitStruct.TIM_CounterMode   = TIM_CounterMode_Up;
	TIM_TimeBaseInitStruct.TIM_Period        =TIM_Period-1;              
	TIM_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseInit(TIM3,&TIM_TimeBaseInitStruct);
	
//	����ʱ���жϣ�����NVIC��
	NVIC_InitStruct.NVIC_IRQChannel                     =TIM3_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority   = 0x01;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority          = 0x01;
	NVIC_InitStruct.NVIC_IRQChannelCmd			    = ENABLE;
	NVIC_Init(&NVIC_InitStruct);
	
//	���� TIM3_DIER  ��������ж�
	TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE);	

//	ʹ�ܶ�ʱ����
	TIM_Cmd(TIM3,DISABLE);
}


void TIM3_IRQHandler(void)
{
	RTC_TimeTypeDef 	RTC_TimeStruct;
	RTC_DateTypeDef 	RTC_DateStruct;
	u8 str[100] = {0},i;
	if(TIM_GetITStatus(TIM3, TIM_IT_Update) == SET)//�ж��жϱ�־λ�Ƿ���Ϊ1
	{
		
		RTC_GetTime(RTC_Format_BIN, &RTC_TimeStruct);	//��ȡʱ��
		RTC_GetDate(RTC_Format_BIN, &RTC_DateStruct);	//��ȡ����	
		
		printf("���ڣ�20%d-%d-%d  ����:%d\n",RTC_DateStruct.RTC_Year, RTC_DateStruct.RTC_Month,
				RTC_DateStruct.RTC_Date, RTC_DateStruct.RTC_WeekDay);
		printf("ʱ�䣺%d:%d:%d\n\n",RTC_TimeStruct.RTC_Hours, RTC_TimeStruct.RTC_Minutes,
				RTC_TimeStruct.RTC_Seconds);
		
		sprintf((char *)str,"20%d-%d-%d",RTC_DateStruct.RTC_Year, RTC_DateStruct.RTC_Month,
				RTC_DateStruct.RTC_Date);//**20%d-%d-%d	
		OLED_ShowStr(28,0,str,2);
		
		sprintf((char *)str,"%d:%d:%d ",RTC_TimeStruct.RTC_Hours, RTC_TimeStruct.RTC_Minutes,
				RTC_TimeStruct.RTC_Seconds);//**%d:%d:%d
		OLED_ShowStr(36,3,str,2);
		
		for(i=0;i<2;i++)
		{
			OLED_ShowCN(44+i*16,6,i);//��ʾ���� �����ڣ�
		}
		
		sprintf((char *)str,"%d ", RTC_DateStruct.RTC_WeekDay);//**%d
		OLED_ShowStr(80,6,str,2);
		
		TIM_ClearITPendingBit(TIM3, TIM_IT_Update);//����жϱ�־λ
	}

}


