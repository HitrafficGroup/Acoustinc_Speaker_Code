

#include "stm32f10x.h"

uint32_t Mass_Memory_Size;
uint32_t Mass_Block_Size;
uint32_t Mass_Block_Count;
uint32_t Max_Lun = 0;


uint16_t MAL_Init(uint8_t lun)
{
    if(lun == SPI_FLASH) 
    {
        bsp_InitSpi1Bus();
        bsp_InitSpiFlash();
    }
    return MAL_OK;
}


uint16_t MAL_Write(uint8_t lun, uint32_t Memory_Offset, uint8_t *Writebuff, uint16_t Transfer_Length)
{
	/* USER CODE BEGIN 7 */
	if(!Transfer_Length) return MAL_FAIL;

	if(lun == SPI_FLASH) sf_WriteBuffer((uint8_t *)Writebuff, Memory_Offset, Transfer_Length);	
	return MAL_OK;
    /* USER CODE END 7 */
}


uint16_t MAL_Read(uint8_t lun, uint32_t Memory_Offset, uint8_t *Readbuff, uint16_t Transfer_Length)
{
	  /* USER CODE BEGIN 6 */
	if(!Transfer_Length) return 0;

	if(lun == SPI_FLASH) sf_ReadBuffer((uint8_t *)Readbuff, Memory_Offset , Transfer_Length); 
	return MAL_OK;
  /* USER CODE END 6 */ 
}


uint16_t MAL_GetStatus(uint8_t lun)
{
    uint16_t status = MAL_OK;
    /* USER CODE BEGIN 4 */ 
    if(lun == SPI_FLASH) 
    {
        if(sf_ReadID()==W25Q128DV_ID)
        {
            Mass_Block_Count = FLASH_SECTOR_COUNT;  //4096
            Mass_Block_Size  = FLASH_SECTOR_SIZE;   //4096
            Mass_Memory_Size = (Mass_Block_Count * Mass_Block_Size);
            status = MAL_OK;
        }
        else
            status = MAL_FAIL;
    }
    /* USER CODE BEGIN 4 */ 
    return status;
}
