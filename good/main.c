#include "usart.h"
#include "usart3.h"
#include "tim2.h"
#include "tim3.h"
#include "exti.h"
#include "rtc.h"
#include "delay.h"
#include "read_maxdata.h"
#include "footprint.h"
#include "dht11.h"
#include "show_settime.h"


extern u8 step_count_flag ;		//计步器启动与关闭标志位
extern u8 Statu_flag ; 			//OLED三种功能切换标志位

u8 TIM3_open_flag  =0 ;    
/*************************
TIM3计数器开：TIM3_open_flag= 1
TIM3计数器关：TIM3_open_flag= 0
************************/

u8 wait_Init_flag = 1;//作用于TIM2中，显示开机图片
/*************************
TIM2显示开机图片：		    wait_Init_flag= 1
TIM2用于控制蓝牙串口接收处理函数：wait_Init_flag= 0
************************/

int main(void)
{
	//设置NVIC分组（一个工程只能有一个分组） 抢占优先级范围:0~3  响应优先级范围:0~3 
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	
	Init_Delay();		//初始化延迟函数，使用嘀嗒定时器，自定义初始化
	
	Oled_I2C_Init();		//OLED的IIC初始化
	OLED_Init();		//OLED初始化
	OLED_CLS();			//清屏

	//先初始化TIM2，用于显示开机图片，同时等待所有传感器模块初始化完成
	Tim2_Init(84,1000000);  //初始化TIM2   设置定时中断时间为1s
	
	Usart1_Init(115200);	//初始化USART1，用于print调试使用  波特率为115200
	Usart3_Init(9600);	//初始化USART3,用于蓝牙通信  波特率为9600
	
	Init_Exti();		//初始化外部中断，用于控制按键
	
	RTC_MyInit();		//初始化RTC实时时钟
	RTC_AlarmA_Init();	//初始化RTC实时时钟，闹钟A
	RTC_AlarmB_Init();	//初始化RTC实时时钟，闹钟B
	
	Dht11_Init();		//初始化DHT11,温湿度传感器
	
	max30102_init();		//初始化MAX30102,心率血氧传感器
	
	MPU_IIC_Init();		//初始化MPU6050,陀螺仪传感器
	while(mpu_dmp_init())	//等待MPU6050初始化成功
	{
		printf("MPU6050 ERROR \r\n");
		delay_ms(500);
	}
	
	wait_Init_flag = 0;//作用于TIM2中，不再显示开机图片
	
	OLED_CLS();//清屏
	delay_ms(10);//延迟10ms
	
	//初始化TIM3  设置定时中断时间为1s，一定要最后初始化这个，初始化完就开始显示时间了，1s刷新一次
	Tim3_Init(8400,10000);	

	while(1)
	{
		while(Statu_flag == 0)			//心率
		{
			printf("0 \n");			//打印当前控制状态量
			if(TIM3_open_flag)
			{
				TIM3_open_flag = 0;
				TIM_Cmd(TIM3,DISABLE);  //禁止刷新时钟显示
			}
			read_hr();				//OLED显示心率数据界面
		}
		
		while(Statu_flag == 1)			//含氧量
		{
			printf("1 \n");			//打印当前控制状态量
			if(TIM3_open_flag)
				TIM_Cmd(TIM3,DISABLE);  //禁止刷新时钟显示
			read_spo2();			//OLED显示含氧量数据界面
		}
		
		while(Statu_flag == 2) 			//步数
		{
			printf("2 \n");			//打印当前控制状态量
			if(TIM3_open_flag)
				TIM_Cmd(TIM3,DISABLE);  //禁止刷新时钟显示
			show_footprint();			//OLED显示步数数据界面
		}
		
		while(Statu_flag == 3) 			//温湿度
		{
			printf("3 \n");			//打印当前控制状态量
			if(TIM3_open_flag)
				TIM_Cmd(TIM3,DISABLE);  //禁止刷新时钟显示
			show_dh11();			//OLED显示温湿度数据界面
		}
		
		while(Statu_flag == 4) 			//设置时间
		{
			printf("4 \n");			//打印当前控制状态量
			if(TIM3_open_flag)		
				TIM_Cmd(TIM3,DISABLE);  //禁止刷新时钟显示
			show_time();			//OLED显示设置时间数据界面
		}
		
		while(Statu_flag == 5 && !TIM3_open_flag) //正常显示时间
		{
			printf("5 \n");			//打印当前控制状态量
			TIM_Cmd(TIM3,ENABLE);		//刷新时钟显示
			TIM3_open_flag = 1;		//TIM3计数器处于打开状态
		}
	}
}





