#ifndef __BSP_24L01_H
#define __BSP_24L01_H
					 					   	 
//-------------------------------------------------------------------------------------

#define uchar unsigned char 
#define uint unsigned int

#define RF_IRQ()           (GPIOB->IDR & 0x1000) //esta asignado al pin 12 de B >> GPIOB, GPIO_Pin_12
#define	nRF24L01_CE_H	    GPIO_SetBits(GPIOB, GPIO_Pin_10)
#define	nRF24L01_CE_L	    GPIO_ResetBits(GPIOB, GPIO_Pin_10)
#define	nRF24L01_CSN_H	    GPIO_SetBits(GPIOB, GPIO_Pin_11)
#define	nRF24L01_CSN_L	    GPIO_ResetBits(GPIOB, GPIO_Pin_11)


//*********************************************NRF24L01*************************************
#define TX_ADR_WIDTH    5   	// 5 uints TX address width
#define RX_ADR_WIDTH    5   	// 5 uints RX address width
#define TX_PLOAD_WIDTH  32  	// 20 uints TX payload
#define RX_PLOAD_WIDTH  32  	// 20 uints TX payload

#define MAX_TX  	0x10
#define TX_OK   	0x20
#define RX_OK   	0x40
//****************************************************************//
// SPI(nRF24L01) commands

#define RFREAD_REG      0x00
#define RFWRITE_REG     0x20
#define RD_RX_PLOAD     0x61
#define WR_TX_PLOAD     0xA0
#define FLUSH_TX        0xE1
#define FLUSH_RX        0xE2
#define REUSE_TX_PL     0xE3

#define CONFIG          0x00
#define EN_AA           0x01
#define EN_RXADDR       0x02
#define SETUP_AW        0x03
#define SETUP_RETR      0x04
#define RF_CH           0x05
#define RF_SETUP        0x06
#define STATUS          0x07
#define OBSERVE_TX      0x08
#define CD              0x09
#define RX_ADDR_P0      0x0A
#define RX_ADDR_P1      0x0B
#define RX_ADDR_P2      0x0C
#define RX_ADDR_P3      0x0D
#define RX_ADDR_P4      0x0E
#define RX_ADDR_P5      0x0F
#define TX_ADDR         0x10
#define RX_PW_P0        0x11
#define RX_PW_P1        0x12
#define RX_PW_P2        0x13
#define RX_PW_P3        0x14
#define RX_PW_P4        0x15
#define RX_PW_P5        0x16
#define FIFO_STATUS     0x17

extern void RX_Mode(void);
extern void TX_Mode(void);
extern void nRF24L01ioConfig(void);//LCD initial IO.

extern u8 SPI_Write_Buf(u8 reg, u8 *pBuf, u8 nchars);
extern u8 SPI_Read_Buf(u8 reg, u8 *pBuf, u8 nchars);
extern u8 SPI_RW_Reg(u8 reg, u8 value);
extern u8 SPI_Read(u8 reg);
extern u8 nRF24L01_TxPacket(u8 * tx_buf);
extern u8 nRF24L01_RxPacket(u8 * rx_buf);
extern u8 SPI_RW(u8 byte);
void rf24l01_irq_init(void);
void rf24l01_rx_process(void);

#endif


