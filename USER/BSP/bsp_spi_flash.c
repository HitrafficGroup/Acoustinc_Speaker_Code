/* Controlador de la memoria Flash externa. */
#include "stm32f10x.h"



    #define SF_CS_LOW()       GPIOA->BRR = GPIO_Pin_4

    #define SF_CS_HIGH()      GPIOA->BSRR = GPIO_Pin_4


#define CMD_AAI       0xAD  	/* Define el comando SPI CMD_AAI. */
#define CMD_DISWR	  0x04		/* Define el comando SPI CMD_DISWR. */
#define CMD_EWRSR	  0x50		/* Define el comando SPI CMD_EWRSR. */
#define CMD_WRSR      0x01  	/* Define el comando SPI CMD_WRSR. */
#define CMD_WREN      0x06		/* Define el comando SPI CMD_WREN. */
#define CMD_READ      0x03  	/* Define el comando SPI CMD_READ. */
#define CMD_RDSR      0x05		/* Define el comando SPI CMD_RDSR. */
#define CMD_RDID      0x9F		/* Define el comando SPI CMD_RDID. */
#define CMD_SE        0x20		/* Define el comando SPI CMD_SE. */
#define CMD_BE        0xC7		/* Define el comando SPI CMD_BE. */
#define DUMMY_BYTE    0xA5

#define WIP_FLAG      0x01

SFLASH_T g_tSF;

void sf_ReadInfo(void);
static void sf_WriteEnable(void);
static void sf_WriteStatus(uint8_t _ucValue);
static void sf_WaitForWriteEnd(void);
static uint8_t sf_NeedErase(uint8_t * _ucpOldBuf, uint8_t *_ucpNewBuf, uint16_t _uiLen);
static uint8_t sf_CmpData(uint32_t _uiSrcAddr, uint8_t *_ucpTar, uint32_t _uiSize);
static uint8_t sf_AutoWritePage(uint8_t *_ucpSrc, uint32_t _uiWrAddr, uint16_t _usWrLen);

static uint8_t s_spiBuf[4096];


void bsp_InitSpiFlash(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;


	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);


	SF_CS_HIGH();
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	sf_ReadInfo();				

	SF_CS_LOW();
	Spi1_SendByte(CMD_DISWR);
	SF_CS_HIGH();

	sf_WaitForWriteEnd();		

	sf_WriteStatus(0);			
}


void sf_EraseSector(uint32_t _uiSectorAddr)
{
	sf_WriteEnable();								


	SF_CS_LOW();
	Spi1_SendByte(CMD_SE);
	Spi1_SendByte((_uiSectorAddr & 0xFF0000) >> 16);
	Spi1_SendByte((_uiSectorAddr & 0xFF00) >> 8);
	Spi1_SendByte(_uiSectorAddr & 0xFF);
	SF_CS_HIGH();

	sf_WaitForWriteEnd();							
}


void sf_EraseChip(void)
{
	sf_WriteEnable();								


	SF_CS_LOW();
	Spi1_SendByte(CMD_BE);
	SF_CS_HIGH();

	sf_WaitForWriteEnd();							
}


void sf_PageWrite(uint8_t * _pBuf, uint32_t _uiWriteAddr, uint16_t _usSize)
{
	uint32_t i, j;

	for (j = 0; j < _usSize / 256; j++)
	{
		sf_WriteEnable();								

		SF_CS_LOW();
		Spi1_SendByte(0x02);
		Spi1_SendByte((_uiWriteAddr & 0xFF0000) >> 16);
		Spi1_SendByte((_uiWriteAddr & 0xFF00) >> 8);
		Spi1_SendByte(_uiWriteAddr & 0xFF);

		for (i = 0; i < 256; i++)
		{
			Spi1_SendByte(*_pBuf++);
		}

		SF_CS_HIGH();

		sf_WaitForWriteEnd();						

		_uiWriteAddr += 256;
	}


	SF_CS_LOW();
	Spi1_SendByte(CMD_DISWR);
	SF_CS_HIGH();

	sf_WaitForWriteEnd();							
}


void sf_ReadBuffer(uint8_t * _pBuf, uint32_t _uiReadAddr, uint32_t _uiSize)
{

	if ((_uiSize == 0) ||(_uiReadAddr + _uiSize) > g_tSF.TotalSize)
	{
		return;
	}


	SF_CS_LOW();
	Spi1_SendByte(CMD_READ);
	Spi1_SendByte((_uiReadAddr & 0xFF0000) >> 16);
	Spi1_SendByte((_uiReadAddr & 0xFF00) >> 8);
	Spi1_SendByte(_uiReadAddr & 0xFF);
	while (_uiSize--)
	{
		*_pBuf++ = Spi1_SendByte(DUMMY_BYTE);
	}
	SF_CS_HIGH();
}


static uint8_t sf_CmpData(uint32_t _uiSrcAddr, uint8_t *_ucpTar, uint32_t _uiSize)
{
	uint8_t ucValue;


	if ((_uiSrcAddr + _uiSize) > g_tSF.TotalSize)
	{
		return 1;
	}

	if (_uiSize == 0)
	{
		return 0;
	}

	SF_CS_LOW();
	Spi1_SendByte(CMD_READ);
	Spi1_SendByte((_uiSrcAddr & 0xFF0000) >> 16);
	Spi1_SendByte((_uiSrcAddr & 0xFF00) >> 8);
	Spi1_SendByte(_uiSrcAddr & 0xFF);
	while (_uiSize--)
	{

		ucValue = Spi1_SendByte(DUMMY_BYTE);
		if (*_ucpTar++ != ucValue)
		{
			SF_CS_HIGH();
			return 1;
		}
	}
	SF_CS_HIGH();
	return 0;
}


static uint8_t sf_NeedErase(uint8_t * _ucpOldBuf, uint8_t *_ucpNewBuf, uint16_t _usLen)
{
	uint16_t i;
	uint8_t ucOld;



	for (i = 0; i < _usLen; i++)
	{
		ucOld = *_ucpOldBuf++;
		ucOld = ~ucOld;


		if ((ucOld & (*_ucpNewBuf++)) != 0)
		{
			return 1;
		}
	}
	return 0;
}


static uint8_t sf_AutoWritePage(uint8_t *_ucpSrc, uint32_t _uiWrAddr, uint16_t _usWrLen)
{
	uint16_t i;
	uint16_t j;
	uint32_t uiFirstAddr;
	uint8_t ucNeedErase;
	uint8_t cRet;


	if (_usWrLen == 0)
	{
		return 1;
	}


	if (_uiWrAddr >= g_tSF.TotalSize)
	{
		return 0;
	}


	if (_usWrLen > g_tSF.PageSize)
	{
		return 0;
	}

	
	sf_ReadBuffer(s_spiBuf, _uiWrAddr, _usWrLen);
	if (memcmp(s_spiBuf, _ucpSrc, _usWrLen) == 0)
	{
		return 1;
	}



	ucNeedErase = 0;
	if (sf_NeedErase(s_spiBuf, _ucpSrc, _usWrLen))
	{
		ucNeedErase = 1;
	}

	uiFirstAddr = _uiWrAddr & (~(g_tSF.PageSize - 1));

	if (_usWrLen == g_tSF.PageSize)
	{
		for	(i = 0; i < g_tSF.PageSize; i++)
		{
			s_spiBuf[i] = _ucpSrc[i];
		}
	}
	else
	{
		
		sf_ReadBuffer(s_spiBuf, uiFirstAddr, g_tSF.PageSize);


		i = _uiWrAddr & (g_tSF.PageSize - 1);
		memcpy(&s_spiBuf[i], _ucpSrc, _usWrLen);
	}


	cRet = 0;
	for (i = 0; i < 3; i++)
	{

		if (ucNeedErase == 1)
		{
			sf_EraseSector(uiFirstAddr);		
		}

		
		sf_PageWrite(s_spiBuf, uiFirstAddr, g_tSF.PageSize);

		if (sf_CmpData(_uiWrAddr, _ucpSrc, _usWrLen) == 0)
		{
			cRet = 1;
			break;
		}
		else
		{
			if (sf_CmpData(_uiWrAddr, _ucpSrc, _usWrLen) == 0)
			{
				cRet = 1;
				break;
			}


			for (j = 0; j < 10000; j++);
		}
	}

	return cRet;
}


uint8_t sf_WriteBuffer(uint8_t* _pBuf, uint32_t _uiWriteAddr, uint16_t _usWriteSize)
{
	uint16_t NumOfPage = 0, NumOfSingle = 0, Addr = 0, count = 0, temp = 0;

	Addr = _uiWriteAddr % g_tSF.PageSize;
	count = g_tSF.PageSize - Addr;
	NumOfPage =  _usWriteSize / g_tSF.PageSize;
	NumOfSingle = _usWriteSize % g_tSF.PageSize;

	if (Addr == 0)
	{
		if (NumOfPage == 0)
		{
			if (sf_AutoWritePage(_pBuf, _uiWriteAddr, _usWriteSize) == 0)
			{
				return 0;
			}
		}
		else
		{
			while (NumOfPage--)
			{
				if (sf_AutoWritePage(_pBuf, _uiWriteAddr, g_tSF.PageSize) == 0)
				{
					return 0;
				}
				_uiWriteAddr +=  g_tSF.PageSize;
				_pBuf += g_tSF.PageSize;
			}
			if (sf_AutoWritePage(_pBuf, _uiWriteAddr, NumOfSingle) == 0)
			{
				return 0;
			}
		}
	}
	else
	{
		if (NumOfPage == 0)
		{
			if (NumOfSingle > count) /* (_usWriteSize + _uiWriteAddr) > SPI_FLASH_PAGESIZE */
			{
				temp = NumOfSingle - count;

				if (sf_AutoWritePage(_pBuf, _uiWriteAddr, count) == 0)
				{
					return 0;
				}

				_uiWriteAddr +=  count;
				_pBuf += count;

				if (sf_AutoWritePage(_pBuf, _uiWriteAddr, temp) == 0)
				{
					return 0;
				}
			}
			else
			{
				if (sf_AutoWritePage(_pBuf, _uiWriteAddr, _usWriteSize) == 0)
				{
					return 0;
				}
			}
		}
		else
		{
			_usWriteSize -= count;
			NumOfPage =  _usWriteSize / g_tSF.PageSize;
			NumOfSingle = _usWriteSize % g_tSF.PageSize;

			if (sf_AutoWritePage(_pBuf, _uiWriteAddr, count) == 0)
			{
				return 0;
			}

			_uiWriteAddr +=  count;
			_pBuf += count;

			while (NumOfPage--)
			{
				if (sf_AutoWritePage(_pBuf, _uiWriteAddr, g_tSF.PageSize) == 0)
				{
					return 0;
				}
				_uiWriteAddr +=  g_tSF.PageSize;
				_pBuf += g_tSF.PageSize;
			}

			if(NumOfSingle != 0)
			{
				if (sf_AutoWritePage(_pBuf, _uiWriteAddr, NumOfSingle) == 0)
				{
					return 0;
				}
			}
		}
	}
	return 1;
}


uint32_t sf_ReadID(void)
{
	uint32_t uiID;
	uint8_t id1, id2, id3;

	SF_CS_LOW();
	Spi1_SendByte(CMD_RDID);
	id1 = Spi1_SendByte(DUMMY_BYTE);
	id2 = Spi1_SendByte(DUMMY_BYTE);
	id3 = Spi1_SendByte(DUMMY_BYTE);
	SF_CS_HIGH();

	uiID = ((uint32_t)id1 << 16) | ((uint32_t)id2 << 8) | id3;

	return uiID;
}


void sf_ReadInfo(void)
{
	
    g_tSF.ChipID = sf_ReadID();	
    if(Debug) printf("ChipID = %x \n",g_tSF.ChipID);

    switch (g_tSF.ChipID)
    {
        case SST25VF016B_ID:
            strcpy(g_tSF.ChipName, "SST25VF016B");
            g_tSF.TotalSize = 2 * 1024 * 1024;
            g_tSF.PageSize = 4 * 1024;
            if(Debug) printf("flash = %s \n",g_tSF.ChipName);
            break;

        case MX25L1606E_ID:
            strcpy(g_tSF.ChipName, "MX25L1606E");
            g_tSF.TotalSize = 2 * 1024 * 1024;
            g_tSF.PageSize = 4 * 1024;
            if(Debug) printf("flash = %s \n",g_tSF.ChipName);
            break;
        
        case W25Q16DV_ID:
            strcpy(g_tSF.ChipName, "W25Q16DV");
            g_tSF.TotalSize = 2 * 1024 * 1024;
            g_tSF.PageSize = 4 * 1024;
            if(Debug) printf("flash = %s \n",g_tSF.ChipName);
            break;
		case W25Q32DV_ID:
		case W25Q32JV_ID:
            strcpy(g_tSF.ChipName, "W25Q32JV");
            g_tSF.TotalSize = 4 * 1024 * 1024;
            g_tSF.PageSize = 4 * 1024;
            if(Debug) printf("flash = %s \n",g_tSF.ChipName);
            break;
        case W25Q64JV_ID:
            strcpy(g_tSF.ChipName, "W25Q64JV");
            g_tSF.TotalSize = 8 * 1024 * 1024;
            g_tSF.PageSize = 4 * 1024;
            if(Debug) printf("flash = %s \n",g_tSF.ChipName);
            break;
        case W25Q128DV_ID:
            strcpy(g_tSF.ChipName, "W25Q128DV");
            g_tSF.TotalSize = 16 * 1024 * 1024;
            g_tSF.PageSize = 4 * 1024;
            if(Debug) printf("flash = %s \n",g_tSF.ChipName);
            break;
        
        default:
            strcpy(g_tSF.ChipName, "Unknow Flash");
            g_tSF.TotalSize = 2 * 1024 * 1024;
            g_tSF.PageSize = 4 * 1024;
            if(Debug) printf("flash = %s \n",g_tSF.ChipName);
            break;
    }
}


static void sf_WriteEnable(void)
{
	SF_CS_LOW();
	Spi1_SendByte(CMD_WREN);
	SF_CS_HIGH();
}


static void sf_WriteStatus(uint8_t _ucValue)
{
	if (g_tSF.ChipID == SST25VF016B_ID)
	{

		SF_CS_LOW();
		Spi1_SendByte(CMD_EWRSR);
		SF_CS_HIGH();


		SF_CS_LOW();
		Spi1_SendByte(CMD_WRSR);
		Spi1_SendByte(_ucValue);
		SF_CS_HIGH();
	}
	else
	{
		SF_CS_LOW();
		Spi1_SendByte(CMD_WRSR);
		Spi1_SendByte(_ucValue);
		SF_CS_HIGH();
	}
}


static void sf_WaitForWriteEnd(void)
{
	SF_CS_LOW();
	Spi1_SendByte(CMD_RDSR);
	while((Spi1_SendByte(DUMMY_BYTE) & WIP_FLAG) == SET);
	SF_CS_HIGH();
}
