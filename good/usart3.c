#include "usart3.h"
/***************************
����˵��
PB10  ---- USART3_TX
PB11 ---- USART3_RX
*************************/

extern unsigned long step_num ; 

u8 rx_i = 0;		//�����ַ�������ʹ�õı���
u8 rx_flag=0;		//ͨ���ñ�־λ�ɵ�USART3 �Ƿ�����������
u8 rx_count = 0;		//���ڼ�¼ͨ��USART3 ���յ����ַ����ݵĸ���
u8 buffer[32] = {0};	//���ڱ���ͨ��USART3 ���յ����ַ�����
u8 rx_buff[32] = {0};	//���ڱ���ͨ��USART3 ���յ��ѹ��˹����ַ�����



void Usart3_Init(u32 baudrate)
{	
	GPIO_InitTypeDef GPIO_InitStruct;
	USART_InitTypeDef USART_InitStruct;
	NVIC_InitTypeDef  NVIC_InitStruct;
	
//	����ʱ��ʹ�ܣ�GPIO ʱ��ʹ�ܡ�
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB , ENABLE);
	
//	�������Ÿ�����ӳ�䣺���� GPIO_PinAFConfig ������
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource10,GPIO_AF_USART3); 
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource11,GPIO_AF_USART3); 
	
//	GPIO ��ʼ�����ã�Ҫ����ģʽΪ���ù��ܡ�
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_10|GPIO_Pin_11;
	GPIO_InitStruct.GPIO_Mode =   GPIO_Mode_AF;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOB,&GPIO_InitStruct);
	
//	���ڲ�����ʼ�������ò����ʣ��ֳ�����żУ��Ȳ�����
	USART_InitStruct.USART_BaudRate            = baudrate;
	USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStruct.USART_Mode                = USART_Mode_Rx | USART_Mode_Tx;
	USART_InitStruct.USART_Parity              = USART_Parity_No;
	USART_InitStruct.USART_StopBits		 = USART_StopBits_1;
	USART_InitStruct.USART_WordLength  		 = USART_WordLength_8b;
	USART_Init(USART3,&USART_InitStruct);

//	�����жϲ��ҳ�ʼ�� NVIC��ʹ���жϣ������Ҫ���������жϲ���Ҫ������裩��
	NVIC_InitStruct.NVIC_IRQChannel                     =USART3_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority   = 0x01;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority          = 0x01;
	NVIC_InitStruct.NVIC_IRQChannelCmd			    = ENABLE;
	NVIC_Init(&NVIC_InitStruct);
	
//	����������������ж�
	USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);	
	
//	ʹ�ܴ��ڡ�
	USART_Cmd( USART3, ENABLE);
}

void USART3_IRQHandler(void)
{	
	if(USART_GetITStatus(USART3, USART_IT_RXNE) == SET)
	{
		//����жϱ�־λ
		USART_ClearITPendingBit(USART3, USART_IT_RXNE);
		buffer[rx_count++] = USART_ReceiveData(USART3);
		
		if(buffer[rx_count -1] == '-' && buffer[rx_count -2] == '-') //��--��β
		{
			for(rx_i=0;rx_i < (rx_count-2) ;rx_i++)
				rx_buff[rx_i] = buffer[rx_i];		//�����ַ����ݣ�ɾ����β�ַ�

			memset(buffer,0, sizeof(buffer));		//�������
			rx_count = 0;					//����ַ����ݸ����ļ�¼
			rx_flag = 1;					//��־�����������
		}		
	}	
}


/*Э��������USART3 ���յ������ݲ�⣬��������
���Э�飺
**	E�����ַ�����Ϊ14	 	F�����ַ�����Ϊ15
**	����Э�� ��֡ͷ  ���� ����  ����  ����  ֡β

**	�������ڸ�ʽ��֡ͷ �� �� �� ����
**	HFDA-XX-XX-XX-X--  �޸�����������
**	H8DY-XX--		 �޸���
**	H8DM-XX--		 �޸���	
**	H8DD-XX--		 �޸���
**	H7DW-X--		 �޸�����	

**	����ʱ���ʽ �� ֡ͷ ʱ �� ��
**	HETA-XX:XX:XX--	�޸�ʱ����
**	H8TH-XX--		�޸�ʱ
**	H8TM-XX--		�޸ķ�
**	H8TS-XX--		�޸���

**	�������Ӹ�ʽ �� 
**	HEC1-XX:XX:XX-xx--	��������A ʱ���� ����
**	HEC2-XX:XX:XX-xx--	��������B ʱ���� ����

**	��ȡ����ָ�� : 	H6 get step count
**	H6GSC--
*******************************************************************/
void Usart3_Deal(void)
{

	RTC_TimeTypeDef 	RTC_TimeStruct;//��ȡʱ��������
	RTC_DateTypeDef 	RTC_DateStruct;//��ȡ��������������
	char str[20];			   //�����ַ���
	
	RTC_GetTime(RTC_Format_BIN, &RTC_TimeStruct);	//��ȡʱ��
	RTC_GetDate(RTC_Format_BIN, &RTC_DateStruct);	//��ȡ����	
	
	if(rx_flag)					//���UASRT3 �ɹ��յ�����
	{
		while(rx_flag)		      //��ȡ���ݽ���
		{
			switch(rx_buff[1])	//Э������������ע��˵��
			{	
				case 'F':
				{
					switch(rx_buff[2])
					{
						case 'D':Set_Date((rx_buff[5]-'0')*10 + (rx_buff[6]-'0'),
							(rx_buff[8]-'0')*10 + (rx_buff[9]-'0'),
							(rx_buff[11]-'0')*10 + (rx_buff[12]-'0'),
							(rx_buff[14]-'0'));//��������������
						break;
					}
				}break;
				
				case 'E':
				{
					switch(rx_buff[2])
					{
						case 'T':Set_Time((rx_buff[5]-'0')*10 + (rx_buff[6]-'0'),
							(rx_buff[8]-'0')*10 + (rx_buff[9]-'0'),
							(rx_buff[11]-'0')*10 + (rx_buff[12]-'0'));//����ʱ����
						break;
						
						case 'C':
						{
							switch(rx_buff[3])
							{
								case '1': Set_Clock1((rx_buff[5]-'0')*10 + (rx_buff[6]-'0'),
									(rx_buff[8]-'0')*10 + (rx_buff[9]-'0'),
									(rx_buff[11]-'0')*10 + (rx_buff[12]-'0'),
									(rx_buff[14]-'0')*10 + (rx_buff[15]-'0'));//��������A						
								printf("����1������Ϊ��%d:%d:%d-%d\n",(rx_buff[5]-'0')*10 + (rx_buff[6]-'0'),
									(rx_buff[8]-'0')*10 + (rx_buff[9]-'0'),
									(rx_buff[11]-'0')*10 + (rx_buff[12]-'0'),
									(rx_buff[14]-'0')*10 + (rx_buff[15]-'0'));
								break;
								
								case '2': Set_Clock2((rx_buff[5]-'0')*10 + (rx_buff[6]-'0'),
									(rx_buff[8]-'0')*10 + (rx_buff[9]-'0'),
									(rx_buff[11]-'0')*10 + (rx_buff[12]-'0'),
									(rx_buff[14]-'0')*10 + (rx_buff[15]-'0'));//��������B								
								printf("����2������Ϊ��%d:%d:%d-%d\n",(rx_buff[5]-'0')*10 + (rx_buff[6]-'0'),
									(rx_buff[8]-'0')*10 + (rx_buff[9]-'0'),
									(rx_buff[11]-'0')*10 + (rx_buff[12]-'0'),
									(rx_buff[14]-'0')*10 + (rx_buff[15]-'0'));
								break;
							}
						}break;
					}
			
				}break;
				
				case '6'://H6GSC--
				{
					switch(rx_buff[2])
					{
						case 'G':
						{
							switch(rx_buff[3])
							{
								case 'S':printf("��ǰ����Ϊ��%d\n",step_num);//��ȡ��ǰ����
								u32tostr(step_num,str);//������ת���ַ���
								UART_Send_Str(str);//���ַ���ͨ��USART3 �����ֻ�	
								break;	
							}
						}break;	
					}	
				}break;
				
				case '7':
				{
					switch(rx_buff[2])
					{
						case 'D':
						{
							switch(rx_buff[3])
							{
								case 'W':Set_Date(RTC_DateStruct.RTC_Year,
									RTC_DateStruct.RTC_Month,
									RTC_DateStruct.RTC_Date,
									(rx_buff[5]-'0'));//��������
								break;	
							}
						}break;	
					}	
				}break;
				
				
				case '8':
				{
					switch(rx_buff[2])
					{
						case 'D':
						{
							switch(rx_buff[3])
							{
								case 'Y':Set_Date((rx_buff[5]-'0')*10 + (rx_buff[6]-'0'),
									RTC_DateStruct.RTC_Month,
									RTC_DateStruct.RTC_Date,
									RTC_DateStruct.RTC_WeekDay);//������
								break;
								
								case 'M':Set_Date(RTC_DateStruct.RTC_Year,
									(rx_buff[5]-'0')*10 + (rx_buff[6]-'0'),
									RTC_DateStruct.RTC_Date,
									RTC_DateStruct.RTC_WeekDay);//������								
								break;
								
								case 'D':Set_Date(RTC_DateStruct.RTC_Year,
									RTC_DateStruct.RTC_Month,
									(rx_buff[5]-'0')*10 + (rx_buff[6]-'0'),
									RTC_DateStruct.RTC_WeekDay);//������									
								break;	
							}
						}break;
						
						case 'T':	
						{
							switch(rx_buff[3])
							{
								case 'H':Set_Time((rx_buff[5]-'0')*10 + (rx_buff[6]-'0'),
									RTC_TimeStruct.RTC_Minutes,
									RTC_TimeStruct.RTC_Seconds);//����ʱ							
								break;
								
								case 'M':Set_Time(RTC_TimeStruct.RTC_Hours,
									(rx_buff[5]-'0')*10 + (rx_buff[6]-'0'),
									RTC_TimeStruct.RTC_Seconds);//���÷�
								break;
								
								case 'S':Set_Time(RTC_TimeStruct.RTC_Hours,
									RTC_TimeStruct.RTC_Minutes,
									(rx_buff[5]-'0')*10 + (rx_buff[6]-'0'));//������
								break;	
							}
						}break;	
					}	
				}break;	
			}	
		memset(rx_buff,0, sizeof(rx_buff));//�������		
		rx_flag = 0;			     //��մ��ڽ��ձ�־λ
		}
	}		
}

//���ڷ����ַ�������
void UART_Send_Str(char *s)
{
	u8 i = 0;
	while(s[i] !='\0') //�涨�ַ�����β�ַ�Ϊ��\0�����Լ��涨�ģ�ͨ������ u32tostr �涨
	{
		USART_SendData(USART3,s[i]);  
		//printf("s[%d] = %d\n" ,i,s[i]);//���ڵ��Է��������Ƿ�����		
		i++;
	}
}

//����ת�ַ�������
void u32tostr(unsigned long dat,char *str) 
{
	char temp[20]={0};
	u8 i=0,j=0;
	while(dat)
	{
		temp[i]=dat%10+0x30;			  //���������Ϊ �� ʮ �� ǧ λ���ֱ�����������
//		printf("temp[%d] = %d\n" ,i,temp[i]); //���ڵ��Է��������Ƿ�����	
		i++;
		dat/=10;
	}
	j=i;
	for(i=0;i<j;i++)
	{
	    str[i]=temp[j-i-1];		//�� �� ʮ �� ǧ λ ���Ӵ�С�������кô����������
	}
	if(i == 0) {str[i]='0';i++;}  //�����������Ϊ0 �� ֱ������ַ�0
	str[i] = '\0';			//�涨�ַ���������Ϊ��\0��,ֱ�ӹ涨�ģ����ں��洮�ڴ��������־ʹ��
}

