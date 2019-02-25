#include "delay.h"


/******************************************
*函数名称：delay_us
*输入参数：无
*输出参数：无
*函数功能：微妙级延时
*备		 注：
*******************************************/
void delay_us(unsigned int time)
{
  unsigned int cnt = 0;

	for(cnt = 0;cnt < time ;cnt ++)
	{
		_nop_();
		_nop_();
		_nop_();
  }

	return;
}

/******************************************
*函数名称：delay_ms
*输入参数：延时时间
*输出参数：无
*函数功能：毫秒级延时
*备		 注：
*******************************************/
void delay_ms(unsigned int time)
{
	
	int cnt = 0;
	unsigned char i, j;


	for(cnt = 0;cnt < time ;cnt ++)
  {
		_nop_();
		_nop_();
		_nop_();
		i = 11;
		j = 190;
		do
		{
			while (--j);
		} while (--i);
  }
}

/******************************************
*函数名称：delay_s
*输入参数：延时时间
*输出参数：无
*函数功能：秒级延时
*备		 注：
*******************************************/
void delay_s(unsigned int time)
{
	int cnt = 0;
	unsigned char i, j, k;

	for(cnt = 0;cnt < time ;cnt ++)
	{
			_nop_();
			_nop_();
			i = 43;
			j = 6;
			k = 203;
			do
			{
				do
				{
					while (--k);
				} while (--j);
			} while (--i);
  }
	
}