#include "ir.h"

#define SysTick     10000       // ��/��, ϵͳ�δ�Ƶ��, ��4000~16000֮��


/****************************** �Զ������ ***********************************/

#define Timer0_Reload   (65536UL - ((MAIN_Fosc + SysTick/2) / SysTick))     //Timer 0 �ж�Ƶ��, ��config.h��ָ��ϵͳ�δ�Ƶ��, ��4000~16000֮��.

/*****************************************************************************/
bit B_1ms;          //1ms��־

unsigned char  cnt_1ms;        //1ms������ʱ


/*************  ������ճ����������    **************/
sbit    P_IR_RX = P3^3;         //��������������IO��

bit P_IR_RX_temp;       //Last sample
bit B_IR_Sync;          //���յ�ͬ����־
bit B_IR_Press;         //������������


unsigned char  IR_SampleCnt;       //��������
unsigned char  IR_BitCnt;          //����λ��
unsigned char  IR_UserH;           //�û���(��ַ)���ֽ�
unsigned char  IR_UserL;           //�û���(��ַ)���ֽ�
unsigned char  IR_data;            //����ԭ��
unsigned char  IR_DataShit;        //������λ


unsigned char  IR_code;            //�������
unsigned int   UserCode;           //�û���





/******************************************
*�������ƣ�timer0_init
*�����������
*�����������
*�������ܣ���ʱ��0��ʼ������
*��		 ע��
*******************************************/
void timer0_init (void) 
{
		AUXR = 0x80;                       //Timer0 set as 1T, 16 bits timer auto-reload, 
		TH0 = (unsigned char)(Timer0_Reload / 256);
		TL0 = (unsigned char)(Timer0_Reload % 256);
		ET0 = 1;                           //Timer0 interrupt enable
		TR0 = 1;                           //Tiner0 run

		cnt_1ms = SysTick / 1000;
		EA = 1;                           //�����ж�
 
}

//*******************************************************************
//*********************** IR Remote Module **************************
//*********************** By  (Coody) 2002-8-24 *********************
//*********************** IR Remote Module **************************
//this programme is used for Receive IR Remote (NEC Code).

//data format: Synchro, AddressH, AddressL, data, /data, (total 32 bit).

//send a frame(85ms), pause 23ms, send synchro of continue frame, pause 94ms

//data rate: 108ms/Frame


//Synchro: low=9ms, high=4.5 / 2.25ms, low=0.5626ms
//Bit0: high=0.5626ms, low=0.5626ms
//Bit1: high=1.6879ms, low=0.5626ms
//frame rate = 108ms ( pause 23 ms or 96 ms)

/******************** �������ʱ��궨��, �û���Ҫ�����޸�  *******************/

#define IR_SAMPLE_TIME      (1000000UL/SysTick)         //��ѯʱ����, us, �������Ҫ����60us~250us֮��
#if ((IR_SAMPLE_TIME <= 250) && (IR_SAMPLE_TIME >= 60))
    #define D_IR_sample         IR_SAMPLE_TIME      //�������ʱ�䣬��60us~250us֮��
#endif

#define D_IR_SYNC_MAX       (15000/D_IR_sample) //SYNC max time
#define D_IR_SYNC_MIN       (9700 /D_IR_sample) //SYNC min time
#define D_IR_SYNC_DIVIDE    (12375/D_IR_sample) //decide data 0 or 1
#define D_IR_DATA_MAX       (3000 /D_IR_sample) //data max time
#define D_IR_DATA_MIN       (600  /D_IR_sample) //data min time
#define D_IR_DATA_DIVIDE    (1687 /D_IR_sample) //decide data 0 or 1
#define D_IR_BIT_NUMBER     32                  //bit number
//*******************************************************************************************
//**************************** IR RECEIVE MODULE ********************************************
/******************************************
*�������ƣ�IR_RX_NEC
*�����������
*�����������
*�������ܣ�������պ���
*��		 ע��
*******************************************/
void IR_RX_NEC(void)
{
    unsigned char SampleTime;

    IR_SampleCnt++;                         //Sample + 1

    F0 = P_IR_RX_temp;                      //Save Last sample status
    P_IR_RX_temp = P_IR_RX;                 //Read current status
    if(F0 && !P_IR_RX_temp)                 //Pre-sample is high��and current sample is low, so is fall edge
    {
        SampleTime = IR_SampleCnt;          //get the sample time
        IR_SampleCnt = 0;                   //Clear the sample counter

        if(SampleTime > D_IR_SYNC_MAX)    
				{

					B_IR_Sync = 0;  //large the Maxim SYNC time, then error
       
				}	
				else if(SampleTime >= D_IR_SYNC_MIN)                    //SYNC
        {
            if(SampleTime >= D_IR_SYNC_DIVIDE)
            {
                B_IR_Sync = 1;                  //has received SYNC
                IR_BitCnt = D_IR_BIT_NUMBER;    //Load bit number
            }
        }
        else if(B_IR_Sync)                      //has received SYNC
        {
            if(SampleTime > D_IR_DATA_MAX)     
            {
								B_IR_Sync=0;    //data samlpe time too large
            }
   					else
            {
                IR_DataShit >>= 1;                  //data shift right 1 bit
                if(SampleTime >= D_IR_DATA_DIVIDE)  
								{
									IR_DataShit |= 0x80;    //devide data 0 or 1
								}              
								if(--IR_BitCnt == 0)                //bit number is over?
                {
                    B_IR_Sync = 0;                  //Clear SYNC
                    if(~IR_DataShit == IR_data)     //�ж�����������
                    {
                        UserCode = ((unsigned int)IR_UserH << 8) + IR_UserL;
                        IR_code      = IR_data;
                        B_IR_Press   = 1;           //������Ч
                    }
                }
                else if((IR_BitCnt & 7)== 0)        //one byte receive
                {
                    IR_UserL = IR_UserH;            //Save the User code high byte
                    IR_UserH = IR_data;             //Save the User code low byte
                    IR_data  = IR_DataShit;         //Save the IR data byte
                }
            }
        }
    }
}
/******************************************
*�������ƣ�timer0
*�����������
*�����������
*�������ܣ���ʱ��0�жϺ���
*��		 ע��
*******************************************/
void timer0 (void) interrupt 1
{

    IR_RX_NEC();
    if(--cnt_1ms == 0)
    {
        cnt_1ms = SysTick / 1000;
        B_1ms = 1;      //1ms��־
    }

}
