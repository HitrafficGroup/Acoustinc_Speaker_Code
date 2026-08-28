/*
*********************************************************************************************************
*
*	模块名称 : SPI总线驱动
*	文件名称 : bsp_spi_bus.h
*	版    本 : V1.0
*	说    明 : 头文件
*
*********************************************************************************************************
*/

#ifndef _BSP_SPI_BUS_H
#define _BSP_SPI_BUS_H

#include <stdint.h>


void bsp_InitSpi1Bus(void);
void bsp_InitSpi2Bus(void);
uint8_t Spi1_SendByte(uint8_t _ucValue);
uint8_t Spi2_SendByte(uint8_t _ucValue);

#endif
