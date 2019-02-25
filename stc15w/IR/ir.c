#include "ir.h"
#include "delay.h"






/*************  ���ⷢ����ر���    **************/
#define User_code   0xFF00       //��������û���

sbit    P_IR_TX   = P3^6;        //������ⷢ�Ͷ˿�
sbit    IRIN      = P3^3;        //���������ն˿�
bit     B_Space;                 //���Ϳ���(��ʱ)��־

unsigned int   PCA_Timer2;       //PCA2�����ʱ������
unsigned int   tx_cnt;                  //���ͻ���е��������(����38KHZ������������Ӧʱ��), ����Ƶ��Ϊ38KHZ, ����26.3us
unsigned char  TxTime;                  //����ʱ��

unsigned char  IrValue[5];        // IrValue��0-3������ԭʼ���ݣ�4�����ž���У��ȷ������ļ�ֵ
unsigned char  Time;

/******************************************
*�������ƣ�DelayMs
*�����������ʱʱ�䳤��
*�����������
*�������ܣ������ʱ
*��		 ע��
*******************************************/
void DelayMs(unsigned int x)   //0.14ms��� 0us
{
	unsigned char i;
	while(x--)
	{
		for (i = 0; i<13; i++)
		{}
	}
}
/******************************************
*�������ƣ�IrInit
*�����������
*�����������
*�������ܣ�����ӿڳ�ʼ��
*��		 ע��ʹ���ⲿ�ж�1
*******************************************/
void IrInit(void)
{
	IT1=1;//�½��ش���
	EX1=1;//���ⲿ�ж�1����
	EA=1;	//�����ж�

	IRIN=1;//��ʼ���˿�
}

/******************************************
*�������ƣ�ReadIr
*�����������
*�����������
*�������ܣ���������ж�
*��		 ע��
*******************************************/
void ReadIr() interrupt 0
{
	unsigned char j,k;
	unsigned int err;
	Time = 0;					 
	DelayMs(40);			// 136us*40


	if (IRIN == 0)		//ȷ���Ƿ���Ľ��յ���ȷ���ź�
	{	 		
		err = 1000;				//1000*10us=10ms,����˵�����յ�������ź�
											/*
											������������Ϊ��ʱѭ���������һ������Ϊ�ٵ�ʱ������ѭ������ó�������ʱ
											�������������
											*/	
		while ((IRIN==0) && (err>0))	//�ȴ�ǰ��9ms�ĵ͵�ƽ��ȥ  		
		{			
			DelayMs(1);			// 136us
			err--;
		} 
		if (IRIN == 1)			//�����ȷ�ȵ�9ms�͵�ƽ
		{
			err = 500;
			while ((IRIN==1) && (err>0))		 //�ȴ�4.5ms����ʼ�ߵ�ƽ��ȥ
			{
				DelayMs(1);
				err--;
			}
			for (k=0; k<4; k++)		//����4������
			{			
				for (j=0; j<8; j++)	//����һ������
				{
					err = 60;		
					while ((IRIN==0) && (err>0))//�ȴ��ź�ǰ���560us�͵�ƽ��ȥ
					{
						DelayMs(1);
						err--;
					}
					err = 500;
					while ((IRIN==1) && (err>0))	 //����ߵ�ƽ��ʱ�䳤�ȡ�
					{
						DelayMs(1);//0.14ms
						Time++;
						err--;
						if (Time > 30)
						{
							EX0 = 1;
							return;
						}
					}
					IrValue[k] >>= 1;	 //k��ʾ�ڼ�������
					if (Time >= 8)			//����ߵ�ƽ���ִ���565us����ô��1
					{
						IrValue[k] |= 0x80;
					}
					Time = 0;		//����ʱ��Ҫ���¸�ֵ							
				}
			}
		}
		if (IrValue[2] == ~IrValue[3])
		{
			IrValue[4] = IrValue[2];
			return;
		}
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
    AUXR1   = (AUXR1 & ~0x30) | 0x10;   //�л�IO��, 0x00: P1.2 P1.1 P1.0 P3.7,  0x10: P3.4 P3.5 P3.6 P3.7, 0x20: P2.4 P2.5 P2.6 P2.7

    CCON    = 0x00;                     //��������жϱ�־��PCA���ƼĴ���
    CCAPM2  = 0x48+ 1;                  //����ģʽ + �����ж� 0x00: PCA_Mode_Capture,  0x42: PCA_Mode_PWM,  0x48: PCA_Mode_SoftTimer
    CCAPM2 |= 0x04;                     //�������ȡ�������һ������16λ�����ʱ��
    PCA_Timer2 = 100;                   //��ֵ
    CCAP2L = (unsigned char)PCA_Timer2;            //��ӳ��Ĵ���д�벶��Ĵ���,��дCCAPxL
    CCAP2H = (unsigned char)(PCA_Timer2 >> 8);      //��дCCAPxH

    PPCA = 1;                            //�����ȼ��ж�
    CMOD  = (CMOD  & ~0xe0) | 0x08;      //ѡ��ʱ��Դ, 0x00: 12T, 0x02: 2T, 0x04: Timer0??, 0x06: ECI, 0x08: 1T, 0x0A: 4T, 0x0C: 6T, 0x0E: 8T
    CH = 0;
    CL = 0;
    CR = 0;
    tx_cnt = 2;
}


#define D_38K_DUTY  ((MAIN_Fosc * 26) / 1000000UL + MAIN_Fosc / 3000000UL)  /*  38KHZ????   26.3us */
#define D_38K_OFF   ((MAIN_Fosc * 17) / 1000000UL + MAIN_Fosc / 3000000UL)  /* ???????   17.3us */
#define D_38K_ON    ((MAIN_Fosc * 9) / 1000000UL)                           /* ???????   9us */

/******************************************
*�������ƣ�IR_TxPulse
*�����������
*�����������
*�������ܣ��������庯��
*��		 ע��
*******************************************/
void IR_TxPulse(unsigned int  pulse)
{
    tx_cnt = pulse;
    B_Space = 0;                    //������
    CCAPM2 = 0x48 | 0x04 | 0x01;    //����ģʽ 0x00: PCA_Mode_Capture,  0x42: PCA_Mode_PWM,  0x48: PCA_Mode_SoftTimer
    CR = 1;                         //����
    while(CR);
}

/******************************************
*�������ƣ�IR_TxSpace
*�����������
*�����������
*�������ܣ����Ϳ��к���
*��		 ע��
*******************************************/
void IR_TxSpace(unsigned int  pulse)
{
    tx_cnt = pulse;
    B_Space = 1;            //����
    CCAPM2 = 0x48 | 0x01;   //����ģʽ 0x00: PCA_Mode_Capture,  0x42: PCA_Mode_PWM,  0x48: PCA_Mode_SoftTimer
    CR = 1;                 //����
    while(CR);
}

/******************************************
*�������ƣ�PCA_config
*�����������
*�����������
*�������ܣ�PCA_config��������
*��		 ע��
*******************************************/

void PCA_Handler (void) interrupt 7
{
    CCON = 0x40;    //��������жϱ�־,������CR
    if(!B_Space)    //�����ز�
    {                               //�������壬����װ��TH0ֵ�����ʱ�Զ���װ
        if(P_IR_TX)
        {
            PCA_Timer2 += D_38K_OFF;    //װ�ظߵ�ƽʱ��    17.3us
            if(--tx_cnt == 0)   CR = 0; //pulse has sent,   stop
        }
        else    PCA_Timer2 += D_38K_ON; //װ�ص͵�ƽʱ��    9us
    }
    else    //������ͣʱ��
    {
        PCA_Timer2 += D_38K_DUTY;   //װ������ʱ��  26.3us
        if(--tx_cnt == 0)   CR = 0; //����ʱ��
    }
    CCAP2L = (unsigned char)PCA_Timer2;            //��Ӱ��Ĵ���д�벶��Ĵ�������дCCAP0L
    CCAP2H = (unsigned char)(PCA_Timer2 >> 8); //��дCCAP0H
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
*�������ƣ�IR_TxPulse
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

