#ifndef __UART_FIFO_GPS_H
#define __UART_FIFO_GPS_H

/* Include core types and hardware definitions */
#include "stm32f10x.h"
//#include <stdint.h>

/* Inherit struct definitions (e.g., RtcType, system_temp) from the BSP */
#include "bsp_uart_fifo.h" 
#include "bsp_io.h"

/* 0. Array must be exposed via extern to avoid multiple definition errors */
/* Exported global variables */
extern const uint8_t DayMonth[13];
extern UART_T Uart2Gps; /* Fixes: error #20: identifier "Uart2Gps" is undefined */

/* Function Prototypes */
uint8_t Is_Leap_Year(uint16_t year);
void calc_week(RtcType* rtc);
void second_to_rtc(uint32_t seconds, RtcType* rtc);
void utc_to_local(RtcType* local, RtcType* utc, uint8_t* TimeZone);
void rtcConvert(uint8_t* prtc_bcd, uint8_t* prtc_dec);
void Gps_ReciveNew(uint16_t RxCount);
void Auto_adjust_time(void);
void auto_adjust_time_periodically(void);

#endif /* __UART_FIFO_GPS_H */
