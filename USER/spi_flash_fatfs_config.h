#ifndef __SPI_FLASH_FATFS_CONFIG_H
#define __SPI_FLASH_FATFS_CONFIG_H

#include "stm32f10x.h"
#include "ff.h"

/* Global Constants - Externally exposed variables */
extern const uint8_t DefaultConfig[52];

/* Public function prototypes */
void get_cpuid(uint8_t *pdata);
void WriteConfigFile(uint8_t* data, uint8_t pos, uint8_t len);
void CreateConfigFile(void);
void DeleteConfigFile(void);
void Load_Net_Parameters(uint8_t *pdata);
void Load_Period_Parameters(uint8_t *pdata);
void Config(void);
uint8_t ReadConfigFile(void);
uint8_t ReadAndCheckConfigFile(void);

#endif /* __SPI_FLASH_FATFS_CONFIG_H */
