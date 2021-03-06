#include "usart.h"
/*********************************
引脚说明
PA9  ---- USART1_TX
PA10 ---- USART1_RX
*********************************/

//重定义fputc函数   printf 是一个宏
int fputc(int ch, FILE *f)
{ 	
	USART_SendData(USART1,ch);  //通过串口发送数据
	while(USART_GetFlagStatus(USART1,USART_FLAG_TXE)==RESET);      
	return ch;
}

void Usart1_Init(int MYBaudRate)
{
	
	GPIO_InitTypeDef  	GPIO_InitStruct;
	USART_InitTypeDef	USART_InitStruct;
	
	//串口时钟使能，GPIO 时钟使能
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
	
	//设置引脚复用器映射：调用 GPIO_PinAFConfig 函数。
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource9,GPIO_AF_USART1); 
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource10,GPIO_AF_USART1); 
	
	GPIO_InitStruct.GPIO_Pin	= GPIO_Pin_9|GPIO_Pin_10;	//引脚9 10
	GPIO_InitStruct.GPIO_Mode	= GPIO_Mode_AF;			//输出模式
	GPIO_InitStruct.GPIO_OType	= GPIO_OType_PP;			//推挽输出
	GPIO_InitStruct.GPIO_PuPd	= GPIO_PuPd_UP;			//上拉
	GPIO_InitStruct.GPIO_Speed	= GPIO_Speed_50MHz;		//速度
	GPIO_Init(GPIOA, &GPIO_InitStruct);		
	//GPIO 初始化设置：要设置模式为复用功能。
	
	USART_InitStruct.USART_BaudRate			= MYBaudRate;		//波特率
	USART_InitStruct.USART_Mode				= USART_Mode_Tx; 		//收发模式 全双工
	USART_InitStruct.USART_WordLength			= USART_WordLength_8b;	//8位
	USART_InitStruct.USART_StopBits			= USART_StopBits_1;	//停止位
	USART_InitStruct.USART_Parity				= USART_Parity_No;	//无奇偶校验位
	USART_InitStruct.USART_HardwareFlowControl	= USART_HardwareFlowControl_None;//无硬件控制流
	//串口参数初始化：设置波特率，字长，奇偶校验等参数。
	USART_Init(USART1, &USART_InitStruct);
	
	//使能串口。
	USART_Cmd(USART1, ENABLE);
}
