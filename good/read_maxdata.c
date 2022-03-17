#include "read_maxdata.h"

uint32_t 	aun_ir_buffer[500]; 	//IR LED sensor data
int32_t	n_ir_buffer_length;   	//data length
uint32_t 	aun_red_buffer[500];    //Red LED sensor data
int32_t 	n_sp02; 			//SPO2 value
int8_t 	ch_spo2_valid;  		 //indicator to show if the SP02 calculation is valid
int32_t 	n_heart_rate;  		 //heart rate value
int8_t  	ch_hr_valid;   		 //indicator to show if the heart rate calculation is valid
uint8_t 	uch_dummy;

#define MAX_BRIGHTNESS 255

extern u8 Statu_flag ; 			//OLED三种功能切换标志位
extern const unsigned char BMP2[];  //显示心率图
extern const unsigned char BMP3[];  //显示血氧图


extern u8 flag;
u8 x= 0;

void read_hr(void)
{

	int i;//variables to calculate the on-board LED brightness that reflects the heartbeats	
	uint32_t un_min, un_max, un_prev_data;  
	int32_t n_brightness;
	float f_temp;
	u8 temp[6];
	u8 str[100] ={0};
	u8 dis_hr=0;

	un_min=0x3FFFF;
	un_max=0;
	n_ir_buffer_length=500; //buffer length of 100 stores 5 seconds of samples running at 100sps
	//read the first 500 samples, and determine the signal range
	
	u8 flag_show = 1;
//	TIM_Cmd(TIM3,DISABLE);//禁止刷新时钟显示
	OLED_Fill(0x00);//全屏灭
	OLED_DrawBMP(0,0,128,8,(unsigned char *)BMP2);//测试BMP位图显示//显示心率图

	for(i=0;i<n_ir_buffer_length;i++)
	{
		
		while(MAX30102_INT==1);   //wait until the interrupt pin asserts
		max30102_FIFO_ReadBytes(REG_FIFO_DATA,temp);
		aun_red_buffer[i] =  (long)((long)((long)temp[0]&0x03)<<16) | (long)temp[1]<<8 | (long)temp[2];    // Combine values to get the actual number
		aun_ir_buffer[i] = (long)((long)((long)temp[3] & 0x03)<<16) |(long)temp[4]<<8 | (long)temp[5];   // Combine values to get the actual number

		if(un_min>aun_red_buffer[i])
			un_min=aun_red_buffer[i];    //update signal min
		if(un_max<aun_red_buffer[i])
			un_max=aun_red_buffer[i];    //update signal max
	}
	un_prev_data=aun_red_buffer[i];
	//calculate heart rate and SpO2 after first 500 samples (first 5 seconds of samples)
	maxim_heart_rate_and_oxygen_saturation(aun_ir_buffer, n_ir_buffer_length, 
								aun_red_buffer, &n_sp02, &ch_spo2_valid, 
								&n_heart_rate, &ch_hr_valid); 

	while(Statu_flag ==0 )
	{
		i=0;
		un_min=0x3FFFF;
		un_max=0;
		
		//dumping the first 100 sets of samples in the memory and shift the last 400 sets of samples to the top
		for(i=100;i<500;i++)
		{
			aun_red_buffer[i-100]=aun_red_buffer[i];
			aun_ir_buffer[i-100]=aun_ir_buffer[i];

			//update the signal min and max
			if(un_min>aun_red_buffer[i])
				un_min=aun_red_buffer[i];
			if(un_max<aun_red_buffer[i])
				un_max=aun_red_buffer[i];
		}
		//take 100 sets of samples before calculating the heart rate.
		for(i=400;i<500;i++)
		{
		  
			un_prev_data=aun_red_buffer[i-1];
			while(MAX30102_INT==1);
		  
			max30102_FIFO_ReadBytes(REG_FIFO_DATA,temp);
			aun_red_buffer[i] =  
				(long)((long)((long)temp[0]&0x03)<<16) | 
				(long)temp[1]<<8 | (long)temp[2];    // Combine values to get the actual number
			aun_ir_buffer[i] = 
				(long)((long)((long)temp[3] & 
				0x03)<<16) |(long)temp[4]<<8 | 
				(long)temp[5];   // Combine values to get the actual number
        
			if(aun_red_buffer[i]>un_prev_data)
			{
				f_temp=aun_red_buffer[i]-un_prev_data;
				f_temp/=(un_max-un_min);
				f_temp*=MAX_BRIGHTNESS;
				n_brightness-=(int)f_temp;
				if(n_brightness<0)
					n_brightness=0;
			}
			else
			{
				f_temp=un_prev_data-aun_red_buffer[i];
				f_temp/=(un_max-un_min);
				f_temp*=MAX_BRIGHTNESS;
				n_brightness+=(int)f_temp;
				if(n_brightness>MAX_BRIGHTNESS)
					n_brightness=MAX_BRIGHTNESS;
			}
			//send samples and calculation result to terminal program through UART
			if(n_heart_rate<120)//**/ ch_hr_valid == 1 && ch_spo2_valid ==1 && n_heart_rate<120 && n_sp02<101
			{
				dis_hr = n_heart_rate;
			}
			else
			{
				dis_hr = 0;

			}		
		}
		maxim_heart_rate_and_oxygen_saturation(aun_ir_buffer, n_ir_buffer_length, aun_red_buffer,
									 &n_sp02, &ch_spo2_valid, &n_heart_rate, &ch_hr_valid);						
		//显示刷新
		if(dis_hr >=50 && dis_hr<=120) 
		{		
			sprintf((char *)str,"HR:%3d ",dis_hr);//**HR:%3d SpO2:%3d 
	
		}
		if(flag)
		{	
			OLED_DrawLINE(x,dis_hr);
			x = x+2;
			if(x>=127) x = 0;
		}	


		if(flag_show)//将心率图那张图片清空，然后显示心率数据
		{
			OLED_Fill(0x00);//全屏灭
			flag_show = 0;
		}
		if(!flag)
			OLED_ShowStr(40,3,str,2);


		
	}	

	OLED_Fill(0x00);//全屏灭
}

void read_spo2(void)
{

	int i;//variables to calculate the on-board LED brightness that reflects the heartbeats	
	uint32_t un_min, un_max, un_prev_data;  
	int32_t n_brightness;
	float f_temp;
	u8 temp[6];
	u8 str[100] ={0};
	u8 dis_spo2=0;

	un_min=0x3FFFF;
	un_max=0;
	n_ir_buffer_length=300; //buffer length of 100 stores 5 seconds of samples running at 100sps
	//read the first 500 samples, and determine the signal range
	
	u8 flag_show = 1;
	
	OLED_Fill(0x00);//全屏灭
	OLED_DrawBMP(0,0,128,8,(unsigned char *)BMP3);//显示血氧图
	
	for(i=0;i<n_ir_buffer_length;i++)
	{
		
		while(MAX30102_INT==1);   //wait until the interrupt pin asserts
		max30102_FIFO_ReadBytes(REG_FIFO_DATA,temp);
		aun_red_buffer[i] =  (long)((long)((long)temp[0]&0x03)<<16) | (long)temp[1]<<8 | (long)temp[2];    // Combine values to get the actual number
		aun_ir_buffer[i] = (long)((long)((long)temp[3] & 0x03)<<16) |(long)temp[4]<<8 | (long)temp[5];   // Combine values to get the actual number

		if(un_min>aun_red_buffer[i])
			un_min=aun_red_buffer[i];    //update signal min
		if(un_max<aun_red_buffer[i])
			un_max=aun_red_buffer[i];    //update signal max
	}
	un_prev_data=aun_red_buffer[i];
	//calculate heart rate and SpO2 after first 500 samples (first 5 seconds of samples)
	maxim_heart_rate_and_oxygen_saturation(aun_ir_buffer, n_ir_buffer_length, aun_red_buffer, &n_sp02, &ch_spo2_valid, &n_heart_rate, &ch_hr_valid); 

	while(Statu_flag ==1 )
	{
		i=0;
		un_min=0x3FFFF;
		un_max=0;
		
		//dumping the first 100 sets of samples in the memory and shift the last 400 sets of samples to the top
		for(i=100;i<300;i++)
		{
			aun_red_buffer[i-100]=aun_red_buffer[i];
			aun_ir_buffer[i-100]=aun_ir_buffer[i];

			//update the signal min and max
			if(un_min>aun_red_buffer[i])
				un_min=aun_red_buffer[i];
			if(un_max<aun_red_buffer[i])
				un_max=aun_red_buffer[i];
		}
		//take 100 sets of samples before calculating the heart rate.
		for(i=200;i<300;i++)
		{
		  
			un_prev_data=aun_red_buffer[i-1];
			while(MAX30102_INT==1);
		  
			max30102_FIFO_ReadBytes(REG_FIFO_DATA,temp);
			aun_red_buffer[i] =  (long)((long)((long)temp[0]&0x03)<<16) | (long)temp[1]<<8 | (long)temp[2];    // Combine values to get the actual number
			aun_ir_buffer[i] = (long)((long)((long)temp[3] & 0x03)<<16) |(long)temp[4]<<8 | (long)temp[5];   // Combine values to get the actual number
        
			if(aun_red_buffer[i]>un_prev_data)
			{
				f_temp=aun_red_buffer[i]-un_prev_data;
				f_temp/=(un_max-un_min);
				f_temp*=MAX_BRIGHTNESS;
				n_brightness-=(int)f_temp;
				if(n_brightness<0)
					n_brightness=0;
			}
			else
			{
				f_temp=un_prev_data-aun_red_buffer[i];
				f_temp/=(un_max-un_min);
				f_temp*=MAX_BRIGHTNESS;
				n_brightness+=(int)f_temp;
				if(n_brightness>MAX_BRIGHTNESS)
					n_brightness=MAX_BRIGHTNESS;
			}
			//send samples and calculation result to terminal program through UART
			if(n_sp02<101)//**/ ch_hr_valid == 1 && ch_spo2_valid ==1 && n_heart_rate<120 && n_sp02<101
			{
				dis_spo2 = n_sp02;
			}
			else
			{
				dis_spo2 = 0;
			}	
		}
		maxim_heart_rate_and_oxygen_saturation(aun_ir_buffer, n_ir_buffer_length, aun_red_buffer, &n_sp02, &ch_spo2_valid, &n_heart_rate, &ch_hr_valid);
		
		//显示刷新
		if(dis_spo2 >90)  //**dis_hr == 0 && dis_spo2 == 0
		{
			sprintf((char *)str,"SpO2:%3d ",dis_spo2);//**HR:%3d SpO2:%3d 
		}
	
		if(flag_show)//将心率图那张图片清空，然后显示心率数据
		{
			OLED_Fill(0x00);//全屏灭
			flag_show = 0;
		}
		OLED_ShowStr(40,3,str,2);
	}	
	
	OLED_Fill(0x00);//全屏灭
}

