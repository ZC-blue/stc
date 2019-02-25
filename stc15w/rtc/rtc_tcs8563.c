#include "rtc_tcs8563.h"

#define CTRL_STA1_REG           (0x00)
#define CTRL_STA2_REG           (0x01)
#define VL_SECOND_REG           (0x02)
#define MINUTES_REG             (0x03)
#define HOURS_REG               (0x04)
#define DAYS_REG                (0x05)
#define WEEKDAYS_REG            (0x06)
#define MONTHS_REG              (0x07)
#define YEARS_REG               (0x08)
#define MINUTE_ALM_REG          (0x09)
#define HOUR_ALM_REG            (0x0A)
#define DAY_ALM_REG             (0x0B)
#define WEEKDAY_ALM_REG         (0x0C)
#define CLKOUT_CTRL_REG         (0x0D)
#define TIMER_CTRL_REG          (0x0E)
#define TIMER_REG               (0x0F)


#define Int8ToBCD(x)            (((x)/10)<<4)|((x)%10)
#define BCDToInt8(x)            ((((x)>>4)&0xf)*10+((x)&0xf))


/******************************************
*函数名称：rtc_tcs8563_init
*输入参数：无
*输出参数：无
*函数功能：rtc芯片初始化
*备		 注：
*******************************************/
void rtc_tcs8563_init(void)
{






}
/******************************************
*函数名称：rtc_tcs8563_SetDate
*输入参数：设置的年月日时间参数
*输出参数：无
*函数功能：设置rtc时间
*备		 注：
*******************************************/

int  rtc_tcs8563_SetDate(unsigned int  year, unsigned int month, unsigned int day)
{
    unsigned char     tmpDay, tmpMonth, tmpYear;

    if(   (year<1900 || year>2100)
       || (month>12) || (day>31))
    {

        return 0;
    }

    tmpDay   = Int8ToBCD(day);
    tmpMonth = Int8ToBCD(month);
		
    if(year<2000)
    {
        tmpMonth |= (1<<7);
        year     -= 1900;
    }
    else
    {
        year     -= 2000;
    }
    tmpYear  = Int8ToBCD(year);

    I2C_WriteReg(DAYS_REG, &tmpDay);
    I2C_WriteReg(MONTHS_REG, &tmpMonth);
    I2C_WriteReg(YEARS_REG, &tmpYear);
 
    return 0;
}
/******************************************
*函数名称：rtc_tcs8563_GetDate
*输入参数：获取到年月日数据缓冲区
*输出参数：返回0
*函数功能：获取rtc时间
*备		 注：
*******************************************/
int  rtc_tcs8563_GetDate(unsigned int *year, unsigned int *month, unsigned int *day)
{
    unsigned char tmpDay, tmpMonth, tmpYear;

    I2C_ReadReg(DAYS_REG, &tmpDay);
    I2C_ReadReg(MONTHS_REG, &tmpMonth);
    I2C_ReadReg(YEARS_REG, &tmpYear);


    *day  = BCDToInt8(tmpDay & 0x3f);
    *year = BCDToInt8(tmpYear);

    if(tmpMonth & 0x80)
    {
        *year += 1900;
    }
    else
    {
        *year += 2000;
    }
    *month = BCDToInt8(tmpMonth & 0x1f);

    return 0;
}
/******************************************
*函数名称：rtc_tcs8563_SetTime
*输入参数：设置时分秒参数
*输出参数：返回0
*函数功能：设置rtc时分秒参数
*备		 注：
*******************************************/


int  rtc_tcs8563_SetTime(unsigned int hour, unsigned int minute, unsigned int second)
{
    unsigned char    tmpHour, tmpMinute, tmpSecond;

    if((hour>24) || (minute>59) || (second>59))
    {
        return 0;
    }

    //convert the value of calendar's member from integer to BCD code
    tmpHour   =  Int8ToBCD(hour);
    tmpMinute =  Int8ToBCD(minute);
    tmpSecond =  Int8ToBCD(second);

    I2C_WriteReg(VL_SECOND_REG, &tmpSecond);
    I2C_WriteReg(MINUTES_REG, &tmpMinute);
    I2C_WriteReg(HOURS_REG, &tmpHour);

    return 0;
}

/******************************************
*函数名称：rtc_tcs8563_GetTime
*输入参数：获取时分秒的数据缓冲区
*输出参数：返回0
*函数功能：获取rtc时分秒时间
*备		 注：
*******************************************/

int  rtc_tcs8563_GetTime(unsigned int *hour, unsigned int *minute, unsigned int *second)
{
    unsigned char    tmpHour, tmpMinute, tmpSecond;

    I2C_ReadReg(VL_SECOND_REG, &tmpSecond);
    I2C_ReadReg(MINUTES_REG, &tmpMinute);
    I2C_ReadReg(HOURS_REG, &tmpHour);
 
    *hour = BCDToInt8(tmpHour & 0x3f);
    *minute = BCDToInt8(tmpMinute & 0x7f);
    *second = BCDToInt8(tmpSecond & 0x7f);

    return 0;
}