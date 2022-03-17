#include "show_settime.h"

extern signed char year,mon,day,hour,min,sec,week;
extern u8 Statu_flag ; 		//OLED���ֹ����л���־λ

void show_time(void)
{
	u8 str[100] = {0},i;
	
	sprintf((char *)str,"20%d-%d-%d",year, mon,day);
	OLED_ShowStr(28,0,str,2);//����6*8�ַ�
				
	sprintf((char *)str,"%d:%d:%d ",hour, min,sec);
	OLED_ShowStr(36,3,str,2);//����6*8�ַ�
	
	for(i=0;i<2;i++)
	{
		OLED_ShowCN(44+i*16,6,i);//������ʾ����
	}
	sprintf((char *)str,"%d ", week);//**HR:%3d SpO2:%3d 
	OLED_ShowStr(80,6,str,2);//����6*8�ַ�
	
	while(Statu_flag == 4 ) //����ʱ��
	{

	}
	OLED_Fill(0x00);//ȫ����
}
