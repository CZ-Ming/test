#include "footprint.h"

extern u8 Statu_flag ; 			//OLED���ֹ����л���־λ
extern const unsigned char BMP4[];  //�Ų�ͼ
unsigned long step_num = 0; 		//��¼��ǰ�ߵĲ���
 
void show_footprint(void)
{
	u8 str[100] ={0};

	OLED_Fill(0x00);//ȫ����
	OLED_DrawBMP(0,0,64,8,(unsigned char *)BMP4);//�Ų�ͼ
	
	while(Statu_flag == 2)
	{
		dmp_get_pedometer_step_count(&step_num);
		sprintf((char *)str,"%d",step_num);
		OLED_ShowStr(80,3,str,2);
		printf("step_num=%ld \n",step_num);
		delay_s(1);
	}
	OLED_Fill(0x00);//ȫ����
}

