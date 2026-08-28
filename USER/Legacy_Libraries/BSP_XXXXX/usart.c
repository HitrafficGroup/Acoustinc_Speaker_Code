#include "usart.h"	
#include "stdio.h"

uint8_t SendBuff[7];

static void UartVarInit(void);
void UartSend(uint8_t *_ucaBuf, uint16_t _usLen);
uint8_t UartGetChar(uint8_t *_pByte);
static void ConfigUartNVIC(void);

/*
*********************************************************************************************************
*********************************************************************************************************
*/
void bsp_InitUart(void)
{
	USART1_Configuration();
}

/*
*********************************************************************************************************
*********************************************************************************************************
*/
void UartClearTxFifo(void)
{
	usTxWrite = 0;
	usTxRead = 0;
	usTxCount = 0;
}

/*
*********************************************************************************************************
*********************************************************************************************************
*/
void UartClearRxFifo(void)
{
	usRxWrite = 0;
	usRxRead = 0;
	usRxCount = 0;
}

/*
*********************************************************************************************************
*********************************************************************************************************
*/
static void UartVarInit(void)
{
}


/**
  */
void USART1_Configuration(void)
{
	USART_InitTypeDef USART_InitStructure;
	
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 | RCC_APB2Periph_GPIOA, ENABLE);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	USART_InitStructure.USART_BaudRate = 9600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USART1, &USART_InitStructure);

    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
	USART_Cmd(USART1, ENABLE);

    USART_ClearFlag(USART1, USART_FLAG_TC);
}

void USART2_Configuration(void)
{
	USART_InitTypeDef USART_InitStructure;
	
    GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2 , ENABLE);
    RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA, ENABLE);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	//GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	//GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	//GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	USART_InitStructure.USART_BaudRate = 9600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USART2, &USART_InitStructure);

  //USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);

	USART_Cmd(USART2, ENABLE);

  
}

void GpuSend(char * buf1)
{	u8 i=0;
	while (1)
	{	if (buf1[i]!=0)
		{	USART_SendData(USART2, buf1[i]);
			while(USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET){}; 
			i++;
		}
		else return;
	}
}
/*
*********************************************************************************************************
*********************************************************************************************************
*/
static void ConfigUartNVIC(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;

	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

/*
*********************************************************************************************************
*********************************************************************************************************
*/
void UartSend(uint8_t *_ucaBuf, uint16_t _usLen)
{
	uint16_t i;

	for (i = 0; i < _usLen; i++)
	{
        USART_SendData(USART1, _ucaBuf[i]);
        while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET){};
	}
}

/*
*********************************************************************************************************
*********************************************************************************************************
*/
uint8_t UartGetChar(uint8_t *_pByte)
{
	uint16_t usCount;

	DISABLE_INT();
	usCount =usRxCount;
	ENABLE_INT();

	//if (usRxRead == usRxWrite)
	{
		return 0;
	}
	else
	{

		DISABLE_INT();
		if (++usRxRead >= usRxBufSize)
		{
			usRxRead = 0;
		}
		usRxCount--;
		ENABLE_INT();
		return 1;
	}
}
/*
*********************************************************************************************************
*********************************************************************************************************
*/
extern u8 Usart_CRC;
extern u8 robot;
extern u8 SSize;
extern u8 CRCH;
extern u8 CRCL;
extern u8 mast;
volatile uint8_t ch, receive_data_flag,receive_CRC_flag;
volatile uint8_t receive_data_flag = 0;
volatile uint8_t receive_CRC_flag  =0;

void USART1_IRQHandler(void)
{
	uint8_t i;
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
	{
		ch = USART_ReceiveData(USART1);
        if(ch == 0XB1)
				{
					  if(usRxWrite > 0)
				    {
							usRxWrite++;
				    }
						else
					  {
							SSize=mast;
				    }
			  }
				else
				{
					usRxWrite++;
				}
        pRxBuf[usRxWrite] = ch;
        if(usRxWrite == 3)
				{
						if(pRxBuf[1] == robot)
						{
							 if(pRxBuf[2] == 0X80)
							 {
								SSize=6;
							 }
							 if(pRxBuf[2] == 0X81)
							 {
								SSize=15;
							 }
							 if(pRxBuf[2] == 0X82)
							 {
								SSize=6+pRxBuf[3];
							 }

						}
				}
        if(usRxWrite == SSize-1)receive_CRC_flag = 1;
				if(SSize>mast)receive_CRC_flag = 1	;	
	      //=============================================
				if(receive_CRC_flag)
        {
					receive_CRC_flag=0;
					modbus_crc16(pRxBuf, usRxWrite-2);
					if
					 (	\
		        (CRCL==pRxBuf[usRxWrite-1]) && \
		        (CRCH==pRxBuf[usRxWrite]) 
		       )
          {
           Usart_CRC=1;
				  }
					else
					{
					 Usart_CRC=2;
						
					}					
	        for (i = SSize; i < 20; i++)
	         {
	           pRxBuf[i]=0;
	         }
						SSize=mast;
						usRxWrite = 0;
        }
	}
    if(USART_GetITStatus(USART1, USART_IT_TXE) != RESET)
	{
        USART_ITConfig(USART1, USART_IT_TXE, DISABLE);

        USART_ITConfig(USART1, USART_IT_TC, ENABLE);
    }

//	if(USART_GetITStatus(USART1, USART_IT_TXE) != RESET)
//	{
//		if (usTxCount == 0)
//		{

//			USART_ITConfig(USART1, USART_IT_TXE, DISABLE);


//			USART_ITConfig(USART1, USART_IT_TC, ENABLE);
//		}
//		else
//		{

//			USART_SendData(USART1, pTxBuf[usTxRead]);
//			if (++usTxRead >= usTxBufSize)
//			{
//				usTxRead = 0;
//			}
//			usTxCount--;
//		}
//	}

//	else if (USART_GetITStatus(USART1, USART_IT_TC) != RESET)
//	{
//		if (usTxCount == 0)
//		{

//			USART_ITConfig(USART1, USART_IT_TC, DISABLE);
//		}
//		else
//		{
//			USART_SendData(USART1, pTxBuf[usTxRead]);
//			if (++usTxRead >= usTxBufSize)
//			{
//				usTxRead = 0;
//			}
//			usTxCount--;
//		}
//	}

}

//===================================CRC16===========================
const uint8_t auchCRCHi[] = {
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 
0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 
0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 
0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 
0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 
0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 
0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 
0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 
0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 
0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 
0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 
0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 
0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 
0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 
0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 
0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 
0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 
0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 
0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 
0x80, 0x41, 0x00, 0xC1, 0x81, 0x40
};

const uint8_t auchCRCLo[] = {
0x00, 0xC0, 0xC1, 0x01, 0xC3, 0x03, 0x02, 0xC2, 0xC6, 0x06, 
0x07, 0xC7, 0x05, 0xC5, 0xC4, 0x04, 0xCC, 0x0C, 0x0D, 0xCD, 
0x0F, 0xCF, 0xCE, 0x0E, 0x0A, 0xCA, 0xCB, 0x0B, 0xC9, 0x09, 
0x08, 0xC8, 0xD8, 0x18, 0x19, 0xD9, 0x1B, 0xDB, 0xDA, 0x1A, 
0x1E, 0xDE, 0xDF, 0x1F, 0xDD, 0x1D, 0x1C, 0xDC, 0x14, 0xD4, 
0xD5, 0x15, 0xD7, 0x17, 0x16, 0xD6, 0xD2, 0x12, 0x13, 0xD3, 
0x11, 0xD1, 0xD0, 0x10, 0xF0, 0x30, 0x31, 0xF1, 0x33, 0xF3, 
0xF2, 0x32, 0x36, 0xF6, 0xF7, 0x37, 0xF5, 0x35, 0x34, 0xF4, 
0x3C, 0xFC, 0xFD, 0x3D, 0xFF, 0x3F, 0x3E, 0xFE, 0xFA, 0x3A, 
0x3B, 0xFB, 0x39, 0xF9, 0xF8, 0x38, 0x28, 0xE8, 0xE9, 0x29, 
0xEB, 0x2B, 0x2A, 0xEA, 0xEE, 0x2E, 0x2F, 0xEF, 0x2D, 0xED, 
0xEC, 0x2C, 0xE4, 0x24, 0x25, 0xE5, 0x27, 0xE7, 0xE6, 0x26, 
0x22, 0xE2, 0xE3, 0x23, 0xE1, 0x21, 0x20, 0xE0, 0xA0, 0x60, 
0x61, 0xA1, 0x63, 0xA3, 0xA2, 0x62, 0x66, 0xA6, 0xA7, 0x67, 
0xA5, 0x65, 0x64, 0xA4, 0x6C, 0xAC, 0xAD, 0x6D, 0xAF, 0x6F, 
0x6E, 0xAE, 0xAA, 0x6A, 0x6B, 0xAB, 0x69, 0xA9, 0xA8, 0x68, 
0x78, 0xB8, 0xB9, 0x79, 0xBB, 0x7B, 0x7A, 0xBA, 0xBE, 0x7E, 
0x7F, 0xBF, 0x7D, 0xBD, 0xBC, 0x7C, 0xB4, 0x74, 0x75, 0xB5, 
0x77, 0xB7, 0xB6, 0x76, 0x72, 0xB2, 0xB3, 0x73, 0xB1, 0x71, 
0x70, 0xB0, 0x50, 0x90, 0x91, 0x51, 0x93, 0x53, 0x52, 0x92, 
0x96, 0x56, 0x57, 0x97, 0x55, 0x95, 0x94, 0x54, 0x9C, 0x5C, 
0x5D, 0x9D, 0x5F, 0x9F, 0x9E, 0x5E, 0x5A, 0x9A, 0x9B, 0x5B, 
0x99, 0x59, 0x58, 0x98, 0x88, 0x48, 0x49, 0x89, 0x4B, 0x8B, 
0x8A, 0x4A, 0x4E, 0x8E, 0x8F, 0x4F, 0x8D, 0x4D, 0x4C, 0x8C, 
0x44, 0x84, 0x85, 0x45, 0x87, 0x47, 0x46, 0x86, 0x82, 0x42, 
0x43, 0x83, 0x41, 0x81, 0x80, 0x40
};

void modbus_crc16(unsigned char *puchMsg, unsigned short usDataLen) 
{ 
	{ 
		CRCH = uchCRCLo ^ auchCRCHi[uIndex] ; 
		CRCL = auchCRCLo[uIndex] ; 
	} 
}

int fputc(int ch, FILE *f)
{
	USART_SendData(USART1, (uint8_t) ch);
	while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);
	return ch;
}

int fgetc(FILE *f)
{
	while (USART_GetFlagStatus(USART1, USART_FLAG_RXNE) == RESET);
	return (int)USART_ReceiveData(USART1);
}
