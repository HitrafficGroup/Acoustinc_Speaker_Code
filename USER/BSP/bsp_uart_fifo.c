/* Controlador UART con FIFO y recepcion GPS. */
/*
*********************************************************************************************************
*********************************************************************************************************
*/


#include "stm32f10x.h"




UART_T g_tUart1;
UART_T Uart2Gps;

static void UartVarInit(void);
static void InitHardUart(void);

void UartSend(uint8_t *_ucaBuf, uint16_t _usLen);
uint8_t UartGetChar(uint8_t *_pByte);
static void Uart1IRQ(void);
static void ConfigUartNVIC(void);


#define DMA_BUFFER_SIZE	2048
uint8_t dma_usart2_rx_buffer[DMA_BUFFER_SIZE];
volatile uint16_t rx_length = 0;

void DMA_Configuration(void)
{
	DMA_InitTypeDef DMA_InitStructure;
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
	
    
	
    DMA_DeInit(DMA1_Channel6);
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&USART2->DR;
    DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)dma_usart2_rx_buffer;
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
    DMA_InitStructure.DMA_BufferSize = DMA_BUFFER_SIZE;
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
    DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;//DMA_Mode_Normal;
    DMA_InitStructure.DMA_Priority = DMA_Priority_High;
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
    DMA_Init(DMA1_Channel6, &DMA_InitStructure);
	
	DMA_Cmd(DMA1_Channel6, ENABLE);
}

/*
*********************************************************************************************************
*********************************************************************************************************
*/
void bsp_InitUart(void)
{
	DMA_Configuration();
}


/*
*********************************************************************************************************
*********************************************************************************************************
*/
void UartClearTxFifo(void)
{
	g_tUart1.usTxWrite = 0;
	g_tUart1.usTxRead = 0;
	g_tUart1.usTxCount = 0;
}

/*
*********************************************************************************************************
*********************************************************************************************************
*/
void UartClearRxFifo(void)
{
	g_tUart1.usRxWrite = 0;
	g_tUart1.usRxRead = 0;
	g_tUart1.usRxCount = 0;
}
/*
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
    OP.gps_seconds = local_seconds;
}
*/



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
            /* 
            sscanf(OP.Gps.Latitude+2,"%f", &fLat);
            fLat /= 60;
            fLat += (OP.Gps.Latitude[0] - '0')*10 + (OP.Gps.Latitude[1] - '0');
            
            sscanf(OP.Gps.Longitude+3,"%f", &fLng);
            fLng /= 60;
            fLng += (OP.Gps.Longitude[0] - '0')*100 + (OP.Gps.Longitude[1] - '0')*10 + (OP.Gps.Longitude[2] - '0');
            printf("Lng,Lat:%.06f,%.06f\n", fLng, fLat);
            */
			#if DEBUG > 8
				printf("Time : %s\n", system_temp.Gps.time_str);
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
            if(sscanf((char*)Uart2Gps.pRxBuf,"%[^,],%[^,],%[^,],%[^,],%[^,]", tmp,
               system_temp.Gps.time_str, system_temp.Gps.day_str, system_temp.Gps.month_str, system_temp.Gps.year_str)==5)
            {
                //2022/01/07 035457.000
                sscanf(system_temp.Gps.year_str+2,"%2d", &n);    system_temp.Gps.utc.year = n;
                sscanf(system_temp.Gps.month_str,"%2d", &n);     system_temp.Gps.utc.month = n;
                sscanf(system_temp.Gps.day_str,"%2d", &n);       system_temp.Gps.utc.day = n;
                sscanf(system_temp.Gps.time_str,"%2d", &n);      system_temp.Gps.utc.hour = n;
                sscanf(&system_temp.Gps.time_str[2],"%2d", &n);  system_temp.Gps.utc.minute = n;
                sscanf(&system_temp.Gps.time_str[4],"%2d", &n);  system_temp.Gps.utc.second = n;
                
                utc_to_local(&system_temp.Gps.local, &system_temp.Gps.utc, system_temp.TimeZone);
                #if DEBUG > 8
                printf_fifo_hex(&system_temp.Gps.local.second, 7);
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

/*
*********************************************************************************************************
*********************************************************************************************************
*/
static void UartVarInit(void)
{
	
}
void Init_Uart1(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;

	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No ;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USART1, &USART_InitStructure);

	/*
		USART_ITConfig(USART1, USART_IT_TXE, ENABLE);
	*/
}
void Init_Uart2(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);

	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	
	*/
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	*/
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No ;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_Init(USART2, &USART_InitStructure);

	
	USART_DMACmd(USART2, USART_DMAReq_Rx, ENABLE);	
	USART_ITConfig(USART2, USART_IT_IDLE, ENABLE);	
	/*
		USART_ITConfig(USART1, USART_IT_TXE, ENABLE);
	*/

	
}


/*
*********************************************************************************************************
*********************************************************************************************************
*/
static void InitHardUart(void)
{
	Init_Uart1();
	Init_Uart2();
}

/*
*********************************************************************************************************
*********************************************************************************************************
*/
static void ConfigUartNVIC(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;

    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
	
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	
	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

/*
*********************************************************************************************************
*********************************************************************************************************
*/
void UartSendBuf(uint8_t *_ucaBuf, uint16_t _usLen)
{
	uint16_t i;
    
	if(g_tUart1.SendBefor != 0)
	{
	}
    
	for(i = 0; i < _usLen; i++)
	{
		while(1)
		{
			__IO uint16_t usCount;

			DISABLE_INT();
			usCount = g_tUart1.usTxCount;
			ENABLE_INT();

			if (usCount < g_tUart1.usTxBufSize)
			{
				break;
			}
		}
		
		g_tUart1.pTxBuf[g_tUart1.usTxWrite] = _ucaBuf[i];

		DISABLE_INT();
		if(++g_tUart1.usTxWrite >= g_tUart1.usTxBufSize)
		{
			g_tUart1.usTxWrite = 0;
		}
		g_tUart1.usTxCount++;
		ENABLE_INT();
	}

	USART_ITConfig(g_tUart1.uart, USART_IT_TXE, ENABLE);
}

void UartSendChar(uint8_t _ucByte)
{
	UartSendBuf(&_ucByte, 1);
}

/*
*********************************************************************************************************
*********************************************************************************************************
*/
uint8_t UartGetChar(uint8_t *_pByte)
{
	uint16_t usCount;

	
	DISABLE_INT();
	usCount = g_tUart1.usRxCount;
	ENABLE_INT();

	
	//if (_g_tUart1.usRxRead == usRxWrite)
	{
		return 0;
	}
	else
	{

		
		DISABLE_INT();
		if (++g_tUart1.usRxRead >= g_tUart1.usRxBufSize)
		{
			g_tUart1.usRxRead = 0;
		}
		g_tUart1.usRxCount--;
		ENABLE_INT();
		return 1;
	}
}

static void Uart1IRQ(void)
{
	if(USART_GetITStatus(g_tUart1.uart, USART_IT_RXNE) != RESET)
	{
		uint8_t ch;
		
		ch = USART_ReceiveData(g_tUart1.uart);
		g_tUart1.pRxBuf[g_tUart1.usRxWrite] = ch;
		if (++g_tUart1.usRxWrite >= g_tUart1.usRxBufSize)
		{
			g_tUart1.usRxWrite = 0;
		}
		if (g_tUart1.usRxCount < g_tUart1.usRxBufSize)
		{
			g_tUart1.usRxCount++;
		}

        if (g_tUart1.ReciveNew)
        {
            g_tUart1.ReciveNew(ch);
        }
	}

	if(USART_GetITStatus(g_tUart1.uart, USART_IT_TXE) != RESET)
	{
		if(g_tUart1.usTxCount == 0)
		{
			USART_ITConfig(g_tUart1.uart, USART_IT_TXE, DISABLE);

			USART_ITConfig(g_tUart1.uart, USART_IT_TC, ENABLE);
		}
		else
		{
			USART_SendData(g_tUart1.uart, g_tUart1.pTxBuf[g_tUart1.usTxRead]);
			if (++g_tUart1.usTxRead >= g_tUart1.usTxBufSize)
			{
				g_tUart1.usTxRead = 0;
			}
			g_tUart1.usTxCount--;
		}
	}
	else if (USART_GetITStatus(g_tUart1.uart, USART_IT_TC) != RESET)
	{
		if (g_tUart1.usTxCount == 0)
		{
			USART_ITConfig(g_tUart1.uart, USART_IT_TC, DISABLE);

			if (g_tUart1.SendOver)
			{
				g_tUart1.SendOver();
			}
		}
		else
		{
			USART_SendData(g_tUart1.uart, g_tUart1.pTxBuf[g_tUart1.usTxRead]);
			if (++g_tUart1.usTxRead >= g_tUart1.usTxBufSize)
			{
				g_tUart1.usTxRead = 0;
			}
			g_tUart1.usTxCount--;
		}
	}
}

static void GpsIRQ(void)
{
	
//	if(USART_GetITStatus(Uart2Gps.uart, USART_IT_RXNE) != RESET)
//	{

//		uint8_t ch;
//		
//		ch = USART_ReceiveData(Uart2Gps.uart);
//        if(ch == '$')
//        {
//            Uart2Gps.usRxWrite = 0;
//            Uart2Gps.usRxCount = 0;
//        }
//		Uart2Gps.pRxBuf[Uart2Gps.usRxWrite] = ch;
//        Uart2Gps.usRxWrite++;
//        
//		if(Uart2Gps.usRxCount < Uart2Gps.usRxBufSize)
//		{
//			Uart2Gps.usRxCount++;
//		}
//        
//        //Uart2Gps is for GPS, and if GPS get a packet end code then we analysis it; 
//        if(ch == '\n')//0x0a
//		{
//			if(Uart2Gps.ReciveNew)
//			{
//				Uart2Gps.ReciveNew(Uart2Gps.usRxCount);
//			}
//		}
//	}
    if (USART_GetITStatus(USART2, USART_IT_IDLE) != RESET) 
	{
		uint16_t i;
        
        USART_ReceiveData(USART2); 
        
        rx_length = DMA_BUFFER_SIZE - DMA_GetCurrDataCounter(DMA1_Channel6);
		/*
		printf("USART3_IT_IDLE rx_length = %d\n",rx_length);
		printf_fifo_hex(rx_buffer, rx_length);
        
		*/
        for (i = 0; i < rx_length; i++) 
		{
            
			uint8_t ch = dma_usart2_rx_buffer[i];
			
			if(Uart2Gps.usRxWrite == 0)
			{
				if(ch == '$')
				{
					Uart2Gps.pRxBuf[Uart2Gps.usRxWrite] = ch;
					Uart2Gps.usRxWrite++;
				}
			}
			else
			{
				Uart2Gps.pRxBuf[Uart2Gps.usRxWrite] = ch;
				if (Uart2Gps.usRxWrite < Uart2Gps.usRxBufSize)
				{
					Uart2Gps.usRxWrite++;
				}

				if(ch == '\n')
				{
					
					if (Uart2Gps.ReciveNew)
					{
						Uart2Gps.ReciveNew(Uart2Gps.usRxWrite);
						Uart2Gps.usRxWrite = 0;
					}
				}
			}
        }
		
        
        DMA_Cmd(DMA1_Channel6, DISABLE);
        DMA_SetCurrDataCounter(DMA1_Channel6, DMA_BUFFER_SIZE);
        DMA_Cmd(DMA1_Channel6, ENABLE);
    }
	
	
	if(USART_GetITStatus(Uart2Gps.uart, USART_IT_TXE) != RESET)
	{
		if (Uart2Gps.usTxCount == 0)
		{
			
			USART_ITConfig(Uart2Gps.uart, USART_IT_TXE, DISABLE);

			
			USART_ITConfig(Uart2Gps.uart, USART_IT_TC, ENABLE);
		}
		else
		{
			
			USART_SendData(Uart2Gps.uart, Uart2Gps.pTxBuf[Uart2Gps.usTxRead]);
			if (++Uart2Gps.usTxRead >= Uart2Gps.usTxBufSize)
			{
				Uart2Gps.usTxRead = 0;
			}
			Uart2Gps.usTxCount--;
		}
	}
	
	else if (USART_GetITStatus(Uart2Gps.uart, USART_IT_TC) != RESET)
	{
		if (Uart2Gps.usTxCount == 0)
		{
			
			USART_ITConfig(Uart2Gps.uart, USART_IT_TC, DISABLE);

			
			if (Uart2Gps.SendOver)
			{
				Uart2Gps.SendOver();
			}
		}
		else
		{
			
			
			USART_SendData(Uart2Gps.uart, Uart2Gps.pTxBuf[Uart2Gps.usTxRead]);
			if (++Uart2Gps.usTxRead >= Uart2Gps.usTxBufSize)
			{
				Uart2Gps.usTxRead = 0;
			}
			Uart2Gps.usTxCount--;
		}
	}
	
//	if(USART_GetITStatus(Uart2Gps.uart, USART_FLAG_ORE) != RESET)
//	{
//		USART_ReceiveData(Uart2Gps.uart);
//		//USART_ClearFlag(Uart2Gps.uart, USART_FLAG_ORE);
//	}
	
	
	
	
	
	if ((Uart2Gps.uart->SR & (USART_FLAG_PE|USART_FLAG_NE|USART_IT_FE|USART_FLAG_ORE)) != (uint16_t)RESET)
	{
		//USART_ClearFlag(_pUart->uart, USART_FLAG_ORE);
		USART_ReceiveData(Uart2Gps.uart);
	}
}

/*
*********************************************************************************************************
*********************************************************************************************************
*/
void USART1_IRQHandler(void)
{
    Uart1IRQ();
}

void USART2_IRQHandler(void)
{
    GpsIRQ();
}

void printf_fifo_hex(uint8_t* tx, uint8_t len)
{
    while(len--)
    {
        printf("%02x ",*tx++);
    }
    printf("\n");
}


/*
*********************************************************************************************************
*********************************************************************************************************
*/
int fputc(int ch, FILE *f)
{
    UartSendChar(ch);
	return ch;
}

/*
*********************************************************************************************************
*********************************************************************************************************
*/
int fgetc(FILE *f)
{
	uint8_t ucData;
	while(UartGetChar(&ucData) == 0);
	return ucData;
}
