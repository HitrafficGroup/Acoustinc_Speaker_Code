/* Configura o realiza una transferencia por el bus SPI. */

//#ifndef _DEMO_SPI_FLASH_FATFS_H
//#define _DEMO_SPI_FLASH_FATFS_H

#ifndef __DEMO_SPI_FLASH_FATFS_H
#define __DEMO_SPI_FLASH_FATFS_H

#include "stm32f10x.h"

#define BUF_SIZE				  (4096*4)		/* Define el tamano del buffer BUF_SIZE. */

enum WORKMODE {MODEA = 1, MODEB, MODEC, MODED};

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

typedef struct
{
    char filename[13];          /* Declara el recurso de datos utilizado por esta rutina. */
	uint16_t total;			    /* Declara el recurso de datos utilizado por esta rutina. */
	uint16_t get;			    /* Declara el recurso de datos utilizado por esta rutina. */
    uint16_t filebufbytes;
	uint32_t bytes;		        /* Declara el recurso de datos utilizado por esta rutina. */
    uint8_t* Pdata;
    uint32_t timeCount;
}FIle_TRANS_T;

extern FIle_TRANS_T fileTrans;
extern MP3_T MP3;

extern uint8_t Time_Volume[2][6][3];
extern uint8_t Par[64];
extern uint8_t FileBuf[BUF_SIZE];

/* Public function prototypes */
void CreateNewFile(char *filename, uint8_t* data, uint16_t len);
void AddFileData(char *filename, uint8_t* data, uint16_t len);
void ReadFileData(char *filename);
void CreateDir(void);
void DeleteDirFile(void);
void WriteFileTest(void);

void FileFormat(void);
void ViewRootDir(void);

static void CreateNewFile(char *filename, uint8_t* data, uint16_t len);//

void CreateNewFileWithNotClose(char *filename, uint8_t* data, uint16_t len);

void AddFileDataInClearMode(char *filename, uint8_t* data, uint16_t len);

void FileClose(void);


void PlaySound(char *filename);
void PlayStart(void);
void Playing(void);

void get_cpuid(uint8_t *pdata);

//void SYS_TEST(void);
void Config(void);
void CheckVolume(void);

uint8_t ReadConfigFile(void);
uint8_t ReadAndCheckConfigFile(void);
void ReadIPConfigFile(void);
//uint8_t ReceiveProcess(uint8_t *rdata, uint8_t reSize);
void WriteConfigFile(uint8_t* data, uint8_t pos,uint8_t len);
void Load_Period_Parameters(uint8_t *pdata);

void fileChange(void);
void mp3_par_init(void);
void get_filename(uint8_t num);

void CreateConfigFile (void);
void DeleteConfigFile (void);

#endif /* __DEMO_SPI_FLASH_FATFS_H */
