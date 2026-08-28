

#ifndef _BSP_USART_FIFO_H_
#define _BSP_USART_FIFO_H_

#include <core_cm3.h>



/* Define la velocidad de comunicacion UART1_BAUD. */
#define UART1_BAUD			115200
#define UART1_TX_BUF_SIZE	256
#define UART1_RX_BUF_SIZE	256

#define UART2_BAUD			9600
#define UART2_TX_BUF_SIZE	128
#define UART2_RX_BUF_SIZE	2048


typedef struct
{
	USART_TypeDef *uart;
	uint8_t *pTxBuf;
	uint8_t *pRxBuf;
	uint16_t usTxBufSize;
	uint16_t usRxBufSize;
	__IO uint16_t usTxWrite;
	__IO uint16_t usTxRead;
	__IO uint16_t usTxCount;

	__IO uint16_t usRxWrite;
	__IO uint16_t usRxRead;
	__IO uint16_t usRxCount;
    __IO uint16_t SendOverCount;
    
	void (*SendBefor)(void);
	void (*SendOver)(void);
	void (*ReciveNew)(uint16_t _byte);
}UART_T;


typedef struct
{
    uint8_t  mi_mode;

}SHOW;

extern UART_T g_tUart1;


void bsp_InitUart(void);
void UartClearTxFifo(void);
void UartClearRxFifo(void);
void UartSendBuf(uint8_t *_ucaBuf, uint16_t _usLen);
uint8_t UartGetChar(uint8_t *_pByte);
void printf_fifo_hex(uint8_t* tx, uint8_t len);



#endif
