#ifndef __RTC_TCS8563_H_
#define __RTC_TCS8563_H_

#include<reg51.h>
#include "i2c.h"

void rtc_tcs8563_init(void);
int  rtc_tcs8563_SetDate(unsigned int year,  unsigned int month,   unsigned int day);
int  rtc_tcs8563_GetDate(unsigned int *year, unsigned int *month,  unsigned int *day);
int  rtc_tcs8563_SetTime(unsigned int hour,  unsigned int minute,  unsigned int second);
int  rtc_tcs8563_GetTime(unsigned int *hour, unsigned int *minute, unsigned int *second);

#endif
