/**
  ******************************************************************************
  * @file    usbd_storage_msd.c
  * @author  MCD application Team
  * @version V1.1.0
  * @date    19-March-2012
  * @brief   This file provides the disk operations functions.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2012 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software
  * distributed under the License is distributed on an "AS IS" BASIS,
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */

/* Inclusiones ------------------------------------------------------------------*/
#include "usbd_msc_mem.h"
#include "usb_conf.h"

#include "bsp_spi_flash.h"
#include "usbd_core.h"
#include "stdio.h"

//#define mass_printf	printf
#define mass_printf(...)

#define SPI_FLASH_SECTOR_SIZE    4096   
#define SPI_FLASH_START_SECTOR   0    	
#define SPI_FLASH_SECTOR_COUNT   512    
#define STORAGE_BLOCK_SIZE       4096

/** @addtogroup STM32_USB_OTG_DEVICE_LIBRARY
  * @{
  */


/** @defgroup STORAGE
  * @brief media storage application module
  * @{
  */

/** @defgroup STORAGE_Private_TypesDefinitions
  * @{
  */
/**
  * @}
  */


/** @defgroup STORAGE_Private_Defines
  * @{
  */

#define STORAGE_LUN_NBR           1
/**
  * @}
  */


/** @defgroup STORAGE_Private_Macros
  * @{
  */
/**
  * @}
  */


/** @defgroup STORAGE_Private_Variables
  * @{
  */
/* USB Mass storage Standard Inquiry Data */
const int8_t  STORAGE_Inquirydata[] =
{
	/* LUN 0 */
	0x00,
	0x80,
	0x02,
	0x02,
	(USBD_STD_INQUIRY_LENGTH - 5),
	0x00,
	0x00,
	0x00,
  'W', 'L', 'H', ' ', 'i', 's', ' ', 'a', /* Manufacturer : 8 bytes */
  'l', 'i', 't', 't', 'l', 'e', ' ', ' ', /* Product      : 16 Bytes */
  'd', 'o', 'g', '~', ' ', ' ', ' ', ' ',
  '1', '.', '0' ,'0',                     /* Version      : 4 Bytes */
};

/**
  * @}
  */


/** @defgroup STORAGE_Private_FunctionPrototypes
  * @{
  */
int8_t STORAGE_Init (uint8_t lun);

int8_t STORAGE_GetCapacity (uint8_t lun,
                           uint32_t *block_num,
                           uint32_t *block_size);

int8_t  STORAGE_IsReady (uint8_t lun);

int8_t  STORAGE_IsWriteProtected (uint8_t lun);

int8_t STORAGE_Read (uint8_t lun,
                        uint8_t *buf,
                        uint32_t blk_addr,
                        uint16_t blk_len);

int8_t STORAGE_Write (uint8_t lun,
                        uint8_t *buf,
                        uint32_t blk_addr,
                        uint16_t blk_len);

int8_t STORAGE_GetMaxLun (void);


USBD_STORAGE_cb_TypeDef USBD_user_fops =
{
  STORAGE_Init,
  STORAGE_GetCapacity,
  STORAGE_IsReady,
  STORAGE_IsWriteProtected,
  STORAGE_Read,
  STORAGE_Write,
  STORAGE_GetMaxLun,
  (int8_t *)STORAGE_Inquirydata,
};

USBD_STORAGE_cb_TypeDef  *USBD_STORAGE_fops = &USBD_user_fops;

//#ifndef USE_STM3210C_EVAL
//extern SD_CardInfo SDCardInfo;
//#endif

__IO uint32_t count = 0;
/**
  * @}
  */


/** @defgroup STORAGE_Private_Functions
  * @{
  */

/**
  * @brief  Initialize the storage medium
  * @param  lun : logical unit number
  * @retval Status
  */

int8_t STORAGE_Init (uint8_t lun)
{
  /* USER CODE BEGIN 2 */
  int8_t res=-1;
  bsp_InitSFlash();
	
  if(sf_ReadID()==W25Q16DV_ID)
  {
    res = USBD_OK;
  }
  return res;
  /* USER CODE END 2 */ 
}

/**
  * @brief  return medium capacity and block size
  * @param  lun : logical unit number
  * @param  block_num :  number of physical block
  * @param  block_size : size of a physical block
  * @retval Status
  */
int8_t STORAGE_GetCapacity (uint8_t lun, uint32_t *block_num, uint32_t *block_size)
{
	/* USER CODE BEGIN 3 */  
  *block_size = STORAGE_BLOCK_SIZE;
  *block_num  = SPI_FLASH_SECTOR_COUNT;	
  return (USBD_OK);
  /* USER CODE END 3 */ 
}


/**
  * @brief  check whether the medium is ready
  * @param  lun : logical unit number
  * @retval Status
  */
int8_t  STORAGE_IsReady (uint8_t lun)
{
	/* USER CODE BEGIN 4 */ 
  if(sf_ReadID()==W25Q16DV_ID)
  {
    return USBD_OK;
  }
  else
    return -1;
	/* USER CODE BEGIN 4 */ 
}

/**
  * @brief  check whether the medium is write-protected
  * @param  lun : logical unit number
  * @retval Status
  */
int8_t  STORAGE_IsWriteProtected (uint8_t lun)
{
  /* USER CODE BEGIN 5 */ 
  return (USBD_OK);
  /* USER CODE END 5 */ 
}

/**
  * @brief  Read data from the medium
  * @param  lun : logical unit number
  * @param  buf : Pointer to the buffer to save data
  * @param  blk_addr :  address of 1st block to be read
  * @param  blk_len : nmber of blocks to be read
  * @retval Status
  */
int8_t STORAGE_Read (uint8_t lun,
                 uint8_t *buf,
                 uint32_t blk_addr,
                 uint16_t blk_len)
{
  /* USER CODE BEGIN 6 */
	if(!blk_len) return 0;

	//sf_ReadBuffer(buf, blk_addr , blk_len); 	
	sf_ReadBuffer(buf, blk_addr << 12 , blk_len << 12 ); 	

	return USBD_OK;

  /* USER CODE END 6 */ 
}
/**
  * @brief  Write data to the medium
  * @param  lun : logical unit number
  * @param  buf : Pointer to the buffer to write from
  * @param  blk_addr :  address of 1st block to be written
  * @param  blk_len : nmber of blocks to be read
  * @retval Status
  */
int8_t STORAGE_Write (uint8_t lun,
                  uint8_t *buf,
                  uint32_t blk_addr,
                  uint16_t blk_len)
{
  /* USER CODE BEGIN 7 */
	if(!blk_len) return 0;

	//sf_WriteBuffer((uint8_t *)buf, blk_addr, blk_len);	
	sf_EraseSector(blk_addr << 12);
	sf_WriteBuffer((uint8_t *)buf, blk_addr << 12, blk_len << 12);	
	return USBD_OK;

  /* USER CODE END 7 */ 
}

/**
  * @brief  Return number of supported logical unit
  * @param  None
  * @retval number of logical unit
  */

int8_t STORAGE_GetMaxLun (void)
{
  /* USER CODE BEGIN 8 */ 
  return (STORAGE_LUN_NBR - 1);
  /* USER CODE END 8 */ 
}
/**
  * @}
  */


/**
  * @}
  */


/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

