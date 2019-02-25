#include "ir.h"
#include "delay.h"



/*************  红外发送相关变量    **************/
#define User_code   0xFF00      //定义红外用户码
#define IR_TX_ON    0
#define IR_TX_OFF   1

sbit    P_IR_TX   = P3^6;   //定义红外发送端口
bit     B_Space;    //发送空闲(延时)标志

unsigned int     PCA_Timer0; //PCA2软件定时器变量
unsigned int     tx_cnt;     //发送或空闲的脉冲计数(等于38KHZ的脉冲数，对应时间), 红外频率为38KHZ, 周期26.3us
unsigned char    TxTime;     //发送时间



/******************************************
*函数名称：IR_TxPulse
*输入参数：无
*输出参数：无
*函数功能：发送脉冲函数
*备		 注：
*******************************************/
void IR_TxPulse(unsigned int pulse)
{
    tx_cnt = pulse;
    B_Space = 0;    //发脉冲
    CCAPM2 = 0x48 | 0x04 | 0x01;    //工作模式 0x00: PCA_Mode_Capture,  0x42: PCA_Mode_PWM,  0x48: PCA_Mode_SoftTimer
    CR = 1;     //启动
    while(CR);
    P_IR_TX = IR_TX_OFF;
}
/******************************************
*函数名称：IR_TxSpace
*输入参数：无
*输出参数：无
*函数功能：发送空闲函数
*备		 注：
*******************************************/
void IR_TxSpace(unsigned int pulse)
{
    tx_cnt = pulse;
    B_Space = 1;    //空闲
    CCAPM2 = 0x48 | 0x01;   //工作模式 0x00: PCA_Mode_Capture,  0x42: PCA_Mode_PWM,  0x48: PCA_Mode_SoftTimer
    CR = 1;     //启动
    while(CR);
    P_IR_TX = IR_TX_OFF;
}
/******************************************
*函数名称：IR_TxByte
*输入参数：无
*输出参数：无
*函数功能：发送一个字节函数
*备		 注：
*******************************************/
void IR_TxByte(unsigned char dat)
{
    unsigned char i;
    for(i=0; i<8; i++)
    {
        if(dat & 1)     IR_TxSpace(63), TxTime += 2;    //数据1对应 1.6875 + 0.5625 ms 
        else            IR_TxSpace(21), TxTime++;       //数据0对应 0.5625 + 0.5625 ms
        IR_TxPulse(21);         //脉冲都是0.5625ms
        dat >>= 1;              //下一个位
    }
}

/******************************************
*函数名称：PCA_config
*输入参数：无
*输出参数：无
*函数功能：PCA_config功能配置
*备		 注：
*******************************************/
void PCA_config(void)
{
    //PCA0初始化
    CCON = 0x00;                          //清除所有中断标志
    CCAPM0  = 0x48 | 0x01;                //工作模式 + 中断允许 0x00: PCA_Mode_Capture,  0x42: PCA_Mode_PWM,  0x48: PCA_Mode_SoftTimer
    PCA_Timer0 = 100;                     //随便给一个小的初值
    CCAP0L = (unsigned char)PCA_Timer0;    //将影射寄存器写入捕获寄存器，先写CCAP0L
    CCAP0H = (unsigned char)(PCA_Timer0 >> 8); //后写CCAP0H

    PPCA = 1;                                  //高优先级中断
    CMOD  = (CMOD  & ~0xe0) | 0x08;            //选择时钟源, 0x00: 12T, 0x02: 2T, 0x04: Timer0溢出, 0x06: ECI, 0x08: 1T, 0x0A: 4T, 0x0C: 6T, 0x0E: 8T
    CH = 0;
    CL = 0;
    CR = 0;
    tx_cnt = 2;
}


/******************************************
*函数名称：Send_UserCode
*输入参数：无
*输出参数：无
*函数功能：发送脉冲函数
*备		 注：
*******************************************/
void Send_UserCode(unsigned int senddata)
{
		TxTime = 0;
												//一帧数据最小长度 = 9 + 4.5 + 0.5625 + 24 * 1.125 + 8 * 2.25 = 59.0625 ms
												//一帧数据最大长度 = 9 + 4.5 + 0.5625 + 8 * 1.125 + 24 * 2.25 = 77.0625 ms
		IR_TxPulse(342);    //对应9ms，同步头       9ms
		IR_TxSpace(171);    //对应4.5ms，同步头间隔 4.5ms
		IR_TxPulse(21);     //开始发送数据          0.5625ms

		IR_TxByte(User_code%256);   //发用户码高字节
		IR_TxByte(User_code/256);   //发用户码低字节
		IR_TxByte(senddata);         //发数据
		IR_TxByte(~senddata);        //发数据反码

		if(TxTime < 56)     //一帧按最大77ms发送, 不够的话,补偿时间     108ms
		{
				TxTime = 56 - TxTime;
				TxTime = TxTime + TxTime / 8;
				delay_ms(TxTime);
		}

    return ;
}
/******************************************
*函数名称：PCA_Handler
*输入参数：无
*输出参数：无
*函数功能：发送脉冲函数
*备		 注：
*******************************************/

#define D_38K_DUTY  ((MAIN_Fosc * 26) / 1000000UL + MAIN_Fosc / 3000000UL)  /*  38KHZ周期时间   26.3us */
#define D_38K_OFF   ((MAIN_Fosc * 17) / 1000000UL + MAIN_Fosc / 3000000UL)  /* 发射管关闭时间   17.3us */
#define D_38K_ON    ((MAIN_Fosc * 9) / 1000000UL)                           /* 发射管导通时间   9us */

void PCA_Handler (void) interrupt 7
{
    CCON = 0x40;    //清除所有中断标志,但不关CR
    if(!B_Space)    //发送载波
    {                               //发送脉冲，交替装载TH0值，溢出时自动重装
        P_IR_TX = ~P_IR_TX;
        if(P_IR_TX)
        {
            PCA_Timer0 += D_38K_OFF;    //装载高电平时间    17.3us
            if(--tx_cnt == 0)   CR = 0; //pulse has sent,   stop
        }
        else    PCA_Timer0 += D_38K_ON; //装载低电平时间    9us
    }
    else    //发送暂停时间
    {
        PCA_Timer0 += D_38K_DUTY;   //装载周期时间  26.3us
        if(--tx_cnt == 0)   CR = 0; //空闲时间
    }
    CCAP0L = (unsigned char)PCA_Timer0;            //将影射寄存器写入捕获寄存器，先写CCAP0L
    CCAP0H = (unsigned char)(PCA_Timer0 >> 8); //后写CCAP0H
}
