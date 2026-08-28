/* Configura o ejecuta una transaccion I2C o RTC. */

#ifndef _BSP_I2C_H
#define _BSP_I2C_H

#include <stdint.h>

#define RTC_IRQ_PORT	    GPIOB
#define RTC_IRQ_PIN	        GPIO_Pin_5

#define ISL1208_ADDR 0xDE


#define SecReg  0x00
#define MinReg  0x01
#define HourReg 0x02
#define DayReg  0x03
#define MonReg  0x04
#define YearReg 0x05
#define WeekReg 0x06

#define SR_Reg      0x07 //
#define INT_Reg     0x08 //
#define ATR_Reg     0x0A //
#define DTR_Reg     0x0B //

#define SecAlarm    0x0C //
#define MinAlarm    0x0D //
#define HourAlarm   0x0E //
#define DayAlarm    0x0F //
#define MonAlarm    0x10 //
#define WeekAlarm   0x11 //
#define User1       0x12
#define User2       0x13

typedef struct
{
	uint8_t second;     //00-59 
	uint8_t minute;     //00-59 
	uint8_t hour;       //00-23 
	uint8_t day;        //00-31 
	uint8_t month;      //01-12
	uint8_t year;       //00-99
	uint8_t week;
}RtcType;


extern uint8_t rtc[7];
extern RtcType *SYS_RTC;

void bsp_InitI2C(void);
void RtcRead(RtcType* Time);
void RtcWrite(RtcType* Time);

#endif
