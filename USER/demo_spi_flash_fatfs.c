/* Utilidades de almacenamiento FATFS, configuracion persistente y reproduccion de audio. */

#include "stm32f10x.h" 
#include "ff.h"			/* FatFS????????*/

#define SONG_LIST_MAX	24

const char filenameBuf[24][4]={
    "000.","001.","002.","003.","004.","005.","006.","007.",
    "008.","009.","010.","011.","012.","013.","016.","015.",
    "016.","017.","018.","019.","020.","021.","022.","023.",
};

const char zerobuf[32] = "\0\0\0\0\0\0\0\0\0\0\0";

PACK_T  *pack;
FIle_TRANS_T fileTrans;

MP3_T MP3;
FATFS fs;
FIL file;
FIL mfile;

uint8_t *bufptr;
/* Define la constante TEST_FILE_LEN utilizada por este modulo. */
#define TEST_FILE_LEN			(2*1024*1024)	/* ??????????????? */

//(4096*1) ???21S
//(4096*2) ???24S
//(4096*4) ???22S

uint8_t Par[64];//
uint8_t Time_Volume[2][6][3];
uint8_t FileBuf[BUF_SIZE];
static unsigned char fileBuf[128];

/* ???????????????????????? */
static void DispMenu(void);
void FileFormat(void);
void ViewRootDir(void);

static void CreateNewFile(char *filename, uint8_t* data, uint16_t len);//

//static void CreateNewFileWithNotClose(char *filename, uint8_t* data, uint16_t len);
void CreateNewFileWithNotClose(char *filename, uint8_t* data, uint16_t len);
static void AddFileData(char *filename, uint8_t* data, uint16_t len);
//static void AddFileDataInClearMode(char *filename, uint8_t* data, uint16_t len);
void AddFileDataInClearMode(char *filename, uint8_t* data, uint16_t len);
//static void FileClose(void);
void FileClose(void);

static void ReadFileData(char *filename);
static void CreateDir(void);
static void DeleteDirFile(void);
static void WriteFileTest(void);
void PlaySound(char *filename);

void Load_Net_Parameters(uint8_t *pdata);
void Load_Period_Parameters(uint8_t *pdata);

/* FatFs API?????? */
static const char * FR_Table[]= 
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

void get_cpuid(uint8_t *pdata)
{
    uint32_t Device_Serial[3];
    Device_Serial[0] = *(__IO uint32_t*)(0x1FFFF7E8);
    Device_Serial[1] = *(__IO uint32_t*)(0x1FFFF7EC);
    Device_Serial[2] = *(__IO uint32_t*)(0x1FFFF7F0);
    
    pdata[0] = (Device_Serial[2]&0xff);
    pdata[1] = (Device_Serial[2]>>8);
    pdata[2] = (Device_Serial[2]>>16);
    pdata[3] = (Device_Serial[2]>>24);
    printf("%08x %08x %08x \n\r",Device_Serial[0],Device_Serial[1],Device_Serial[2]);
    //05d9ff38 37304642 57178008 
    //05d8ff38 37304642 57177212 
}

char sysfile0[] = "/sys/0.mp3";
char sysfile1[] = "/sys/1.wav";
char file1[] = "/sound/1.mp3";
char file2[] = "/sound/2.mp3";
char file3[] = "/sound/3.mp3";
char file4[] = "/sound/4.mp3";

/////////////////////////////////////////////////////////////////////////////////////////////////////////
/* Tabla de mensajes asociada a los codigos de resultado de FatFS. */

int rin_test;
int gin_test;
int ain_test;
void SYS_TEST(void)
{
	uint8_t cmd;
    if(UartGetChar(&cmd))
    {
        //cmd = 'C';
        switch(cmd)
        {
            case '0':	
                printf("??0 - FileFormat??\r\n");
                FileFormat();		/* ???SD?????????????? */
                break;
            case '1':
                printf("??1 - ViewRootDir??\r\n");
                ViewRootDir();		/* ???SD?????????????? */
                break;
            case '3':
                printf("??3 - nRF24L01ioConfig\r\n");
               	nRF24L01ioConfig();		//????????
				SPI_RW_Reg(FLUSH_RX,0xff);
				SPI_RW_Reg(FLUSH_TX,0xff);
				RX_Mode();
                break;
			////////////////// Deshabilitado start
			case '2':
				printf("??2 - CreateNewFile??\r\n");	//Prueba deshabilitada para crear un archivo desde la consola serie.
				break;
			case '7':	//case '3':
				printf("??3 - ReadFileData??\r\n");
				ReadFileData(fileTrans.filename);
				break;
			case 'X':
				printf("??3 - ReadFileData??\r\n");
				AddFileData("armfly.bin", "sinowatcher", 11);		/* ?????????armfly.txt?????? */
				break;
			case '4':
				printf("??4 - CreateDir??\r\n");
				CreateDir();		/* ?????? */
				break;
			case '5':
				printf("??5 - DeleteDirFile??\r\n");
				DeleteDirFile();	/* ?????????? */
				break;
			case '6':
				printf("??6 - TestSpeed??\r\n");
				WriteFileTest();	/* ?????? */
				break;
            ////////////////// Deshabilitado fin
            case 'I':
                printf("??i - vs1053_ReadChipID??\r\n");
                vs1053_ReadChipID();
                break;
            case 'a':
                printf("??7 - PlaySound??\r\n");
                PlaySound(sysfile0);
                break;
            case 'A':
                printf("??7 - PlaySound??\r\n");
                PlaySound("001.mp3");//PlaySound("001.MP3");
                break;
            case 'B':
                printf("??7 - PlaySound??\r\n");
                PlaySound("002.mp3");
                break;
            case 'C':
                printf("??7 - PlaySound??\r\n");
                PlaySound("003.mp3");
                break;
            case 'D':
                printf("??7 - PlaySound??\r\n");
                PlaySound("004.mp3");
                break;
            case 'E':
                printf("??7 - PlaySound??\r\n");
                PlaySound("005.mp3");
                break;
            case 'F':
                printf("??7 - PlaySound??\r\n");
                PlaySound("001.WAV");
                break;
            case 'G':
                printf("??7 - PlaySound??\r\n");
                PlaySound("002.WAV");
                break;
            case 'H':
                printf("??7 - PlaySound??\r\n");
                memset(MP3.filename,0x00,13);
                memcpy(MP3.filename,"002.MP3",7);
                PlayStart();
                break;
            case 'S':
                printf("MP3.dir = %d\r\n",MP3.dir);
                break;
            case '+':
                if(MP3.ucVolume <= 244)MP3.ucVolume += 10;
                vs1053_SetVolume(MP3.ucVolume);
                printf("++ Volume = %d\r\n", MP3.ucVolume);
                break;
			case 'M':
                //if(MP3.ucVolume <= 244)MP3.ucVolume += 10;
                vs1053_SetVolume(244);
                printf("++ Volume = %d\r\n", MP3.ucVolume);
                break;
            case '-':
                if(MP3.ucVolume >= 10)
					MP3.ucVolume -= 10;
					//MP3.ucVolume ++;
                vs1053_SetVolume(MP3.ucVolume);
                printf("-- Volume = %d\r\n", MP3.ucVolume);
                break;
				////////////////// Deshabilitado start
			case '8':
				printf("??8 - ????????\r\n");
				vs1053_TestSine();
				vs1053_TestSine();
				vs1053_TestSine();
				vs1053_TestSine();
				break;
			case '9':
				printf("??9 - ???????????\r\n");
				vs1053_TestSineExit();
				break;
			case 'W':	//Prueba deshabilitada para fijar manualmente la fecha y hora del RTC.
				SYS_RTC->second  = 0x00;
				SYS_RTC->minute  = 0x21;
				SYS_RTC->hour    = 0x15;
				SYS_RTC->week    = 0x02;
				SYS_RTC->day     = 0x08;
				SYS_RTC->month   = 0x12;
				SYS_RTC->year    = 0x20;
				RtcWrite(SYS_RTC);
				break;
			////////////////// Deshabilitado END
            case 'T':
                printf("??T - ???????\r\n");
                RtcRead(SYS_RTC);
                printf("20%02x-%02x-%02x %02x %02x:%02x:%02x\r\n",SYS_RTC->year,SYS_RTC->month,SYS_RTC->day,SYS_RTC->week,SYS_RTC->hour,SYS_RTC->minute,SYS_RTC->second);
                printf("%08x\r\n",SCB->CPUID);
                //get_cpuid();
                break;
			////////
			case 'U':
                printf("??T - ???????\r\n");
				Auto_adjust_time();
                RtcRead(SYS_RTC);
                printf("20%02x-%02x-%02x %02x %02x:%02x:%02x\r\n",SYS_RTC->year,SYS_RTC->month,SYS_RTC->day,SYS_RTC->week,SYS_RTC->hour,SYS_RTC->minute,SYS_RTC->second);
                printf("%08x\r\n",SCB->CPUID);
                //get_cpuid();
                break;
			case 'z':
               printf("==SWIN %d \r\n", (GPIOC->IDR & 0x000f));

			   rin_test=((GPIOC->IDR & 0x2000)? 0:1);
			   //rin_test=((GPIOC->IDR & 0x2000));
			   printf("==RIN  %x \r\n", rin_test);

			   gin_test=((GPIOC->IDR & 0x4000)? 0:1);
			   //gin_test=((GPIOC->IDR & 0x4000));
			   printf("==GIN  %x \r\n", gin_test);

			   ain_test=((GPIOC->IDR & 0x8000)? 0:1);
			   //ain_test=((GPIOC->IDR & 0x8000));
			   printf("==AIN  %x \r\n", ain_test);
               break;
			case 'b':
			   printf("==DR1 ?Toggle \r\n");
               DR1_Toggle();
               break;
			case 'n':
			   printf("==DR2 ?Toggle \r\n");
               DR2_Toggle();
               break;
			case 'm':
			   printf("==DR3 ?Toggle \r\n");
               DR3_Toggle();
               break;
			////////
            default:
                DispMenu();
                break;
        }
    }
}

/* Muestra por UART las opciones de prueba disponibles para el sistema de archivos. */
static void DispMenu(void)
{
	printf("\r\n------------------------------------------------\r\n");
	printf("?????????????????0????SPI Flash?????\r\n");
	printf("????????????:\r\n");
	printf("0 - ??SPI_Flash??????????????\r\n");
	printf("1 - ??????????????锟斤拷?\r\n");
	printf("2 - ????????????armfly.txt\r\n");
	printf("3 - ??armfly.txt?????????\r\n");
	printf("4 - ??????\r\n");
	printf("5 - ??????????\r\n");
	printf("6 - ??锟斤拷?????????\r\n");
    printf("7 - ????WAV????\r\n");
}

void mp3_par_init(void)
{
    MP3.playing = 0;
    MP3.stopCount = 0;
    MP3.VolumeChangeFlag = 0;
    MP3.fileOpenFlag = 0;
    MP3.fileChangeFlag = 0;
    MP3.cycleFlag = 1;
    MP3.stopFlag = 0;
    MP3.dir = 0;
    system_temp.timeUpdate = 1;    
    MP3.CycleTime = RedCycleTime;
    MP3.writeParFlag = 0;
    MP3.ucMuteOn = 0;
    MP3.lamp_chge_sound_flag = 0;
    MP3.WorkMode = MODEA;
    
    MP3.Writingflag = 0;
}

uint8_t ReceiveProcess(uint8_t *rdata, uint8_t reSize)
{
    pack = (PACK_T*)rdata;
    if(pack->cardType == 0x32)
    {
        if(pack->packType == 0x68)
        {
            if(pack->operaType == 0x02) //recibe audio
            {
                if(pack->now == 1)
                {
                    strcpy(fileTrans.filename, pack->filename);
                    fileTrans.total = pack->total;
                    fileTrans.get = pack->now;
                    fileTrans.bytes = pack->len;
                    printf("??????? (%d)\r\n", pack->total);
                    fileTrans.Pdata = FileBuf;
                    memcpy(fileTrans.Pdata, &pack->data, pack->len);
                    fileTrans.filebufbytes = pack->len;
                    
                    fileTrans.timeCount = 1;
                    MP3.Writingflag = 1;
                    MP3.fileOpenFlag = 0;
                    MP3.stopCount = 0;
                }
                else if(pack->now == (fileTrans.get+1) && strcmp(fileTrans.filename, pack->filename)==0)
                {
                    fileTrans.get = pack->now;
                    fileTrans.bytes += pack->len;
                    memcpy(fileTrans.Pdata+fileTrans.filebufbytes, &pack->data, pack->len);
                    fileTrans.filebufbytes += pack->len;
                    
                    if(fileTrans.filebufbytes >= BUF_SIZE)
                    {
                        if(fileTrans.get <= (BUF_SIZE/1024))
                            CreateNewFileWithNotClose(fileTrans.filename, fileTrans.Pdata, BUF_SIZE);
                        else
                            AddFileDataInClearMode(fileTrans.filename, fileTrans.Pdata, BUF_SIZE);
                        fileTrans.filebufbytes = 0;
                        IWDG_Feed();
                    }
                    
                    fileTrans.timeCount = 1;
                    MP3.Writingflag = 1;
                    if(fileTrans.get == fileTrans.total) 
                    {
                        if(fileTrans.filebufbytes != 0)
                        {
                            if(fileTrans.get <= (BUF_SIZE/1024))
                                CreateNewFileWithNotClose(fileTrans.filename, fileTrans.Pdata, fileTrans.filebufbytes);
                            else
                                AddFileDataInClearMode(fileTrans.filename, fileTrans.Pdata, fileTrans.filebufbytes);
                        }
                        FileClose();
                        memset(&fileTrans.filename, 0x00, 13);
                        printf("??????????\r\n");
                    }
                }
                return 1;
            }
        }
        else if(pack->packType == 0x60)
        {
            if(pack->operaType == 0x01)//??????
            {
                return 3;
            }
            else if(pack->operaType == 0x02)
            {
                memcpy(Par, &pack->data, 47);
                MP3.writeParFlag = 1;
                return 2;
            }
        }
    }
    return 0;
}


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


static void CreateNewFile(char *filename, uint8_t* data, uint16_t len)
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


static void AddFileData(char *filename, uint8_t* data, uint16_t len)
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

void WriteConfigFile(uint8_t* data, uint8_t pos,uint8_t len)
{
	DIR DirInf;
	FRESULT result;
	uint32_t bw;
    char ConfigFile[] = "/sys/Config.ini";;
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
	result = f_open(&file, ConfigFile, FA_OPEN_EXISTING | FA_WRITE);
	if (result !=  FR_OK)
	{
		printf("Don't Find File : %s\r\n", ConfigFile);
		return;
	}

    f_lseek(&file, pos);
    
	
	result = f_write(&file, data, len, &bw);
    
    if (result == FR_OK && bw == len)
    {
        printf("%s ???锟斤拷????\r\n", ConfigFile);
    }
    else
    {
        printf("%s ???锟斤拷?????\r\n", ConfigFile);
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



static void ReadFileData(char *filename)
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


/* Valores predeterminados de red, horario y volumen almacenados en Config.ini. */

const uint8_t DefaultConfig[52]={
    192,168, 1,172,     192,168, 1, 1,      255,255,255, 0,//12 IP????
	0x01, 0x00, 0x70, 0x80,
    0x00,0x00,0xe7,	    0x09,0x00,0xe7,		0x13,0x00,0xe7,		0x16,0x00,0xe7,		0x21,0x00,0xe7,		0x23,0x59,0xe7,//???????   18
    0x00,0x00,0xe7,		0x09,0x00,0xe7,     0x13,0x00,0xe7,	    0x16,0x00,0xe7,		0x21,0x00,0xe7,		0x23,0x59,0xe7,//??????   18
};

static void CreateConfigFile(void)
{
	/* Crea la carpeta de sistema y escribe la configuracion inicial del equipo. */
	FRESULT result;
	uint32_t bw;
    char ConfigFile[] = "/sys/Config.ini";
    
 	/* ????????? */
	result = f_mount(&fs, "0:", 0);			/* Mount a logical drive */
	if (result != FR_OK)
	{
		printf("???????????? (%s)\r\n",  FR_Table[result]);
	}
    
    /* ??????/Dir1 */
	result = f_mkdir("/sys");
	if (result == FR_OK)
	{
		printf("f_mkdir /sys Ok\r\n");
	}
	else if (result == FR_EXIST)
	{
		printf("/sys ?????????(%s)\r\n",  FR_Table[result]);
	}
	else
	{
		printf("f_mkdir /sys ??? (%s)\r\n",  FR_Table[result]);
		return;
	}
    
	/* ????? */
	result = f_open(&file, ConfigFile, FA_CREATE_ALWAYS | FA_WRITE);
    if (result != FR_OK)
    {
        printf("???????? (%s)\r\n", FR_Table[result]);
    }
    
    printf("???锟斤拷??? %s\r\n", ConfigFile);
    result = f_write(&file, DefaultConfig, 52, &bw);
    if (result != FR_OK)
    {
        printf("???锟斤拷??? (%s)\r\n", FR_Table[result]);
    }

    printf("???锟斤拷???\r\n");
	/* ??????*/
	f_close(&file);

	
	result  = f_mount(NULL, "0:", 0);
}


uint8_t ReadConfigFile(void)
{
	FRESULT result;
	uint32_t i,bw;
    
 	/* ????????? */
	result = f_mount(&fs, "0:", 0);			/* Mount a logical drive */
	if (result != FR_OK)
	{
		printf("????????????(%s)\r\n",  FR_Table[result]);
	}
    
	/* ????? */
    result = f_open(&file, "/sys/Config.ini", FA_OPEN_EXISTING | FA_READ);
    if (result !=  FR_OK)//?????????,?????????
    {
        printf("Don't Find File : Config.ini\r\n");
        result  = f_mount(NULL, "0:", 0);   
        return 0;
    }
    
    /* ?????? */
    result = f_read(&file, FileBuf, 52, &bw);
    if(bw == 52)
    {
        printf("\r\n/sys/Config.ini ??????? : \r\n");
        for(i=0;i<bw;i++)
        {
            printf("%02x ",FileBuf[i]);
        }
        printf("\r\n");
    }
    
	/* ??????*/
	f_close(&file);

	
	result  = f_mount(NULL, "0:", 0);
    return 1;
}

uint8_t ReadAndCheckConfigFile(void)
{
    if(ReadConfigFile())//????????????
    {
        printf("???????锟斤拷???\r\n");
        Load_Net_Parameters(FileBuf);
        Load_Period_Parameters(&FileBuf[12]);
        return 1;
    }
    else
    {
        FileFormat();       // ?????
        CreateConfigFile(); // ??????????????
        return 0;
    }
}

void Config(void)
{
    while(ReadAndCheckConfigFile() != 1)
    {
        Delay(1000);
    }
}

/*******************************************************************************
* ??????  : Load_Net_Parameters
* ????    : ??????????
* ????    : ??
* ???    : ??
* ?????  : ??
* ???    : ??????????????????????IP?????????????IP????????????????????
*******************************************************************************/
void Load_Net_Parameters(uint8_t *pdata)
{
	Net.IP_Addr[0] = pdata[0];       //???????IP???
	Net.IP_Addr[1] = pdata[1];
	Net.IP_Addr[2] = pdata[2];
	Net.IP_Addr[3] = pdata[3];
	
	Net.Gateway_IP[0] = pdata[4];    //???????????
	Net.Gateway_IP[1] = pdata[5];
	Net.Gateway_IP[2] = pdata[6];
	Net.Gateway_IP[3] = pdata[7];

	Net.Sub_Mask[0] = pdata[8];      //????????????
	Net.Sub_Mask[1] = pdata[9];
	Net.Sub_Mask[2] = pdata[10];
	Net.Sub_Mask[3] = pdata[11];
    
    Socket[0].Mode = UDP_MODE;	    //??????0???????,UDP?? UDP_MODE
	Socket[1].Mode = TCP_SERVER;    //??????1???????,TCP???????? TCP_SERVER
	Socket[2].Mode = 3;             //??????2???????,TCP??????? TCP_CLIENT
    Socket[3].Mode = 3;
    Socket[4].Mode = 3;
    Socket[5].Mode = 3;
    Socket[6].Mode = 3;
    Socket[7].Mode = 3;
    
    Socket[0].LocalPort = 5000; //??????0?????161 
    Socket[1].LocalPort = 5001; //??????1?????5001 
    Socket[2].LocalPort = 5002; //??????2?????5002 
    
	Net.Phy_Addr[0] = 0x0c;     //???????????
	Net.Phy_Addr[1] = 0x29; 
	Net.Phy_Addr[2] = 0xab; 
	Net.Phy_Addr[3] = 0x7c; 
	Net.Phy_Addr[4] = 0x00; 
	Net.Phy_Addr[5] = 0x01; 
    
    Socket[0].State = 0;
    Socket[1].State = 0;
    Socket[2].State = 0;
    Socket[0].DataState = 0;
    Socket[1].DataState = 0;
    Socket[2].DataState = 0;
}

void Load_Period_Parameters(uint8_t *pdata)
{
	memcpy(system_temp.TimeZone, pdata, 4);
    memcpy(Time_Volume, pdata+4, 36);
	printf_fifo_hex(system_temp.TimeZone, 4);
}

/* Entrega bloques de 32 bytes al VS1053 cuando el decodificador solicita datos. */
static uint8_t Mp3Pro(void)
{
	uint32_t bw;
    FRESULT result;
	
	if (vs1053_ReqNewData())
	{
		result = f_read(&mfile, &fileBuf, 32, &bw);
        if (result !=  FR_OK)
        {
            printf("failed to read File : %s\r\n", MP3.filename);
            f_close(&mfile);
            MP3.fileOpenFlag = 0;
            PlayStart();
            return 1;
        }
		if (bw <= 0)
		{
			return 1;
		}
		vs1053_PreWriteData();	
		vs1053_WriteDatas((uint8_t*)fileBuf, 32);
//		if(lamp_status == GS && gre_off == 1)
//		{
//			vs1053_WriteDatas((uint8_t*)zerobuf, 32);
//		}
//		else 
//		{
//			vs1053_WriteDatas((uint8_t*)fileBuf, 32);
//		}
	}

	return 0;
}

void PlaySound(char *filename)
{
	/* Reinicia el decodificador, abre el archivo indicado y lo reproduce hasta finalizar. */
	FRESULT result;
    LED_Toggle();
    vs1053_SoftReset();
 	/* ????????? */
	result = f_mount(&fs, "0:", 0);			/* Mount a logical drive */
	if(result != FR_OK)
	{
		printf("????????????(%s)\r\n",  FR_Table[result]);
	}

    /* ???????? */
    result = f_open(&mfile, filename, FA_OPEN_EXISTING | FA_READ);
    if (result !=  FR_OK)
    {
      printf("??%s??????\r\n",filename);
    }
    else
    {
      printf("???????:%s\r\n",filename);
    }

    while(Mp3Pro() == 0)
    {
        //Delay(1);
    }
    /* ???????????*/
    f_close(&mfile);
    //vs1053_SoftReset();
    LED_Toggle();
}

void PlayStart(void)
{
	/* Abre la pista seleccionada en MP3 y prepara la reproduccion asincrona. */
	FRESULT result;
    LED_Toggle();
    vs1053_SoftReset();
 	/* ????????? */
	result = f_mount(&fs, "0:", 0);			/* Mount a logical drive */
	if(result != FR_OK)
	{
		printf("????????????(%s)\r\n",  FR_Table[result]);
	}

    /* ???????? */
    result = f_open(&mfile, MP3.filename, FA_OPEN_EXISTING | FA_READ);
    if (result !=  FR_OK)
    {
        printf("??%s??????\r\n",MP3.filename);
        return;
    }
    else
    {
        //printf("Now Play:%s!\r\n",MP3.filename);
    }
    MP3.fileOpenFlag = 1;
}

void Playing(void)
{
    if(MP3.fileOpenFlag && MP3.playing)
    {
        if(Mp3Pro())
        {
            if(MP3.cycleFlag)
            {
                if(MP3.lamp_chge_sound_flag == 1)
                {
                    MP3.lamp_chge_sound_flag = 0;
                    if(lamp_status == RS)
                    {
                        if(MP3.WorkMode == MODEA)
                        {
                            MP3.dir = 1;
                            memset(MP3.filename,0x00,13);
                            get_filename(MP3.dir);
                            MP3.fileChangeFlag = 1;
                        }
                        else MP3.dir = 1;
                    }
                    else if(lamp_status == GS)
                    {
						if(green_flash_sound_en)
						{
							if(gre_flash_flag == 0) 
								MP3.dir = 4;
							else 
								MP3.dir = 3;
						}
						else 
							MP3.dir = 3;
                        memset(MP3.filename,0x00,13);
                        get_filename(MP3.dir);
                        MP3.fileChangeFlag = 1;
                    }
					else if(lamp_status == BS)
					{
						MP3.stopFlag = 1;
//						MP3.dir = 3;
//						memset(MP3.filename,0x00,13);
//						get_filename(MP3.dir);
//						MP3.fileChangeFlag = 1;
					}
                }
                else
                {
                    f_lseek(&mfile, 0);
                    MP3.playing = 0;
					
					MP3.stopCount = 1;
					//MP3.playing = 1;
					//PlayStart();
                    //printf("Re:%s!\r\n",MP3.filename);
                }
            }
            else
            {
                f_close(&mfile);
                MP3.fileOpenFlag = 0;
                MP3.playing = 0;
                MP3.stopCount = 1;
				
				//MP3.playing = 1;
				//PlayStart();
                LED_Toggle();
            }
        }
        else
        {
            MP3.playing = 1;
            if(MP3.stopFlag)
            {
                MP3.stopFlag = 0;
                MP3.cycleFlag = 0;
                MP3.playing = 0;
                f_close(&mfile);
                MP3.fileOpenFlag = 0;
                LED_Toggle();
				
				MP3.stopCount = 1;
				//MP3.playing = 1;
				//PlayStart();
                printf("Stop Play:%s!\r\n",MP3.filename);
            }
        }
    }
}

void fileChange(void)
{
    if(MP3.fileChangeFlag==1 && MP3.Writingflag==0)
    {
        MP3.fileChangeFlag = 0;
        if(MP3.fileOpenFlag)
        {
            MP3.fileOpenFlag = 0;
            f_close(&mfile); // funcion no definida pero cierra un fichero algo asi
        }
        PlayStart();
        MP3.playing = 1;
        MP3.cycleFlag = 1;
        MP3.stopCount = 0;
    }
}

void get_filename(uint8_t num)
{
    if(num == 0)
        memcpy(MP3.filename,"001.MP3",7);
    else if(num == 1)
        memcpy(MP3.filename,"002.MP3",7);
    else if(num == 2)
        memcpy(MP3.filename,"003.MP3",7);
    else if(num == 3)
        memcpy(MP3.filename,"004.MP3",7);
    else if(num == 4)
        memcpy(MP3.filename,"005.MP3",7);
}
/* Crea los directorios de prueba usados para validar las operaciones de FatFS. */
static void CreateDir(void)
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

static void DeleteDirFile(void)
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


static void WriteFileTest(void)
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

uint8_t clac_Volume(void)
{
	uint8_t value,i;
	uint16_t day_by_mins;
	uint16_t plan_by_mins;

	day_by_mins = BCD_to_DEC(SYS_RTC->hour)*60+BCD_to_DEC(SYS_RTC->minute);
	if((SYS_RTC->week==0)||(SYS_RTC->week==6))
	{
		for(i=0;i<6;i++)
		{
			plan_by_mins=BCD_to_DEC(Time_Volume[1][i][0])*60+BCD_to_DEC(Time_Volume[1][i][1]);
			if(day_by_mins<plan_by_mins)
			{
				if(i==0) value=0;
				else value=Time_Volume[1][i-1][2];
				break;
			}
			else
			{
				if(i==5){value=0;break;}
				else continue;
			}
		}
	}
	else
	{
		for(i=0;i<6;i++)
		{
			plan_by_mins=BCD_to_DEC(Time_Volume[0][i][0])*60+BCD_to_DEC(Time_Volume[0][i][1]);
			if(day_by_mins<plan_by_mins)
			{
				if(i==0) value=0;
				else value=Time_Volume[0][i-1][2];
				break;
			}
			else
			{
				if(i==5){value=0;break;}
				else continue;
			}
		}
	}
	if(value>0xe0) value = 184 + ((value-0xe0)*10);  //1-7   254
    else value = 0;
	return value;
}

extern uint8_t StartFlag;

void CheckVolume(void)
{
    uint8_t Volume;
    if(system_temp.timeUpdate)
    {
        system_temp.timeUpdate = 0;
        RtcRead(SYS_RTC);
		system_temp.seconds =  SYS_RTC->hour * 3600 + SYS_RTC->minute * 60 + SYS_RTC->second;
		
        Volume = clac_Volume();
        if(MP3.VolumePeriod != Volume)
        {
            MP3.VolumePeriod = Volume;
            MP3.VolumeChangeFlag = 1;
			MP3.ucVolume = 1;//????????????1??????????? 20250219
        }
        //printf("VolumePeriod = %d\r\n", MP3.VolumePeriod);
		//(rtc, 7); //Vino asi desde el original, no se que hace, lo comento para que compile sin warnings ya que RTC no tiene que ver con el volumen 
    }
    
    if(ain.stab_state)
    {
        MP3.ucMuteOn = 1;
        if(MP3.ucVolume != 0)
        {
            MP3.ucVolume = 0;
            vs1053_SetVolume(MP3.ucVolume);
        }
    }
    else
    {
        if(MP3.ucMuteOn)
        {
            MP3.ucMuteOn = 0;
            if(MP3.fileOpenFlag && MP3.playing)
            {
                f_lseek(&mfile, 0);
            }
        }
    }
    
    if(MP3.ucMuteOn)
    {
        if(MP3.ucVolume != 0)
        {
            MP3.ucVolume = 0;
            vs1053_SetVolume(MP3.ucVolume);
			PA_OFF();
        }
    }
    else
    {
        if(StartFlag)
        {
            StartFlag = 0;
            MP3.ucVolume = 0;
            vs1053_SetVolume(MP3.ucVolume);
			PA_ON();
        }
        else
        {
			static uint8_t gre_off_bk = 0;
			if(lamp_status == GS && gre_off == 1)
			{
				if(gre_off != gre_off_bk)
				{
					gre_off_bk = gre_off;
					if(green_flash_sound_en)
					{
						MP3.ucVolume = 0;
						vs1053_SetVolume(MP3.ucVolume);
					}
				}
			}
			else 
			{
				gre_off_bk = 0;
				if(MP3.ucVolume != MP3.VolumePeriod)
				{
					MP3.ucVolume = MP3.VolumePeriod;
					vs1053_SetVolume(MP3.ucVolume);
				}

//				if(SW4())//???????
//				{
//					if(MP3.ucVolume != MP3.VolumePeriod)
//					{
//						MP3.ucVolume = MP3.VolumePeriod;
//						vs1053_SetVolume(MP3.ucVolume);
//					}
//				}
// Prueba deshabilitada para ajustar el volumen segun la entrada ambiental.
//				{
//					if(MP3.ucVolume != MP3.VolumeEnviron)
//					{
//						MP3.ucVolume = MP3.VolumeEnviron;
//						vs1053_SetVolume(MP3.ucVolume);
//					}
//				}
			}
        }
    }
}


