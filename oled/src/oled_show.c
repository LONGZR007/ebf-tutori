/****************************************************************************************************
 * ������OLED ��ʵ����
 *
 * ���ߣ�������
 *
 * �汾��v1.0.0    ���ڣ�220-05-10
 *                                                                     
****************************************************************************************************/

#include "./inc/drv_oled.h"
#include <stdio.h>

int main(int argc, char* argv[])
{
	printf("oled show demo\r\n");

	OLED_Init();

	OLED_ShowString(0, 0, "oled show", 16, 1);
	OLED_ShowString(0, 2, "oled show", 16, 0);

	OLED_DeInit();

	return 0;
	// while (1)
	// {
	// 	OLED_ShowString(0, 0, "oled show", 16, 1);
	// }
	
}

/* End Of File */
