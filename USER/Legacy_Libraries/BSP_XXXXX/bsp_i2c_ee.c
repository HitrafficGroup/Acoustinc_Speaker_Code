/**
  ******************************************************************************
  * @file    bsp_i2c_ee.c
  * @version V1.0
  * @date    2013-xx-xx
  ******************************************************************************
  * @attention
  *
  *
  ******************************************************************************
  */ 

#include "bsp_i2c_ee.h"
#include "bsp_i2c_gpio.h"
#include "stm32f10x.h"
#include "Systick.h"
#include "stdio.h"
#include "usart.h"

/*
*********************************************************************************************************
*********************************************************************************************************
*/
uint8_t ee_CheckOk(void)
{
	if (i2c_CheckDevice(EE_DEV_ADDR) == 0)
	{
		return 1;
	}
	else
	{
		i2c_Stop();		
		return 0;
	}
}

/*
*********************************************************************************************************
*********************************************************************************************************
*/
uint8_t ee_ReadBytes(uint8_t *_pReadBuf, uint16_t _usAddress, uint16_t _usSize)
{
	uint16_t i;
	
	
	i2c_Start();
	
	
	if (i2c_WaitAck() != 0)
	{
	}

	i2c_SendByte((uint8_t)_usAddress);
	
	if (i2c_WaitAck() != 0)
	{
	}
	
	i2c_Start();
	
	
	if (i2c_WaitAck() != 0)
	{
	}	
	
	for (i = 0; i < _usSize; i++)
	{
		
		if (i != _usSize - 1)
		{
		}
		else
		{
		}
	}
	i2c_Stop();

	i2c_Stop();
	return 0;
}

/*
*********************************************************************************************************
*********************************************************************************************************
*/
uint8_t ee_WriteBytes(uint8_t *_pWriteBuf, uint16_t _usAddress, uint16_t _usSize)
{
	uint16_t i,m;
	uint16_t usAddr;
	
	/* 
	*/

	usAddr = _usAddress;	
	for (i = 0; i < _usSize; i++)
	{
		if ((i == 0) || (usAddr & (EE_PAGE_SIZE - 1)) == 0)
		{
			i2c_Stop();
			
			*/
			for (m = 0; m < 100; m++)
			{				
				i2c_Start();
				
				
				if (i2c_WaitAck() == 0)
				{
					break;
				}
			}
			if (m  == 1000)
			{
			}
		
			i2c_SendByte((uint8_t)usAddr);
			
			if (i2c_WaitAck() != 0)
			{
			}
		}
	
		i2c_SendByte(_pWriteBuf[i]);
	
		if (i2c_WaitAck() != 0)
		{
		}

	}
	
	i2c_Stop();
	return 1;

	i2c_Stop();
	return 0;
}

void ee_Erase(void)
{
	uint16_t i;
	uint8_t buf[EE_SIZE];
	
	for (i = 0; i < EE_SIZE; i++)
	{
		buf[i] = 0xFF;
	}
	
	if (ee_WriteBytes(buf, 0, EE_SIZE) == 0)
	{
		
		
		return;
	}
	else
	{
		
		
	}
}


/*--------------------------------------------------------------------------------------------------*/
static void ee_Delay(__IO uint32_t nCount)	 
{
	for(; nCount != 0; nCount--);
}

//============================================================================================
uint8_t write_buf[EE_SIZE];
uint8_t read_buf[EE_SIZE];

void ee_Init(void)
{
	ee_CheckOk();
	ee_ReadBytes(read_buf,0, EE_SIZE);
    
    //Tem_UP     = Tem_UP+read_buf[0];
    
    //Tem_DOWN   = Tem_DOWN+read_buf[2];
    
    //Tim_imm    = Tim_imm+read_buf[4];
    
    //Tim_froat  = Tim_froat+read_buf[6]; 
    
    //Tim_fix    = Tim_fix+read_buf[8];
    
    //Oil_first  = Oil_first+read_buf[10];
    
    //Oil_second = Oil_second+read_buf[12];
}
/*
 *///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void ee_Test(void)
{
  uint16_t i;

/*-----------------------------------------------------------------------------------*/  
  if (ee_CheckOk() == 0)
	{
		
		
	}
/*------------------------------------------------------------------------------------*/  
	for (i = 0; i < EE_SIZE; i++)
	{		
		write_buf[i] = i;
	}
/*------------------------------------------------------------------------------------*/  
  if (ee_WriteBytes(write_buf, 0, EE_SIZE) == 0)
	{
		

		return;
	}
	else
	{		
		

	}
  
  ee_Delay(0x0FFFFF);
/*-----------------------------------------------------------------------------------*/
  if (ee_ReadBytes(read_buf, 0, EE_SIZE) == 0)
	{
		

		return;
	}
	else
	{		
		

	}
/*-----------------------------------------------------------------------------------*/  
  for (i = 0; i < EE_SIZE; i++)
	{
		if(read_buf[i] != write_buf[i])
		{
			//printf("0x%02X ", read_buf[i]);
			

			return;
		}
   // printf(" %02X", read_buf[i]);
		
		if ((i & 15) == 15)
		{
			//printf("\r\n");	
		}		
	}
 
}
/*********************************************END OF FILE**********************/
