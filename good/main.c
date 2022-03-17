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


extern u8 step_count_flag ;		//�Ʋ���������رձ�־λ
extern u8 Statu_flag ; 			//OLED���ֹ����л���־λ

u8 TIM3_open_flag  =0 ;    
/*************************
TIM3����������TIM3_open_flag= 1
TIM3�������أ�TIM3_open_flag= 0
************************/

u8 wait_Init_flag = 1;//������TIM2�У���ʾ����ͼƬ
/*************************
TIM2��ʾ����ͼƬ��		    wait_Init_flag= 1
TIM2���ڿ����������ڽ��մ�������wait_Init_flag= 0
************************/

int main(void)
{
	//����NVIC���飨һ������ֻ����һ�����飩 ��ռ���ȼ���Χ:0~3  ��Ӧ���ȼ���Χ:0~3 
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	
	Init_Delay();		//��ʼ���ӳٺ�����ʹ����શ�ʱ�����Զ����ʼ��
	
	Oled_I2C_Init();		//OLED��IIC��ʼ��
	OLED_Init();		//OLED��ʼ��
	OLED_CLS();			//����

	//�ȳ�ʼ��TIM2��������ʾ����ͼƬ��ͬʱ�ȴ����д�����ģ���ʼ�����
	Tim2_Init(84,1000000);  //��ʼ��TIM2   ���ö�ʱ�ж�ʱ��Ϊ1s
	
	Usart1_Init(115200);	//��ʼ��USART1������print����ʹ��  ������Ϊ115200
	Usart3_Init(9600);	//��ʼ��USART3,��������ͨ��  ������Ϊ9600
	
	Init_Exti();		//��ʼ���ⲿ�жϣ����ڿ��ư���
	
	RTC_MyInit();		//��ʼ��RTCʵʱʱ��
	RTC_AlarmA_Init();	//��ʼ��RTCʵʱʱ�ӣ�����A
	RTC_AlarmB_Init();	//��ʼ��RTCʵʱʱ�ӣ�����B
	
	Dht11_Init();		//��ʼ��DHT11,��ʪ�ȴ�����
	
	max30102_init();		//��ʼ��MAX30102,����Ѫ��������
	
	MPU_IIC_Init();		//��ʼ��MPU6050,�����Ǵ�����
	while(mpu_dmp_init())	//�ȴ�MPU6050��ʼ���ɹ�
	{
		printf("MPU6050 ERROR \r\n");
		delay_ms(500);
	}
	
	wait_Init_flag = 0;//������TIM2�У�������ʾ����ͼƬ
	
	OLED_CLS();//����
	delay_ms(10);//�ӳ�10ms
	
	//��ʼ��TIM3  ���ö�ʱ�ж�ʱ��Ϊ1s��һ��Ҫ����ʼ���������ʼ����Ϳ�ʼ��ʾʱ���ˣ�1sˢ��һ��
	Tim3_Init(8400,10000);	

	while(1)
	{
		while(Statu_flag == 0)			//����
		{
			printf("0 \n");			//��ӡ��ǰ����״̬��
			if(TIM3_open_flag)
			{
				TIM3_open_flag = 0;
				TIM_Cmd(TIM3,DISABLE);  //��ֹˢ��ʱ����ʾ
			}
			read_hr();				//OLED��ʾ�������ݽ���
		}
		
		while(Statu_flag == 1)			//������
		{
			printf("1 \n");			//��ӡ��ǰ����״̬��
			if(TIM3_open_flag)
				TIM_Cmd(TIM3,DISABLE);  //��ֹˢ��ʱ����ʾ
			read_spo2();			//OLED��ʾ���������ݽ���
		}
		
		while(Statu_flag == 2) 			//����
		{
			printf("2 \n");			//��ӡ��ǰ����״̬��
			if(TIM3_open_flag)
				TIM_Cmd(TIM3,DISABLE);  //��ֹˢ��ʱ����ʾ
			show_footprint();			//OLED��ʾ�������ݽ���
		}
		
		while(Statu_flag == 3) 			//��ʪ��
		{
			printf("3 \n");			//��ӡ��ǰ����״̬��
			if(TIM3_open_flag)
				TIM_Cmd(TIM3,DISABLE);  //��ֹˢ��ʱ����ʾ
			show_dh11();			//OLED��ʾ��ʪ�����ݽ���
		}
		
		while(Statu_flag == 4) 			//����ʱ��
		{
			printf("4 \n");			//��ӡ��ǰ����״̬��
			if(TIM3_open_flag)		
				TIM_Cmd(TIM3,DISABLE);  //��ֹˢ��ʱ����ʾ
			show_time();			//OLED��ʾ����ʱ�����ݽ���
		}
		
		while(Statu_flag == 5 && !TIM3_open_flag) //������ʾʱ��
		{
			printf("5 \n");			//��ӡ��ǰ����״̬��
			TIM_Cmd(TIM3,ENABLE);		//ˢ��ʱ����ʾ
			TIM3_open_flag = 1;		//TIM3���������ڴ�״̬
		}
	}
}





