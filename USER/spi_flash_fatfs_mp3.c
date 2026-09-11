#include "spi_flash_fatfs_mp3.h"
#include "spi_flash_fatfs.h"
#include "stm32f10x.h"
#include "ff.h"
//#include <stdio.h>
//#include <string.h>
//#include "bsp_vs1053b.h"  /* Provides vs1053_* prototypes */
//#include "spi_w5500_eth.h"

/* Global MP3 Variable Definition */
MP3_T MP3;

/* File handle and buffer dedicated to MP3 operations */
FIL mfile;
static unsigned char fileBuf[128];/* Private/Local Audio Data Buffer required by Mp3Pro*/

/* External Dependencies from FATFS and System */
//extern FIL mfile;
//extern const char * FR_Table[];
extern FATFS fs;
extern uint8_t Time_Volume[2][6][3];
extern uint8_t StartFlag;

/* External system functions called within MP3 functions */
//extern void LED_Toggle(void);
//extern void vs1053_SoftReset(void);
//extern uint8_t vs1053_ReqNewData(void);
//extern void vs1053_PreWriteData(void);
//extern void vs1053_WriteDatas(uint8_t *buf, uint16_t len);
//extern void vs1053_SetVolume(uint8_t ucVol);
//extern void RtcRead(void *rtc_struct);
//extern uint8_t BCD_to_DEC(uint8_t bcd);
//extern void PA_ON(void);
//extern void PA_OFF(void);

/* Function Definitions */
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

/* Entrega bloques de 32 bytes al VS1053 cuando el decodificador solicita datos. */
const char zerobuf[32] = "\0\0\0\0\0\0\0\0\0\0\0";
//static uint8_t Mp3Pro(void)
uint8_t Mp3Pro(void)
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
        memcpy(MP3.filename,"001.mp3",7);
    else if(num == 1)
        memcpy(MP3.filename,"002.mp3",7);
    else if(num == 2)
        memcpy(MP3.filename,"003.mp3",7);
    else if(num == 3)
        memcpy(MP3.filename,"004.mp3",7);
    else if(num == 4)
        memcpy(MP3.filename,"005.mp3",7);
}


uint8_t clac_Volume(void)
{
	uint8_t value,i;
	uint16_t day_by_mins;
	uint16_t plan_by_mins;

    RtcRead(SYS_RTC);   //Actualiza variable SYS_RTC con reloj del sistema --PRUEBA llamada clac_Volume
	day_by_mins = BCD_to_DEC(SYS_RTC->hour)*60+BCD_to_DEC(SYS_RTC->minute);
	if((SYS_RTC->week==0)||(SYS_RTC->week==6))  //Weekend Period (Sunday week==0 and Saturday week==6)
	{
		for(i=0;i<6;i++) //Recorre los 6 horarios para el fin de semana y determina el volumen correspondiente al horario actual.
		{
			plan_by_mins=BCD_to_DEC(Time_Volume[1][i][0])*60+BCD_to_DEC(Time_Volume[1][i][1]); //
			if(day_by_mins<plan_by_mins)    //Compara hora actual < con la de los horarios
			{
				if(i==0)
                    value=0;
				else
                    value=Time_Volume[1][i-1][2];   //get Volumen 0 - 7
				break;
			}
			else    //Compara hora actual > con la de los horarios
			{
				if(i==5){
                    value=0;
                    break;
                }
				else
                    continue;
			}
		}
	}
	else    //WeekDay Period (Monday week==1, Tuesday week==2, Wednesday week==3, Thursday week==4, Friday week==5)
	{
		for(i=0;i<6;i++) //Recorre los 6 horarios para el dia de semana y determina el volumen correspondiente al horario actual.
		{
			plan_by_mins=BCD_to_DEC(Time_Volume[0][i][0])*60+BCD_to_DEC(Time_Volume[0][i][1]);
            //printf("i = %d - PLANmin = %d - VAL = [%d][0x%02X] \r\n", i, plan_by_mins, Time_Volume[0][i][2], Time_Volume[0][i][2]);
			if(day_by_mins<plan_by_mins)    //Compara hora actual < con la de los horario
			{
				if(i==0)
                    value=0;
				else
                    value=Time_Volume[0][i-1][2];   //get Volumen 0 - 7
				break;
			}
            else        //Compara hora actual > con la de los horarios
			{
				if(i==5){
                    value=0;
                    break;
                }
				else
                    continue;
			}
		}
	}
	if(value>0xe0)
        //value = 184 + ((value-0xe0)*10);//Original  //1-7   254
        if(value==0xe1) //Reestructuracion de volumen
            value=195;
        else if(value==0xe2)
            value=205;
        else if(value==0xe3)
            value=215;
        else if(value==0xe4)
            value=225;
        else if(value==0xe5)
            value=235;
        else if(value==0xe6)
            value=245;
        else if(value==0xe7)
            value=255;
        else
            value=255;
    else
        value = 0;
    printf("DAYmin = %d \r\n", day_by_mins); //Muestra hora actual en minutos
    printf("#PLAN = %d - PLANmin = %d - VOL = %d \r\n",(i-1) , plan_by_mins, value); //Muestra PLAN seleccionado, PLAN en minutos y VOLUMEN
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
//				{   // Prueba deshabilitada para ajustar el volumen segun la entrada ambiental.
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
