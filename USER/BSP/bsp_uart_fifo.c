

#include "stm32f10x.h"
#include "uart_fifo_gps.h"

static uint8_t g_TxBuf1[UART1_TX_BUF_SIZE];
static uint8_t g_RxBuf1[UART1_RX_BUF_SIZE];

UART_T g_tUart1;
UART_T Uart2Gps;
static uint8_t g_TxBuf2[UART2_TX_BUF_SIZE];
static uint8_t g_RxBuf2[UART2_RX_BUF_SIZE];

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


void bsp_InitUart(void)
{
	UartVarInit();
	InitHardUart();
	DMA_Configuration();
	ConfigUartNVIC();
}


void UartClearTxFifo(void)
{
	g_tUart1.usTxWrite = 0;
	g_tUart1.usTxRead = 0;
	g_tUart1.usTxCount = 0;
}


void UartClearRxFifo(void)
{
	g_tUart1.usRxWrite = 0;
	g_tUart1.usRxRead = 0;
	g_tUart1.usRxCount = 0;
}


static void UartVarInit(void)
{
	g_tUart1.uart = USART1;
	g_tUart1.pTxBuf = g_TxBuf1;
	g_tUart1.pRxBuf = g_RxBuf1;
	g_tUart1.usTxBufSize = UART1_TX_BUF_SIZE;
	g_tUart1.usRxBufSize = UART1_RX_BUF_SIZE;
	g_tUart1.usTxWrite = 0;
	g_tUart1.usTxRead = 0;
	g_tUart1.usRxWrite = 0;
	g_tUart1.usRxRead = 0;
	g_tUart1.usRxCount = 0;
	g_tUart1.usTxCount = 0;
	g_tUart1.SendBefor = 0;
	g_tUart1.SendOver = 0;
	g_tUart1.ReciveNew = 0;
	
	Uart2Gps.uart = USART2;
	Uart2Gps.pTxBuf = g_TxBuf2;
	Uart2Gps.pRxBuf = g_RxBuf2;
	Uart2Gps.usTxBufSize = UART2_TX_BUF_SIZE;
	Uart2Gps.usRxBufSize = UART2_RX_BUF_SIZE;
	Uart2Gps.usTxWrite = 0;
	Uart2Gps.usTxRead = 0;
	Uart2Gps.usRxWrite = 0;
	Uart2Gps.usRxRead = 0;
	Uart2Gps.usRxCount = 0;
	Uart2Gps.usTxCount = 0;
	Uart2Gps.SendBefor = 0;
	Uart2Gps.SendOver = 0;
	Uart2Gps.ReciveNew = Gps_ReciveNew;
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

	USART_InitStructure.USART_BaudRate = UART1_BAUD;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No ;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USART1, &USART_InitStructure);

	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);

	USART_Cmd(USART1, ENABLE);
	USART_ClearFlag(USART1, USART_FLAG_TC);
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

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	GPIO_Init(GPIOA, &GPIO_InitStructure);

	USART_InitStructure.USART_BaudRate = UART2_BAUD;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No ;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
	USART_Init(USART2, &USART_InitStructure);

	USART_DMACmd(USART2, USART_DMAReq_Rx, ENABLE);
	USART_ITConfig(USART2, USART_IT_IDLE, ENABLE);

	USART_Cmd(USART2, ENABLE);

	USART_ClearFlag(USART2, USART_FLAG_TC);
}


static void InitHardUart(void)
{
	Init_Uart1();
	Init_Uart2();
}


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


void UartSendBuf(uint8_t *_ucaBuf, uint16_t _usLen)
{
	uint16_t i;
    
	if(g_tUart1.SendBefor != 0)
	{
		g_tUart1.SendBefor();
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


uint8_t UartGetChar(uint8_t *_pByte)
{
	uint16_t usCount;

	DISABLE_INT();
	usCount = g_tUart1.usRxCount;
	ENABLE_INT();

	//if (_g_tUart1.usRxRead == usRxWrite)
	if (usCount == 0)
	{
		return 0;
	}
	else
	{
		*_pByte = g_tUart1.pRxBuf[g_tUart1.usRxRead];

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
//		if(Uart2Gps.usRxCount < Uart2Gps.usRxBufSize)
//		{
//			Uart2Gps.usRxCount++;
//		}
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


int fputc(int ch, FILE *f)
{
    UartSendChar(ch);
	return ch;
}


int fgetc(FILE *f)
{
	uint8_t ucData;
	while(UartGetChar(&ucData) == 0);
	return ucData;
}
