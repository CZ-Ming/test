#include "exti.h"
/*************************************************
����˵����
KEY0----PA0
KEY1----PE2
KEY2----PE3
KEY3----PE4
*****************************************************/

u8 Statu_flag = 5; 	//OLED���ֹ����л���־λ
/*************************************
Statu_flag�����л�״̬�ı�־λ
0Ϊ����
1ΪѪ��
2Ϊ����
3Ϊ��ʪ�Ȳɼ�
4Ϊʱ������
5Ϊ������ʾģʽ
Ĭ�ϳ�ʼֵΪ5

ͨ������1�����л�
******************************************/

RTC_TimeTypeDef 	RTC_TimeStruct;
RTC_DateTypeDef 	RTC_DateStruct;
signed char year,mon,day,hour,min,sec,week;
/******************************
��ȡRTC�е�ʱ�������������������
������ʱ������ֱ������ϱ����洢
*********************************/

u8 set_time_whether_flag = 0;//�жϽ����޸�ʱ��ģʽ��ʱ���Ƿ����  0Ϊδ�޸� 1 Ϊ�޸�
u8 set_time_flag =0;	//�޸�ʱ���־λ
/*************************************
set_time_flag�����л��޸ĵı���
0Ϊ��
1Ϊ��
2Ϊ��
3Ϊ����
4Ϊʱ
5Ϊ��
6Ϊ��
Ĭ�ϳ�ʼֵΪ0
ͨ������2�����л�
******************************************/

//u8 step_count_flag = 0;	//�Ʋ���������رձ�־λ
///**************************************************
//�Ʋ���������رձ�־λ
//step_count_flag =0 Ϊֹͣ�Ʋ�
//step_count_flag =0 Ϊ���ڼƲ�
//***************************************************/

extern u8 x;


//�����ⲿ�жϳ�ʼ��
void Init_Exti(void)
{	
	GPIO_InitTypeDef GPIO_InitStruct;//GPIO��ʼ��
	EXTI_InitTypeDef EXTI_InitStruct;//�ⲿ�жϳ�ʼ��
	NVIC_InitTypeDef NVIC_InitStruct;//�ж����ȼ�����
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);//ʹ���ж�ʱ��
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA | RCC_AHB1Periph_GPIOE , ENABLE);
	
	GPIO_InitStruct.GPIO_Pin  = GPIO_Pin_0;
	GPIO_InitStruct.GPIO_Mode =   GPIO_Mode_IN;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOA,&GPIO_InitStruct);//GPIO��ʼ��
	
	GPIO_InitStruct.GPIO_Pin  = GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4;
	GPIO_Init(GPIOE,&GPIO_InitStruct);
	
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA,EXTI_PinSource0);//�ж�IOӳ��
		
	//�ⲿ�жϳ�ʼ��
	EXTI_InitStruct.EXTI_Line    = EXTI_Line0;	    //�ⲿ�ж�0
	EXTI_InitStruct.EXTI_Mode    = EXTI_Mode_Interrupt; //�ж�ģʽ
	EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Falling;//�½��ش���
	EXTI_InitStruct.EXTI_LineCmd = ENABLE;		    //ʹ���ж�
	EXTI_Init(&EXTI_InitStruct);
	
	//�ж����ȼ�����
	NVIC_InitStruct.NVIC_IRQChannel                    = EXTI0_IRQn;//�����ж�ͨ��
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority  = 1;		//��ռ���ȼ�Ϊ1
 	NVIC_InitStruct.NVIC_IRQChannelSubPriority         = 1;		//��Ӧ���ȼ�Ϊ1
	NVIC_InitStruct.NVIC_IRQChannelCmd                 = ENABLE; 	//ʹ���ж�ͨ��
	NVIC_Init(&NVIC_InitStruct);
	
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOE,EXTI_PinSource2);
	EXTI_InitStruct.EXTI_Line        = EXTI_Line2;	    
	EXTI_Init(&EXTI_InitStruct);
	NVIC_InitStruct.NVIC_IRQChannel  = EXTI2_IRQn;//�����ж�ͨ��
	NVIC_Init(&NVIC_InitStruct);
	
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOE,EXTI_PinSource3);
	EXTI_InitStruct.EXTI_Line        = EXTI_Line3;	    
	EXTI_Init(&EXTI_InitStruct);
	NVIC_InitStruct.NVIC_IRQChannel  = EXTI3_IRQn;//�����ж�ͨ��
	NVIC_Init(&NVIC_InitStruct);
	
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOE,EXTI_PinSource4);	
	EXTI_InitStruct.EXTI_Line        = EXTI_Line4;	    
	EXTI_Init(&EXTI_InitStruct);
	NVIC_InitStruct.NVIC_IRQChannel  = EXTI4_IRQn;//�����ж�ͨ��
	NVIC_Init(&NVIC_InitStruct);
	
}

//����1
void EXTI0_IRQHandler(void)//�жϷ�����
{

	if(EXTI_GetITStatus(EXTI_Line0) == SET)
	{		
		if(!GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_0))
		{
			delay_ms(15);//15ms�ӳ�����	
			if(!GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_0))
			{
				switch(Statu_flag)
				{
					case 0: Statu_flag=1;break; 	//Statu_flag=1Ϊ������
					case 1: Statu_flag=2;break;	//Statu_flag=2Ϊ����
					case 2: Statu_flag=3;break;	//Statu_flag=3Ϊ��ʪ�Ȳɼ�
					case 3: Statu_flag=4;set_time_flag = 0;break;	//Statu_flag=4Ϊʱ������
					case 4: 
					{
						if(set_time_whether_flag)//�ж��Ƿ����޸���ֵ���޸������ʱ������
						{
							set_time_whether_flag = 0;//��־λ��0
							Set_Date_Time(year ,mon ,day ,hour ,min ,sec ,week);//����ʱ������
						}
						Statu_flag=5;break;	//Statu_flag=5Ϊ������ʾʱ��	
					}						
					case 5: Statu_flag=0;break;	//Statu_flag=0Ϊ����ͼ		
				}
				
				if(Statu_flag == 4 )//Statu_flag=4Ϊʱ������ģʽ
				{
					RTC_GetTime(RTC_Format_BIN, &RTC_TimeStruct);	//��ȡʱ��
					RTC_GetDate(RTC_Format_BIN, &RTC_DateStruct);	//��ȡ����	
					year = RTC_DateStruct.RTC_Year;
					mon  = RTC_DateStruct.RTC_Month;
					day  = RTC_DateStruct.RTC_Date;
					hour = RTC_TimeStruct.RTC_Hours;
					min  = RTC_TimeStruct.RTC_Minutes;
					sec  = RTC_TimeStruct.RTC_Seconds;
					week = RTC_DateStruct.RTC_WeekDay;
//					printf("���ڣ�20%d-%d-%d, ����:%d\n",year, mon,day,week);//���ڵ���
//					printf("ʱ�䣺%d:%d:%d\n\n",hour, min,sec);		   //���ڵ���			
				}
			}	
		}
		EXTI_ClearITPendingBit(EXTI_Line0);//����жϱ�־λ
	}
}


u8 flag =0;
//����2
void EXTI2_IRQHandler(void)//�жϷ�����
{
	if(EXTI_GetITStatus(EXTI_Line2) == SET)
	{
		if(!GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_2))
		{
			delay_ms(15);//15ms�ӳ�����	
			if(!GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_2))
			{
				if(Statu_flag == 0)
				{
					flag =!flag;
					x= 0;
					OLED_Fill(0x00);//ȫ����
					
				}
				if(Statu_flag == 4)
				{
					switch(set_time_flag)
					{
						case 0: set_time_flag=1;break; 	//set_time_flag=1 ��
						case 1: set_time_flag=2;break;	//set_time_flag=2 ��
						case 2: set_time_flag=3;break;	//set_time_flag=3 ����	
						case 3: set_time_flag=4;break;	//set_time_flag=4 ʱ
						case 4: set_time_flag=5;break;	//set_time_flag=5 ��
						case 5: set_time_flag=6;break;	//set_time_flag=6 ��
						case 6: set_time_flag=0;break;	//set_time_flag=0	��
					}
				}
			}
		}
		EXTI_ClearITPendingBit(EXTI_Line2);//����жϱ�־λ
	}
}


//����3
void EXTI3_IRQHandler(void)//�жϷ�����
{
	u8 str[100] = {0},i;
	if(EXTI_GetITStatus(EXTI_Line3) == SET)
	{
		if(!GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_3))
		{
			delay_ms(15);//15ms�ӳ�����	
			if(!GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_3))
			{
				if(Statu_flag == 4)
				{
					set_time_whether_flag = 1;
					switch(set_time_flag)
					{
						case 0:
						{
							year++;	//set_time_flag=0	��
							if(year >= 99)year =99;
						}break; 
						
						case 1: 
						{
							mon++;	//set_time_flag=1 ��
							if(mon >= 12)mon =12;
						}
						break;
						
						case 2:
						{	
							day++;	//set_time_flag=2 ��
							if(day >= 31)day =31;
							
						}break;
						
						case 3: 
						{
							week++;	//set_time_flag=3 ����
							if(week >= 7)week = 7;
						}break;
						
						case 4: 
						{
							hour++;	//set_time_flag=4 ʱ
							if(hour >= 23)hour = 23;
						}break; 
						
						case 5:
						{	
							min++;	//set_time_flag=5 ��
							if(min >= 59)min = 59;
						}break;
						
						case 6: 
						{
							sec++;	//set_time_flag=6 ��
							if(sec >= 59)sec = 59;
						}break;
				
					}
//					printf("���ڣ�20%d-%d-%d, ����:%d\n",year, mon,day,week);
//					printf("ʱ�䣺%d:%d:%d\n",hour, min,sec);
					sprintf((char *)str,"20%d-%d-%d",year, mon,day);
					OLED_ShowStr(28,0,str,2);//����6*8�ַ�
								
					sprintf((char *)str,"%d:%d:%d ",hour, min,sec);
					OLED_ShowStr(36,3,str,2);//����6*8�ַ�
					
					for(i=0;i<2;i++)
					{
						OLED_ShowCN(44+i*16,6,i);//������ʾ����
					}
					sprintf((char *)str,"%d ", week);//**HR:%3d SpO2:%3d 
					OLED_ShowStr(80,6,str,2);//����6*8�ַ�
				}
			}
		}
		EXTI_ClearITPendingBit(EXTI_Line3);//����жϱ�־λ
	}
}

//����4
void EXTI4_IRQHandler(void)//�жϷ�����
{
	u8 str[100] = {0},i;
	if(EXTI_GetITStatus(EXTI_Line4) == SET)
	{
		if(!GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_4))
		{
			delay_ms(15);//15ms�ӳ�����	
			if(!GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_4))
			{
				if(Statu_flag == 4)
				{
					set_time_whether_flag = 1;
					switch(set_time_flag)
					{
						case 0:
						{
							if(year <= 0)year =1;
							year--;	//set_time_flag=0	��
							
						}break; 
						
						case 1: 
						{
							mon--;	//set_time_flag=1 ��
							if(mon <= 1)mon =1;
						}
						break;
						
						case 2:
						{	
							day--;	//set_time_flag=2 ��
							if(day <= 1)day =1;
							
						}break;
						
						case 3: 
						{
							week--;	//set_time_flag=3 ����
							if(week <= 1)week = 1;
						}break;
						
						case 4: 
						{
							if(hour <= 0)hour = 1;
							hour--;	//set_time_flag=4 ʱ
							
						}break; 
						
						case 5:
						{	
							if(min <= 0)min = 1;
							min--;	//set_time_flag=5 ��
							
						}break;
						
						case 6: 
						{
							if(sec <= 0)sec = 1;
							sec--;	//set_time_flag=6 ��
							
						}break;
				
				
					}
//					printf("���ڣ�20%d-%d-%d, ����:%d\n",year, mon,day,week);
//					printf("ʱ�䣺%d:%d:%d\n",hour, min,sec);		
					sprintf((char *)str,"20%d-%d-%d",year, mon,day);
					OLED_ShowStr(28,0,str,2);//����6*8�ַ�
					
					sprintf((char *)str,"%d:%d:%d ",hour, min,sec);
					OLED_ShowStr(36,3,str,2);//����6*8�ַ�	

					for(i=0;i<2;i++)
					{
						OLED_ShowCN(44+i*16,6,i);//������ʾ����
					}
					sprintf((char *)str,"%d ", week);//**HR:%3d SpO2:%3d 
					OLED_ShowStr(80,6,str,2);//����6*8�ַ�					
				}
			}
		}
		EXTI_ClearITPendingBit(EXTI_Line4);//����жϱ�־λ
	}
}

