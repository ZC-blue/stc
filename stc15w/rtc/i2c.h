#ifndef __I2C_H_
#define __I2C_H_

#include<reg51.h>

//---重定义关键词---//
#ifndef uchar
#define uchar unsigned char
#endif

#ifndef uint 
#define uint unsigned int
#endif

//--定义使用的IO口--//
sbit I2C_SCL = P1^1;
sbit I2C_SDA = P1^0;

//--声明全局变量--//
void  I2C_Delay10us();
void  I2C_Start();
void  I2C_Stop();
uchar I2C_SendByte(uchar dat, uchar ack);
uchar I2C_ReadByte();

void I2C_ReadReg(unsigned char addr,unsigned char *dat);
void I2C_WriteReg(unsigned char addr,unsigned char *dat);


#endif
