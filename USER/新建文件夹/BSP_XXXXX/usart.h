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
#define ENABLE_INT()	__set_PRIMASK(0)	/* 使能全局中断 */
#define DISABLE_INT()	__set_PRIMASK(1)	/* 禁止全局中断 */

/* 定义串口波特率和FIFO缓冲区大小，分为发送缓冲区和接收缓冲区, 支持全双工 */


	#define UART1_TX_BUF_SIZE	254//1024
	#define UART1_RX_BUF_SIZE	254//1024


extern uint8_t pTxBuf[20];			/* 发送缓冲区 */
extern uint8_t pRxBuf[20];			/* 接收缓冲区 */
extern	uint16_t usTxBufSize;		/* 发送缓冲区大小 */
extern	uint16_t usRxBufSize;		/* 接收缓冲区大小 */
extern	uint16_t usTxWrite;	/* 发送缓冲区写指针 */
extern	uint16_t usTxRead;		/* 发送缓冲区读指针 */
extern	uint16_t usTxCount;	/* 等待发送的数据个数 */

extern	uint16_t usRxWrite;	/* 接收缓冲区写指针 */
extern	uint16_t usRxRead;		/* 接收缓冲区读指针 */
extern	uint16_t usRxCount;	/* 还未读取的新数据个数 */


	static uint8_t g_TxBuf1[UART1_TX_BUF_SIZE];		/* 发送缓冲区 */
	static uint8_t g_RxBuf1[UART1_RX_BUF_SIZE];		/* 接收缓冲区 */

void bsp_InitUart(void);

void UartClearTxFifo(void);
void UartClearRxFifo(void);
void UartSend(uint8_t *_ucaBuf, uint16_t _usLen);
void modbus_crc16(unsigned char *puchMsg, unsigned short usDataLen) ; 

#endif	   
