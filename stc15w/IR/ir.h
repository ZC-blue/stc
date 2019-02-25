#ifndef __IR_H__
#define __IR_H__


#include<reg51.h>
#include    "intrins.h"

#define     MAIN_Fosc       11059200L   //ϵͳ��ʱ��Ƶ��

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

sfr  ADC_CONTR = 0xBC;      //��ADϵ��
sfr  ADC_RES   = 0xBD;      //��ADϵ��
sfr  ADC_RESL  = 0xBE;      //��ADϵ��
sfr  P1ASF     = 0x9D;      //ֻд��ģ������(AD��LVD)ѡ��

sfr  CCON      = 0xD8;        //
sbit CCF0      = CCON^0;    //PCA ģ��0�жϱ�־����Ӳ����λ�������������0��
sbit CCF1      = CCON^1;    //PCA ģ��1�жϱ�־����Ӳ����λ�������������0��
sbit CCF2      = CCON^2;    //PCA ģ��2�жϱ�־����Ӳ����λ�������������0��
sbit CR        = CCON^6;    //1: ����PCA�����������������������0��
sbit CF        = CCON^7;    //PCA�����������CH��CL��FFFFH��Ϊ0000H����־��PCA�������������Ӳ����λ�������������0��
sfr  CMOD      = 0xD9;      //
sfr  CCAPM0    = 0xDA;      //PCAģ��0�Ĺ���ģʽ�Ĵ�����
sfr  CCAPM1    = 0xDB;      //PCAģ��1�Ĺ���ģʽ�Ĵ�����
sfr  CCAPM2    = 0xDC;      //PCAģ��2�Ĺ���ģʽ�Ĵ�����
sfr  CL        = 0xE9;      //
sfr  CCAP0L    = 0xEA;      //PCAģ��0�Ĳ�׽/�ȽϼĴ�����8λ��
sfr  CCAP1L    = 0xEB;      //PCAģ��1�Ĳ�׽/�ȽϼĴ�����8λ��
sfr  CCAP2L    = 0xEC;      //PCAģ��2�Ĳ�׽/�ȽϼĴ�����8λ��
sfr  CH        = 0xF9;
sfr  CCAP0H    = 0xFA;      //PCAģ��0�Ĳ�׽/�ȽϼĴ�����8λ��
sfr  CCAP1H    = 0xFB;      //PCAģ��1�Ĳ�׽/�ȽϼĴ�����8λ��
sfr  CCAP2H    = 0xFC;      //PCAģ��2�Ĳ�׽/�ȽϼĴ�����8λ��
sfr  PCA_PWM0  = 0xF2;      //PCAģ��0 PWM�Ĵ�����
sfr  PCA_PWM1  = 0xF3;      //PCAģ��1 PWM�Ĵ�����
sfr  PCA_PWM2  = 0xF4;      //PCAģ��2 PWM�Ĵ�����

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