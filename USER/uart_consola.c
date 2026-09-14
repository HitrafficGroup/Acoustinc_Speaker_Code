#include "uart_consola.h"
#include <stdio.h>
#include <string.h>
#include "ff.h"
#include "io_functions.h"  /* Add this include */
#include "spi_w5500_eth.h"    /* Ensures access to MP3, fileTrans, and other external definitions */
#include "spi_flash_fatfs.h"  /* Resolves FATFS file operation warnings */
#include "bsp_vs1053b.h"           /* Replace with the actual header that contains PlaySound() */
#include "uart_fifo_gps.h"

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

static uint8_t day, slot;//para visualizacion TIME_VOLUME MATRIX
static const char *day_labels[2] = {"Weekday (Index 0)", "Weekend (Index 1)"};//para visualizacion TIME_VOLUME MATRIX

/* Public function implementation */

void SYS_TEST(void)
{
	uint8_t cmd;
    if(UartGetChar(&cmd))
    {
        switch(cmd)//cmd = 'C';
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
                PlaySound(sysfile0);
                break;
            case 'G':
                printf("??G - PlaySound??\r\n");
                PlaySound("001.WAV");
                break;
            case 'H':
                printf("??H - PlaySound??\r\n");
                PlaySound("002.WAV");
                break;
            case 'I':
                printf("??I - PlaySound??\r\n");
                memset(MP3.filename,0x00,13);
                memcpy(MP3.filename,"002.MP3",7);
                PlayStart();
                break;
            case 'J':
                printf("??J - MP3.dir = %d\r\n",MP3.dir);
                break;
            case '+':
                printf("Volume = %d\r\n", MP3.ucVolume);
                if(MP3.ucVolume <= 255) //MP3.ucVolume <= 244
                    MP3.ucVolume += 10;
                vs1053_SetVolume(MP3.ucVolume);
                printf("??+ Volume = %d\r\n", MP3.ucVolume);
                break;
			case 'K'://Volumen MAXIMO
                printf("Volume = %d\r\n", MP3.ucVolume);
                MP3.ucVolume = 255;
                vs1053_SetVolume(MP3.ucVolume);
                printf("??K - MAX Volume = %d\r\n", MP3.ucVolume);
                break;
            case 'L'://Muestra el volumen actual
                printf("??L - Volume Clac = %d\r\n", clac_Volume());
                break;
            case '-':
                printf("Volume = %d\r\n", MP3.ucVolume);
                if(MP3.ucVolume >= 10)
					MP3.ucVolume -= 10;
					//MP3.ucVolume ++;
                vs1053_SetVolume(MP3.ucVolume);
                printf("??- Volume = %d\r\n", MP3.ucVolume);
                break;
			case 'M'://Test del modulo vs1053  //////////////// Deshabilitado start
				printf("??M - TestSignal ????????\r\n");
				vs1053_TestSine();
				vs1053_TestSine();
				vs1053_TestSine();
				vs1053_TestSine();
				break;
			case 'N'://Test del modulo vs1053 FIN
				printf("??N - TestSineExit ???????????\r\n");
				vs1053_TestSineExit();
				break;
            case 'a'://Lee los horarios H:m y Volumen
					printf("\r\n??a - ??????????? TIME_VOLUME MATRIX ===================\r\n");/////Muestra como se han cargado los valores de Time_Volume desde Config.ini
                    for (day = 0; day < 2; day++)
                    {
                        printf("\r\n--- %s ---\r\n", day_labels[day]);
                        printf("Slot | Hour (BCD/DEC) | Min (BCD/DEC) | Vol Byte (Hex/Dec)\r\n");
                        printf("----------------------------------------------------------\r\n");
                        for (slot = 0; slot < 6; slot++)
                        {
                            uint8_t raw_hour = Time_Volume[day][slot][0];
                            uint8_t raw_min  = Time_Volume[day][slot][1];
                            uint8_t raw_vol  = Time_Volume[day][slot][2];
                            printf("  %d  |   0x%02X (%02d)    |   0x%02X (%02d)   |   0x%02X (%3d)\r\n",
                                slot,
                                raw_hour, BCD_to_DEC(raw_hour),
                                raw_min,  BCD_to_DEC(raw_min),
                                raw_vol,  raw_vol);
                        }
                    }
                    printf("\r\n==========================================================\r\n\r\n");
				break;
            case 'b': //Muestra los bytes TimeZone
                printf("??b - TimeZone ???????????\r\n");
                printf_fifo_hex(system_temp.TimeZone, 4); //Imprime en Hexa los 4 bytes de TimeZone //printf("system_temp.TimeZone = %d \r\n", system_temp.TimeZone); //Imprime en Decimal los 4 bytes de TimeZone
                break;
			case 'c':	//Prueba deshabilitada para fijar manualmente la fecha y hora del RTC.
                printf("??c - Fijar fecha y hora del RTC\r\n");
				SYS_RTC->second  = 0x00;
				SYS_RTC->minute  = 0x21;
				SYS_RTC->hour    = 0x15;
				SYS_RTC->week    = 0x02;
				SYS_RTC->day     = 0x08;
				SYS_RTC->month   = 0x12;
				SYS_RTC->year    = 0x20;
				RtcWrite(SYS_RTC);
				break;////////////////// Deshabilitado END
            case 'd': //Leer hora RTC
                printf("??d - Lee RTC ???????\r\n");
                RtcRead(SYS_RTC);
                printf("20%02x-%02x-%02x %02x %02x:%02x:%02x\r\n",SYS_RTC->year,SYS_RTC->month,SYS_RTC->day,SYS_RTC->week,SYS_RTC->hour,SYS_RTC->minute,SYS_RTC->second);
                printf("%08x\r\n",SCB->CPUID);
                //get_cpuid();
                break;////////
			case 'e': //Lee GPS
                printf("??e - Lee GPS ???????\r\n");
                printf("1.system_temp.gps_flag: %d\r\n",system_temp.gps_flag);//Bandera muestra que el gps esta funcionando OK
                printf("2.system_temp.sync_with_gps_flag: %d\r\n", system_temp.sync_with_gps_flag);//Bandera indica se puede actualizar desde GPS
                printf("GPS UTC: %d-%d-%d %d:%d:%d\r\n", system_temp.Gps.utc.year, system_temp.Gps.utc.month, system_temp.Gps.utc.day, system_temp.Gps.utc.hour, system_temp.Gps.utc.minute, system_temp.Gps.utc.second);
                printf("GPS LOCAL: system_temp.Gps.local.second = ");   //printf("GPS LUT: %d, %d, %d \r\n", &system_temp.Gps.local, &system_temp.Gps.utc, system_temp.TimeZone); //system_temp.seconds, system_temp.gps_seconds
                printf_fifo_hex(&system_temp.Gps.local.second, 7);
                break;
            case 'f': //Sincroniza hora RTC con el GPS
                printf("??f - Sincroniza RTC con GPS ???????\r\n");
				Auto_adjust_time(); //Sincroniza hora con el GPS
                RtcRead(SYS_RTC);
                printf("20%02x-%02x-%02x %02x %02x:%02x:%02x\r\n",SYS_RTC->year,SYS_RTC->month,SYS_RTC->day,SYS_RTC->week,SYS_RTC->hour,SYS_RTC->minute,SYS_RTC->second);
                printf("%08x\r\n",SCB->CPUID);
                //get_cpuid();
                break;
			case 'V': //Test inputs estables
                printf("??V - STABLE INPUTS Y R G\r\n");
                printf("> AMAR ST %d \r\n", ain.stab_state); //Imprimir estado AIN estable
                printf("> ROJO ST %d \r\n", lamp_state[0]); //Imprimir estados rojo estable
                printf("> VERD ST %d \r\n", lamp_state[1]); //Imprimir estados verde estable

                if(lamp_status == RS)//YO Imprimir estados BS = 0, RS = 1, GS = 2)
                    printf("> LS=RS >LCF %x \r\n", lamp_chge_flag);//Imprimir estados
                else if(lamp_status == GS)
				    printf("> LS=GS >LCF %x \r\n", lamp_chge_flag);//Imprimir estados
				else if(lamp_status == BS)
                    printf("> LS=BS >LCF %x \r\n", lamp_chge_flag);//Imprimir estados
                printf("> gre_flash_flag %d \r\n", gre_flash_flag);//Imprimir estados
                //study_mode_filterAC_DC
                //printf(">>lamp_state[0] %d \r\n", display_data[1] );
                //printf(">>lamp_status %d \r\n", study_lamp_stab_state);
               break;
            case 'W': //Test inputs actuales
            printf("??W - INPUTS valor actual\r\n");
               printf("SWIN %d \r\n", (GPIOC->IDR & 0x000f));
			   rin_test=((GPIOC->IDR & 0x2000)? 0:1);   		   //rin_test=((GPIOC->IDR & 0x2000));
			   printf("> RIN  %x \r\n", rin_test);
			   gin_test=((GPIOC->IDR & 0x4000)? 0:1);			   //gin_test=((GPIOC->IDR & 0x4000));
			   printf("> GIN  %x \r\n", gin_test);
			   ain_test=((GPIOC->IDR & 0x8000)? 0:1);			   //ain_test=((GPIOC->IDR & 0x8000));
			   printf("> AIN  %x \r\n", ain_test);
               break;
			case 'X':
			   printf("??X - DR1 ?Toggle \r\n");
               DR1_Toggle();
               break;
			case 'Y':
			   printf("??Y - DR2 ?Toggle \r\n");
               DR2_Toggle();
               break;
			case 'Z':
			   printf("??Z - DR3 ?Toggle \r\n");
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
