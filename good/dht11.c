#include "dht11.h"

/**********************************
����˵����
PG9 -- DQ
***********************************/

extern u8 Statu_flag ; 		//OLED���ֹ����л���־λ

extern const unsigned char BMP5[];


//��ʼDht11
void Dht11_Init(void)
{
	GPIO_InitTypeDef  	GPIO_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOG, ENABLE);
	
	GPIO_InitStructure.GPIO_Pin 	= GPIO_Pin_9;		//��9������
	GPIO_InitStructure.GPIO_Mode 	= GPIO_Mode_OUT;		//���ģʽ
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;		//�����������ǿ�������������ŵ������������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;	//���ŵ��ٶ����Ϊ100MHz
	GPIO_InitStructure.GPIO_PuPd 	= GPIO_PuPd_NOPULL;	//û��ʹ���ڲ���������
	GPIO_Init(GPIOG, &GPIO_InitStructure);		
	PGout(9) = 1;					//��ʪ��ģ�黹û�й�������ô���Ĵ��������Ǹߵ�ƽ
}

//����ģʽ���
void Dht11_Pin_Mode(GPIOMode_TypeDef mode)
{
	GPIO_InitTypeDef  	GPIO_InitStructure;
	
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_9;		//��9������
	GPIO_InitStructure.GPIO_Mode  = mode;			//����/���ģʽ
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;		//�����������ǿ�������������ŵ������������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;	//���ŵ��ٶ����Ϊ100MHz
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;	//û��ʹ���ڲ���������
	GPIO_Init(GPIOG, &GPIO_InitStructure);	
}

//����DHT11��������0
int  Dht11_Start(void)
{
	int t = 0;
	
	Dht11_Pin_Mode(GPIO_Mode_OUT);
	//�����ź� 
	PGout(9) = 1;
	delay_us(10);
	PGout(9) = 0;
	delay_ms(20);
	PGout(9) = 1;
	delay_us(30);
	
	/*�����ź� */
	Dht11_Pin_Mode(GPIO_Mode_IN);
	//�ȴ��͵�ƽ����
	while(PGin(9) == 1)
	{
		t++;
		delay_us(5);
		if(t == 200) //1msδ�е͵�ƽ
			return -1;		
	}	
	
	delay_us(40);
	t = 0;
	//�ȴ��ߵ�ƽ���������˵͵�ƽ
	while(PGin(9) == 0)
	{
		t++;
		delay_us(5);
		if(t == 200) //1msδ�е͵�ƽ
			return -2;		
	}		
	
	delay_us(40);
	
	t = 0;
	//�ȴ��͵�ƽ���������˸ߵ�ƽ
	while(PGin(9) == 1)
	{
		t++;
		delay_us(5);
		if(t == 200) //1msδ�е͵�ƽ
			return -3;		
	}		
	
	
	return 0;
}

//һ���Զ�ȡ��λ���ݺϳ�һ���ֽ�
uint8_t Dht11_Read_Byte(void)
{
	u8 data = 0x00;  //1000 0000
	u16 i, t;
	
	
	for(i=0; i<8; i++)
	{
		t = 0;
		//�ȴ��ߵ�ƽ���������˵͵�ƽ
		while(PGin(9) == 0)
		{
			t++;
			delay_us(2);
			if(t == 100) //200usδ�е͵�ƽ
				return 0;		
		}				
		//��ʱ���жϵ�ƽ
		delay_us(40);
		
		if(PGin(9) == 1) //�ж������Ƿ�Ϊ�ߵ�ƽ
		{
			data |= (0x01<<(7-i));
			t = 0;
			//�ȴ��͵�ƽ���������˸ߵ�ƽ
			while(PGin(9) == 1)
			{
				t++;
				delay_us(2);
				if(t == 100) //200usδ�е͵�ƽ
					return 0;		
			}			
		}
	}
	return data;
}

//�ɹ�����0��ʧ�ܷ���-1
int32_t Dht11_Read(u8 *data)
{
	u8 i;
	for(i=0; i<5; i++)
	{
		data[i] = Dht11_Read_Byte();
	}
	//�ж������Ƿ���ȷ
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
	
	OLED_Fill(0x00);//ȫ����
	OLED_DrawBMP(0,0,80,8,(unsigned char *)BMP5);//��ʾѪ��ͼ
	
	while(Statu_flag == 3)
	{	
		ret = Dht11_Start();
		if(!ret)
		{
			ret = Dht11_Read(DH11_data);  
			if(!ret)
			{
				printf("ʪ��:%d.%d, �¶ȣ�%d.%d\n", DH11_data[0],DH11_data[1],DH11_data[2],DH11_data[3]);
				
				sprintf((char *)str,"%d.%d",DH11_data[2],DH11_data[3]);//ʪ��:%d.%d
				OLED_ShowStr(90,1,str,2);
				
				sprintf((char *)str,"%d.%d",DH11_data[0],DH11_data[1]);//�¶ȣ�%d.%d
				OLED_ShowStr(90,5,str,2);

				delay_s(1);
			}
		}
	}
	OLED_Fill(0x00);//ȫ����
}
