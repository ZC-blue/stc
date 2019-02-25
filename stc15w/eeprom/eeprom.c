#include "eeprom.h"


sfr IAP_DATA    =   0xC2;           //IAP���ݼĴ���
sfr IAP_ADDRH   =   0xC3;           //IAP��ַ�Ĵ������ֽ�
sfr IAP_ADDRL   =   0xC4;           //IAP��ַ�Ĵ������ֽ�
sfr IAP_CMD     =   0xC5;           //IAP����Ĵ���
sfr IAP_TRIG    =   0xC6;           //IAP������Ĵ���
sfr IAP_CONTR   =   0xC7;           //IAP���ƼĴ���

#define CMD_IDLE    0               //����ģʽ
#define CMD_READ    1               //IAP�ֽڶ�����
#define CMD_PROGRAM 2               //IAP�������
#define CMD_ERASE   3               //IAP����ɾ������


#define ENABLE_IAP  0x82            //if SYSCLK<20MHz


/******************************************
*�������ƣ�IapIdle
*�����������
*�����������
*�������ܣ��ر�IAP
*��		 ע��
*******************************************/
void IapIdle(void)
{
    IAP_CONTR = 0;                  //�ر�IAP����
    IAP_CMD = 0;                    //�������Ĵ���
    IAP_TRIG = 0;                   //��������Ĵ���
    IAP_ADDRH = 0x80;               //����ַ���õ���IAP����
    IAP_ADDRL = 0;
}


/******************************************
*�������ƣ�IapReadByte
*���������Ҫ��ȡ�ļĴ�����ַ
*�����������ȡ��������
*�������ܣ���IAP/ISP/EEPROM�����ȡһ���ֽ�
*��		 ע��
*******************************************/
unsigned char IapReadByte(unsigned int  addr)
{
    unsigned char dat;                       //���ݻ�����

    IAP_CONTR = ENABLE_IAP;         //ʹ��IAP
    IAP_CMD = CMD_READ;             //����IAP����
    IAP_ADDRL = addr;               //����IAP�͵�ַ
    IAP_ADDRH = addr >> 8;          //����IAP�ߵ�ַ
    IAP_TRIG = 0x5a;                //д��������0x5a
    IAP_TRIG = 0xa5;                //д��������0xa5
    _nop_();                        //�ȴ�ISP/IAP/EEPROM�������
    dat = IAP_DATA;                 //��ȡISP/IAP/EEPROM����
    IapIdle();                      //�ر�IAP����

    return dat;                     //����
}


/******************************************
*�������ƣ�IapProgramByte
*���������Ҫд�ļĴ�����Ҫд�������
*�����������
*�������ܣ���IAP/ISP/EEPROM����дһ���ֽ�
*��		 ע��
*******************************************/
void IapProgramByte(unsigned int  addr, unsigned char  dat)
{
    IAP_CONTR = ENABLE_IAP;         //ʹ��IAP
    IAP_CMD = CMD_PROGRAM;          //����IAP����
    IAP_ADDRL = addr;               //����IAP�͵�ַ
    IAP_ADDRH = addr >> 8;          //����IAP�ߵ�ַ
    IAP_DATA = dat;                 //дISP/IAP/EEPROM����
    IAP_TRIG = 0x5a;                //д��������0x5a
    IAP_TRIG = 0xa5;                //д��������0xa5
    _nop_();                        //�ȴ�ISP/IAP/EEPROM�������
    IapIdle();											//�ر�IAP����
}


/******************************************
*�������ƣ�IapEraseSector
*�������������ɾ������ʼ��ַ
*�����������
*�������ܣ���������
*��		 ע��
*******************************************/
void IapEraseSector(unsigned int  addr)
{
    IAP_CONTR = ENABLE_IAP;         //ʹ��IAP
    IAP_CMD = CMD_ERASE;            //����IAP����
    IAP_ADDRL = addr;               //����IAP�͵�ַ
    IAP_ADDRH = addr >> 8;          //����IAP�ߵ�ַ
    IAP_TRIG = 0x5a;                //д��������0x5a
    IAP_TRIG = 0xa5;                //д��������0xa5
    _nop_();                        //�ȴ�ISP/IAP/EEPROM�������
    IapIdle();											//�ر�IAP����
}








