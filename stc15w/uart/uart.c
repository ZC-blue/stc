#include "uart.h"


/******************************************
*�������ƣ�uart_init
*�����������
*�����������
*�������ܣ���������Ϊ: ������9600������λ8��ֹͣλ1����żУ����
*��		 ע��ʹ�õľ�����11.0592MHz�ģ�ע��12MHz��24MHz�Ĳ���
*******************************************/
void uart_init(void)
{
	                  // ������9600
	SCON = 0x50;   	  // ���ڹ�����ģʽ1��8λ���ڣ�����������
	PCON = 0x00;	    // �����ʲ��ӱ�

	                  // ͨ�Ų�������ص�����
	TMOD = 0x20;	    // ����T1Ϊģʽ2
	TH1 = 253;
	TL1 = 253;	   	  // 8λ�Զ���װ����˼����TH1������֮����һ������TL1��
					          // �Զ���װ��TH1ȥ

	TR1 = 1;		      // ����T1������ʼ����
	
	ES = 1;
	EA = 1;
}


/******************************************
*�������ƣ�uart_send_byte
*���������Ҫ���͵�����
*�����������
*�������ܣ����ڷ���һ���ֽڵ�����
*��		 ע��
*******************************************/
void uart_send_byte(unsigned char c)
{
      
   ES=0;           //�رմ����ж�
   SBUF = c;
           
   while (!TI);    //�ȴ��������

   TI = 0;         //����жϷ��ͱ�־
	 ES=0;           //�򿪴����ж�
}


/******************************************
*�������ƣ�UART1_SendString
*���������Ҫ���͵��ַ���
*�����������
*�������ܣ������ַ���
*��		 ע��
*******************************************/
void UART1_SendString(char *s)
{
    while(*s)
    {
        uart_send_byte(*s++);
    }
}


/******************************************
*�������ƣ�putchar
*���������Ҫ���͵�����
*�������������Ҫ���͵�����
*�������ܣ��ض���printf����
*��		 ע��
*******************************************/
char putchar(char c)
{
    uart_send_byte(c);
    return c;
}