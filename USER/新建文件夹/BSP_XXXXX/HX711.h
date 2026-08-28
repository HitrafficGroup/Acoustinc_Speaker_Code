#ifndef __HX711_H
#define __HX711_H

#include "sys.h"

#define BUFFERMAX 1024
#define RXLENGTH 10
#define TXLENGTH 35

//#define CLR_ON GPIO_SetBits(GPIOB,GPIO_Pin_0)
//#define CLR_OFF GPIO_ResetBits(GPIOB,GPIO_Pin_0)

//#define OE595_ON GPIO_SetBits(GPIOB,GPIO_Pin_4)
//#define OE595_OFF GPIO_ResetBits(GPIOB,GPIO_Pin_4)

#define SER_595_ON GPIO_SetBits(GPIOB,GPIO_Pin_5)
#define SER_595_OFF GPIO_ResetBits(GPIOB,GPIO_Pin_5)

#define Latch_595_OFF GPIO_SetBits(GPIOD,GPIO_Pin_2)
#define Latch_595_ON GPIO_ResetBits(GPIOD,GPIO_Pin_2)

#define SRCLK_595_OFF GPIO_SetBits(GPIOB,GPIO_Pin_3)
#define SRCLK_595_ON GPIO_ResetBits(GPIOB,GPIO_Pin_3)


//定义变量

static u8 BufferWptr=0;
static u8 BufferRptr=0;
static u8 UsartBuffer[BUFFERMAX];
static u16 RxCounter = 0;
static u8  TEMPBUF[50];
//static u8 RxBuffer[RXLENGTH];
static u8 TxBuffer[TXLENGTH];
static char Relayvalue[4] ;



//定义函数
void delay(u32 nCount);
void RCC_Configuration(void);
void RCC_HSE_Configuration(void);
void GPIO_Configuration(void);
void USART_Configuration(void);
void NVIC_Configuration(void);
void delay_us(u32 nus);
void delay_ms(u16 nms);
void BufferWrite(void);
void BufferRead(void);
void processCommand(void);
void  RelaySet(int RelayIndex,int Value);
void Shift595(void);
u8 checksum_Compute(void);
int checksum_Test(void);
void RetMSG(void);
void HC595Config(void);
void SetRelayValue(int Relayindex,int value);


#endif
