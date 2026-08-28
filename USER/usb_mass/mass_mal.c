/*
*********************************************************************************************************
*	                                  
*	模块名称 : 大容量存储中间层模块  MAL = Middle Access Layer
*	文件名称 : mass_mal.c
*	版    本 : V1.0
*	说    明 :  提供大容量存储中间层接口函数。 实现USB Mass Storage不需要文件系统支持，文件分区表和文件的
*			访问完全由windows系统控制，板子只需要应答USB Mass相关的命令（读写扇区）即可。
*			FAT文件系统（含Fat32）扇区大小为512字节。开发板实际使用的Nand Flash扇区大小为2048字节，因
*			此需要做封装变换处理。
*				
*			宏定义  mass_printf_err() 用于打印底层读写存储设备函数执行过程，主要用于跟踪调试。
*			修改 mass_mal.h文件可以取消或使能调试打印功能
*				#define  mass_printf_err_EN	1     1表示打印调试信息使能，0禁止
*
*********************************************************************************************************
*/

#include "stm32f10x.h"

uint32_t Mass_Memory_Size;
uint32_t Mass_Block_Size;
uint32_t Mass_Block_Count;
uint32_t Max_Lun = 0;	/* 0 表示SPI_FLASH卡 */

/*
*********************************************************************************************************
*	函 数 名: MAL_Init
*	功能说明: 初始化存储设备（用于USB Mass Storage）
*	形    参：lun ： SCSI逻辑单元号，0表示SD卡，1表示NAND Flash
*	返 回 值: MAL_OK : 成功；MAL_FAIL : 失败
*********************************************************************************************************
*/
uint16_t MAL_Init(uint8_t lun)
{
    if(lun == SPI_FLASH) 
    {
        bsp_InitSpi1Bus();
        bsp_InitSpiFlash();
    }
    return MAL_OK;
}

/*
*********************************************************************************************************
*	函 数 名: MAL_Write
*	功能说明: 写扇区
*	形    参：lun ： SCSI逻辑单元号，0表示SD卡，1表示NAND Flash
*			  Memory_Offset : 存储单元偏移地址
*			  Writebuff     ：待写入的数据缓冲区的指针
*			  Transfer_Length ：待写入的字节数， 不大于 512
*	返 回 值: MAL_OK : 成功；MAL_FAIL : 失败
*********************************************************************************************************
*/
uint16_t MAL_Write(uint8_t lun, uint32_t Memory_Offset, uint8_t *Writebuff, uint16_t Transfer_Length)
{
	/* USER CODE BEGIN 7 */
	if(!Transfer_Length) return MAL_FAIL;

	if(lun == SPI_FLASH) sf_WriteBuffer((uint8_t *)Writebuff, Memory_Offset, Transfer_Length);	
	return MAL_OK;
    /* USER CODE END 7 */
}

/*
*********************************************************************************************************
*	函 数 名: MAL_Read
*	功能说明: 读扇区，支持多个扇区
*	形    参：lun ： SCSI逻辑单元号，0表示SD卡，1表示NAND Flash
*			  Memory_Offset : 存储单元偏移地址
*			  Readbuff      ：存储读出的数据的缓冲区的指针
*			  Transfer_Length ：需要读出的字节数， 不大于 512
*	返 回 值: MAL_OK : 成功；MAL_FAIL : 失败
*********************************************************************************************************
*/
uint16_t MAL_Read(uint8_t lun, uint32_t Memory_Offset, uint8_t *Readbuff, uint16_t Transfer_Length)
{
	  /* USER CODE BEGIN 6 */
	if(!Transfer_Length) return 0;

	if(lun == SPI_FLASH) sf_ReadBuffer((uint8_t *)Readbuff, Memory_Offset , Transfer_Length); 
	return MAL_OK;
  /* USER CODE END 6 */ 
}

/*
*********************************************************************************************************
*	函 数 名: MAL_GetStatus
*	功能说明: 读取存储设备的状态信息
*	形    参：lun ： SCSI逻辑单元号，0表示SD卡，1表示NAND Flash
*	返 回 值: MAL_OK : 成功；MAL_FAIL : 失败
*********************************************************************************************************
*/
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
