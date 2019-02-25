#include "eeprom.h"


sfr IAP_DATA    =   0xC2;           //IAP数据寄存器
sfr IAP_ADDRH   =   0xC3;           //IAP地址寄存器高字节
sfr IAP_ADDRL   =   0xC4;           //IAP地址寄存器低字节
sfr IAP_CMD     =   0xC5;           //IAP命令寄存器
sfr IAP_TRIG    =   0xC6;           //IAP命令触发寄存器
sfr IAP_CONTR   =   0xC7;           //IAP控制寄存器

#define CMD_IDLE    0               //空闲模式
#define CMD_READ    1               //IAP字节读命令
#define CMD_PROGRAM 2               //IAP编程命令
#define CMD_ERASE   3               //IAP扇区删除命令


#define ENABLE_IAP  0x82            //if SYSCLK<20MHz


/******************************************
*函数名称：IapIdle
*输入参数：无
*输出参数：无
*函数功能：关闭IAP
*备		 注：
*******************************************/
void IapIdle(void)
{
    IAP_CONTR = 0;                  //关闭IAP功能
    IAP_CMD = 0;                    //清除命令寄存器
    IAP_TRIG = 0;                   //清除触发寄存器
    IAP_ADDRH = 0x80;               //将地址设置到非IAP区域
    IAP_ADDRL = 0;
}


/******************************************
*函数名称：IapReadByte
*输入参数：要读取的寄存器地址
*输出参数：读取到的数据
*函数功能：从IAP/ISP/EEPROM区域读取一个字节
*备		 注：
*******************************************/
unsigned char IapReadByte(unsigned int  addr)
{
    unsigned char dat;                       //数据缓冲区

    IAP_CONTR = ENABLE_IAP;         //使能IAP
    IAP_CMD = CMD_READ;             //设置IAP命令
    IAP_ADDRL = addr;               //设置IAP低地址
    IAP_ADDRH = addr >> 8;          //设置IAP高地址
    IAP_TRIG = 0x5a;                //写触发命令0x5a
    IAP_TRIG = 0xa5;                //写触发命令0xa5
    _nop_();                        //等待ISP/IAP/EEPROM操作完成
    dat = IAP_DATA;                 //读取ISP/IAP/EEPROM数据
    IapIdle();                      //关闭IAP功能

    return dat;                     //返回
}


/******************************************
*函数名称：IapProgramByte
*输入参数：要写的寄存器和要写入的数据
*输出参数：无
*函数功能：从IAP/ISP/EEPROM区域写一个字节
*备		 注：
*******************************************/
void IapProgramByte(unsigned int  addr, unsigned char  dat)
{
    IAP_CONTR = ENABLE_IAP;         //使能IAP
    IAP_CMD = CMD_PROGRAM;          //设置IAP命令
    IAP_ADDRL = addr;               //设置IAP低地址
    IAP_ADDRH = addr >> 8;          //设置IAP高地址
    IAP_DATA = dat;                 //写ISP/IAP/EEPROM数据
    IAP_TRIG = 0x5a;                //写触发命令0x5a
    IAP_TRIG = 0xa5;                //写触发命令0xa5
    _nop_();                        //等待ISP/IAP/EEPROM操作完成
    IapIdle();											//关闭IAP功能
}


/******************************************
*函数名称：IapEraseSector
*输入参数：扇区删除的起始地址
*输出参数：无
*函数功能：扇区擦除
*备		 注：
*******************************************/
void IapEraseSector(unsigned int  addr)
{
    IAP_CONTR = ENABLE_IAP;         //使能IAP
    IAP_CMD = CMD_ERASE;            //设置IAP命令
    IAP_ADDRL = addr;               //设置IAP低地址
    IAP_ADDRH = addr >> 8;          //设置IAP高地址
    IAP_TRIG = 0x5a;                //写触发命令0x5a
    IAP_TRIG = 0xa5;                //写触发命令0xa5
    _nop_();                        //等待ISP/IAP/EEPROM操作完成
    IapIdle();											//关闭IAP功能
}








