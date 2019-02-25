#include "ir.h"
#include "delay.h"






/*************  红外发送相关变量    **************/
#define User_code   0xFF00       //定义红外用户码

sbit    P_IR_TX   = P3^6;        //定义红外发送端口
sbit    IRIN      = P3^3;        //定义红外接收端口
bit     B_Space;                 //发送空闲(延时)标志

unsigned int   PCA_Timer2;       //PCA2软件定时器变量
unsigned int   tx_cnt;                  //发送或空闲的脉冲计数(等于38KHZ的脉冲数，对应时间), 红外频率为38KHZ, 周期26.3us
unsigned char  TxTime;                  //发送时间

unsigned char  IrValue[5];        // IrValue的0-3用来放原始数据，4用来放经过校验确认无误的键值
unsigned char  Time;

/******************************************
*函数名称：DelayMs
*输入参数：延时时间长短
*输出参数：无
*函数功能：软件延时
*备		 注：
*******************************************/
void DelayMs(unsigned int x)   //0.14ms误差 0us
{
	unsigned char i;
	while(x--)
	{
		for (i = 0; i<13; i++)
		{}
	}
}
/******************************************
*函数名称：IrInit
*输入参数：无
*输出参数：无
*函数功能：红外接口初始化
*备		 注：使用外部中断1
*******************************************/
void IrInit(void)
{
	IT1=1;//下降沿触发
	EX1=1;//打开外部中断1允许
	EA=1;	//打开总中断

	IRIN=1;//初始化端口
}

/******************************************
*函数名称：ReadIr
*输入参数：无
*输出参数：无
*函数功能：红外接收中断
*备		 注：
*******************************************/
void ReadIr() interrupt 0
{
	unsigned char j,k;
	unsigned int err;
	Time = 0;					 
	DelayMs(40);			// 136us*40


	if (IRIN == 0)		//确认是否真的接收到正确的信号
	{	 		
		err = 1000;				//1000*10us=10ms,超过说明接收到错误的信号
											/*
											当两个条件都为真时循环，如果有一个条件为假的时候跳出循环，免得程序出错的时
											侯，程序死在这里
											*/	
		while ((IRIN==0) && (err>0))	//等待前面9ms的低电平过去  		
		{			
			DelayMs(1);			// 136us
			err--;
		} 
		if (IRIN == 1)			//如果正确等到9ms低电平
		{
			err = 500;
			while ((IRIN==1) && (err>0))		 //等待4.5ms的起始高电平过去
			{
				DelayMs(1);
				err--;
			}
			for (k=0; k<4; k++)		//共有4组数据
			{			
				for (j=0; j<8; j++)	//接收一组数据
				{
					err = 60;		
					while ((IRIN==0) && (err>0))//等待信号前面的560us低电平过去
					{
						DelayMs(1);
						err--;
					}
					err = 500;
					while ((IRIN==1) && (err>0))	 //计算高电平的时间长度。
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
					IrValue[k] >>= 1;	 //k表示第几组数据
					if (Time >= 8)			//如果高电平出现大于565us，那么是1
					{
						IrValue[k] |= 0x80;
					}
					Time = 0;		//用完时间要重新赋值							
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
*函数名称：PCA_config
*输入参数：无
*输出参数：无
*函数功能：PCA_config功能配置
*备		 注：
*******************************************/
void PCA_config(void)
{
    AUXR1   = (AUXR1 & ~0x30) | 0x10;   //切换IO口, 0x00: P1.2 P1.1 P1.0 P3.7,  0x10: P3.4 P3.5 P3.6 P3.7, 0x20: P2.4 P2.5 P2.6 P2.7

    CCON    = 0x00;                     //清除所有中断标志，PCA控制寄存器
    CCAPM2  = 0x48+ 1;                  //工作模式 + 允许中断 0x00: PCA_Mode_Capture,  0x42: PCA_Mode_PWM,  0x48: PCA_Mode_SoftTimer
    CCAPM2 |= 0x04;                     //允许高速取反输出，一般用在16位软件定时器
    PCA_Timer2 = 100;                   //初值
    CCAP2L = (unsigned char)PCA_Timer2;            //将映射寄存器写入捕获寄存器,先写CCAPxL
    CCAP2H = (unsigned char)(PCA_Timer2 >> 8);      //再写CCAPxH

    PPCA = 1;                            //高优先级中断
    CMOD  = (CMOD  & ~0xe0) | 0x08;      //选择时钟源, 0x00: 12T, 0x02: 2T, 0x04: Timer0??, 0x06: ECI, 0x08: 1T, 0x0A: 4T, 0x0C: 6T, 0x0E: 8T
    CH = 0;
    CL = 0;
    CR = 0;
    tx_cnt = 2;
}


#define D_38K_DUTY  ((MAIN_Fosc * 26) / 1000000UL + MAIN_Fosc / 3000000UL)  /*  38KHZ????   26.3us */
#define D_38K_OFF   ((MAIN_Fosc * 17) / 1000000UL + MAIN_Fosc / 3000000UL)  /* ???????   17.3us */
#define D_38K_ON    ((MAIN_Fosc * 9) / 1000000UL)                           /* ???????   9us */

/******************************************
*函数名称：IR_TxPulse
*输入参数：无
*输出参数：无
*函数功能：发送脉冲函数
*备		 注：
*******************************************/
void IR_TxPulse(unsigned int  pulse)
{
    tx_cnt = pulse;
    B_Space = 0;                    //发脉冲
    CCAPM2 = 0x48 | 0x04 | 0x01;    //工作模式 0x00: PCA_Mode_Capture,  0x42: PCA_Mode_PWM,  0x48: PCA_Mode_SoftTimer
    CR = 1;                         //启动
    while(CR);
}

/******************************************
*函数名称：IR_TxSpace
*输入参数：无
*输出参数：无
*函数功能：发送空闲函数
*备		 注：
*******************************************/
void IR_TxSpace(unsigned int  pulse)
{
    tx_cnt = pulse;
    B_Space = 1;            //空闲
    CCAPM2 = 0x48 | 0x01;   //工作模式 0x00: PCA_Mode_Capture,  0x42: PCA_Mode_PWM,  0x48: PCA_Mode_SoftTimer
    CR = 1;                 //启动
    while(CR);
}

/******************************************
*函数名称：PCA_config
*输入参数：无
*输出参数：无
*函数功能：PCA_config功能配置
*备		 注：
*******************************************/

void PCA_Handler (void) interrupt 7
{
    CCON = 0x40;    //清除所有中断标志,但不关CR
    if(!B_Space)    //发送载波
    {                               //发送脉冲，交替装载TH0值，溢出时自动重装
        if(P_IR_TX)
        {
            PCA_Timer2 += D_38K_OFF;    //装载高电平时间    17.3us
            if(--tx_cnt == 0)   CR = 0; //pulse has sent,   stop
        }
        else    PCA_Timer2 += D_38K_ON; //装载低电平时间    9us
    }
    else    //发送暂停时间
    {
        PCA_Timer2 += D_38K_DUTY;   //装载周期时间  26.3us
        if(--tx_cnt == 0)   CR = 0; //空闲时间
    }
    CCAP2L = (unsigned char)PCA_Timer2;            //将影射寄存器写入捕获寄存器，先写CCAP0L
    CCAP2H = (unsigned char)(PCA_Timer2 >> 8); //后写CCAP0H
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
*函数名称：IR_TxPulse
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

