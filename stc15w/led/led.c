#include "led.h"

sbit LED  = P1^5;



/******************************************
*函数名称：LED_init
*输入参数：无
*输出参数：无
*函数功能：LED初始化
*备		 注：
*******************************************/
void LED_init(void)
{
	LED = 0;   //关闭led  
}

/******************************************
*函数名称：LED_light
*输入参数：无
*输出参数：无
*函数功能：LED闪
*备		 注：
*******************************************/
void LED_light(void)
{
	LED = 1;  //打开LED
}
/******************************************
*函数名称：LED_off
*输入参数：无
*输出参数：无
*函数功能：LED关闭
*备		 注：
*******************************************/
void LED_off(void)
{
	LED = 0;  //关闭LED
}