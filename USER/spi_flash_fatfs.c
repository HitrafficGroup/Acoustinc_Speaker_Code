/* Utilidades de almacenamiento FATFS, configuracion persistente y reproduccion de audio. */

#include "spi_flash_fatfs_mp3.h"/* Header inclusion for MP3 module */
#include "spi_flash_fatfs_config.h"
#include "stm32f10x.h" 
#include "ff.h"			/* FatFS????????*/

#include "spi_w5500_eth.h"

//#define SONG_LIST_MAX	24

//const char filenameBuf[24][4]={
//    "000.","001.","002.","003.","004.","005.","006.","007.",
//    "008.","009.","010.","011.","012.","013.","016.","015.",
//    "016.","017.","018.","019.","020.","021.","022.","023.",
//};

extern PACK_T  *pack;
FIle_TRANS_T fileTrans;

//MP3_T MP3;
FATFS fs;
FIL file;
//FIL mfile;

uint8_t *bufptr;
/* Define la constante TEST_FILE_LEN utilizada por este modulo. */
#define TEST_FILE_LEN			(2*1024*1024)	/* ??????????????? */

//(4096*1) ???21S
//(4096*2) ???24S
//(4096*4) ???22S

uint8_t Par[64];//
uint8_t Time_Volume[2][6][3];
uint8_t FileBuf[BUF_SIZE];
//static unsigned char fileBuf[128];

/* ???????????????????????? */
//static void DispMenu(void);
void FileFormat(void);
void ViewRootDir(void);

//static void CreateNewFile(char *filename, uint8_t* data, uint16_t len);//
void CreateNewFile(char *filename, uint8_t* data, uint16_t len);//
//static void CreateNewFileWithNotClose(char *filename, uint8_t* data, uint16_t len);
void CreateNewFileWithNotClose(char *filename, uint8_t* data, uint16_t len);
//static void AddFileData(char *filename, uint8_t* data, uint16_t len);
void AddFileData(char *filename, uint8_t* data, uint16_t len);
//static void AddFileDataInClearMode(char *filename, uint8_t* data, uint16_t len);
void AddFileDataInClearMode(char *filename, uint8_t* data, uint16_t len);
//static void FileClose(void);
void FileClose(void);
//static void ReadFileData(char *filename);
void ReadFileData(char *filename);
//static void CreateDir(void);
void CreateDir(void);
//static void DeleteDirFile(void);
void DeleteDirFile(void);
//static void WriteFileTest(void);
void WriteFileTest(void);
//void PlaySound(char *filename);

//void Load_Net_Parameters(uint8_t *pdata);
//void Load_Period_Parameters(uint8_t *pdata);

//void CreateConfigFile(void);
//void DeleteConfigFile(void);

/* FatFs API?????? */
//static const char * FR_Table[]= 
const char * FR_Table[]= 
{
	"FR_OK?????",				                             /* (0) Succeeded */
	"FR_DISK_ERR????????????",			                 /* (1) A hard error occurred in the low level disk I/O layer */
	"FR_INT_ERR?????????",				                     /* (2) Assertion failed */
	"FR_NOT_READY????????????锟斤拷???",			             /* (3) The physical drive cannot work */
	"FR_NO_FILE???????????",				                 /* (4) Could not find the file */
	"FR_NO_PATH??锟斤拷????????",				                 /* (5) Could not find the path */
	"FR_INVALID_NAME????锟斤拷?????",		                     /* (6) The path name format is invalid */
	"FR_DENIED?????????????????????????????",         /* (7) Access denied due to prohibited access or directory full */
	"FR_EXIST????????????",			                     /* (8) Access denied due to prohibited access */
	"FR_INVALID_OBJECT?????????????????锟斤拷",		         /* (9) The file/directory object is invalid */
	"FR_WRITE_PROTECTED????????????锟斤拷????",		             /* (10) The physical drive is write protected */
	"FR_INVALID_DRIVE?????????????锟斤拷",		                 /* (11) The logical drive number is invalid */
	"FR_NOT_ENABLED?????????????",			                 /* (12) The volume has no work area */
	"FR_NO_FILESYSTEM???????锟斤拷??FAT??",		             /* (13) There is no valid FAT volume */
	"FR_MKFS_ABORTED?????????????f_mkfs()?????",	         /* (14) The f_mkfs() aborted due to any parameter error */
	"FR_TIMEOUT????锟斤拷????????????锟斤拷??????????",		 /* (15) Could not get a grant to access the volume within defined period */
	"FR_LOCKED?????????????????????????",				 /* (16) The operation is rejected according to the file sharing policy */
	"FR_NOT_ENOUGH_CORE????????????????????",		     /* (17) LFN working buffer could not be allocated */
	"FR_TOO_MANY_OPEN_FILES?????????????????_FS_SHARE", /* (18) Number of open files > _FS_SHARE */
	"FR_INVALID_PARAMETER????????锟斤拷"	                     /* (19) Given parameter is invalid */
};
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//char sysfile0[] = "/sys/0.mp3";
//char sysfile1[] = "/sys/1.wav";
//char file1[] = "/sound/1.mp3";
//char file2[] = "/sound/2.mp3";
//char file3[] = "/sound/3.mp3";
//char file4[] = "/sound/4.mp3";

//void mp3_par_init(void)
//{
//    MP3.playing = 0;
//    MP3.stopCount = 0;
//    MP3.VolumeChangeFlag = 0;
//    MP3.fileOpenFlag = 0;
//    MP3.fileChangeFlag = 0;
//    MP3.cycleFlag = 1;
//    MP3.stopFlag = 0;
//    MP3.dir = 0;
//    system_temp.timeUpdate = 1;    
//    MP3.CycleTime = RedCycleTime;
//    MP3.writeParFlag = 0;
//    MP3.ucMuteOn = 0;
//    MP3.lamp_chge_sound_flag = 0;
//    MP3.WorkMode = MODEA;
//    
//    MP3.Writingflag = 0;
//}

void FileFormat(void)
{
	/* Monta el volumen antes de formatearlo y lo desmonta al terminar. */
	FRESULT result;

	/* ????????? */
	result = f_mount(&fs, "0:", 0);	
	if (result != FR_OK)
	{
		printf("???????????? (%s)\r\n", FR_Table[result]);
	}
	else
	{
		printf("???????????? (%s)\r\n", FR_Table[result]);
	}
	
	
	result = f_mkfs("0:",0,4096);
	if (result != FR_OK)
	{
		printf("???????? (%s)\r\n", FR_Table[result]);
	}
	else
	{
		printf("???????? (%s)\r\n", FR_Table[result]);
	}

	
	result  = f_mount(NULL, "0:", 0);
	if (result != FR_OK)
	{
		printf("锟斤拷?????????? (%s)\r\n", FR_Table[result]);
	}
	else
	{
		printf("锟斤拷?????????? (%s)\r\n", FR_Table[result]);
	}
}

/* Recorre y muestra por UART las entradas del directorio raiz de la unidad logica. */
void ViewRootDir(void)
{
	/* Estructuras de FatFS empleadas para enumerar archivos y directorios. */
	FRESULT result;
	DIR DirInf;
	FILINFO FileInf;
	uint32_t cnt = 0;

	/* ????????? */
	result = f_mount(&fs, "0:", 0);	
	if (result != FR_OK)
	{
		printf("???????????? (%s)\r\n", FR_Table[result]);
	}
	else
	{
		printf("???????????? (%s)\r\n", FR_Table[result]);
	}
	
	/* ???????? */
	result = f_opendir(&DirInf, "0:"); /* ??????????????????????? */
	if (result != FR_OK)
	{
		printf("???????? (%s)\r\n", FR_Table[result]);
		return;
	}

	/* ????????????????????? */
	printf("????        |  ?????锟斤拷 | ??????? | \r\n");
	for (cnt = 0; ;cnt++)
	{
		result = f_readdir(&DirInf,&FileInf); 		/* ???????????????????? */
		if (result != FR_OK || FileInf.fname[0] == 0)
		{
			break;
		}

		if (FileInf.fname[0] == '.')
		{
			continue;
		}

		
		if (FileInf.fattrib & AM_DIR)
		{
			printf("??(0x%02d)  ", FileInf.fattrib);
		}
		else
		{
			printf("???(0x%02d)  ", FileInf.fattrib);
		}

		
		printf(" %10d", (int)FileInf.fsize);
		printf("  %s |", FileInf.fname);	        /* ??????? */
        printf("  %d", FileInf.fdate);
        printf("  %d\r\n", FileInf.ftime);	        /* ??????? */
	}

	/* ????????? */
	result = f_opendir(&DirInf, "0:/SYS"); /* ??????????????????????? */
	if (result != FR_OK)
	{
		printf("????????? (%s)\r\n", FR_Table[result]);
		return;
	}

	/* ????????????????????? */
	printf("????        |  ?????锟斤拷 | ??????? | \r\n");
	for (cnt = 0; ;cnt++)
	{
		result = f_readdir(&DirInf,&FileInf); 		/* ???????????????????? */
		if (result != FR_OK || FileInf.fname[0] == 0)
		{
			break;
		}

		if (FileInf.fname[0] == '.')
		{
			continue;
		}

		
		if (FileInf.fattrib & AM_DIR)
		{
			printf("??(0x%02d)  ", FileInf.fattrib);
		}
		else
		{
			printf("???(0x%02d)  ", FileInf.fattrib);
		}

		
		printf(" %10d", (int)FileInf.fsize);
		printf("  %s |", FileInf.fname);	        /* ??????? */
        printf("  %d", FileInf.fdate);
        printf("  %d\r\n", FileInf.ftime);	        /* ??????? */
	}
    
	
	result  = f_mount(NULL, "0:", 0);
	if (result != FR_OK)
	{
		printf("锟斤拷?????????? (%s)\r\n", FR_Table[result]);
	}
	else
	{
		printf("锟斤拷?????????? (%s)\r\n", FR_Table[result]);
	}
}


//static void CreateNewFile(char *filename, uint8_t* data, uint16_t len)
void CreateNewFile(char *filename, uint8_t* data, uint16_t len)
{
	/* Resultado de las operaciones de montaje, apertura, escritura y desmontaje. */
	FRESULT result;
	DIR DirInf;
	uint32_t bw;
    
 	/* ????????? */
	result = f_mount(&fs, "0:", 0);			/* Mount a logical drive */
	if (result != FR_OK)
	{
		printf("???????????? (%s)\r\n",  FR_Table[result]);
	}
    
	/* ???????? */
	result = f_opendir(&DirInf, "0:/"); /* ??????????????????????? */
	if (result != FR_OK)
	{
		printf("???????? (%s)\r\n",  FR_Table[result]);
		return;
	}
    
	/* ????? */
	result = f_open(&file, filename, FA_CREATE_ALWAYS | FA_WRITE);
    
	
	result = f_write(&file, data, len, &bw);
	if (result == FR_OK)
	{
		printf("%s ???锟斤拷????\r\n",filename);
	}
	else
	{
		printf("%s ???锟斤拷?????\r\n",filename);
	}

	/* ??????*/
	f_close(&file);

	
	result  = f_mount(NULL, "0:", 0);
}


//static void CreateNewFileWithNotClose(char *filename, uint8_t* data, uint16_t len)
void CreateNewFileWithNotClose(char *filename, uint8_t* data, uint16_t len)
{
	/* Mantiene el archivo abierto para que llamadas posteriores agreguen mas datos. */
	FRESULT result;
	DIR DirInf;
	uint32_t bw;
    
 	/* ????????? */
	result = f_mount(&fs, "0:", 0);			/* Mount a logical drive */
	if (result != FR_OK)
	{
		printf("???????????? (%s)\r\n",  FR_Table[result]);
	}
    
	/* ???????? */
	result = f_opendir(&DirInf, "0:/"); /* ??????????????????????? */
	if (result != FR_OK)
	{
		printf("???????? (%s)\r\n",  FR_Table[result]);
		return;
	}
    
	/* ????? */
	result = f_open(&file, filename, FA_CREATE_ALWAYS | FA_WRITE);
    
	
	result = f_write(&file, data, len, &bw);
	if (result == FR_OK)
	{
		printf("%s ???锟斤拷????\r\n",filename);
	}
	else
	{
		printf("%s ???锟斤拷?????\r\n",filename);
	}
}


//static void AddFileData(char *filename, uint8_t* data, uint16_t len)
void AddFileData(char *filename, uint8_t* data, uint16_t len)
{
	/* Posiciona el cursor al final del archivo antes de anexar el bloque recibido. */
	DIR DirInf;
	FRESULT result;
	uint32_t bw;

 	/* ????????? */
	result = f_mount(&fs, "0:", 0);			/* Mount a logical drive */
	if (result != FR_OK)
	{
		printf("????????????(%s)\r\n",  FR_Table[result]);
	}

	/* ???????? */
	result = f_opendir(&DirInf, "/"); /* ??????????????????????? */
	if (result != FR_OK)
	{
		printf("????????(%s)\r\n",  FR_Table[result]);
		return;
	}

	/* ????? */
	result = f_open(&file, filename, FA_OPEN_EXISTING | FA_WRITE);
	if (result !=  FR_OK)
	{
		printf("Don't Find File : %s\r\n", filename);
		return;
	}

    f_lseek(&file, file.fsize);
    
	
	result = f_write(&file, data, len, &bw);
    
    if(Debug) 
    {
        if (result == FR_OK)
        {
            printf("%s ???锟斤拷????\r\n", filename);
        }
        else
        {
            printf("%s ???锟斤拷?????\r\n", filename);
        }
    }
    
	/* ??????*/
	f_close(&file);

	
	result  = f_mount(NULL, "0:", 0);
}


//static void FileClose(void)
void FileClose(void)
{
	/* ??????*/
	f_close(&file);

	
    f_mount(NULL, "0:", 0);
}


//static void AddFileDataInClearMode(char *filename, uint8_t* data, uint16_t len)
void AddFileDataInClearMode(char *filename, uint8_t* data, uint16_t len)
{
	FRESULT result;
	uint32_t bw;

	
	result = f_write(&file, data, len, &bw);
    
    if(Debug) 
    {
        if (result == FR_OK)
        {
            printf("%s ???锟斤拷????\r\n", filename);
        }
        else
        {
            printf("%s ???锟斤拷?????\r\n", filename);
        }
    }
}


//static void ReadFileData(char *filename)
void ReadFileData(char *filename)
{
	/* Variables necesarias para montar, abrir y leer el archivo solicitado. */
	DIR DirInf;
	FRESULT result;
	uint32_t bw;
	char buf[256];

 	/* ????????? */
	result = f_mount(&fs, "0:", 0);			/* Mount a logical drive */
	if (result != FR_OK)
	{
		printf("????????????(%s)\r\n",  FR_Table[result]);
	}

	/* ???????? */
	result = f_opendir(&DirInf, "/"); /* ??????????????????????? */
	if (result != FR_OK)
	{
		printf("????????(%s)\r\n",  FR_Table[result]);
		return;
	}

	/* ????? */
	result = f_open(&file, filename, FA_OPEN_EXISTING | FA_READ);
	if (result !=  FR_OK)
	{
		printf("Don't Find File : %s\r\n", filename);
		return;
	}

	/* ?????? */
	result = f_read(&file, &buf, sizeof(buf) - 1, &bw);
	if (bw > 0)
	{
		buf[bw] = 0;
		printf("\r\n%s ??????? : %s\r\n",filename,buf);
	}
    
	/* ??????*/
	f_close(&file);

	
	result  = f_mount(NULL, "0:", 0);
}


/* Crea los directorios de prueba usados para validar las operaciones de FatFS. */
//static void CreateDir(void)
void CreateDir(void)
{
	/* Monta la unidad antes de crear la jerarquia de directorios de prueba. */
	FRESULT result;

 	/* ????????? */
	result = f_mount(&fs, "0:", 0);			/* Mount a logical drive */
	if (result != FR_OK)
	{
		printf("???????????? (%s)\r\n",  FR_Table[result]);
	}

	/* ??????/Dir1 */
	result = f_mkdir("/Dir1");
	if (result == FR_OK)
	{
		printf("f_mkdir Dir1 Ok\r\n");
	}
	else if (result == FR_EXIST)
	{
		printf("Dir1 ?????????(%s)\r\n",  FR_Table[result]);
	}
	else
	{
		printf("f_mkdir Dir1 ??? (%s)\r\n",  FR_Table[result]);
		return;
	}

	/* ??????/Dir2 */
	result = f_mkdir("/Dir2");
	if (result == FR_OK)
	{
		printf("f_mkdir Dir2 Ok\r\n");
	}
	else if (result == FR_EXIST)
	{
		printf("Dir2 ?????????(%s)\r\n",  FR_Table[result]);
	}
	else
	{
		printf("f_mkdir Dir2 ??? (%s)\r\n",  FR_Table[result]);
		return;
	}

	/* ???????? /Dir1/Dir1_1	   ???????????Dir1_1??????????????Dir1 */
	result = f_mkdir("/Dir1/Dir1_1"); /* */
	if (result == FR_OK)
	{
		printf("f_mkdir Dir1_1 ???\r\n");
	}
	else if (result == FR_EXIST)
	{
		printf("Dir1_1 ????????? (%s)\r\n",  FR_Table[result]);
	}
	else
	{
		printf("f_mkdir Dir1_1 ??? (%s)\r\n",  FR_Table[result]);
		return;
	}

	
	result  = f_mount(NULL, "0:", 0);
}


//static void DeleteDirFile(void)
void DeleteDirFile(void)
{
	/* Elimina archivos y directorios de prueba, comprobando cada resultado de FatFS. */
	FRESULT result;
	char FileName[13];
	uint8_t i;

 	/* ????????? */
	result = f_mount(&fs, "0:", 0);			/* Mount a logical drive */
	if (result != FR_OK)
	{
		printf("???????????? (%s)\r\n",  FR_Table[result]);
	}

	#if 0
	/* ???????? */
	result = f_opendir(&DirInf, "/"); /* ??????????????????????? */
	if (result != FR_OK)
	{
		printf("????????(%s)\r\n",  FR_Table[result]);
		return;
	}
	#endif

	/* ?????/Dir1 ?????????????????????????)??????????????????*/
	result = f_unlink("/Dir1");
	if (result == FR_OK)
	{
		printf("?????Dir1???\r\n");
	}
	else if (result == FR_NO_FILE)
	{
		printf("??锟斤拷?????????? :%s\r\n", "/Dir1");
	}
	else
	{
		printf("???Dir1???(??????? = %s) ?????????????\r\n",  FR_Table[result]);
	}

	/* ???????/Dir1/Dir1_1 */
	result = f_unlink("/Dir1/Dir1_1");
	if (result == FR_OK)
	{
		printf("??????? // Dir1 // Dir1_1 ??? \r\n");
	}
	else if ((result == FR_NO_FILE) || (result == FR_NO_PATH))
	{
		printf("??锟斤拷?????????? :%s\r\n", "/Dir1/Dir1_1");
	}
	else
	{
		printf("??????? // Dir1 // Dir1_1 ???(??????? = %s) ????????????? \r\n",  FR_Table[result]);
	}

	/* ???????/Dir1 */
	result = f_unlink("/Dir1");
	if (result == FR_OK)
	{
		printf("?????Dir1???\r\n");
	}
	else if (result == FR_NO_FILE)
	{
		printf("??锟斤拷?????????? :%s\r\n", "/Dir1");
	}
	else
	{
		printf("???Dir1???(??????? = %s) ?????????????\r\n",  FR_Table[result]);
	}

	/* ?????/Dir2 */
	result = f_unlink("/Dir2");
	if (result == FR_OK)
	{
		printf("????? Dir2 ???\r\n");
	}
	else if (result == FR_NO_FILE)
	{
		printf("??锟斤拷?????????? :%s\r\n", "/Dir2");
	}
	else
	{
		printf("???Dir2 ???(??????? = %s) ?????????????\r\n",  FR_Table[result]);
	}

	/* ?????? armfly.txt */
	result = f_unlink("armfly.txt");
	if (result == FR_OK)
	{
		printf("?????? armfly.txt ???\r\n");
	}
	else if (result == FR_NO_FILE)
	{
		printf("??锟斤拷?????????? :%s\r\n", "armfly.txt");
	}
	else
	{
		printf("???armfly.txt???(??????? = %s) ?????????????\r\n",  FR_Table[result]);
	}

	/* ?????? speed1.txt */
	for (i = 0; i < 20; i++)
	{
		sprintf(FileName, "Speed%02d.txt", i);		
		result = f_unlink(FileName);
		if (result == FR_OK)
		{
			printf("??????%s???\r\n", FileName);
		}
		else if (result == FR_NO_FILE)
		{
			printf("??锟斤拷??????:%s\r\n", FileName);
		}
		else
		{
			printf("???%s??????(??????? = %d) ?????????????\r\n", FileName, result);
		}
	}

	
	result  = f_mount(NULL, "0:", 0);
}


//static void WriteFileTest(void)
void WriteFileTest(void)
{
	/* Prepara la transferencia de bloques para medir la velocidad de escritura en FATFS. */
	FRESULT result;

	DIR DirInf;
	uint32_t bw;
	uint32_t i,k;
	uint32_t runtime1,runtime2,timelen;
	uint8_t err = 0;
	char TestFileName[13];
	static uint8_t s_ucTestSn = 0;

	for (i = 0; i < sizeof(FileBuf); i++)
	{
		FileBuf[i] = (i / 512) + '0';
	}

  	/* ????????? */
	result = f_mount(&fs, "0:", 0);			/* Mount a logical drive */
	if (result != FR_OK)
	{
		printf("???????????? (%s)\r\n",  FR_Table[result]);
	}

	/* ???????? */
	result = f_opendir(&DirInf, "/"); /* ??????????????????????? */
	if (result != FR_OK)
	{
		printf("???????? (%s)\r\n",  FR_Table[result]);
		return;
	}

	/* ????? */
	sprintf(TestFileName, "Speed%02d.txt", s_ucTestSn++);		
	result = f_open(&file, TestFileName, FA_CREATE_ALWAYS | FA_WRITE);

	
	printf("???锟斤拷???%s %dKB ...\r\n", TestFileName, TEST_FILE_LEN / 1024);
	runtime1 = 10;//wcx bsp_GetRunTime();	/* ???????????? */
	for (i = 0; i < TEST_FILE_LEN / BUF_SIZE; i++)
	{
		result = f_write(&file, FileBuf, sizeof(FileBuf), &bw);
		if (result == FR_OK)
		{
			if (((i + 1) % 8) == 0)
			{
				printf(".");
			}
		}
		else
		{
			err = 1;
			printf("%s???锟斤拷???\r\n", TestFileName);
			break;
		}
	}
	runtime2 = 20;//wcx bsp_GetRunTime();	/* ???????????? */

	if (err == 0)
	{
		timelen = (runtime2 - runtime1);
		printf("\r\n  锟斤拷??? : %dms   ???锟斤拷??? : %dB/S (%dKB/S)\r\n",
			timelen,
			(TEST_FILE_LEN * 1000) / timelen,
			((TEST_FILE_LEN / 1024) * 1000) / timelen);
	}

	f_close(&file);		/* ??????*/
    
	/* ???????????? */
	result = f_open(&file, TestFileName, FA_OPEN_EXISTING | FA_READ);
	if (result !=  FR_OK)
	{
		printf("?????????: %s\r\n", TestFileName);
		return;
	}
    
	printf("???????? %dKB ...\r\n", TEST_FILE_LEN / 1024);
	runtime1 = 10;//wcx  bsp_GetRunTime();	/* ???????????? */
	for (i = 0; i < TEST_FILE_LEN / BUF_SIZE; i++)
	{
		result = f_read(&file, FileBuf, sizeof(FileBuf), &bw);
		if (result == FR_OK)
		{
			if (((i + 1) % 8) == 0)
			{
				printf(".");
			}

			/* Recorre los elementos requeridos para completar la operacion. */
			for (k = 0; k < sizeof(FileBuf); k++)
			{
				if (FileBuf[k] != (k / 512) + '0')
				{
				  	err = 1;
					printf("Speed1.txt ?????????????????????\r\n");
					break;
				}
			}
			if (err == 1)
			{
				break;
			}
		}
		else
		{
			err = 1;
			printf("Speed1.txt ????????\r\n");
			break;
		}
	}
	runtime2 = 20;//wcx  bsp_GetRunTime();	/* ???????????? */

	if (err == 0)
	{
		timelen = (runtime2 - runtime1);
		printf("\r\n  ????? : %dms   ???????? : %dB/S (%dKB/S)\r\n", timelen,
			(TEST_FILE_LEN * 1000) / timelen, ((TEST_FILE_LEN / 1024) * 1000) / timelen);
	}

	/* ??????*/
	f_close(&file);

	
	result  = f_mount(NULL, "0:", 0);
}
