/*
*********************************************************************************************************
*
*
*********************************************************************************************************
*/

#ifndef _BSP_USART_FIFO_H_
#define _BSP_USART_FIFO_H_

#include <core_cm3.h>




#define UART1_BAUD			115200
#define UART1_TX_BUF_SIZE	256
#define UART1_RX_BUF_SIZE	256

#define UART2_BAUD			9600
#define UART2_TX_BUF_SIZE	128
#define UART2_RX_BUF_SIZE	2048


typedef struct
{

    __IO uint16_t SendOverCount;
    
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
