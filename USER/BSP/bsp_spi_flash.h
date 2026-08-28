#ifndef _BSP_SPI_FLASH_H
#define _BSP_SPI_FLASH_H

#include "stm32f10x.h"
#include <stdio.h>

#define FLASH_SECTOR_SIZE   4096
#define FLASH_SECTOR_COUNT  1024    //4096-16M  1024-4M



enum
{
	SST25VF016B_ID = 0xBF2541,
	MX25L1606E_ID  = 0xC22015,
    W25Q16DV_ID    = 0xEF4015,
	W25Q32JV_ID    = 0xEF4016,
	W25Q32DV_ID    = 0xEF7016,
	W25Q64JV_ID    = 0xEF4017, /* BV, JV, FV */
	W25Q128DV_ID   = 0xEF4018,
    //0xD84016
};

typedef struct
{
	uint32_t ChipID;
	char ChipName[20];
	uint32_t TotalSize;
	uint32_t PageSize;
}SFLASH_T;


#define DEC_to_BCD(x)   ((((x)/10)<<4)+((x)%10))
#define BCD_to_DEC(x)   ((((x)>>4)*10)+((x)&0x0f))


void sf_ReadInfo(void);
void bsp_InitSpiFlash(void);
uint32_t sf_ReadID(void);
void sf_EraseChip(void);
void sf_EraseSector(uint32_t _uiSectorAddr);
void sf_PageWrite(uint8_t * _pBuf, uint32_t _uiWriteAddr, uint16_t _usSize);
uint8_t sf_WriteBuffer(uint8_t* _pBuf, uint32_t _uiWriteAddr, uint16_t _usWriteSize);
void sf_ReadBuffer(uint8_t * _pBuf, uint32_t _uiReadAddr, uint32_t _uiSize);
extern SFLASH_T g_tSF;


#endif
