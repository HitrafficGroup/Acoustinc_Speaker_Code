#include "uart_fifo_gps.h"
#include <string.h> 
#include <stdio.h>  

 //                             0   1   2   3   4   5   6   7   8   9  10  11  12
const uint8_t DayMonth[13] = { 0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
uint8_t Is_Leap_Year(uint16_t year)
{
	if(year%4==0)
	{
		if(year%100==0)
		{
			if(year%400==0)return 1;
			else return 0;
		}else return 1;
	}else return 0;
}


void calc_week(RtcType* rtc)
{
    uint16_t y,m,d,a,b,c;

    y = rtc->year;
    m = rtc->month;
    d = rtc->day;
    if(m<3)
    {
        m = m+12;
        y = y-1;
    }
    a = y/4;
    b = (m+1)*13/5;
    c = y+a+b+d-1;
    c = c%7;
    rtc->week = c;
}


void second_to_rtc(uint32_t seconds, RtcType* rtc)
{
    uint8_t  temp;
    uint32_t seconds_temp = seconds;
	
    temp = seconds_temp/3600;
    rtc->hour = DEC_to_BCD(temp);
    
    temp = (seconds_temp%3600)/60;
    rtc->minute = DEC_to_BCD(temp);
    
    temp = seconds_temp%60;
    rtc->second = DEC_to_BCD(temp);
}


void utc_to_local(RtcType* local, RtcType* utc, uint8_t* TimeZone)
{
    uint8_t days;
    uint32_t timezone, seconds, local_seconds;
	
    timezone = (TimeZone[2]<<8)|TimeZone[3];
    seconds = utc->hour * 3600 + utc->minute * 60 + utc->second;
    if(utc->month != 2) days = DayMonth[utc->month];
    else 
    {
        if(Is_Leap_Year(2000+utc->year)) days = 29; else days = 28;
    }
    
    local->year = utc->year;
    local->month = utc->month;
    local->day = utc->day;
    
    if(TimeZone[0])
    {
        if(seconds + timezone < 86400)
        {
            local_seconds = seconds + timezone;
        }
        else 
        {
            local_seconds = seconds + timezone - 86400;
            if(local->day < days) local->day++;
            else 
            {
                local->day = 1;
                if(local->month < 12) local->month++;
                else 
                {
                    local->month = 1;
                    local->year++;
                }
            }
        }
    }
    else
    {
        if(seconds >= timezone)
        {
            local_seconds = seconds - timezone;
        }
        else 
        {
            local_seconds = seconds + 86400 - timezone;
            if(local->day > 1) local->day--;
            else 
            {
                if(local->month > 1) 
                {
                    local->month--;
                    if(local->month != 2) local->day = DayMonth[local->month];
                    else 
                    {
                        if(Is_Leap_Year(2000+local->year)) local->day = 29; else local->day = 28;
                    }
                }
                else 
                {
                    local->year--;
                    local->month = 12;
                    local->day = 31;
                }
            }
        }
    }
    
    calc_week(local);
    local->year = DEC_to_BCD(local->year);
    local->month = DEC_to_BCD(local->month);
    local->day = DEC_to_BCD(local->day);
    
    second_to_rtc(local_seconds, local);
    system_temp.gps_seconds = local_seconds;
}


void rtcConvert(uint8_t* prtc_bcd, uint8_t* prtc_dec)
{
	RtcType* rtc_bcd = (RtcType*)prtc_bcd;
	RtcType* rtc_dec = (RtcType*)prtc_dec;
	
	rtc_dec->year = BCD_to_DEC(rtc_bcd->year);
    rtc_dec->month = BCD_to_DEC(rtc_bcd->month);
	rtc_dec->day = BCD_to_DEC(rtc_bcd->day);
    rtc_dec->minute = BCD_to_DEC(rtc_bcd->minute);
	rtc_dec->hour = BCD_to_DEC(rtc_bcd->hour);
    rtc_dec->second = BCD_to_DEC(rtc_bcd->second);
	rtc_dec->week = rtc_bcd->week;
}


void Gps_ReciveNew(uint16_t RxCount)
{
    if(strncmp((char*)(&Uart2Gps.pRxBuf[3]), "GGA", 3) == 0)
    {
        if(strstr((char*)Uart2Gps.pRxBuf, ",,,,,"))
        {
			system_temp.gps_flag = 0;
            if(DEBUG > 8)printf("Place the GPS to open area\n");
            return;
        }
        else
        {
            //float fLat,fLng;
            char tmp[10];
            //$GNGGA,073741.000,2243.0486,N,11348.3295,E,1,09,1.6,18.8,M,0.0,M,,*44
            sscanf((char*)Uart2Gps.pRxBuf,"%[^,],%[^,],%[^,],%[^,],%[^,],%[^,]", tmp, system_temp.Gps.time_str, system_temp.Gps.Latitude, system_temp.Gps.NS, system_temp.Gps.Longitude, system_temp.Gps.EW);
            
            // sscanf(OP.Gps.Latitude+2,"%f", &fLat);
            // fLat /= 60;
            // fLat += (OP.Gps.Latitude[0] - '0')*10 + (OP.Gps.Latitude[1] - '0');
            
            // sscanf(OP.Gps.Longitude+3,"%f", &fLng);
            // fLng /= 60;
            // fLng += (OP.Gps.Longitude[0] - '0')*100 + (OP.Gps.Longitude[1] - '0')*10 + (OP.Gps.Longitude[2] - '0');
            // printf("Lng,Lat:%.06f,%.06f\n", fLng, fLat);
            
			#if DEBUG > 8
				printf("Time : %s -system_temp.Gps.time_str- \n", system_temp.Gps.time_str);
				printf("ns   : %s\n", system_temp.Gps.NS);
				printf("ew   : %s\n", system_temp.Gps.EW);
				printf("Lat  : %s\n", system_temp.Gps.Latitude);
				printf("Lng  : %s\n", system_temp.Gps.Longitude);
			#endif
        }
    }
    else if(strncmp((char*)(&Uart2Gps.pRxBuf[3]), "ZDA", 3) == 0)
    {
        if(strstr((char*)Uart2Gps.pRxBuf, ",,,,,"))
        {
            system_temp.gps_flag = 0;
			system_temp.gps_count = 0;
            //printf("Place the GPS to open area\n");
            return;
        }
        else 
        {
            int n;
            char tmp[10];
            //$GNZDA,073741.000,22,11,2018,00,00*45
            if(sscanf((char*)Uart2Gps.pRxBuf,"%[^,],%[^,],%[^,],%[^,],%[^,]", tmp, system_temp.Gps.time_str, system_temp.Gps.day_str, system_temp.Gps.month_str, system_temp.Gps.year_str)==5)
            {
                //2022/01/07 035457.000  //String to int
                sscanf(system_temp.Gps.year_str+2,"%2d", &n);    system_temp.Gps.utc.year = n;
                sscanf(system_temp.Gps.month_str,"%2d", &n);     system_temp.Gps.utc.month = n;
                sscanf(system_temp.Gps.day_str,"%2d", &n);       system_temp.Gps.utc.day = n;
                sscanf(system_temp.Gps.time_str,"%2d", &n);      system_temp.Gps.utc.hour = n;
                sscanf(&system_temp.Gps.time_str[2],"%2d", &n);  system_temp.Gps.utc.minute = n;
                sscanf(&system_temp.Gps.time_str[4],"%2d", &n);  system_temp.Gps.utc.second = n;
                
                utc_to_local(&system_temp.Gps.local, &system_temp.Gps.utc, system_temp.TimeZone);
                #if DEBUG > 8
				printf("GPS LOCAL: system_temp.Gps.local.second = ");
                printf_fifo_hex(&system_temp.Gps.local.second, 7);
				printf("\r\n");
                #endif
                if(system_temp.Gps.utc.second >= 2 && system_temp.seconds >= 2)
                {
                    if(system_temp.seconds > system_temp.gps_seconds)
                    {
                        if(system_temp.seconds - system_temp.gps_seconds > 2) system_temp.sync_with_gps_flag = 1;
                    }
                    else
                    {
                         if(system_temp.gps_seconds - system_temp.seconds > 2) system_temp.sync_with_gps_flag = 1;
                    }
                }
                system_temp.gps_flag = 1;
				system_temp.gps_count = 0;
				LED_Toggle();
            }
        }
    }
}


void Auto_adjust_time(void) //Actualiza el RTC con la hora GPS
{
    if(system_temp.sync_with_gps_flag)//wcxmask
    {
        RtcWrite(&system_temp.Gps.local);
		system_temp.timeUpdate = 1;
        system_temp.sync_with_gps_flag = 0;
        #if DEBUG > 2
        printf("sync_with_gps, Seconds = %d, gps_seconds = %d\n", system_temp.seconds, system_temp.gps_seconds);
        #endif
    }
}


static  uint32_t gps_count;
void auto_adjust_time_periodically(void) //Actualiza el RTC con la hora GPS cada cierto tiempo si la bandera de sincronización está activa
{
    if(system_temp.gps_flag && system_temp.sync_with_gps_flag)
    {
        if(gps_count >= 86400) //60s=1minute //600s=10minutes //86400s=1440min=1day
        {
            gps_count = 0;
            
            RtcWrite(&system_temp.Gps.local);
            system_temp.timeUpdate = 1;
            system_temp.sync_with_gps_flag = 0;
            
            #if DEBUG > 2
            //printf("sync_with_gps, Seconds = %d, gps_seconds = %d\n", system_temp.seconds, system_temp.gps_seconds);
            #endif
        }
        else
        {
            gps_count++;
            //printf("GPS COUNT = %d \n", gps_count);
        }
    }
}
