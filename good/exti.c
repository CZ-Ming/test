#include "exti.h"
/*************************************************
引脚说明：
KEY0----PA0
KEY1----PE2
KEY2----PE3
KEY3----PE4
*****************************************************/

u8 Statu_flag = 5; 	//OLED五种功能切换标志位
/*************************************
Statu_flag用于切换状态的标志位
0为心率
1为血氧
2为步数
3为温湿度采集
4为时间设置
5为正常显示模式
默认初始值为5

通过按键1进行切换
******************************************/

RTC_TimeTypeDef 	RTC_TimeStruct;
RTC_DateTypeDef 	RTC_DateStruct;
signed char year,mon,day,hour,min,sec,week;
/******************************
读取RTC中的时间存在以上两个变量中
将各个时间变量分别用以上变量存储
*********************************/

u8 set_time_whether_flag = 0;//判断进入修改时间模式，时间是否被需改  0为未修改 1 为修改
u8 set_time_flag =0;	//修改时间标志位
/*************************************
set_time_flag用于切换修改的变量
0为年
1为月
2为日
3为星期
4为时
5为分
6为秒
默认初始值为0
通过按键2进行切换
******************************************/

//u8 step_count_flag = 0;	//计步器启动与关闭标志位
///**************************************************
//计步器启动与关闭标志位
//step_count_flag =0 为停止计步
//step_count_flag =0 为正在计步
//***************************************************/

extern u8 x;


//按键外部中断初始化
void Init_Exti(void)
{	
	GPIO_InitTypeDef GPIO_InitStruct;//GPIO初始化
	EXTI_InitTypeDef EXTI_InitStruct;//外部中断初始化
	NVIC_InitTypeDef NVIC_InitStruct;//中断优先级设置
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);//使能中断时钟
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA | RCC_AHB1Periph_GPIOE , ENABLE);
	
	GPIO_InitStruct.GPIO_Pin  = GPIO_Pin_0;
	GPIO_InitStruct.GPIO_Mode =   GPIO_Mode_IN;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOA,&GPIO_InitStruct);//GPIO初始化
	
	GPIO_InitStruct.GPIO_Pin  = GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4;
	GPIO_Init(GPIOE,&GPIO_InitStruct);
	
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA,EXTI_PinSource0);//中断IO映射
		
	//外部中断初始化
	EXTI_InitStruct.EXTI_Line    = EXTI_Line0;	    //外部中断0
	EXTI_InitStruct.EXTI_Mode    = EXTI_Mode_Interrupt; //中断模式
	EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Falling;//下降沿触发
	EXTI_InitStruct.EXTI_LineCmd = ENABLE;		    //使能中断
	EXTI_Init(&EXTI_InitStruct);
	
	//中断优先级设置
	NVIC_InitStruct.NVIC_IRQChannel                    = EXTI0_IRQn;//设置中断通道
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority  = 1;		//抢占优先级为1
 	NVIC_InitStruct.NVIC_IRQChannelSubPriority         = 1;		//响应优先级为1
	NVIC_InitStruct.NVIC_IRQChannelCmd                 = ENABLE; 	//使能中断通道
	NVIC_Init(&NVIC_InitStruct);
	
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOE,EXTI_PinSource2);
	EXTI_InitStruct.EXTI_Line        = EXTI_Line2;	    
	EXTI_Init(&EXTI_InitStruct);
	NVIC_InitStruct.NVIC_IRQChannel  = EXTI2_IRQn;//设置中断通道
	NVIC_Init(&NVIC_InitStruct);
	
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOE,EXTI_PinSource3);
	EXTI_InitStruct.EXTI_Line        = EXTI_Line3;	    
	EXTI_Init(&EXTI_InitStruct);
	NVIC_InitStruct.NVIC_IRQChannel  = EXTI3_IRQn;//设置中断通道
	NVIC_Init(&NVIC_InitStruct);
	
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOE,EXTI_PinSource4);	
	EXTI_InitStruct.EXTI_Line        = EXTI_Line4;	    
	EXTI_Init(&EXTI_InitStruct);
	NVIC_InitStruct.NVIC_IRQChannel  = EXTI4_IRQn;//设置中断通道
	NVIC_Init(&NVIC_InitStruct);
	
}

//按键1
void EXTI0_IRQHandler(void)//中断服务函数
{

	if(EXTI_GetITStatus(EXTI_Line0) == SET)
	{		
		if(!GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_0))
		{
			delay_ms(15);//15ms延迟消抖	
			if(!GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_0))
			{
				switch(Statu_flag)
				{
					case 0: Statu_flag=1;break; 	//Statu_flag=1为含氧量
					case 1: Statu_flag=2;break;	//Statu_flag=2为步数
					case 2: Statu_flag=3;break;	//Statu_flag=3为温湿度采集
					case 3: Statu_flag=4;set_time_flag = 0;break;	//Statu_flag=4为时间设置
					case 4: 
					{
						if(set_time_whether_flag)//判断是否有修改数值，修改则更新时间数据
						{
							set_time_whether_flag = 0;//标志位清0
							Set_Date_Time(year ,mon ,day ,hour ,min ,sec ,week);//更新时间数据
						}
						Statu_flag=5;break;	//Statu_flag=5为正常显示时间	
					}						
					case 5: Statu_flag=0;break;	//Statu_flag=0为心率图		
				}
				
				if(Statu_flag == 4 )//Statu_flag=4为时间设置模式
				{
					RTC_GetTime(RTC_Format_BIN, &RTC_TimeStruct);	//获取时间
					RTC_GetDate(RTC_Format_BIN, &RTC_DateStruct);	//获取日期	
					year = RTC_DateStruct.RTC_Year;
					mon  = RTC_DateStruct.RTC_Month;
					day  = RTC_DateStruct.RTC_Date;
					hour = RTC_TimeStruct.RTC_Hours;
					min  = RTC_TimeStruct.RTC_Minutes;
					sec  = RTC_TimeStruct.RTC_Seconds;
					week = RTC_DateStruct.RTC_WeekDay;
//					printf("日期：20%d-%d-%d, 星期:%d\n",year, mon,day,week);//用于调试
//					printf("时间：%d:%d:%d\n\n",hour, min,sec);		   //用于调试			
				}
			}	
		}
		EXTI_ClearITPendingBit(EXTI_Line0);//清空中断标志位
	}
}


u8 flag =0;
//按键2
void EXTI2_IRQHandler(void)//中断服务函数
{
	if(EXTI_GetITStatus(EXTI_Line2) == SET)
	{
		if(!GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_2))
		{
			delay_ms(15);//15ms延迟消抖	
			if(!GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_2))
			{
				if(Statu_flag == 0)
				{
					flag =!flag;
					x= 0;
					OLED_Fill(0x00);//全屏灭
					
				}
				if(Statu_flag == 4)
				{
					switch(set_time_flag)
					{
						case 0: set_time_flag=1;break; 	//set_time_flag=1 月
						case 1: set_time_flag=2;break;	//set_time_flag=2 日
						case 2: set_time_flag=3;break;	//set_time_flag=3 星期	
						case 3: set_time_flag=4;break;	//set_time_flag=4 时
						case 4: set_time_flag=5;break;	//set_time_flag=5 分
						case 5: set_time_flag=6;break;	//set_time_flag=6 秒
						case 6: set_time_flag=0;break;	//set_time_flag=0	年
					}
				}
			}
		}
		EXTI_ClearITPendingBit(EXTI_Line2);//清空中断标志位
	}
}


//按键3
void EXTI3_IRQHandler(void)//中断服务函数
{
	u8 str[100] = {0},i;
	if(EXTI_GetITStatus(EXTI_Line3) == SET)
	{
		if(!GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_3))
		{
			delay_ms(15);//15ms延迟消抖	
			if(!GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_3))
			{
				if(Statu_flag == 4)
				{
					set_time_whether_flag = 1;
					switch(set_time_flag)
					{
						case 0:
						{
							year++;	//set_time_flag=0	年
							if(year >= 99)year =99;
						}break; 
						
						case 1: 
						{
							mon++;	//set_time_flag=1 月
							if(mon >= 12)mon =12;
						}
						break;
						
						case 2:
						{	
							day++;	//set_time_flag=2 日
							if(day >= 31)day =31;
							
						}break;
						
						case 3: 
						{
							week++;	//set_time_flag=3 星期
							if(week >= 7)week = 7;
						}break;
						
						case 4: 
						{
							hour++;	//set_time_flag=4 时
							if(hour >= 23)hour = 23;
						}break; 
						
						case 5:
						{	
							min++;	//set_time_flag=5 分
							if(min >= 59)min = 59;
						}break;
						
						case 6: 
						{
							sec++;	//set_time_flag=6 秒
							if(sec >= 59)sec = 59;
						}break;
				
					}
//					printf("日期：20%d-%d-%d, 星期:%d\n",year, mon,day,week);
//					printf("时间：%d:%d:%d\n",hour, min,sec);
					sprintf((char *)str,"20%d-%d-%d",year, mon,day);
					OLED_ShowStr(28,0,str,2);//测试6*8字符
								
					sprintf((char *)str,"%d:%d:%d ",hour, min,sec);
					OLED_ShowStr(36,3,str,2);//测试6*8字符
					
					for(i=0;i<2;i++)
					{
						OLED_ShowCN(44+i*16,6,i);//测试显示中文
					}
					sprintf((char *)str,"%d ", week);//**HR:%3d SpO2:%3d 
					OLED_ShowStr(80,6,str,2);//测试6*8字符
				}
			}
		}
		EXTI_ClearITPendingBit(EXTI_Line3);//清空中断标志位
	}
}

//按键4
void EXTI4_IRQHandler(void)//中断服务函数
{
	u8 str[100] = {0},i;
	if(EXTI_GetITStatus(EXTI_Line4) == SET)
	{
		if(!GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_4))
		{
			delay_ms(15);//15ms延迟消抖	
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
							year--;	//set_time_flag=0	年
							
						}break; 
						
						case 1: 
						{
							mon--;	//set_time_flag=1 月
							if(mon <= 1)mon =1;
						}
						break;
						
						case 2:
						{	
							day--;	//set_time_flag=2 日
							if(day <= 1)day =1;
							
						}break;
						
						case 3: 
						{
							week--;	//set_time_flag=3 星期
							if(week <= 1)week = 1;
						}break;
						
						case 4: 
						{
							if(hour <= 0)hour = 1;
							hour--;	//set_time_flag=4 时
							
						}break; 
						
						case 5:
						{	
							if(min <= 0)min = 1;
							min--;	//set_time_flag=5 分
							
						}break;
						
						case 6: 
						{
							if(sec <= 0)sec = 1;
							sec--;	//set_time_flag=6 秒
							
						}break;
				
				
					}
//					printf("日期：20%d-%d-%d, 星期:%d\n",year, mon,day,week);
//					printf("时间：%d:%d:%d\n",hour, min,sec);		
					sprintf((char *)str,"20%d-%d-%d",year, mon,day);
					OLED_ShowStr(28,0,str,2);//测试6*8字符
					
					sprintf((char *)str,"%d:%d:%d ",hour, min,sec);
					OLED_ShowStr(36,3,str,2);//测试6*8字符	

					for(i=0;i<2;i++)
					{
						OLED_ShowCN(44+i*16,6,i);//测试显示中文
					}
					sprintf((char *)str,"%d ", week);//**HR:%3d SpO2:%3d 
					OLED_ShowStr(80,6,str,2);//测试6*8字符					
				}
			}
		}
		EXTI_ClearITPendingBit(EXTI_Line4);//清空中断标志位
	}
}

