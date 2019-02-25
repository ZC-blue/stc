#include "uart.h"


/******************************************
*函数名称：uart_init
*输入参数：无
*输出参数：无
*函数功能：串口设置为: 波特率9600、数据位8、停止位1、奇偶校验无
*备		 注：使用的晶振是11.0592MHz的，注意12MHz和24MHz的不行
*******************************************/
void uart_init(void)
{
	                  // 波特率9600
	SCON = 0x50;   	  // 串口工作在模式1（8位串口）、允许接收
	PCON = 0x00;	    // 波特率不加倍

	                  // 通信波特率相关的设置
	TMOD = 0x20;	    // 设置T1为模式2
	TH1 = 253;
	TL1 = 253;	   	  // 8位自动重装，意思就是TH1用完了之后下一个周期TL1会
					          // 自动重装到TH1去

	TR1 = 1;		      // 开启T1让它开始工作
	
	ES = 1;
	EA = 1;
}


/******************************************
*函数名称：uart_send_byte
*输入参数：要发送的数据
*输出参数：无
*函数功能：串口发送一个字节的数据
*备		 注：
*******************************************/
void uart_send_byte(unsigned char c)
{
      
   ES=0;           //关闭串口中断
   SBUF = c;
           
   while (!TI);    //等待发送完成

   TI = 0;         //清除中断发送标志
	 ES=0;           //打开串口中断
}


/******************************************
*函数名称：UART1_SendString
*输入参数：要发送的字符串
*输出参数：无
*函数功能：发送字符串
*备		 注：
*******************************************/
void UART1_SendString(char *s)
{
    while(*s)
    {
        uart_send_byte(*s++);
    }
}


/******************************************
*函数名称：putchar
*输入参数：要发送的数据
*输出参数：返回要发送的数据
*函数功能：重定向printf函数
*备		 注：
*******************************************/
char putchar(char c)
{
    uart_send_byte(c);
    return c;
}
