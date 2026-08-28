/* Controlador de memoria Flash externa SPI. */
#include "stm32f10x.h"
	/*
	*/

    
    #define SF_CS_LOW()       GPIOA->BRR = GPIO_Pin_4
    
    #define SF_CS_HIGH()      GPIOA->BSRR = GPIO_Pin_4




SFLASH_T g_tSF;

void sf_ReadInfo(void);
static void sf_WriteEnable(void);
static void sf_WriteStatus(uint8_t _ucValue);
static void sf_WaitForWriteEnd(void);
static uint8_t sf_NeedErase(uint8_t * _ucpOldBuf, uint8_t *_ucpNewBuf, uint16_t _uiLen);
static uint8_t sf_CmpData(uint32_t _uiSrcAddr, uint8_t *_ucpTar, uint32_t _uiSize);
static uint8_t sf_AutoWritePage(uint8_t *_ucpSrc, uint32_t _uiWrAddr, uint16_t _usWrLen);


/*
*********************************************************************************************************
*********************************************************************************************************
*/
void bsp_InitSpiFlash(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
	GPIO_Init(GPIOA, &GPIO_InitStructure);




}

/*
*********************************************************************************************************
*********************************************************************************************************
*/
void sf_EraseSector(uint32_t _uiSectorAddr)
{

	

}

/*
*********************************************************************************************************
*********************************************************************************************************
*/
void sf_EraseChip(void)
{

	

}

/*
*********************************************************************************************************
*********************************************************************************************************
*/
void sf_PageWrite(uint8_t * _pBuf, uint32_t _uiWriteAddr, uint16_t _usSize)
{
	uint32_t i, j;

	for (j = 0; j < _usSize / 256; j++)
	{


		for (i = 0; i < 256; i++)
		{
		}



		_uiWriteAddr += 256;
	}

	
	SF_CS_LOW();
	Spi1_SendByte(CMD_DISWR);
	SF_CS_HIGH();

}

/*
*********************************************************************************************************
*********************************************************************************************************
*/
void sf_ReadBuffer(uint8_t * _pBuf, uint32_t _uiReadAddr, uint32_t _uiSize)
{
	
	if ((_uiSize == 0) ||(_uiReadAddr + _uiSize) > g_tSF.TotalSize)
	{
		return;
	}

	
	while (_uiSize--)
	{
	}
}

/*
*********************************************************************************************************
*********************************************************************************************************
*/
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

/*
*********************************************************************************************************
*********************************************************************************************************
*/
static uint8_t sf_NeedErase(uint8_t * _ucpOldBuf, uint8_t *_ucpNewBuf, uint16_t _usLen)
{
	uint16_t i;
	uint8_t ucOld;

	/*
	      old    new
		  1101   0101
	~     1111
		= 0010   0101

		  0010   old
	&	  0101   new
		 =0000

	*/

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

/*
*********************************************************************************************************
*********************************************************************************************************
*/
static uint8_t sf_AutoWritePage(uint8_t *_ucpSrc, uint32_t _uiWrAddr, uint16_t _usWrLen)
{
	uint16_t i;
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

	{
		for	(i = 0; i < g_tSF.PageSize; i++)
		{
			s_spiBuf[i] = _ucpSrc[i];
		}
	}
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

/*
*********************************************************************************************************
*********************************************************************************************************
*/
uint8_t sf_WriteBuffer(uint8_t* _pBuf, uint32_t _uiWriteAddr, uint16_t _usWriteSize)
{
	uint16_t NumOfPage = 0, NumOfSingle = 0, Addr = 0, count = 0, temp = 0;

	Addr = _uiWriteAddr % g_tSF.PageSize;
	count = g_tSF.PageSize - Addr;
	NumOfPage =  _usWriteSize / g_tSF.PageSize;
	NumOfSingle = _usWriteSize % g_tSF.PageSize;

	{
		{
			if (sf_AutoWritePage(_pBuf, _uiWriteAddr, _usWriteSize) == 0)
			{
				return 0;
			}
		}
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
	{
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
}

/*
*********************************************************************************************************
*********************************************************************************************************
*/
uint32_t sf_ReadID(void)
{
	uint32_t uiID;
	uint8_t id1, id2, id3;


	uiID = ((uint32_t)id1 << 16) | ((uint32_t)id2 << 8) | id3;

	return uiID;
}

/*
*********************************************************************************************************
*********************************************************************************************************
*/
void sf_ReadInfo(void)
{
	
    if(Debug) printf("ChipID = %x \n",g_tSF.ChipID);

    switch (g_tSF.ChipID)
    {
        case SST25VF016B_ID:
            strcpy(g_tSF.ChipName, "SST25VF016B");
            if(Debug) printf("flash = %s \n",g_tSF.ChipName);
            break;

        case MX25L1606E_ID:
            strcpy(g_tSF.ChipName, "MX25L1606E");
            if(Debug) printf("flash = %s \n",g_tSF.ChipName);
            break;
        
        case W25Q16DV_ID:
            strcpy(g_tSF.ChipName, "W25Q16DV");	
            if(Debug) printf("flash = %s \n",g_tSF.ChipName);
            break;
		case W25Q32DV_ID:
		case W25Q32JV_ID:
            strcpy(g_tSF.ChipName, "W25Q32JV");	
            if(Debug) printf("flash = %s \n",g_tSF.ChipName);
            break;
        case W25Q64JV_ID:
            strcpy(g_tSF.ChipName, "W25Q64JV");
            if(Debug) printf("flash = %s \n",g_tSF.ChipName);
            break;
        case W25Q128DV_ID:
            strcpy(g_tSF.ChipName, "W25Q128DV");
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

/*
*********************************************************************************************************
*********************************************************************************************************
*/
static void sf_WriteEnable(void)
{
}

/*
*********************************************************************************************************
*********************************************************************************************************
*/
static void sf_WriteStatus(uint8_t _ucValue)
{
	if (g_tSF.ChipID == SST25VF016B_ID)
	{
		

		
	}
	else
	{
	}
}

/*
*********************************************************************************************************
*********************************************************************************************************
*/
static void sf_WaitForWriteEnd(void)
{
}
