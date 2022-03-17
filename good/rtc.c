#include "rtc.h"

/*******************************************************
����ʹ��˵����
0.����������ʼ���ӳٺ���	   Init_Delay();
1.����������ʼ��RTC 		  Rtc_MyInit();
2.����������ʼ������A 		  RTC_AlarmA_Init();
3.����������ʼ������B		  RTC_AlarmB_Init();
**********************************************************/

void RTC_MyInit(void)
{
	RTC_InitTypeDef 	RTC_InitStruct;
	RTC_TimeTypeDef 	RTC_TimeStruct;
	RTC_DateTypeDef 	RTC_DateStruct;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);	//ʹ��PWRʱ��   
	PWR_BackupAccessCmd(ENABLE);					//ʹ�ܺ󱸼Ĵ�������
	
	if(RTC_ReadBackupRegister(RTC_BKP_DR0) != BKP_VALUE)  //�жϺ󱸼Ĵ���ֵ�Ƿ�ȴ�ĳֵ
	{
		RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);		// ����RTCʱ��Դ��ʹ��RTCʱ��
		RCC_RTCCLKCmd(ENABLE);
		
		RCC_LSEConfig(RCC_LSE_ON);				//���ʹ��LSE��Ҫ��LSE
		delay_ms(50);  						//��ʱ���ȴ�ʱ���ȶ�
		
		RTC_InitStruct.RTC_HourFormat	= RTC_HourFormat_24;//24Сʱ��
		RTC_InitStruct.RTC_AsynchPrediv	= 0x7F;	  //�첽ͨ����Ƶ�� 	128��Ƶ
		RTC_InitStruct.RTC_SynchPrediv	= 0xFF;	  //ͬ��ͨ����Ƶ�� 	256��Ƶ	
		RTC_Init(&RTC_InitStruct);				//��ʼ��RTC(ͬ��/�첽��Ƶϵ����ʱ�Ӹ�ʽ)
		
		RTC_TimeStruct.RTC_H12		= RTC_H12_PM;	//����
		RTC_TimeStruct.RTC_Hours	= 16;			//ʱ
		RTC_TimeStruct.RTC_Minutes	= 31;			//��
		RTC_TimeStruct.RTC_Seconds	= 0;			//ADC+
		RTC_SetTime(RTC_Format_BIN, &RTC_TimeStruct);	//����ʱ��
		
		RTC_DateStruct.RTC_Year		= 21;				//��
		RTC_DateStruct.RTC_Month	= 7;				//��
		RTC_DateStruct.RTC_Date		= 15;				//��
		RTC_DateStruct.RTC_WeekDay	= RTC_Weekday_Thursday; //������	
		RTC_SetDate(RTC_Format_BIN, &RTC_DateStruct);		//��������
	
		RTC_WriteBackupRegister(RTC_BKP_DR0, BKP_VALUE);	//ֵд��󱸼Ĵ���
	}
}

//��������A
void RTC_AlarmA_Init(void)
{
	RTC_AlarmTypeDef  	RTC_AlarmStruct;
	RTC_TimeTypeDef 		RTC_AlarmTime;
	EXTI_InitTypeDef		EXTI_InitStruct;
	NVIC_InitTypeDef  	NVIC_InitStruct;
	
	RTC_AlarmCmd(RTC_Alarm_A,DISABLE); //�ر�����
	
	//ʱ������
	RTC_AlarmTime.RTC_H12		= RTC_H12_PM;	//����
	RTC_AlarmTime.RTC_Hours		= 16;			//ʱ
	RTC_AlarmTime.RTC_Minutes	= 31;			//��
	RTC_AlarmTime.RTC_Seconds	= 5;			//ADC+
	
	RTC_AlarmStruct.RTC_AlarmTime			= RTC_AlarmTime;			 //��������ʱ��
	RTC_AlarmStruct.RTC_AlarmMask			= RTC_AlarmMask_None;		 //������λ����ʵ��ʱ����Ӧ
	RTC_AlarmStruct.RTC_AlarmDateWeekDaySel 	= RTC_AlarmDateWeekDaySel_Date;//��������Ӧ
	RTC_AlarmStruct.RTC_AlarmDateWeekDay	= 15;					 //����
	RTC_SetAlarm(RTC_Format_BIN, RTC_Alarm_A, &RTC_AlarmStruct);		 //�������Ӳ���
	
//	RTC_AlarmCmd(RTC_Alarm_A,ENABLE);//��������	
//	RTC_ITConfig(RTC_IT_ALRA, ENABLE);//�������������ж�
	
	EXTI_InitStruct.EXTI_Line	= EXTI_Line17;			//�ж���17
	EXTI_InitStruct.EXTI_Mode	= EXTI_Mode_Interrupt;		//�ж�
	EXTI_InitStruct.EXTI_Trigger	= EXTI_Trigger_Rising;  	//������
	EXTI_InitStruct.EXTI_LineCmd	= ENABLE;				//ʹ��
	EXTI_Init(&EXTI_InitStruct);		//��ʼ�������жϣ����ô��������ȡ�
		
	NVIC_InitStruct.NVIC_IRQChannel				= RTC_Alarm_IRQn;   //�ж�ͨ��
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority	= 1;			  //��ռ���ȼ�
	NVIC_InitStruct.NVIC_IRQChannelSubPriority		= 1;			  //��Ӧ���ȼ�
	NVIC_InitStruct.NVIC_IRQChannelCmd				= ENABLE;		  //ʹ��
	NVIC_Init(&NVIC_InitStruct);		//�����жϷ��飨NVIC������ʹ���жϡ�
}

//��������B
void RTC_AlarmB_Init(void)
{
	RTC_AlarmTypeDef  	RTC_AlarmStruct;
	RTC_TimeTypeDef 		RTC_AlarmTime;
	EXTI_InitTypeDef		EXTI_InitStruct;
	NVIC_InitTypeDef  	NVIC_InitStruct;	
	
	RTC_AlarmCmd(RTC_Alarm_B,DISABLE); //�ر�����
	
	//ʱ������
	RTC_AlarmTime.RTC_H12		= RTC_H12_PM;	//����
	RTC_AlarmTime.RTC_Hours		= 16;			//ʱ
	RTC_AlarmTime.RTC_Minutes	= 31;			//��
	RTC_AlarmTime.RTC_Seconds	= 10;			//ADC+
	
	RTC_AlarmStruct.RTC_AlarmTime			= RTC_AlarmTime;			 //��������ʱ��
	RTC_AlarmStruct.RTC_AlarmMask			= RTC_AlarmMask_None;		 //������λ����ʵ��ʱ����Ӧ
	RTC_AlarmStruct.RTC_AlarmDateWeekDaySel 	= RTC_AlarmDateWeekDaySel_Date;//��������Ӧ
	RTC_AlarmStruct.RTC_AlarmDateWeekDay	= 15;					 //����
	RTC_SetAlarm(RTC_Format_BIN, RTC_Alarm_B, &RTC_AlarmStruct);//�������Ӳ���
	
//	RTC_AlarmCmd(RTC_Alarm_B,ENABLE);//��������	
//	RTC_ITConfig(RTC_IT_ALRB, ENABLE);//�������������ж�
		
	EXTI_InitStruct.EXTI_Line	= EXTI_Line17;			//�ж���17
	EXTI_InitStruct.EXTI_Mode	= EXTI_Mode_Interrupt;		//�ж�
	EXTI_InitStruct.EXTI_Trigger	= EXTI_Trigger_Rising;  	//������
	EXTI_InitStruct.EXTI_LineCmd	= ENABLE;				//ʹ��	
	EXTI_Init(&EXTI_InitStruct);		//��ʼ�������жϣ����ô��������ȡ�
		
	NVIC_InitStruct.NVIC_IRQChannel				= RTC_Alarm_IRQn;  //�ж�ͨ��
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority	= 1;			 //��ռ���ȼ�
	NVIC_InitStruct.NVIC_IRQChannelSubPriority		= 1;			 //��Ӧ���ȼ�
	NVIC_InitStruct.NVIC_IRQChannelCmd				= ENABLE;		 //ʹ��	
	NVIC_Init(&NVIC_InitStruct);		//�����жϷ��飨NVIC������ʹ���жϡ�

}


//��д�����жϷ�������
void RTC_Alarm_IRQHandler(void)
{
	//�ж��жϱ�־λ�Ƿ���Ϊ1
	if(EXTI_GetITStatus(EXTI_Line17) == SET)
	{
		if(RTC_GetFlagStatus(RTC_FLAG_ALRAF) == SET)//����A
		{

			RTC_ClearFlag(RTC_FLAG_ALRAF);	//��ձ�־λ
		}
		if(RTC_GetFlagStatus(RTC_FLAG_ALRBF) == SET)//����B
		{

			RTC_ClearFlag(RTC_FLAG_ALRBF);	//��ձ�־λ
		}			
		EXTI_ClearITPendingBit(EXTI_Line17);	//����жϱ�־λ
	}
}

//��������Aʱ�亯��
void Set_Clock1(u8 hour,u8 min,u8 sec,u8 day)
{
	RTC_AlarmTypeDef  	RTC_AlarmStruct;
	RTC_TimeTypeDef 		RTC_AlarmTime;
	
	RTC_AlarmCmd(RTC_Alarm_A,DISABLE);		 //�ر�����
	RTC_AlarmTime.RTC_H12		= RTC_H12_PM;//����
	RTC_AlarmTime.RTC_Hours		= hour;	 //ʱ
	RTC_AlarmTime.RTC_Minutes	= min;	 //��
	RTC_AlarmTime.RTC_Seconds	= sec;	 //ADC+
	
	RTC_AlarmStruct.RTC_AlarmTime			= RTC_AlarmTime;			 //��������ʱ��
	RTC_AlarmStruct.RTC_AlarmMask			= RTC_AlarmMask_None;		 //������λ����ʵ��ʱ����Ӧ
	RTC_AlarmStruct.RTC_AlarmDateWeekDaySel 	= RTC_AlarmDateWeekDaySel_Date;//��������Ӧ
	RTC_AlarmStruct.RTC_AlarmDateWeekDay	= day;				 //����	
	RTC_SetAlarm(RTC_Format_BIN, RTC_Alarm_A, &RTC_AlarmStruct);//�������Ӳ���
	RTC_AlarmCmd(RTC_Alarm_A,ENABLE);					//��������
		
	RTC_ITConfig(RTC_IT_ALRA, ENABLE);	//�������������ж�
}

//��������Bʱ�亯��
void Set_Clock2(u8 hour,u8 min,u8 sec,u8 day)
{
	RTC_AlarmTypeDef  	RTC_AlarmStruct;
	RTC_TimeTypeDef 		RTC_AlarmTime;
	
	RTC_AlarmCmd(RTC_Alarm_B,DISABLE);			//�ر�����
	RTC_AlarmTime.RTC_H12		= RTC_H12_PM;	//����
	RTC_AlarmTime.RTC_Hours		= hour;		//ʱ
	RTC_AlarmTime.RTC_Minutes	= min;		//��
	RTC_AlarmTime.RTC_Seconds	= sec;		//ADC+
	
	RTC_AlarmStruct.RTC_AlarmTime			= RTC_AlarmTime;		 	 //��������ʱ��
	RTC_AlarmStruct.RTC_AlarmMask			= RTC_AlarmMask_None;	 	 //������λ����ʵ��ʱ����Ӧ
	RTC_AlarmStruct.RTC_AlarmDateWeekDaySel 	= RTC_AlarmDateWeekDaySel_Date;//��������Ӧ
	RTC_AlarmStruct.RTC_AlarmDateWeekDay	= day;				 //����
	RTC_SetAlarm(RTC_Format_BIN, RTC_Alarm_B, &RTC_AlarmStruct);//�������Ӳ���	
	RTC_AlarmCmd(RTC_Alarm_B,ENABLE);//��������
	
	RTC_ITConfig(RTC_IT_ALRB, ENABLE);//�������������ж�

}

void Set_Date(u8 year,u8 mon,u8 day,u8 week)
{
	RTC_DateTypeDef 	RTC_DateStruct;
	
	RTC_DateStruct.RTC_Year		= year;		//��
	RTC_DateStruct.RTC_Month	= mon;		//��
	RTC_DateStruct.RTC_Date		= day;		//��
	RTC_DateStruct.RTC_WeekDay	= week; 		//����
	RTC_SetDate(RTC_Format_BIN, &RTC_DateStruct);   //��������
}
	
void Set_Time(u8 hour,u8 min,u8 sec)
{
	RTC_TimeTypeDef 	RTC_TimeStruct;

	RTC_TimeStruct.RTC_H12		= RTC_H12_PM;	//����
	RTC_TimeStruct.RTC_Hours	= hour;		//ʱ
	RTC_TimeStruct.RTC_Minutes	= min;		//��
	RTC_TimeStruct.RTC_Seconds	= sec;		//ADC+
	RTC_SetTime(RTC_Format_BIN, &RTC_TimeStruct);	//����ʱ��

}	

void Set_Date_Time(u8 year,u8 mon,u8 day,u8 hour,u8 min,u8 sec,u8 week)
{
	RTC_TimeTypeDef 	RTC_TimeStruct;
	RTC_DateTypeDef 	RTC_DateStruct;

	RTC_TimeStruct.RTC_H12		= RTC_H12_PM;	//����
	RTC_TimeStruct.RTC_Hours	= hour;		//ʱ
	RTC_TimeStruct.RTC_Minutes	= min;		//��
	RTC_TimeStruct.RTC_Seconds	= sec;		//ADC+
	RTC_SetTime(RTC_Format_BIN, &RTC_TimeStruct);	//����ʱ��

	RTC_DateStruct.RTC_Year		= year;		//��
	RTC_DateStruct.RTC_Month	= mon;		//��
	RTC_DateStruct.RTC_Date		= day;		//��
	RTC_DateStruct.RTC_WeekDay	= week; 		//����
	RTC_SetDate(RTC_Format_BIN, &RTC_DateStruct);	//��������
}


