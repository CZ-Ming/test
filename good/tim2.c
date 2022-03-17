#include "tim2.h"
/********************************************
����˵����
TIM2 -- APB1 -- 84MHZ      ��Ƶϵ����Χ  1 �� 65536 ֮�䡣
TIM2 -- 32λ��ʱ��         32λ������Χ 0 �� 4 294 967 296
************************************************/

extern u8 wait_Init_flag;
extern const unsigned char BMP6[];  //����ͼƬ

void Tim2_Init(u16 Prescaler,u32 TIM_Period)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
	NVIC_InitTypeDef NVIC_InitStruct;

//	�ܶ�ʱ��ʱ�ӡ�
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

//	��ʼ����ʱ��������ARR,PSC��
	TIM_TimeBaseInitStruct.TIM_Prescaler     = Prescaler-1;   
	TIM_TimeBaseInitStruct.TIM_CounterMode   = TIM_CounterMode_Up;
	TIM_TimeBaseInitStruct.TIM_Period        =TIM_Period-1;              
	TIM_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseInit(TIM2,&TIM_TimeBaseInitStruct);
	
//	����ʱ���жϣ�����NVIC��
	NVIC_InitStruct.NVIC_IRQChannel                     =TIM2_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority   = 0x01;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority          = 0x01;
	NVIC_InitStruct.NVIC_IRQChannelCmd			    = ENABLE;
	NVIC_Init(&NVIC_InitStruct);
	
//	���� TIM2_DIER  ��������ж�
	TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE);	
//	ʹ�ܶ�ʱ����
	TIM_Cmd(TIM2,ENABLE);
	
}

void TIM2_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM2, TIM_IT_Update) == SET)//�ж��жϱ�־λ�Ƿ���Ϊ1
	{
		if(wait_Init_flag)
		{
			OLED_DrawBMP(0,0,128,8,(unsigned char *)BMP6);//��ʾ����logo
		}
		else
		{
			Usart3_Deal();			//���ڴ����������͵���Ϣ�� ���
		}
		TIM_ClearITPendingBit(TIM2, TIM_IT_Update);//����жϱ�־λ
	}
}


