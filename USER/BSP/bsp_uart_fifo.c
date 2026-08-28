/*
*********************************************************************************************************
*	ģ������ : �����ж�+FIFO����ģ��
*	�ļ����� : bsp_uart_fifo.c
*	��    �� : V1.0
*	˵    �� : ���ô����ж�+FIFOģʽʵ�ֶ�����ڵ�ͬʱ����
*********************************************************************************************************
*/


#include "stm32f10x.h"


/* ����ÿ�����ڽṹ����� */
static uint8_t g_TxBuf1[UART1_TX_BUF_SIZE];		/* ���ͻ����� */
static uint8_t g_RxBuf1[UART1_RX_BUF_SIZE];		/* ���ջ����� */

UART_T g_tUart1;
UART_T Uart2Gps;
static uint8_t g_TxBuf2[UART2_TX_BUF_SIZE];		/* ���ͻ����� */
static uint8_t g_RxBuf2[UART2_RX_BUF_SIZE];		/* ���ջ����� */

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
	
    // ����DMA1ͨ��3��USART3_RX��
	// ����DMA1ͨ��6��USART2_RX��
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
	
	DMA_Cmd(DMA1_Channel6, ENABLE);// ����DMA����
}

/*
*********************************************************************************************************
*	�� �� ��: bsp_InitUart
*	����˵��: ��ʼ������Ӳ��������ȫ�ֱ�������ֵ.
*	��    ��:  ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void bsp_InitUart(void)
{
	UartVarInit();		/* �����ȳ�ʼ��ȫ�ֱ���,������Ӳ�� */
	InitHardUart();		/* ���ô��ڵ�Ӳ������(�����ʵ�) */
	DMA_Configuration();
	ConfigUartNVIC();	/* ���ô����ж� */
}


/*
*********************************************************************************************************
*	�� �� ��: UartClearTxFifo
*	����˵��: ���㴮�ڷ��ͻ�����
*	��    ��: ��
*	�� �� ֵ: ��
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
*	�� �� ��: UartClearRxFifo
*	����˵��: ���㴮�ڽ��ջ�����
*	��    ��: ��
*	�� �� ֵ: ��
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
    
    if(TimeZone[0])//����
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
    else //����
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

//����:���
//���:������ǲ������� 1�� 0����
uint8_t Is_Leap_Year(uint16_t year)
{
	if(year%4==0)//�����ܱ�4����
	{
		if(year%100==0)
		{
			if(year%400==0)return 1;//�����00��β,��Ҫ�ܱ�400���� 	   
			else return 0;
		}else return 1;
	}else return 0;
}

void calc_week(RtcType* rtc)
{
    uint16_t y,m,d,a,b,c;
    //��������
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
    
    if(TimeZone[0])//����
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
    else //����
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
*	�� �� ��: UartVarInit
*	����˵��: ��ʼ��������صı���
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void UartVarInit(void)
{
	g_tUart1.uart = USART1;						/* STM32 �����豸 */
	g_tUart1.pTxBuf = g_TxBuf1;					/* ���ͻ�����ָ�� */
	g_tUart1.pRxBuf = g_RxBuf1;					/* ���ջ�����ָ�� */
	g_tUart1.usTxBufSize = UART1_TX_BUF_SIZE;	/* ���ͻ�������С */
	g_tUart1.usRxBufSize = UART1_RX_BUF_SIZE;	/* ���ջ�������С */
	g_tUart1.usTxWrite = 0;						/* ����FIFOд���� */
	g_tUart1.usTxRead = 0;						/* ����FIFO������ */
	g_tUart1.usRxWrite = 0;						/* ����FIFOд���� */
	g_tUart1.usRxRead = 0;						/* ����FIFO������ */
	g_tUart1.usRxCount = 0;						/* ���յ��������ݸ��� */
	g_tUart1.usTxCount = 0;						/* �����͵����ݸ��� */
	g_tUart1.SendBefor = 0;						/* ��������ǰ�Ļص����� */
	g_tUart1.SendOver = 0;						/* ������Ϻ�Ļص����� */
	g_tUart1.ReciveNew = 0;						/* ���յ������ݺ�Ļص����� */
	
	Uart2Gps.uart = USART2;						/* STM32 �����豸 */
	Uart2Gps.pTxBuf = g_TxBuf2;					/* ���ͻ�����ָ�� */
	Uart2Gps.pRxBuf = g_RxBuf2;					/* ���ջ�����ָ�� */
	Uart2Gps.usTxBufSize = UART2_TX_BUF_SIZE;	/* ���ͻ�������С */
	Uart2Gps.usRxBufSize = UART2_RX_BUF_SIZE;	/* ���ջ�������С */
	Uart2Gps.usTxWrite = 0;						/* ����FIFOд���� */
	Uart2Gps.usTxRead = 0;						/* ����FIFO������ */
	Uart2Gps.usRxWrite = 0;						/* ����FIFOд���� */
	Uart2Gps.usRxRead = 0;						/* ����FIFO������ */
	Uart2Gps.usRxCount = 0;						/* ���յ��������ݸ��� */
	Uart2Gps.usTxCount = 0;						/* �����͵����ݸ��� */
	Uart2Gps.SendBefor = 0;						/* ��������ǰ�Ļص����� */
	Uart2Gps.SendOver = 0;						/* ������Ϻ�Ļص����� */
	Uart2Gps.ReciveNew = Gps_ReciveNew;			/* ���յ������ݺ�Ļص����� */
}
void Init_Uart1(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;

	/* ����1 TX = PA9   RX = PA10 �� TX = PB6   RX = PB7*/
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	USART_InitStructure.USART_BaudRate = UART1_BAUD;	/* ������ */
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No ;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USART1, &USART_InitStructure);

	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);	/* ʹ�ܽ����ж� */
	/*
		USART_ITConfig(USART1, USART_IT_TXE, ENABLE);
		ע��: ��Ҫ�ڴ˴��򿪷����ж�
		�����ж�ʹ����SendUart()������
	*/
	USART_Cmd(USART1, ENABLE);		/* ʹ�ܴ��� */
	USART_ClearFlag(USART1, USART_FLAG_TC);     /* �巢����ɱ�־��Transmission Complete flag */
}
void Init_Uart2(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	/* ��1������GPIO��USART������ʱ�� */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);

	/* ��2������USART Tx��GPIO����Ϊ���츴��ģʽ */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	/* ��3������USART Rx��GPIO����Ϊ��������ģʽ
		����CPU��λ��GPIOȱʡ���Ǹ�������ģʽ���������������費�Ǳ����
		���ǣ��һ��ǽ�����ϱ����Ķ������ҷ�ֹ�����ط��޸���������ߵ����ò���
	*/
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	/*  ��3���Ѿ����ˣ�����ⲽ���Բ���
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	*/
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	/* ��4���� ���ô���Ӳ������ */
	USART_InitStructure.USART_BaudRate = UART2_BAUD;	/* ������ */
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No ;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;		/* ��ѡ�����ģʽ */
	USART_Init(USART2, &USART_InitStructure);

	//USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);	/* ʹ�ܽ����ж� */
	USART_DMACmd(USART2, USART_DMAReq_Rx, ENABLE);	// ʹ��USART3��DMA����
	USART_ITConfig(USART2, USART_IT_IDLE, ENABLE);	// ʹ��USART3�Ŀ����ж�
	/*
		USART_ITConfig(USART1, USART_IT_TXE, ENABLE);
		ע��: ��Ҫ�ڴ˴��򿪷����ж�
		�����ж�ʹ����SendUart()������
	*/
	USART_Cmd(USART2, ENABLE);		/* ʹ�ܴ��� */

	/* CPU��Сȱ�ݣ��������úã����ֱ��Send�����1���ֽڷ��Ͳ���ȥ
		�����������1���ֽ��޷���ȷ���ͳ�ȥ������ */
	USART_ClearFlag(USART2, USART_FLAG_TC);     /* �巢����ɱ�־��Transmission Complete flag */
}


/*
*********************************************************************************************************
*	�� �� ��: InitHardUart
*	����˵��: ���ô��ڵ�Ӳ�������������ʣ�����λ��ֹͣλ����ʼλ��У��λ���ж�ʹ�ܣ��ʺ���STM32-F4������
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void InitHardUart(void)
{
	Init_Uart1();
	Init_Uart2();
}

/*
*********************************************************************************************************
*	�� �� ��: ConfigUartNVIC
*	����˵��: ���ô���Ӳ���ж�.
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void ConfigUartNVIC(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;

    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
	/* ʹ�ܴ���1�ж� */
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	/* ʹ�ܴ���2�ж� */
	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

/*
*********************************************************************************************************
*	�� �� ��: UartSend
*	����˵��: ��д���ݵ�UART���ͻ�����,�����������жϡ��жϴ�������������Ϻ��Զ��رշ����ж�
*	��    ��:  ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void UartSendBuf(uint8_t *_ucaBuf, uint16_t _usLen)
{
	uint16_t i;
    
	if(g_tUart1.SendBefor != 0)
	{
		g_tUart1.SendBefor();		/* �����RS485ͨ�ţ���������������н�RS485����Ϊ����ģʽ */
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
		/* �����������뷢�ͻ����� */
		g_tUart1.pTxBuf[g_tUart1.usTxWrite] = _ucaBuf[i];

		DISABLE_INT();
		if(++g_tUart1.usTxWrite >= g_tUart1.usTxBufSize)
		{
			g_tUart1.usTxWrite = 0;
		}
		g_tUart1.usTxCount++;
		ENABLE_INT();
	}

	USART_ITConfig(g_tUart1.uart, USART_IT_TXE, ENABLE);//���������ж� 
}

void UartSendChar(uint8_t _ucByte)
{
	UartSendBuf(&_ucByte, 1);
}

/*
*********************************************************************************************************
*	�� �� ��: UartGetChar
*	����˵��: �Ӵ��ڽ��ջ�������ȡ1�ֽ����� ��������������ã�
*	��    ��: _pUart : �����豸
*			  _pByte : ��Ŷ�ȡ���ݵ�ָ��
*	�� �� ֵ: 0 ��ʾ������  1��ʾ��ȡ������
*********************************************************************************************************
*/
uint8_t UartGetChar(uint8_t *_pByte)
{
	uint16_t usCount;

	/* usRxWrite �������жϺ����б���д���������ȡ�ñ���ʱ����������ٽ������� */
	DISABLE_INT();
	usCount = g_tUart1.usRxCount;
	ENABLE_INT();

	/* �������д������ͬ���򷵻�0 */
	//if (_g_tUart1.usRxRead == usRxWrite)
	if (usCount == 0)	/* �Ѿ�û������ */
	{
		return 0;
	}
	else
	{
		*_pByte = g_tUart1.pRxBuf[g_tUart1.usRxRead];		/* �Ӵ��ڽ���FIFOȡ1������ */

		/* ��дFIFO������ */
		DISABLE_INT();
		if (++g_tUart1.usRxRead >= g_tUart1.usRxBufSize)//������������β��
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
	/* ���������ж�  */
//	if(USART_GetITStatus(Uart2Gps.uart, USART_IT_RXNE) != RESET)
//	{
//		/* �Ӵ��ڽ������ݼĴ�����ȡ���ݴ�ŵ�����FIFO */
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
        // ��������жϱ�־
        USART_ReceiveData(USART2); // ��ȡDR�Ĵ����������־
        // �����ѽ��յ����ݳ���
        rx_length = DMA_BUFFER_SIZE - DMA_GetCurrDataCounter(DMA1_Channel6);
		/*
		printf("USART3_IT_IDLE rx_length = %d\n",rx_length);
		printf_fifo_hex(rx_buffer, rx_length);
        // �������յ�������
		*/
        for (i = 0; i < rx_length; i++) 
		{
            // ����rx_buffer�е�����
			uint8_t ch = dma_usart2_rx_buffer[i];
			
			if(Uart2Gps.usRxWrite == 0)
			{
				if(ch == '$')//֡ͷ
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

				if(ch == '\n')//֡β
				{
					// �ص�����,֪ͨӦ�ó����յ�������,һ���Ƿ���1����Ϣ��������һ����� 
					if (Uart2Gps.ReciveNew)
					{
						Uart2Gps.ReciveNew(Uart2Gps.usRxWrite);
						Uart2Gps.usRxWrite = 0;
					}
				}
			}
        }
		
        // ��������DMA����
        DMA_Cmd(DMA1_Channel6, DISABLE);
        DMA_SetCurrDataCounter(DMA1_Channel6, DMA_BUFFER_SIZE);
        DMA_Cmd(DMA1_Channel6, ENABLE);
    }
	
	/* �������ͻ��������ж� */
	if(USART_GetITStatus(Uart2Gps.uart, USART_IT_TXE) != RESET)
	{
		if (Uart2Gps.usTxCount == 0)
		{
			/* ���ͻ�������������ȡ��ʱ�� ��ֹ���ͻ��������ж� ��ע�⣺��ʱ���1�����ݻ�δ����������ϣ�*/
			USART_ITConfig(Uart2Gps.uart, USART_IT_TXE, DISABLE);

			/* ʹ�����ݷ�������ж� */
			USART_ITConfig(Uart2Gps.uart, USART_IT_TC, ENABLE);
		}
		else
		{
			/* �ӷ���FIFOȡ1���ֽ�д�봮�ڷ������ݼĴ��� */
			USART_SendData(Uart2Gps.uart, Uart2Gps.pTxBuf[Uart2Gps.usTxRead]);
			if (++Uart2Gps.usTxRead >= Uart2Gps.usTxBufSize)
			{
				Uart2Gps.usTxRead = 0;
			}
			Uart2Gps.usTxCount--;
		}
	}
	/* ����bitλȫ��������ϵ��ж� */
	else if (USART_GetITStatus(Uart2Gps.uart, USART_IT_TC) != RESET)
	{
		if (Uart2Gps.usTxCount == 0)
		{
			/* �������FIFO������ȫ��������ϣ���ֹ���ݷ�������ж� */
			USART_ITConfig(Uart2Gps.uart, USART_IT_TC, DISABLE);

			/* �ص�����, һ����������RS485ͨ�ţ���RS485оƬ����Ϊ����ģʽ��������ռ���� */
			if (Uart2Gps.SendOver)
			{
				Uart2Gps.SendOver();
			}
		}
		else
		{
			/* ��������£��������˷�֧ */
			/* �������FIFO�����ݻ�δ��ϣ���ӷ���FIFOȡ1������д�뷢�����ݼĴ��� */
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
	//USART_IT_FE	 ֡���� (Framing error)
	//USART_FLAG_ORE ������� ���ش��� (Overrun error)
	//USART_FLAG_PE	 ��ż����� У����� (Parity error)
	//USART_FLAG_NE	 ���������־ (Noise error flag)
	//NE��ORT��FE	 ������־���໺��ͨ���е���������֡����
	if ((Uart2Gps.uart->SR & (USART_FLAG_PE|USART_FLAG_NE|USART_IT_FE|USART_FLAG_ORE)) != (uint16_t)RESET)//�����������
	{
		//USART_ClearFlag(_pUart->uart, USART_FLAG_ORE);
		USART_ReceiveData(Uart2Gps.uart);
	}
}

/*
*********************************************************************************************************
*	�� �� ��: USART1_IRQHandler
*	����˵��: USART�жϷ������
*	��    ��: ��
*	�� �� ֵ: ��
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
*	�� �� ��: fputc
*	����˵��: �ض���putc��������������ʹ��printf�����Ӵ���1��ӡ���
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
int fputc(int ch, FILE *f)
{
    UartSendChar(ch);
	return ch;
}

/*
*********************************************************************************************************
*	�� �� ��: fgetc
*	����˵��: �ض���getc��������������ʹ��getchar�����Ӵ���1��������
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
int fgetc(FILE *f)
{
	uint8_t ucData;
	while(UartGetChar(&ucData) == 0);
	return ucData;
}
