/****************************************************************************************************
 * 描述：OLED 硬件驱动层声明
 *
 * 作者：龙泽润
 *
 * 版本：v1.0.0    日期：2018-04-25
 *                                                                              大连智海科技有限公司
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
#define I2C_SLAVE                                   0x0703       //IIC从器件的地址设置
#define I2C_BUS_MODE                                0x0780
	 
/* 写类型定义 */

typedef unsigned char uint8;
typedef unsigned short uint16;
typedef unsigned int uint32;
	 
typedef uint8 uint8;
typedef uint16 u16;
typedef uint32 u32;
	 
#define OLED_CMD  0	   // 写命令
#define OLED_DATA 1	   // 写数据

/* 暂时 */
	 
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
 * 描述：初始化SSD1306声明	
 *
 * 参数：无
 *
 * 返回：无
****************************************************************************************************/

void OLED_Init(void);

/****************************************************************************************************
 * 描述：oled i2c 初始化
 *
 * 参数：void
 *
 * 返回：void
****************************************************************************************************/

int oled_i2c(void);
	 
/****************************************************************************************************
 * 描述：发送写命令声明
 *
 * 参数：Command = 命令
 *
 * 返回：无
****************************************************************************************************/

void Write_IIC_Command(uint8 Command);

/****************************************************************************************************
 * 描述：发送数据声明
 *
 * 参数：Data = 数据
 *
 * 返回：无
****************************************************************************************************/

void Write_IIC_Data(uint8 Data, uint8 aga);

/****************************************************************************************************
 * 描述：发送数据 / 命令
 *
 * 参数：dat = 数据
 * 参数：cmd = 发送的是数据还是命令
 *
 * 返回：无
****************************************************************************************************/

void OLED_WR_Byte(uint8 dat,uint8 cmd,uint8 aga);

/****************************************************************************************************
 * 描述：显示一个小数
 *
 * 参数：x, y:起点坐标
 * 参数：num：数值
 * 参数：vir:小数点的有效位
 * 参数：size2：字体大小
 *
 * 返回：无
****************************************************************************************************/

void OLED_ShowDec(uint8 x,uint8 y,float num,uint8 vir,uint8 size2,uint8 aga);

/****************************************************************************************************
 * 描述：显示一个整数可以不指定长度
 *
 * 参数：x, y:起点坐标
 * 参数：num：数值
 * 参数：size2：字体大小
 *
 * 返回：无
****************************************************************************************************/

void OLED_ShowNum_NL(uint8 x,uint8 y,u32 num, uint8 size2,uint8 aga);

/* 未整理的函数声明 */

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

/* 选择 I2C 的开关 */

#define SE_I2C1    0    // I2C 使用选择, = 1 使用 I2C1; = 0 使用 SI2C

#ifdef __cplusplus
}
#endif

#endif /* STM32F10X_OLED_H */

/* End Of File */
