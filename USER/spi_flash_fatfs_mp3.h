#ifndef __SPI_FLASH_FATFS_MP3_H
#define __SPI_FLASH_FATFS_MP3_H

#include "stm32f10x.h"
#include "ff.h"
//#include "spi_flash_fatfs.h"

/* Work Mode Enumeration */
enum WORKMODE { MODEA = 1, MODEB, MODEC, MODED };

/* MP3 Control Data Structure */
typedef struct
{
    char filename[13];          /* Declara el recurso de datos utilizado por esta rutina. */
    uint8_t ucMuteOn;			/* Declara el recurso de datos utilizado por esta rutina. */
    uint8_t ucVolume;			/* Declara el recurso de datos utilizado por esta rutina. */
    uint8_t VolumeEnviron;
    uint8_t VolumePeriod;
    uint8_t VolumeChangeFlag;
    uint32_t uiProgress;		/* Declara el recurso de datos utilizado por esta rutina. */
    uint8_t ucPauseEn;			/* Declara el recurso de datos utilizado por esta rutina. */
    uint8_t fileOpenFlag;
    uint8_t fileChangeFlag;
    uint8_t cycleFlag;
    uint8_t stopFlag;
    uint8_t writeParFlag;
    
    uint8_t playing;
    uint32_t stopCount;
    uint32_t CycleTime;
    uint8_t dir;
    uint8_t WorkMode;       //SW7 ON mode B, OFF mode A;    SW6 ON mode C, OFF mode A
    uint8_t PushbuttonMode; //ON mode PushbuttonMode, OFF mode A or B 
    uint8_t PushbuttonValid_flag;
    uint8_t PushbuttonCount;
    uint8_t lamp_chge_sound_flag;
    uint8_t Writingflag;
}MP3_T;

/* Global Instance Declaration */
extern MP3_T MP3;

/* Public Function Prototypes */
void mp3_par_init(void);
uint8_t Mp3Pro(void); /* Removed static, so PlaySound() in spi_flash_fatfs.c can access/calls it */
void PlayStart(void);
void Playing(void);
void fileChange(void);
void get_filename(uint8_t num);
uint8_t clac_Volume(void);
void CheckVolume(void);
void PlaySound(char *filename);

#endif /* __SPI_FLASH_FATFS_MP3_H */
