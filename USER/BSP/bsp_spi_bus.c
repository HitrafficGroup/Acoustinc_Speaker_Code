/*
*********************************************************************************************************
*
*	模块名称 : SPI总线驱动
*	文件名称 : bsp_spi_bus.h
*	版    本 : V1.0
*	说    明 : SPI总线底层驱动。提供SPI配置、收发数据、多设备共享SPI支持。
*	修改记录 :
*		版本号  日期        作者    说明
*   v1.0    2014-10-24      wcx     
*
*********************************************************************************************************
*/

#include "stm32f10x.h"

#define SPI1_BAUD			SPI_BaudRatePrescaler_4
#define SPI2_BAUD			SPI_BaudRatePrescaler_32



/*
	SPI1 口线分配
	PA5	    SPI1_SCK
	PA6	    SPI1_MISO
	PA7	    SPI1_MOSI
    
	SPI2 口线分配
	PB13	SPI1_SCK
	PA14	SPI1_MISO
	PA15	SPI1_MOSI
*/

uint8_t g_spi_busy = 0;		/* SPI 总线共享标志 */

static void bsp_InitSpi1GPIO(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
    
	/* 使能GPIO 时钟 */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	/* PA5-SCK PA6-MISO PA7-MOSI */
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	    /* IO口最大速度 */
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;		    /* 设为输出口 */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
}

static void bsp_InitSpi2GPIO(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
    
	/* 使能GPIO 时钟 */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	/* PA5-SCK PA6-MISO PA7-MOSI */
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	    /* IO口最大速度 */
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;		    /* 设为输出口 */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
}

/*
*********************************************************************************************************
*	函 数 名: bsp_InitSPIBus
*	功能说明: 配置STM32内部SPI硬件的工作模式、速度等参数，用于访问SPI接口的串行Flash。只包括 SCK、 MOSI、 MISO口线的配置。不包括片选CS，也不包括外设芯片特有的INT、BUSY等
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
void bsp_InitSpi1Bus(void)
{
	SPI_InitTypeDef  SPI_InitStructure;
	
	bsp_InitSpi1GPIO();
	
	/* 打开SPI时钟 */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, DISABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);
	
	/* 配置SPI硬件参数 */
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;	/* 数据方向：2线全双工 */
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;		/* STM32的SPI工作模式 ：主机模式 */
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;	/* 数据位长度 ： 8位 */
	/* SPI_CPOL和SPI_CPHA结合使用决定时钟和数据采样点的相位关系、
       配置: 总线空闲是高电平,第2个边沿（上升沿采样数据)
	*/
	//SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;			/* CLK引脚空闲状态电平 = 0 */
	//SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;		/* 数据采样在第1个边沿(上升沿) */
    SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;			/* 时钟上升沿采样数据 */
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;		/* 时钟的第2个边沿采样数据 */
    SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;			/* 片选控制方式：软件控制 */

	/* 设置波特率预分频系数 */
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI1_BAUD;
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;	/* 数据位传输次序：高位先传 */
	SPI_InitStructure.SPI_CRCPolynomial = 7;			/* CRC多项式寄存器，复位后为7 */
	SPI_Init(SPI1, &SPI_InitStructure);

	SPI_Cmd(SPI1, DISABLE);			/* 先禁止SPI  */
	SPI_Cmd(SPI1, ENABLE);			/* 使能SPI  */
}

void bsp_InitSpi2Bus(void)
{
	SPI_InitTypeDef  SPI_InitStructure;
	
	bsp_InitSpi2GPIO();
	
	/* 打开SPI时钟 */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, DISABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);
	
	/* 配置SPI硬件参数 */
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;	/* 数据方向：2线全双工 */
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;		/* STM32的SPI工作模式 ：主机模式 */
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;	/* 数据位长度 ： 8位 */
	/* SPI_CPOL和SPI_CPHA结合使用决定时钟和数据采样点的相位关系、
	   配置: 总线空闲是高电平,第2个边沿（上升沿采样数据)
	*/
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;			/* CLK引脚空闲状态电平 = 0 */
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;		/* 数据采样在第1个边沿(上升沿) */
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;			/* 片选控制方式：软件控制 */

	/* 设置波特率预分频系数 */
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI2_BAUD;
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;	/* 数据位传输次序：高位先传 */
	SPI_InitStructure.SPI_CRCPolynomial = 7;			/* CRC多项式寄存器，复位后为7 */
	SPI_Init(SPI2, &SPI_InitStructure);

	SPI_Cmd(SPI2, DISABLE);			/* 先禁止SPI  */
	SPI_Cmd(SPI2, ENABLE);			/* 使能SPI  */
}
/*
*********************************************************************************************************
*	函 数 名: Spi1_SendByte
*	功能说明: 向器件发送一个字节，同时从MISO口线采样器件返回的数据
*	形    参:  _ucByte : 发送的字节值
*	返 回 值: 从MISO口线采样器件返回的数据
*********************************************************************************************************
*/
uint8_t Spi1_SendByte(uint8_t _ucValue)
{
	/* 等待上个数据未发送完毕 */
	while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);

	/* 通过SPI硬件发送1个字节 */
	SPI_I2S_SendData(SPI1, _ucValue);

	/* 等待接收一个字节任务完成 */
	while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET);

	/* 返回从SPI总线读到的数据 */
	return SPI_I2S_ReceiveData(SPI1);
}

uint8_t Spi2_SendByte(uint8_t _ucValue)
{
	/* 等待上个数据未发送完毕 */
	while(SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == RESET);

	/* 通过SPI硬件发送1个字节 */
	SPI_I2S_SendData(SPI2, _ucValue);

	/* 等待接收一个字节任务完成 */
	while(SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE) == RESET);

	/* 返回从SPI总线读到的数据 */
	return SPI_I2S_ReceiveData(SPI2);
}
