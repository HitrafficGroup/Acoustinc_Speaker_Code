#include "uart_consola.h"
#include <stdio.h>
#include <string.h>
#include "ff.h"               
#include "spi_w5500_eth.h"    /* Ensures access to MP3, fileTrans, and other external definitions */
#include "demo_spi_flash_fatfs.h"  /* Resolves FATFS file operation warnings */
#include "bsp_vs1053b.h"           /* Replace with the actual header that contains PlaySound() */

/* Global test variables moved from original file */
char sysfile0[] = "/sys/0.mp3";
char sysfile1[] = "/sys/1.wav";
char file1[] = "/sound/1.mp3";
char file2[] = "/sound/2.mp3";
char file3[] = "/sound/3.mp3";
char file4[] = "/sound/4.mp3";

int rin_test;
int gin_test;
int ain_test;

/* Private function prototypes */
static void DispMenu(void);

/* Public function implementation */

void SYS_TEST(void)
{
	uint8_t cmd;
    if(UartGetChar(&cmd))
    {
        //cmd = 'C';
        switch(cmd)
        {
            case '0':
                printf("??0 - FileFormat??\r\n"); //Borra Config.ino and Audios
                FileFormat();		/* ???SD?????????????? */
                break;
			case '1':////////////////// Deshabilitado start
				printf("??1 - CreateConfigFile??\r\n");	//Prueba deshabilitada para crear un archivo desde la consola serie.                 //Config();
                CreateConfigFile();
				break;
			case '2':	////case '3':
				printf("??2 - ReadConfigFile??\r\n"); 			//ReadFileData(fileTrans.filename);
                ReadConfigFile();
				break;
			case '3':
				printf("??3 - ReadFileData??\r\n");
				//AddFileData("armfly.bin", "sinowatcher", 11);		/* ?????????armfly.txt?????? */                 //DeleteConfigFile();
				break;
            case '4':
                printf("??4 - ViewRootDir??\r\n");
                ViewRootDir();		/* ???SD?????????????? */
            break;
			case '5':
				printf("??5 - CreateDir??\r\n");
				CreateDir();		/* ?????? */
				break;
			case '6':
				printf("??6 - DeleteDirFile??\r\n");
				DeleteDirFile();	/* ?????????? */
				break;
			case '7':
				printf("??7 - TestSpeed??\r\n");
				WriteFileTest();	/* ?????? */
				break;////////////////// Deshabilitado fin
            case '8':
                printf("??8 - nRF24L01ioConfig\r\n");
               	nRF24L01ioConfig();		//????????
				SPI_RW_Reg(FLUSH_RX,0xff);
				SPI_RW_Reg(FLUSH_TX,0xff);
				RX_Mode();
                break;
            case '9':
                printf("??9 - vs1053_ReadChipID??\r\n");
                vs1053_ReadChipID();
                break;
            case 'a':
                printf("??a - PlaySound??\r\n");
                PlaySound(sysfile0);
                break;
            case 'A':
                printf("??A - PlaySound??\r\n");
                PlaySound("001.mp3");//PlaySound("001.MP3");
                break;
            case 'B':
                printf("??B - PlaySound??\r\n");
                PlaySound("002.mp3");
                break;
            case 'C':
                printf("??C - PlaySound??\r\n");
                PlaySound("003.mp3");
                break;
            case 'D':
                printf("??D - PlaySound??\r\n");
                PlaySound("004.mp3");
                break;
            case 'E':
                printf("??E - PlaySound??\r\n");
                PlaySound("005.mp3");
                break;
            case 'F':
                printf("??F - PlaySound??\r\n");
                PlaySound("001.WAV");
                break;
            case 'G':
                printf("??G - PlaySound??\r\n");
                PlaySound("002.WAV");
                break;
            case 'H':
                printf("??H - PlaySound??\r\n");
                memset(MP3.filename,0x00,13);
                memcpy(MP3.filename,"002.MP3",7);
                PlayStart();
                break;
            case 'S':
                printf("??S - MP3.dir = %d\r\n",MP3.dir);
                break;
            case '+':
                if(MP3.ucVolume <= 244)MP3.ucVolume += 10;
                vs1053_SetVolume(MP3.ucVolume);
                printf("+ Volume = %d\r\n", MP3.ucVolume);
                break;
			case 'M':
                //if(MP3.ucVolume <= 244)MP3.ucVolume += 10;
                vs1053_SetVolume(244);
                printf("MAX Volume = %d\r\n", MP3.ucVolume);
                break;
            case '-':
                if(MP3.ucVolume >= 10)
					MP3.ucVolume -= 10;
					//MP3.ucVolume ++;
                vs1053_SetVolume(MP3.ucVolume);
                printf("- Volume = %d\r\n", MP3.ucVolume);
                break;
			case 'Y':////////////////// Deshabilitado start
				printf("??Y - TestSignal ????????\r\n");
				vs1053_TestSine();
				vs1053_TestSine();
				vs1053_TestSine();
				vs1053_TestSine();
				break;
			case 'Q':
				printf("??Q - TestSineExit ???????????\r\n");
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
				break;////////////////// Deshabilitado END
            case 'T':
                printf("??T - ???????\r\n");
                RtcRead(SYS_RTC);
                printf("20%02x-%02x-%02x %02x %02x:%02x:%02x\r\n",SYS_RTC->year,SYS_RTC->month,SYS_RTC->day,SYS_RTC->week,SYS_RTC->hour,SYS_RTC->minute,SYS_RTC->second);
                printf("%08x\r\n",SCB->CPUID);
                //get_cpuid();
                break;////////
			case 'U':
                printf("??U - ???????\r\n");
				Auto_adjust_time();
                RtcRead(SYS_RTC);
                printf("20%02x-%02x-%02x %02x %02x:%02x:%02x\r\n",SYS_RTC->year,SYS_RTC->month,SYS_RTC->day,SYS_RTC->week,SYS_RTC->hour,SYS_RTC->minute,SYS_RTC->second);
                printf("%08x\r\n",SCB->CPUID);
                //get_cpuid();
                break;
			case 'z':
               printf("==SWIN %d \r\n", (GPIOC->IDR & 0x000f));
			   rin_test=((GPIOC->IDR & 0x2000)? 0:1);   		   //rin_test=((GPIOC->IDR & 0x2000));
			   printf("==RIN  %x \r\n", rin_test);
			   gin_test=((GPIOC->IDR & 0x4000)? 0:1);			   //gin_test=((GPIOC->IDR & 0x4000));
			   printf("==GIN  %x \r\n", gin_test);
			   ain_test=((GPIOC->IDR & 0x8000)? 0:1);			   //ain_test=((GPIOC->IDR & 0x8000));
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
               break;			////////
            default:
                DispMenu();
                break;
        }
    }
}


/* Private function implementation */

static void DispMenu(void) /* Muestra por UART las opciones de prueba disponibles para el sistema de archivos. */
{
	printf("\r\n------------------------------------------------\r\n");
	printf("?????????????????0????SPI Flash?????\r\n");
	printf("????????????:\r\n");
	printf("0 - ??SPI_Flash??????????????\r\n");
	printf("1 - ??????????????????\r\n");
	printf("2 - ????????????armfly.txt\r\n");
	printf("3 - ??armfly.txt?????????\r\n");
	printf("4 - ??????\r\n");
	printf("5 - ??????????\r\n");
	printf("6 - ??????????????\r\n");
  printf("7 - ????WAV????\r\n");
}
