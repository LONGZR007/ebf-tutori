/****************************************************************************************************
 * ������OLED Ӳ������������
 *
 * ���ߣ�������
 *
 * �汾��v1.0.0    ���ڣ�2018-04-25
 *                                                                              �����Ǻ��Ƽ����޹�˾
****************************************************************************************************/

#include <stdio.h>

#ifndef STM32F10X_OLED_H
#define STM32F10X_OLED_H

#ifdef __cplusplus
 extern "C" {
#endif
	 
#define OLED_ADDRESS    0x3C
#define I2C_RETRIES                                 0x0701
#define I2C_TIMEOUT                                 0x0702
#define I2C_SLAVE                                   0x0703       //IIC�������ĵ�ַ����
#define I2C_BUS_MODE                                0x0780
	 
/* д���Ͷ��� */

typedef unsigned char uint8;
typedef unsigned short uint16;
typedef unsigned int uint32;
	 
typedef uint8 uint8;
typedef uint16 u16;
typedef uint32 u32;
	 
#define OLED_CMD  0	   // д����
#define OLED_DATA 1	   // д����

/* ��ʱ */
	 
#define OLED_MODE 0
#define SIZE 8
#define XLevelL		0x00
#define XLevelH		0x10
#define Max_Column	128
#define Max_Row		64
#define	Brightness	0xFF 
#define X_WIDTH 	128
#define Y_WIDTH 	64	

void OLED_DeInit(void);

/****************************************************************************************************
 * ��������ʼ��SSD1306����	
 *
 * ��������
 *
 * ���أ���
****************************************************************************************************/

void OLED_Init(void);

/****************************************************************************************************
 * ������oled i2c ��ʼ��
 *
 * ������void
 *
 * ���أ�void
****************************************************************************************************/

int oled_i2c(void);
	 
/****************************************************************************************************
 * ����������д��������
 *
 * ������Command = ����
 *
 * ���أ���
****************************************************************************************************/

void Write_IIC_Command(uint8 Command);

/****************************************************************************************************
 * ������������������
 *
 * ������Data = ����
 *
 * ���أ���
****************************************************************************************************/

void Write_IIC_Data(uint8 Data, uint8 aga);

/****************************************************************************************************
 * �������������� / ����
 *
 * ������dat = ����
 * ������cmd = ���͵������ݻ�������
 *
 * ���أ���
****************************************************************************************************/

void OLED_WR_Byte(uint8 dat,uint8 cmd,uint8 aga);

/****************************************************************************************************
 * ��������ʾһ��С��
 *
 * ������x, y:�������
 * ������num����ֵ
 * ������vir:С�������Чλ
 * ������size2�������С
 *
 * ���أ���
****************************************************************************************************/

void OLED_ShowDec(uint8 x,uint8 y,float num,uint8 vir,uint8 size2,uint8 aga);

/****************************************************************************************************
 * ��������ʾһ���������Բ�ָ������
 *
 * ������x, y:�������
 * ������num����ֵ
 * ������size2�������С
 *
 * ���أ���
****************************************************************************************************/

void OLED_ShowNum_NL(uint8 x,uint8 y,u32 num, uint8 size2,uint8 aga);

/* δ����ĺ������� */

void OLED_Display_On(void);
void OLED_Display_Off(void);	   							   		    
void OLED_Clear(void);
void OLED_DrawPoint(uint8 x,uint8 y,uint8 t);
void OLED_Fill(uint8 x1,uint8 y1,uint8 x2,uint8 y2,uint8 dot);
void OLED_ShowChar(uint8 x,uint8 y,uint8 chr,uint8 Char_Size,uint8 aga);
void OLED_ShowString(uint8 x,uint8 y,uint8 *chr,uint8 Char_Size,uint8 aga);	 
void OLED_Set_Pos(uint8 x, uint8 y);
void OLED_ShowCHinese(uint8 x,uint8 y,uint8 no,uint8 aga);
void OLED_DrawBMP(uint8 x0, uint8 y0,uint8 x1, uint8 y1,uint8 BMP[]);
void fill_picture(unsigned char fill_Data);
void OLED_ShowNum(uint8 x,uint8 y,u32 num,uint8 len,uint8 size2,uint8 aga);

/* ѡ�� I2C �Ŀ��� */

#define SE_I2C1    0    // I2C ʹ��ѡ��, = 1 ʹ�� I2C1; = 0 ʹ�� SI2C

#ifdef __cplusplus
}
#endif

#endif /* STM32F10X_OLED_H */

/* End Of File */
