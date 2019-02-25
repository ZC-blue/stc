#include "ir.h"
#include "delay.h"



/*************  ���ⷢ����ر���    **************/
#define User_code   0xFF00      //��������û���
#define IR_TX_ON    0
#define IR_TX_OFF   1

sbit    P_IR_TX   = P3^6;   //������ⷢ�Ͷ˿�
bit     B_Space;    //���Ϳ���(��ʱ)��־

unsigned int     PCA_Timer0; //PCA2�����ʱ������
unsigned int     tx_cnt;     //���ͻ���е��������(����38KHZ������������Ӧʱ��), ����Ƶ��Ϊ38KHZ, ����26.3us
unsigned char    TxTime;     //����ʱ��



/******************************************
*�������ƣ�IR_TxPulse
*�����������
*�����������
*�������ܣ��������庯��
*��		 ע��
*******************************************/
void IR_TxPulse(unsigned int pulse)
{
    tx_cnt = pulse;
    B_Space = 0;    //������
    CCAPM2 = 0x48 | 0x04 | 0x01;    //����ģʽ 0x00: PCA_Mode_Capture,  0x42: PCA_Mode_PWM,  0x48: PCA_Mode_SoftTimer
    CR = 1;     //����
    while(CR);
    P_IR_TX = IR_TX_OFF;
}
/******************************************
*�������ƣ�IR_TxSpace
*�����������
*�����������
*�������ܣ����Ϳ��к���
*��		 ע��
*******************************************/
void IR_TxSpace(unsigned int pulse)
{
    tx_cnt = pulse;
    B_Space = 1;    //����
    CCAPM2 = 0x48 | 0x01;   //����ģʽ 0x00: PCA_Mode_Capture,  0x42: PCA_Mode_PWM,  0x48: PCA_Mode_SoftTimer
    CR = 1;     //����
    while(CR);
    P_IR_TX = IR_TX_OFF;
}
/******************************************
*�������ƣ�IR_TxByte
*�����������
*�����������
*�������ܣ�����һ���ֽں���
*��		 ע��
*******************************************/
void IR_TxByte(unsigned char dat)
{
    unsigned char i;
    for(i=0; i<8; i++)
    {
        if(dat & 1)     IR_TxSpace(63), TxTime += 2;    //����1��Ӧ 1.6875 + 0.5625 ms 
        else            IR_TxSpace(21), TxTime++;       //����0��Ӧ 0.5625 + 0.5625 ms
        IR_TxPulse(21);         //���嶼��0.5625ms
        dat >>= 1;              //��һ��λ
    }
}

/******************************************
*�������ƣ�PCA_config
*�����������
*�����������
*�������ܣ�PCA_config��������
*��		 ע��
*******************************************/
void PCA_config(void)
{
    //PCA0��ʼ��
    CCON = 0x00;                          //��������жϱ�־
    CCAPM0  = 0x48 | 0x01;                //����ģʽ + �ж����� 0x00: PCA_Mode_Capture,  0x42: PCA_Mode_PWM,  0x48: PCA_Mode_SoftTimer
    PCA_Timer0 = 100;                     //����һ��С�ĳ�ֵ
    CCAP0L = (unsigned char)PCA_Timer0;    //��Ӱ��Ĵ���д�벶��Ĵ�������дCCAP0L
    CCAP0H = (unsigned char)(PCA_Timer0 >> 8); //��дCCAP0H

    PPCA = 1;                                  //�����ȼ��ж�
    CMOD  = (CMOD  & ~0xe0) | 0x08;            //ѡ��ʱ��Դ, 0x00: 12T, 0x02: 2T, 0x04: Timer0���, 0x06: ECI, 0x08: 1T, 0x0A: 4T, 0x0C: 6T, 0x0E: 8T
    CH = 0;
    CL = 0;
    CR = 0;
    tx_cnt = 2;
}


/******************************************
*�������ƣ�Send_UserCode
*�����������
*�����������
*�������ܣ��������庯��
*��		 ע��
*******************************************/
void Send_UserCode(unsigned int senddata)
{
		TxTime = 0;
												//һ֡������С���� = 9 + 4.5 + 0.5625 + 24 * 1.125 + 8 * 2.25 = 59.0625 ms
												//һ֡������󳤶� = 9 + 4.5 + 0.5625 + 8 * 1.125 + 24 * 2.25 = 77.0625 ms
		IR_TxPulse(342);    //��Ӧ9ms��ͬ��ͷ       9ms
		IR_TxSpace(171);    //��Ӧ4.5ms��ͬ��ͷ��� 4.5ms
		IR_TxPulse(21);     //��ʼ��������          0.5625ms

		IR_TxByte(User_code%256);   //���û�����ֽ�
		IR_TxByte(User_code/256);   //���û�����ֽ�
		IR_TxByte(senddata);         //������
		IR_TxByte(~senddata);        //�����ݷ���

		if(TxTime < 56)     //һ֡�����77ms����, �����Ļ�,����ʱ��     108ms
		{
				TxTime = 56 - TxTime;
				TxTime = TxTime + TxTime / 8;
				delay_ms(TxTime);
		}

    return ;
}
/******************************************
*�������ƣ�PCA_Handler
*�����������
*�����������
*�������ܣ��������庯��
*��		 ע��
*******************************************/

#define D_38K_DUTY  ((MAIN_Fosc * 26) / 1000000UL + MAIN_Fosc / 3000000UL)  /*  38KHZ����ʱ��   26.3us */
#define D_38K_OFF   ((MAIN_Fosc * 17) / 1000000UL + MAIN_Fosc / 3000000UL)  /* ����ܹر�ʱ��   17.3us */
#define D_38K_ON    ((MAIN_Fosc * 9) / 1000000UL)                           /* ����ܵ�ͨʱ��   9us */

void PCA_Handler (void) interrupt 7
{
    CCON = 0x40;    //��������жϱ�־,������CR
    if(!B_Space)    //�����ز�
    {                               //�������壬����װ��TH0ֵ�����ʱ�Զ���װ
        P_IR_TX = ~P_IR_TX;
        if(P_IR_TX)
        {
            PCA_Timer0 += D_38K_OFF;    //װ�ظߵ�ƽʱ��    17.3us
            if(--tx_cnt == 0)   CR = 0; //pulse has sent,   stop
        }
        else    PCA_Timer0 += D_38K_ON; //װ�ص͵�ƽʱ��    9us
    }
    else    //������ͣʱ��
    {
        PCA_Timer0 += D_38K_DUTY;   //װ������ʱ��  26.3us
        if(--tx_cnt == 0)   CR = 0; //����ʱ��
    }
    CCAP0L = (unsigned char)PCA_Timer0;            //��Ӱ��Ĵ���д�벶��Ĵ�������дCCAP0L
    CCAP0H = (unsigned char)(PCA_Timer0 >> 8); //��дCCAP0H
}
