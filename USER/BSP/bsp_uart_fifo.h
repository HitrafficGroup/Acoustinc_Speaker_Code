/*
*********************************************************************************************************
*
*	模块名称 : 串口中断+FIFO驱动模块
*	文件名称 : bsp_uart_fifo.h
*	版    本 : V1.0
*	说    明 : 头文件
*
*********************************************************************************************************
*/

#ifndef _BSP_USART_FIFO_H_
#define _BSP_USART_FIFO_H_

#include <core_cm3.h>



/* 定义串口波特率和FIFO缓冲区大小，分为发送缓冲区和接收缓冲区, 支持全双工 */
#define UART1_BAUD			115200
#define UART1_TX_BUF_SIZE	256
#define UART1_RX_BUF_SIZE	256

#define UART2_BAUD			9600
#define UART2_TX_BUF_SIZE	128
#define UART2_RX_BUF_SIZE	2048

/* 串口设备结构体 */
typedef struct
{
	USART_TypeDef *uart;		/* STM32内部串口设备指针 */
	uint8_t *pTxBuf;			/* 发送缓冲区 */
	uint8_t *pRxBuf;			/* 接收缓冲区 */
	uint16_t usTxBufSize;		/* 发送缓冲区大小 */
	uint16_t usRxBufSize;		/* 接收缓冲区大小 */
	__IO uint16_t usTxWrite;	/* 发送缓冲区写指针 */
	__IO uint16_t usTxRead;		/* 发送缓冲区读指针 */
	__IO uint16_t usTxCount;	/* 等待发送的数据个数 */

	__IO uint16_t usRxWrite;	/* 接收缓冲区写指针 */
	__IO uint16_t usRxRead;		/* 接收缓冲区读指针 */
	__IO uint16_t usRxCount;	/* 还未读取的新数据个数 */
    __IO uint16_t SendOverCount;
    
	void (*SendBefor)(void); 	/* 开始发送之前的回调函数指针（主要用于RS485切换到发送模式） */
	void (*SendOver)(void); 	/* 发送完毕的回调函数指针（主要用于RS485将发送模式切换为接收模式） */
	void (*ReciveNew)(uint16_t _byte);	/* 串口收到数据的回调函数指针 */
}UART_T;


typedef struct
{
    uint8_t  mi_mode;    //0灭，1亮，2闪

}SHOW;

extern UART_T g_tUart1;


void bsp_InitUart(void);
void UartClearTxFifo(void);
void UartClearRxFifo(void);
void UartSendBuf(uint8_t *_ucaBuf, uint16_t _usLen);
uint8_t UartGetChar(uint8_t *_pByte);
void printf_fifo_hex(uint8_t* tx, uint8_t len);



#endif
