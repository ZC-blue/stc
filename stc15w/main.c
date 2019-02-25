#include <reg51.h>	   //此文件中定义了51的一些特殊功能寄存器
											 //--包含你要使用的头文件--//

#include "stdio.h"
#include "delay.h"

unsigned char   data1[] = 
{
		0x00,0x01,0x02,0x00,0x01,0x02,0x00,0x01,0x02,0x00,0x01,0x02,0x00,0x01,0x02,
	
};

/******************************************
*函数名称：main
*输入参数：无
*输出参数：无
*函数功能：系统主函数
*备		 注：
*******************************************/
void main(void)
{ 
	
	//***********基本外设初始化*********************
	delay_ms(10);
	
	uart_init();
	
	printf("load code \n");
	
	PCA_config();        //红外发送
	
	LED_init();
	
	buzzer_init();
	
	PW_switch_init();
	
	rtc_tcs8563_init();
	
	timer0_init();
	
	time2_init();
	
	delay_ms(10);
	//***********************************************
	while(1)
	{

		
		
		
		
	}
}