#ifndef __IR_H__
#define __IR_H__


#include<reg51.h>
#include    "intrins.h"

#define     MAIN_Fosc       11059200L   //系统主时钟频率

sfr  TH2       = 0xD6;
sfr  TL2       = 0xD7;
sfr  IE2       = 0xAF;
sfr  INT_CLKO  = 0x8F;
sfr  AUXR      = 0x8E;
sfr  AUXR1     = 0xA2;
sfr  P_SW1     = 0xA2;
sfr  P_SW2     = 0xBA;
sfr  S2CON     = 0x9A;
sfr  S2BUF     = 0x9B;

sfr  ADC_CONTR = 0xBC;      //带AD系列
sfr  ADC_RES   = 0xBD;      //带AD系列
sfr  ADC_RESL  = 0xBE;      //带AD系列
sfr  P1ASF     = 0x9D;      //只写，模拟输入(AD或LVD)选择

sfr  CCON      = 0xD8;        //
sbit CCF0      = CCON^0;    //PCA 模块0中断标志，由硬件置位，必须由软件清0。
sbit CCF1      = CCON^1;    //PCA 模块1中断标志，由硬件置位，必须由软件清0。
sbit CCF2      = CCON^2;    //PCA 模块2中断标志，由硬件置位，必须由软件清0。
sbit CR        = CCON^6;    //1: 允许PCA计数器计数，必须由软件清0。
sbit CF        = CCON^7;    //PCA计数器溢出（CH，CL由FFFFH变为0000H）标志。PCA计数器溢出后由硬件置位，必须由软件清0。
sfr  CMOD      = 0xD9;      //
sfr  CCAPM0    = 0xDA;      //PCA模块0的工作模式寄存器。
sfr  CCAPM1    = 0xDB;      //PCA模块1的工作模式寄存器。
sfr  CCAPM2    = 0xDC;      //PCA模块2的工作模式寄存器。
sfr  CL        = 0xE9;      //
sfr  CCAP0L    = 0xEA;      //PCA模块0的捕捉/比较寄存器低8位。
sfr  CCAP1L    = 0xEB;      //PCA模块1的捕捉/比较寄存器低8位。
sfr  CCAP2L    = 0xEC;      //PCA模块2的捕捉/比较寄存器低8位。
sfr  CH        = 0xF9;
sfr  CCAP0H    = 0xFA;      //PCA模块0的捕捉/比较寄存器高8位。
sfr  CCAP1H    = 0xFB;      //PCA模块1的捕捉/比较寄存器高8位。
sfr  CCAP2H    = 0xFC;      //PCA模块2的捕捉/比较寄存器高8位。
sfr  PCA_PWM0  = 0xF2;      //PCA模块0 PWM寄存器。
sfr  PCA_PWM1  = 0xF3;      //PCA模块1 PWM寄存器。
sfr  PCA_PWM2  = 0xF4;      //PCA模块2 PWM寄存器。

sbit PPCA      = IP^7;      //



void DelayMs(unsigned int x);
void PCA_config(void);
void IR_TxPulse(unsigned int  pulse);
void IR_TxSpace(unsigned int  pulse);
void IR_TxByte(unsigned char dat);
void Send_UserCode(unsigned int senddata);

void timer0_init (void);
void IR_RX_NEC(void);

#endif