/****************************************************************************************************
 * 描述：OLED 硬件驱动层实现
 *
 * 作者：龙泽润
 *
 * 版本：v1.0.0    日期：2018-04-25
 *                                                                        大连智海科技有限公司
****************************************************************************************************/

#include "./inc/drv_oled.h"
#include "./inc/drv_oledfont.h"
#include <sys/time.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>

/****************************************************************************************************
 * 描述：初始化SSD1306	
 *
 * 参数：无
 *
 * 返回：无
****************************************************************************************************/
  
void OLED_Init(void)
{
	oled_i2c();    // 初始化模拟 I2C
	
	OLED_WR_Byte(0xAE,OLED_CMD,0);//--display off
	OLED_WR_Byte(0x00,OLED_CMD,0);//---set low column address
	OLED_WR_Byte(0x10,OLED_CMD,0);//---set high column address
	OLED_WR_Byte(0x40,OLED_CMD,0);//--set start line address  
	OLED_WR_Byte(0xB0,OLED_CMD,0);//--set page address
	OLED_WR_Byte(0x81,OLED_CMD,0); // contract control
	OLED_WR_Byte(0xFF,OLED_CMD,0);//--128   
	OLED_WR_Byte(0xA1,OLED_CMD,0);//set segment remap 
	OLED_WR_Byte(0xA6,OLED_CMD,0);//--normal / reverse
	OLED_WR_Byte(0xA8,OLED_CMD,0);//--set multiplex ratio(1 to 64)
	OLED_WR_Byte(0x3F,OLED_CMD,0);//--1/32 duty
	OLED_WR_Byte(0xC8,OLED_CMD,0);//Com scan direction
	OLED_WR_Byte(0xD3,OLED_CMD,0);//-set display offset
	OLED_WR_Byte(0x00,OLED_CMD,0);//
	
	OLED_WR_Byte(0xD5,OLED_CMD,0);//set osc division
	OLED_WR_Byte(0x80,OLED_CMD,0);//
	
	OLED_WR_Byte(0xD8,OLED_CMD,0);//set area color mode off
	OLED_WR_Byte(0x05,OLED_CMD,0);//
	
	OLED_WR_Byte(0xD9,OLED_CMD,0);//Set Pre-Charge Period
	OLED_WR_Byte(0xF1,OLED_CMD,0);//
	
	OLED_WR_Byte(0xDA,OLED_CMD,0);//set com pin configuartion
	OLED_WR_Byte(0x12,OLED_CMD,0);//
	
	OLED_WR_Byte(0xDB,OLED_CMD,0);//set Vcomh
	OLED_WR_Byte(0x30,OLED_CMD,0);//
	
	OLED_WR_Byte(0x8D,OLED_CMD,0);//set charge pump enable
	OLED_WR_Byte(0x14,OLED_CMD,0);//
	
	OLED_WR_Byte(0xAF,OLED_CMD,0);//--turn on oled panel
	
	OLED_Clear();
}  

static int fd_i2c0;

/****************************************************************************************************
 * 描述：oled i2c 初始化
 *
 * 参数：void
 *
 * 返回：void
****************************************************************************************************/

int oled_i2c(void)
{
	fd_i2c0 = open("/dev/i2c-0", O_RDWR);    // 以读写的方式打开文件
	if (fd_i2c0 < 0)
	{
		printf("open /dev/i2c-0 fail\r\n");
		exit(1);
	}

	if (ioctl(fd_i2c0, I2C_SLAVE, OLED_ADDRESS) < 0)
	{
		printf("fail to set i2c device slave address!\r\n");
		close(fd_i2c0);
		return -1;
	}

	return 0;
}

void OLED_DeInit(void)
{
	close(fd_i2c0);
}

/****************************************************************************************************
 * 描述：发送数据 / 命令
 *
 * 参数：dat = 数据
 * 参数：cmd = 发送的是数据还是命令
 *
 * 返回：无
****************************************************************************************************/

void OLED_WR_Byte(uint8 dat,uint8 cmd,uint8 aga)
{
	if(cmd)
	{
		Write_IIC_Data(dat, aga);       // 发送数据
	}
	else 
	{
		Write_IIC_Command(dat);    // 发送命令
	}
}


/****************************************************************************************************
 *                                        使用 SI2C                                                 * 
****************************************************************************************************/

/****************************************************************************************************
 * 描述：发送写命令
 *
 * 参数：Command = 命令
 *
 * 返回：无
****************************************************************************************************/

void Write_IIC_Command(uint8 Command)
{
	int retries;
    uint8 data[2];

    data[0] = 0x00;
    data[1] = Command;

    for (retries = 5; retries; retries--)
    {
        if (write(fd_i2c0, data, 2) == 2)
        {
            // printf("write cmd succeed\r\n");
			return;
        }

		printf("write cmd fail -> (%d time)\r\n", retries);
        usleep(1000 * 10);
    }

    printf("write cmd fail\r\n");
}

/****************************************************************************************************
 * 描述：发送数据
 *
 * 参数：Data = 数据
 *
 * 返回：无
****************************************************************************************************/

void Write_IIC_Data(uint8 Data, uint8 aga)
{
	int retries;
    uint8 data[2];

    data[0] = 0x40;
    data[1] = aga ? Data : ~Data;

    for (retries = 5; retries; retries--)
    {
        if (write(fd_i2c0, data, 2) == 2)
        {
			// printf("write data succeed\r\n");
            return;

        }

		printf("write data fail -> (%d time)\r\n", retries);
        usleep(1000 * 10);
    }

	printf("write data fail\r\n");
}

 /* SI2CEND */


	
/* 未整理的函数实现 */

//存放格式如下.
//[0]0 1 2 3 ... 127	
//[1]0 1 2 3 ... 127	
//[2]0 1 2 3 ... 127	
//[3]0 1 2 3 ... 127	
//[4]0 1 2 3 ... 127	
//[5]0 1 2 3 ... 127	
//[6]0 1 2 3 ... 127	
//[7]0 1 2 3 ... 127 			   

/********************************************
// fill_Picture
********************************************/
void fill_picture(unsigned char fill_Data)
{
	unsigned char m,n;
	for(m=0;m<8;m++)
	{
		OLED_WR_Byte(0xb0+m,0,1);		//page0-page1
		OLED_WR_Byte(0x00,0,1);		//low column start address
		OLED_WR_Byte(0x10,0,1);		//high column start address
		for(n=0;n<128;n++)
			{
				OLED_WR_Byte(fill_Data,1,1);
			}
	}
}


//坐标设置

void OLED_Set_Pos(uint8 x, uint8 y) 
{ 	
	OLED_WR_Byte(0xb0+y,OLED_CMD,0);
	OLED_WR_Byte(((x&0xf0)>>4)|0x10,OLED_CMD,0);
	OLED_WR_Byte((x&0x0f),OLED_CMD,0); 
}   	  
//开启OLED显示    
void OLED_Display_On(void)
{
	OLED_WR_Byte(0X8D,OLED_CMD,1);  //SET DCDC命令
	OLED_WR_Byte(0X14,OLED_CMD,1);  //DCDC ON
	OLED_WR_Byte(0XAF,OLED_CMD,1);  //DISPLAY ON
}
//关闭OLED显示     
void OLED_Display_Off(void)
{
	OLED_WR_Byte(0X8D,OLED_CMD,1);  //SET DCDC命令
	OLED_WR_Byte(0X10,OLED_CMD,1);  //DCDC OFF
	OLED_WR_Byte(0XAE,OLED_CMD,1);  //DISPLAY OFF
}		   			 
//清屏函数,清完屏,整个屏幕是黑色的!和没点亮一样!!!	  
void OLED_Clear(void)  
{  
	uint8 i,n;		    
	for(i=0;i<8;i++)  
	{  
		OLED_WR_Byte (0xb0+i,OLED_CMD,1);    //设置页地址（0~7）
		OLED_WR_Byte (0x00,OLED_CMD,1);      //设置显示位置—列低地址
		OLED_WR_Byte (0x10,OLED_CMD,1);      //设置显示位置—列高地址   
		for(n=0;n<128;n++)OLED_WR_Byte(0,OLED_DATA,1); 
	} //更新显示
}
void OLED_On(void)  
{  
	uint8 i,n;		    
	for(i=0;i<8;i++)  
	{  
		OLED_WR_Byte (0xb0+i,OLED_CMD,1);    //设置页地址（0~7）
		OLED_WR_Byte (0x00,OLED_CMD,1);      //设置显示位置—列低地址
		OLED_WR_Byte (0x10,OLED_CMD,1);      //设置显示位置—列高地址   
		for(n=0;n<128;n++)OLED_WR_Byte(1,OLED_DATA,1); 
	} //更新显示
}
//在指定位置显示一个字符,包括部分字符
//x:0~127
//y:0~63
//aga:0,反白显示;1,正常显示				 
//size:选择字体 16/12 
// aga :1:正常显示 0：反显
void OLED_ShowChar(uint8 x,uint8 y,uint8 chr,uint8 Char_Size,uint8 aga)
{      	
	unsigned char c=0,i=0;	
	c=chr-' ';//得到偏移后的值			
	if(x > Max_Column-1)
	{
		x=0;
		y=y+2;
	}
	if(Char_Size ==16)
	{
		OLED_Set_Pos(x,y);	
		
		for(i=0;i<8;i++)
			OLED_WR_Byte(F8X16[c*16+i],OLED_DATA,aga);
		
		OLED_Set_Pos(x,y+1);
		
		for(i=0;i<8;i++)
			OLED_WR_Byte(F8X16[c*16+i+8],OLED_DATA,aga);
	}
	else 
	{	
		OLED_Set_Pos(x,y);
		for(i=0;i<6;i++)
			OLED_WR_Byte(F6x8[c][i],OLED_DATA,aga);
			
	}
}
//m^n函数
u32 oled_pow(uint8 m,uint8 n)
{
	u32 result=1;	 
	while(n--)result*=m;    
	return result;
}				  
//显示2个数字
//x,y :起点坐标	 
//len :数字的位数
//size:字体大小
//mode:模式	0,填充模式;1,叠加模式
//num:数值(0~4294967295);	 		  
void OLED_ShowNum(uint8 x,uint8 y,u32 num,uint8 len,uint8 size2,uint8 aga)
{         	
	uint8 t,temp;
	uint8 enshow=0;						   
	for(t=0;t<len;t++)
	{
		temp=(num/oled_pow(10,len-t-1))%10;
		if(enshow==0&&t<(len-1))
		{
			if(temp==0)
			{
				OLED_ShowChar(x+(size2/2)*t,y,' ',size2, aga);
				continue;
			}else enshow=1; 
		 	 
		}
	 	OLED_ShowChar(x+(size2/2)*t,y,temp+'0',size2, aga); 
	}
} 

//显示一个字符号串

void OLED_ShowString(uint8 x,uint8 y,uint8 *chr,uint8 Char_Size,uint8 aga)
{
	unsigned char j=0;
	while (chr[j]!='\0')
	{		OLED_ShowChar(x,y,chr[j],Char_Size,aga);
			x+=8;
		if(x>120){x=0;y+=2;}
			j++;
	}
}

//显示汉字

void OLED_ShowCHinese(uint8 x,uint8 y,uint8 no,uint8 aga)
{      			    
	uint8 t,adder=0;
	OLED_Set_Pos(x,y);	
    for(t=0;t<16;t++)
		{
				OLED_WR_Byte(Hzk[2*no][t],OLED_DATA,aga);
				adder+=1;
     }	
		OLED_Set_Pos(x,y+1);	
    for(t=0;t<16;t++)
			{	
				OLED_WR_Byte(Hzk[2*no+1][t],OLED_DATA,aga);
				adder+=1;
      }					
}

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

void OLED_ShowDec(uint8 x,uint8 y,float num,uint8 vir,uint8 size2,uint8 aga)
{
	uint8 xC = 0;    // 循环计数
	float temp_num = num;
	u32 temp_vir = vir;
	
	do
	{
		xC++;          // 计数整数位数
	}while((u32)(temp_num /= 10.0));
	
	OLED_ShowNum(x, y, (u32)num, xC, size2, aga);                                  // 显示整数部分
	
	OLED_ShowChar(x + xC * (size2 / 2), y, '.', size2, aga);                       // 显示小数点
	
	while(temp_vir--)
	{
		num *= 10.0;          // 计数整数位数do
	}
	
	OLED_ShowNum(x + (xC + 1) * (size2 / 2), y, (u32)num, vir, size2, aga);         // 显示小数部分
}

/****************************************************************************************************
 * 描述：显示一个整数可以不指定长度
 *
 * 参数：x, y:起点坐标
 * 参数：num：数值
 * 参数：size2：字体大小
 *
 * 返回：无
****************************************************************************************************/

void OLED_ShowNum_NL(uint8 x,uint8 y,u32 num, uint8 size2,uint8 aga)
{
	uint8 xC = 0;             // 循环计数
	u32 temp_num = num;    // 临时变量
	
	do
	{
		xC++;                              // 计数整数位数
	}while((u32)(temp_num /= 10.0));       // 未除尽
	
	OLED_ShowNum(x, y, num, xC, size2,aga);    // 显示整数
}

/***********功能描述：显示显示BMP图片128×64起始点坐标(x,y),x的范围0～127，y为页的范围0～7*****************/

void OLED_DrawBMP(uint8 x0, uint8 y0,uint8 x1, uint8 y1,uint8 BMP[])
{ 	
 unsigned int j=0;
 unsigned char x,y;
  
  if(y1%8==0) y=y1/8;      
  else y=y1/8+1;
	for(y=y0;y<y1;y++)
	{
		OLED_Set_Pos(x0,y);
    for(x=x0;x<x1;x++)
	    {      
	    	OLED_WR_Byte(BMP[j++],OLED_DATA,1);	    	
	    }
	}
} 

/* End Of File */
