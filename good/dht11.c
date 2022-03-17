#include "dht11.h"

/**********************************
引脚说明：
PG9 -- DQ
***********************************/

extern u8 Statu_flag ; 		//OLED三种功能切换标志位

extern const unsigned char BMP5[];


//初始Dht11
void Dht11_Init(void)
{
	GPIO_InitTypeDef  	GPIO_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOG, ENABLE);
	
	GPIO_InitStructure.GPIO_Pin 	= GPIO_Pin_9;		//第9号引脚
	GPIO_InitStructure.GPIO_Mode 	= GPIO_Mode_OUT;		//输出模式
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;		//推挽输出，增强驱动能力，引脚的输出电流更大
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;	//引脚的速度最大为100MHz
	GPIO_InitStructure.GPIO_PuPd 	= GPIO_PuPd_NOPULL;	//没有使用内部上拉电阻
	GPIO_Init(GPIOG, &GPIO_InitStructure);		
	PGout(9) = 1;					//温湿度模块还没有工作，那么它的触发引脚是高电平
}

//引脚模式变更
void Dht11_Pin_Mode(GPIOMode_TypeDef mode)
{
	GPIO_InitTypeDef  	GPIO_InitStructure;
	
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_9;		//第9号引脚
	GPIO_InitStructure.GPIO_Mode  = mode;			//输入/输出模式
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;		//推挽输出，增强驱动能力，引脚的输出电流更大
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;	//引脚的速度最大为100MHz
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;	//没有使用内部上拉电阻
	GPIO_Init(GPIOG, &GPIO_InitStructure);	
}

//启动DHT11正常返回0
int  Dht11_Start(void)
{
	int t = 0;
	
	Dht11_Pin_Mode(GPIO_Mode_OUT);
	//启动信号 
	PGout(9) = 1;
	delay_us(10);
	PGout(9) = 0;
	delay_ms(20);
	PGout(9) = 1;
	delay_us(30);
	
	/*回响信号 */
	Dht11_Pin_Mode(GPIO_Mode_IN);
	//等待低电平到来
	while(PGin(9) == 1)
	{
		t++;
		delay_us(5);
		if(t == 200) //1ms未有低电平
			return -1;		
	}	
	
	delay_us(40);
	t = 0;
	//等待高电平到来，过滤低电平
	while(PGin(9) == 0)
	{
		t++;
		delay_us(5);
		if(t == 200) //1ms未有低电平
			return -2;		
	}		
	
	delay_us(40);
	
	t = 0;
	//等待低电平到来，过滤高电平
	while(PGin(9) == 1)
	{
		t++;
		delay_us(5);
		if(t == 200) //1ms未有低电平
			return -3;		
	}		
	
	
	return 0;
}

//一次性读取八位数据合成一个字节
uint8_t Dht11_Read_Byte(void)
{
	u8 data = 0x00;  //1000 0000
	u16 i, t;
	
	
	for(i=0; i<8; i++)
	{
		t = 0;
		//等待高电平到来，过滤低电平
		while(PGin(9) == 0)
		{
			t++;
			delay_us(2);
			if(t == 100) //200us未有低电平
				return 0;		
		}				
		//延时再判断电平
		delay_us(40);
		
		if(PGin(9) == 1) //判断引脚是否为高电平
		{
			data |= (0x01<<(7-i));
			t = 0;
			//等待低电平到来，过滤高电平
			while(PGin(9) == 1)
			{
				t++;
				delay_us(2);
				if(t == 100) //200us未有低电平
					return 0;		
			}			
		}
	}
	return data;
}

//成功返回0，失败返回-1
int32_t Dht11_Read(u8 *data)
{
	u8 i;
	for(i=0; i<5; i++)
	{
		data[i] = Dht11_Read_Byte();
	}
	//判断数据是否正确
	if(data[4] == data[0]+data[1]+data[2]+data[3])
	{
		return 0;
	}
	else
	{
		return -1;
	}
}




void show_dh11(void)
{
	u8 DH11_data[5] ={0};
	u8 str[100] ={0};
	u8 ret =0 ;
	
	OLED_Fill(0x00);//全屏灭
	OLED_DrawBMP(0,0,80,8,(unsigned char *)BMP5);//显示血氧图
	
	while(Statu_flag == 3)
	{	
		ret = Dht11_Start();
		if(!ret)
		{
			ret = Dht11_Read(DH11_data);  
			if(!ret)
			{
				printf("湿度:%d.%d, 温度：%d.%d\n", DH11_data[0],DH11_data[1],DH11_data[2],DH11_data[3]);
				
				sprintf((char *)str,"%d.%d",DH11_data[2],DH11_data[3]);//湿度:%d.%d
				OLED_ShowStr(90,1,str,2);
				
				sprintf((char *)str,"%d.%d",DH11_data[0],DH11_data[1]);//温度：%d.%d
				OLED_ShowStr(90,5,str,2);

				delay_s(1);
			}
		}
	}
	OLED_Fill(0x00);//全屏灭
}
