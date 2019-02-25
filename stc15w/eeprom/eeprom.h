#ifndef __EEPROM_H_
#define __EEPROM_H_

#include<reg51.h>

#include "intrins.h"

void          IapIdle();
unsigned char IapReadByte(unsigned int  addr);
void          IapProgramByte(unsigned int  addr, unsigned char  dat);
void          IapEraseSector(unsigned int  addr);


#endif
