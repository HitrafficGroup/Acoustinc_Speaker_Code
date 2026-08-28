#ifndef __BSP_24L01_H
#define __BSP_24L01_H
					 					   	 
//-------------------------------------------------------------------------------------
//24L01�˿ڶ���
#define uchar unsigned char 
#define uint unsigned int

#define RF_IRQ()           (GPIOB->IDR & 0x1000) //esta asignado al pin 12 de B >> GPIOB, GPIO_Pin_12
#define	nRF24L01_CE_H	    GPIO_SetBits(GPIOB, GPIO_Pin_10)
#define	nRF24L01_CE_L	    GPIO_ResetBits(GPIOB, GPIO_Pin_10)
#define	nRF24L01_CSN_H	    GPIO_SetBits(GPIOB, GPIO_Pin_11)
#define	nRF24L01_CSN_L	    GPIO_ResetBits(GPIOB, GPIO_Pin_11)

/*
#define IRQ					RC0				//�ж�
#define IRQ_T				TRISC0			//�жϷ�������
#define nRF24L01_CE			RC5				//ʹ�ܿ���
#define nRF24L01_CE_T		TRISC5			//ʹ�ܿ��Ʒ�������
#define nRF24L01_CSN		RC4 			//Ƭѡ����
#define nRF24L01_CSN_T		TRISC4			//Ƭѡ���Ʒ�������
#define nRF24L01_MISO		RA2				//����1׼����
#define nRF24L01_MISO_T		TRISA2			//����1׼���÷�������
#define nRF24L01_SCK		RA0				//ʱ�����
#define nRF24L01_SCK_T		TRISA0			//ʱ�ӷ������
#define nRF24L01_MOSI		RA1				//�������
#define nRF24L01_MOSI_T		TRISA1			//���ݷ�������
*/
//*********************************************NRF24L01*************************************
#define TX_ADR_WIDTH    5   	// 5 uints TX address width
#define RX_ADR_WIDTH    5   	// 5 uints RX address width
#define TX_PLOAD_WIDTH  32  	// 20 uints TX payload
#define RX_PLOAD_WIDTH  32  	// 20 uints TX payload

#define MAX_TX  	0x10  //�ﵽ����ʹ����ж�
#define TX_OK   	0x20  //TX��������ж�
#define RX_OK   	0x40  //���յ������ж�
//****************************************************************//
// SPI(nRF24L01) commands
//***************************************NRF24L01�Ĵ���ָ��*******************************************************
#define RFREAD_REG      0x00  	// ���Ĵ���ָ��
#define RFWRITE_REG     0x20 	// д�Ĵ���ָ��
#define RD_RX_PLOAD     0x61  	// ��ȡ��������ָ��
#define WR_TX_PLOAD     0xA0  	// д��������ָ��
#define FLUSH_TX        0xE1 	// ��ϴ���� FIFOָ��
#define FLUSH_RX        0xE2  	// ��ϴ���� FIFOָ��
#define REUSE_TX_PL     0xE3  	// �����ظ�װ������ָ��
//*************************************SPI(nRF24L01)�Ĵ�����ַ****************************************************
#define CONFIG          0x00  // �����շ�״̬��CRCУ��ģʽ�Լ��շ�״̬��Ӧ��ʽ
#define EN_AA           0x01  // �Զ�Ӧ��������
#define EN_RXADDR       0x02  // �����ŵ�����
#define SETUP_AW        0x03  // �շ���ַ��������
#define SETUP_RETR      0x04  // �Զ��ط���������
#define RF_CH           0x05  // ����Ƶ������
#define RF_SETUP        0x06  // �������ʡ����Ĺ�������
#define STATUS          0x07  // ״̬�Ĵ���
#define OBSERVE_TX      0x08  // ���ͼ�⹦��
#define CD              0x09  // ��ַ���           
#define RX_ADDR_P0      0x0A  // Ƶ��0�������ݵ�ַ
#define RX_ADDR_P1      0x0B  // Ƶ��1�������ݵ�ַ
#define RX_ADDR_P2      0x0C  // Ƶ��2�������ݵ�ַ
#define RX_ADDR_P3      0x0D  // Ƶ��3�������ݵ�ַ
#define RX_ADDR_P4      0x0E  // Ƶ��4�������ݵ�ַ
#define RX_ADDR_P5      0x0F  // Ƶ��5�������ݵ�ַ
#define TX_ADDR         0x10  // ���͵�ַ�Ĵ���
#define RX_PW_P0        0x11  // ����Ƶ��0�������ݳ���
#define RX_PW_P1        0x12  // ����Ƶ��0�������ݳ���
#define RX_PW_P2        0x13  // ����Ƶ��0�������ݳ���
#define RX_PW_P3        0x14  // ����Ƶ��0�������ݳ���
#define RX_PW_P4        0x15  // ����Ƶ��0�������ݳ���
#define RX_PW_P5        0x16  // ����Ƶ��0�������ݳ���
#define FIFO_STATUS     0x17  // FIFOջ��ջ��״̬�Ĵ�������

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


