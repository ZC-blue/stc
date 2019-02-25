#include "algorithm.h"


unsigned int v[2]={5263695615,5555555555},k[4]={32,20,30,43};
//V �� Ҫ���ܽ��ܵ�����
//K �� ���ܽ�����Կ


/******************************************
*�������ƣ�encrypt
*�����������
*�����������
*�������ܣ������㷨
*��		 ע��
*******************************************/
void encrypt (unsigned int* v, unsigned int* k) 
{
    unsigned int v0=v[0], v1=v[1], sum=0, i;           /* set up */
    unsigned int delta=0x9e3779b9;                     /* a key schedule constant */
    unsigned int k0=k[0], k1=k[1], k2=k[2], k3=k[3];   /* cache key */
    
	  for (i=0; i < 32; i++) 
	  {                                                  /* basic cycle start */
        sum += delta;
        v0 += ((v1<<4) + k0) ^ (v1 + sum) ^ ((v1>>5) + k1);
        v1 += ((v0<<4) + k2) ^ (v0 + sum) ^ ((v0>>5) + k3);
    }                                                  /* end cycle */
    v[0]=v0; v[1]=v1;
}

/******************************************
*�������ƣ�decrypt
*�����������
*�����������
*�������ܣ������㷨
*��		 ע��
*******************************************/
void decrypt (unsigned int* v, unsigned int* k) 
{
    unsigned int v0=v[0], v1=v[1], sum=0xC6EF3720, i;  /* set up */
    unsigned int delta=0x9e3779b9;                     /* a key schedule constant */
    unsigned int k0=k[0], k1=k[1], k2=k[2], k3=k[3];   /* cache key */
    
	  for (i=0; i<32; i++) 
	  {                                                  /* basic cycle start */
        v1 -= ((v0<<4) + k2) ^ (v0 + sum) ^ ((v0>>5) + k3);
        v0 -= ((v1<<4) + k0) ^ (v1 + sum) ^ ((v1>>5) + k1);
        sum -= delta;
    }                                                  /* end cycle */
    v[0]=v0; v[1]=v1;
}

