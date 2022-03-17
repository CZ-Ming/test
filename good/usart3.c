#include "usart3.h"
/***************************
引脚说明
PB10  ---- USART3_TX
PB11 ---- USART3_RX
*************************/

extern unsigned long step_num ; 

u8 rx_i = 0;		//过滤字符数据需使用的变量
u8 rx_flag=0;		//通过该标志位可得USART3 是否接收数据完成
u8 rx_count = 0;		//用于记录通过USART3 接收到的字符数据的个数
u8 buffer[32] = {0};	//用于保存通过USART3 接收到的字符数据
u8 rx_buff[32] = {0};	//用于保存通过USART3 接收到已过滤过的字符数据



void Usart3_Init(u32 baudrate)
{	
	GPIO_InitTypeDef GPIO_InitStruct;
	USART_InitTypeDef USART_InitStruct;
	NVIC_InitTypeDef  NVIC_InitStruct;
	
//	串口时钟使能，GPIO 时钟使能。
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB , ENABLE);
	
//	设置引脚复用器映射：调用 GPIO_PinAFConfig 函数。
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource10,GPIO_AF_USART3); 
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource11,GPIO_AF_USART3); 
	
//	GPIO 初始化设置：要设置模式为复用功能。
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_10|GPIO_Pin_11;
	GPIO_InitStruct.GPIO_Mode =   GPIO_Mode_AF;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOB,&GPIO_InitStruct);
	
//	串口参数初始化：设置波特率，字长，奇偶校验等参数。
	USART_InitStruct.USART_BaudRate            = baudrate;
	USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStruct.USART_Mode                = USART_Mode_Rx | USART_Mode_Tx;
	USART_InitStruct.USART_Parity              = USART_Parity_No;
	USART_InitStruct.USART_StopBits		 = USART_StopBits_1;
	USART_InitStruct.USART_WordLength  		 = USART_WordLength_8b;
	USART_Init(USART3,&USART_InitStruct);

//	开启中断并且初始化 NVIC，使能中断（如果需要开启串口中断才需要这个步骤）。
	NVIC_InitStruct.NVIC_IRQChannel                     =USART3_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority   = 0x01;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority          = 0x01;
	NVIC_InitStruct.NVIC_IRQChannelCmd			    = ENABLE;
	NVIC_Init(&NVIC_InitStruct);
	
//	设置允许接收数据中断
	USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);	
	
//	使能串口。
	USART_Cmd( USART3, ENABLE);
}

void USART3_IRQHandler(void)
{	
	if(USART_GetITStatus(USART3, USART_IT_RXNE) == SET)
	{
		//清除中断标志位
		USART_ClearITPendingBit(USART3, USART_IT_RXNE);
		buffer[rx_count++] = USART_ReceiveData(USART3);
		
		if(buffer[rx_count -1] == '-' && buffer[rx_count -2] == '-') //以--结尾
		{
			for(rx_i=0;rx_i < (rx_count-2) ;rx_i++)
				rx_buff[rx_i] = buffer[rx_i];		//过滤字符数据，删除结尾字符

			memset(buffer,0, sizeof(buffer));		//清空数组
			rx_count = 0;					//清空字符数据个数的记录
			rx_flag = 1;					//标志接收数据完成
		}		
	}	
}


/*协议解包，将USART3 接收到的数据拆解，分析功能
解包协议：
**	E代表字符长度为14	 	F代表字符长度为15
**	发送协议 ：帧头  长度 命令  类型  数据  帧尾

**	设置日期格式：帧头 年 月 日 星期
**	HFDA-XX-XX-XX-X--  修改年月日星期
**	H8DY-XX--		 修改年
**	H8DM-XX--		 修改月	
**	H8DD-XX--		 修改日
**	H7DW-X--		 修改星期	

**	设置时间格式 ： 帧头 时 分 秒
**	HETA-XX:XX:XX--	修改时分秒
**	H8TH-XX--		修改时
**	H8TM-XX--		修改分
**	H8TS-XX--		修改秒

**	设置闹钟格式 ： 
**	HEC1-XX:XX:XX-xx--	设置闹钟A 时分秒 日期
**	HEC2-XX:XX:XX-xx--	设置闹钟B 时分秒 日期

**	获取步数指令 : 	H6 get step count
**	H6GSC--
*******************************************************************/
void Usart3_Deal(void)
{

	RTC_TimeTypeDef 	RTC_TimeStruct;//获取时分秒数据
	RTC_DateTypeDef 	RTC_DateStruct;//获取年月日星期数据
	char str[20];			   //保存字符串
	
	RTC_GetTime(RTC_Format_BIN, &RTC_TimeStruct);	//获取时间
	RTC_GetDate(RTC_Format_BIN, &RTC_DateStruct);	//获取日期	
	
	if(rx_flag)					//如果UASRT3 成功收到数据
	{
		while(rx_flag)		      //收取数据结束
		{
			switch(rx_buff[1])	//协议解包，看上面注释说明
			{	
				case 'F':
				{
					switch(rx_buff[2])
					{
						case 'D':Set_Date((rx_buff[5]-'0')*10 + (rx_buff[6]-'0'),
							(rx_buff[8]-'0')*10 + (rx_buff[9]-'0'),
							(rx_buff[11]-'0')*10 + (rx_buff[12]-'0'),
							(rx_buff[14]-'0'));//设置年月日星期
						break;
					}
				}break;
				
				case 'E':
				{
					switch(rx_buff[2])
					{
						case 'T':Set_Time((rx_buff[5]-'0')*10 + (rx_buff[6]-'0'),
							(rx_buff[8]-'0')*10 + (rx_buff[9]-'0'),
							(rx_buff[11]-'0')*10 + (rx_buff[12]-'0'));//设置时分秒
						break;
						
						case 'C':
						{
							switch(rx_buff[3])
							{
								case '1': Set_Clock1((rx_buff[5]-'0')*10 + (rx_buff[6]-'0'),
									(rx_buff[8]-'0')*10 + (rx_buff[9]-'0'),
									(rx_buff[11]-'0')*10 + (rx_buff[12]-'0'),
									(rx_buff[14]-'0')*10 + (rx_buff[15]-'0'));//设置闹钟A						
								printf("闹钟1已设置为：%d:%d:%d-%d\n",(rx_buff[5]-'0')*10 + (rx_buff[6]-'0'),
									(rx_buff[8]-'0')*10 + (rx_buff[9]-'0'),
									(rx_buff[11]-'0')*10 + (rx_buff[12]-'0'),
									(rx_buff[14]-'0')*10 + (rx_buff[15]-'0'));
								break;
								
								case '2': Set_Clock2((rx_buff[5]-'0')*10 + (rx_buff[6]-'0'),
									(rx_buff[8]-'0')*10 + (rx_buff[9]-'0'),
									(rx_buff[11]-'0')*10 + (rx_buff[12]-'0'),
									(rx_buff[14]-'0')*10 + (rx_buff[15]-'0'));//设置闹钟B								
								printf("闹钟2已设置为：%d:%d:%d-%d\n",(rx_buff[5]-'0')*10 + (rx_buff[6]-'0'),
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
								case 'S':printf("当前步数为：%d\n",step_num);//获取当前步数
								u32tostr(step_num,str);//将整数转成字符串
								UART_Send_Str(str);//将字符串通过USART3 发回手机	
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
									(rx_buff[5]-'0'));//设置星期
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
									RTC_DateStruct.RTC_WeekDay);//设置年
								break;
								
								case 'M':Set_Date(RTC_DateStruct.RTC_Year,
									(rx_buff[5]-'0')*10 + (rx_buff[6]-'0'),
									RTC_DateStruct.RTC_Date,
									RTC_DateStruct.RTC_WeekDay);//设置月								
								break;
								
								case 'D':Set_Date(RTC_DateStruct.RTC_Year,
									RTC_DateStruct.RTC_Month,
									(rx_buff[5]-'0')*10 + (rx_buff[6]-'0'),
									RTC_DateStruct.RTC_WeekDay);//设置日									
								break;	
							}
						}break;
						
						case 'T':	
						{
							switch(rx_buff[3])
							{
								case 'H':Set_Time((rx_buff[5]-'0')*10 + (rx_buff[6]-'0'),
									RTC_TimeStruct.RTC_Minutes,
									RTC_TimeStruct.RTC_Seconds);//设置时							
								break;
								
								case 'M':Set_Time(RTC_TimeStruct.RTC_Hours,
									(rx_buff[5]-'0')*10 + (rx_buff[6]-'0'),
									RTC_TimeStruct.RTC_Seconds);//设置分
								break;
								
								case 'S':Set_Time(RTC_TimeStruct.RTC_Hours,
									RTC_TimeStruct.RTC_Minutes,
									(rx_buff[5]-'0')*10 + (rx_buff[6]-'0'));//设置秒
								break;	
							}
						}break;	
					}	
				}break;	
			}	
		memset(rx_buff,0, sizeof(rx_buff));//清空数组		
		rx_flag = 0;			     //清空串口接收标志位
		}
	}		
}

//串口发送字符串函数
void UART_Send_Str(char *s)
{
	u8 i = 0;
	while(s[i] !='\0') //规定字符串结尾字符为‘\0’，自己规定的，通过函数 u32tostr 规定
	{
		USART_SendData(USART3,s[i]);  
		//printf("s[%d] = %d\n" ,i,s[i]);//用于调试发送数据是否正常		
		i++;
	}
}

//整数转字符串函数
void u32tostr(unsigned long dat,char *str) 
{
	char temp[20]={0};
	u8 i=0,j=0;
	while(dat)
	{
		temp[i]=dat%10+0x30;			  //将整数拆解为 个 十 百 千 位，分别存放于数组中
//		printf("temp[%d] = %d\n" ,i,temp[i]); //用于调试发送数据是否正常	
		i++;
		dat/=10;
	}
	j=i;
	for(i=0;i<j;i++)
	{
	    str[i]=temp[j-i-1];		//将 个 十 百 千 位 按从大到小依次排列好存放在数组中
	}
	if(i == 0) {str[i]='0';i++;}  //如果传入数据为0 ， 直接输出字符0
	str[i] = '\0';			//规定字符串结束符为‘\0’,直接规定的，用于后面串口传输结束标志使用
}

