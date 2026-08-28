#ifndef __USART_H
#define __USART_H
#include "stm32f10x.h"	
#include "stdio.h"	 

#define USART1_DR_Base  0x40013804
#define SENDBUFF_SIZE 7


void USART1_Configuration(void);
void USART2_Configuration(void);
extern uint8_t SendBuff[7];

//======================================



	#define UART1_TX_BUF_SIZE	254//1024
	#define UART1_RX_BUF_SIZE	254//1024






void bsp_InitUart(void);

void UartClearTxFifo(void);
void UartClearRxFifo(void);
void UartSend(uint8_t *_ucaBuf, uint16_t _usLen);
void modbus_crc16(unsigned char *puchMsg, unsigned short usDataLen) ; 

#endif	   
