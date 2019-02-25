#include "ir.h"

#define SysTick     10000       // 次/秒, 系统滴答频率, 在4000~16000之间


/****************************** 自动定义宏 ***********************************/

#define Timer0_Reload   (65536UL - ((MAIN_Fosc + SysTick/2) / SysTick))     //Timer 0 中断频率, 在config.h中指定系统滴答频率, 在4000~16000之间.

/*****************************************************************************/
bit B_1ms;          //1ms标志

unsigned char  cnt_1ms;        //1ms基本计时


/*************  红外接收程序变量声明    **************/
sbit    P_IR_RX = P3^3;         //定义红外接收输入IO口

bit P_IR_RX_temp;       //Last sample
bit B_IR_Sync;          //已收到同步标志
bit B_IR_Press;         //安键动作发生


unsigned char  IR_SampleCnt;       //采样计数
unsigned char  IR_BitCnt;          //编码位数
unsigned char  IR_UserH;           //用户码(地址)高字节
unsigned char  IR_UserL;           //用户码(地址)低字节
unsigned char  IR_data;            //数据原码
unsigned char  IR_DataShit;        //数据移位


unsigned char  IR_code;            //红外键码
unsigned int   UserCode;           //用户码





/******************************************
*函数名称：timer0_init
*输入参数：无
*输出参数：无
*函数功能：定时器0初始化函数
*备		 注：
*******************************************/
void timer0_init (void) 
{
		AUXR = 0x80;                       //Timer0 set as 1T, 16 bits timer auto-reload, 
		TH0 = (unsigned char)(Timer0_Reload / 256);
		TL0 = (unsigned char)(Timer0_Reload % 256);
		ET0 = 1;                           //Timer0 interrupt enable
		TR0 = 1;                           //Tiner0 run

		cnt_1ms = SysTick / 1000;
		EA = 1;                           //打开总中断
 
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

/******************** 红外采样时间宏定义, 用户不要随意修改  *******************/

#define IR_SAMPLE_TIME      (1000000UL/SysTick)         //查询时间间隔, us, 红外接收要求在60us~250us之间
#if ((IR_SAMPLE_TIME <= 250) && (IR_SAMPLE_TIME >= 60))
    #define D_IR_sample         IR_SAMPLE_TIME      //定义采样时间，在60us~250us之间
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
*函数名称：IR_RX_NEC
*输入参数：无
*输出参数：无
*函数功能：红外接收函数
*备		 注：
*******************************************/
void IR_RX_NEC(void)
{
    unsigned char SampleTime;

    IR_SampleCnt++;                         //Sample + 1

    F0 = P_IR_RX_temp;                      //Save Last sample status
    P_IR_RX_temp = P_IR_RX;                 //Read current status
    if(F0 && !P_IR_RX_temp)                 //Pre-sample is high，and current sample is low, so is fall edge
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
                    if(~IR_DataShit == IR_data)     //判断数据正反码
                    {
                        UserCode = ((unsigned int)IR_UserH << 8) + IR_UserL;
                        IR_code      = IR_data;
                        B_IR_Press   = 1;           //数据有效
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
*函数名称：timer0
*输入参数：无
*输出参数：无
*函数功能：定时器0中断函数
*备		 注：
*******************************************/
void timer0 (void) interrupt 1
{

    IR_RX_NEC();
    if(--cnt_1ms == 0)
    {
        cnt_1ms = SysTick / 1000;
        B_1ms = 1;      //1ms标志
    }

}
