#include "delay.h"


/******************************************
*�������ƣ�delay_us
*�����������
*�����������
*�������ܣ�΢���ʱ
*��		 ע��
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
*�������ƣ�delay_ms
*�����������ʱʱ��
*�����������
*�������ܣ����뼶��ʱ
*��		 ע��
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
*�������ƣ�delay_s
*�����������ʱʱ��
*�����������
*�������ܣ��뼶��ʱ
*��		 ע��
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