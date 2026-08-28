/* Controlador del bus SPI. */
/*
*********************************************************************************************************
*
*   v1.0    2014-10-24      wcx     
*
*********************************************************************************************************
*/

#include "stm32f10x.h"

#define SPI1_BAUD			SPI_BaudRatePrescaler_4
#define SPI2_BAUD			SPI_BaudRatePrescaler_32



/*
	PA5	    SPI1_SCK
	PA6	    SPI1_MISO
	PA7	    SPI1_MOSI
    
	PB13	SPI1_SCK
	PA14	SPI1_MISO
	PA15	SPI1_MOSI
*/


static void bsp_InitSpi1GPIO(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
    
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	/* PA5-SCK PA6-MISO PA7-MOSI */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
}

static void bsp_InitSpi2GPIO(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
    
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	/* PA5-SCK PA6-MISO PA7-MOSI */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
}

/*
*********************************************************************************************************
*********************************************************************************************************
*/
void bsp_InitSpi1Bus(void)
{
	SPI_InitTypeDef  SPI_InitStructure;
	
	bsp_InitSpi1GPIO();
	
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, DISABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);
	
	
	
	*/
	
	

	
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI1_BAUD;
	SPI_Init(SPI1, &SPI_InitStructure);

}

void bsp_InitSpi2Bus(void)
{
	SPI_InitTypeDef  SPI_InitStructure;
	
	bsp_InitSpi2GPIO();
	
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, DISABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);
	
	
	
	*/

	
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI2_BAUD;
	SPI_Init(SPI2, &SPI_InitStructure);

}
/*
*********************************************************************************************************
*********************************************************************************************************
*/
uint8_t Spi1_SendByte(uint8_t _ucValue)
{
	
	while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);

	
	SPI_I2S_SendData(SPI1, _ucValue);

	
	while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET);

	
	return SPI_I2S_ReceiveData(SPI1);
}

uint8_t Spi2_SendByte(uint8_t _ucValue)
{
	
	while(SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == RESET);

	
	SPI_I2S_SendData(SPI2, _ucValue);

	
	while(SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE) == RESET);

	
	return SPI_I2S_ReceiveData(SPI2);
}
