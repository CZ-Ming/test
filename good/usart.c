#include "usart.h"
/*********************************
����˵��
PA9  ---- USART1_TX
PA10 ---- USART1_RX
*********************************/

//�ض���fputc����   printf ��һ����
int fputc(int ch, FILE *f)
{ 	
	USART_SendData(USART1,ch);  //ͨ�����ڷ�������
	while(USART_GetFlagStatus(USART1,USART_FLAG_TXE)==RESET);      
	return ch;
}

void Usart1_Init(int MYBaudRate)
{
	
	GPIO_InitTypeDef  	GPIO_InitStruct;
	USART_InitTypeDef	USART_InitStruct;
	
	//����ʱ��ʹ�ܣ�GPIO ʱ��ʹ��
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
	
	//�������Ÿ�����ӳ�䣺���� GPIO_PinAFConfig ������
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource9,GPIO_AF_USART1); 
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource10,GPIO_AF_USART1); 
	
	GPIO_InitStruct.GPIO_Pin	= GPIO_Pin_9|GPIO_Pin_10;	//����9 10
	GPIO_InitStruct.GPIO_Mode	= GPIO_Mode_AF;			//���ģʽ
	GPIO_InitStruct.GPIO_OType	= GPIO_OType_PP;			//�������
	GPIO_InitStruct.GPIO_PuPd	= GPIO_PuPd_UP;			//����
	GPIO_InitStruct.GPIO_Speed	= GPIO_Speed_50MHz;		//�ٶ�
	GPIO_Init(GPIOA, &GPIO_InitStruct);		
	//GPIO ��ʼ�����ã�Ҫ����ģʽΪ���ù��ܡ�
	
	USART_InitStruct.USART_BaudRate			= MYBaudRate;		//������
	USART_InitStruct.USART_Mode				= USART_Mode_Tx; 		//�շ�ģʽ ȫ˫��
	USART_InitStruct.USART_WordLength			= USART_WordLength_8b;	//8λ
	USART_InitStruct.USART_StopBits			= USART_StopBits_1;	//ֹͣλ
	USART_InitStruct.USART_Parity				= USART_Parity_No;	//����żУ��λ
	USART_InitStruct.USART_HardwareFlowControl	= USART_HardwareFlowControl_None;//��Ӳ��������
	//���ڲ�����ʼ�������ò����ʣ��ֳ�����żУ��Ȳ�����
	USART_Init(USART1, &USART_InitStruct);
	
	//ʹ�ܴ��ڡ�
	USART_Cmd(USART1, ENABLE);
}
