#ifndef __SPI_W5500_ETH_H
#define __SPI_W5500_ETH_H

//#include <stdint.h>
//#include <stdio.h>
//#include <string.h>
#include "stm32f10x.h"
#include "demo_spi_flash_fatfs.h"

typedef struct
{
    uint8_t cardType;           /* Declara el recurso de datos utilizado por esta rutina. */
	uint8_t packType;			/* Declara el recurso de datos utilizado por esta rutina. */
	char filename[13];
	uint8_t operaType;		    /* Declara el recurso de datos utilizado por esta rutina. */
	uint16_t total;			    /* Declara el recurso de datos utilizado por esta rutina. */
    uint16_t now;               /* Declara el recurso de datos utilizado por esta rutina. */
    uint16_t len;               /* Declara el recurso de datos utilizado por esta rutina. */
    uint8_t data;
}PACK_T;

/* Global pointer declaration accessible by other files */
extern PACK_T *pack;

/* Public Function Prototypes */
uint8_t ReceiveProcess(uint8_t *rdata, uint8_t reSize);
void Process_Socket_Data(SOCKET s);
void SocketProcess(void);

#endif /* __SPI_W5500_ETH_H */
