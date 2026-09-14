#include "spi_flash_fatfs_config.h"
//#include "spi_flash_fatfs_mp3.h"
#include "stm32f10x.h" 
#include "ff.h"			
#include "spi_w5500_eth.h"
#include "spi_flash_fatfs.h"
//#include <stdio.h>
//#include <string.h>

//Extern dependencies from spi_flash_fatfs.c External Variable & Function Declarations for Keil uVision Compatibility
extern FATFS fs;
extern FIL file;
extern const char * FR_Table[];
extern uint8_t Time_Volume[2][6][3];

extern uint8_t FileBuf[BUF_SIZE];
extern NET Net;
extern SOCKET_TYPE Socket[8];

/* Extern function prototypes to avoid implicit declaration warnings */
extern void Delay(__IO uint32_t nTime);
extern void printf_fifo_hex(uint8_t *pdata, uint8_t len);

/* Functions and Variables */
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


/* Valores predeterminados de red, horario y volumen almacenados en Config.ini. */
// const uint8_t DefaultConfig[52]={
// 	192,168, 1,172,     192,168, 1, 1,      255,255,255, 0,//12 IP: Local IP Address (192.168.1.172), 192, 168, 1, 1: Default Gateway (192.168.1.1), 255, 255, 255, 0: Subnet Mask (255.255.255.0).
// 	0x01, 0x00, 0x70, 0x80, //4 China Timezone (UTC+8) in 4 bytes
//     0x00,0x00,0xe7,	    0x09,0x00,0xe7,		0x13,0x00,0xe7,		0x16,0x00,0xe7,		0x21,0x00,0xe7,		0x23,0x59,0xe7,//???????   18
//     0x00,0x00,0xe7,		0x09,0x00,0xe7,     0x13,0x00,0xe7,	    0x16,0x00,0xe7,		0x21,0x00,0xe7,		0x23,0x59,0xe7,//??????   18
// };
// const uint8_t DefaultConfig[52]={	//Lee bien desde la app
// 	192,168, 1,172,     192,168, 1, 1,      255,255,255, 0,//12 IP: Local IP Address (192.168.1.172), 192, 168, 1, 1: Default Gateway (192.168.1.1), 255, 255, 255, 0: Subnet Mask (255.255.255.0).
//     0x00,0x00,0xe7,	    0x09,0x00,0xe7,		0x13,0x00,0xe7,		0x16,0x00,0xe7,		0x21,0x00,0xe7,		0x23,0x59,0xe7,//???????   18
//     0x00,0x00,0xe7,		0x09,0x00,0xe7,     0x13,0x00,0xe7,	    0x16,0x00,0xe7,		0x21,0x00,0xe7,		0x23,0x59,0xe7,//??????   18
// 	0xaa, 0xcc, 0x00, 0x00,
// };
// const uint8_t DefaultConfig[52]={	//Lee bien desde la app
// 	192,168, 1,172,     192,168, 1, 1,      255,255,255, 0,//12 IP: Local IP Address (192.168.1.172), 192, 168, 1, 1: Default Gateway (192.168.1.1), 255, 255, 255, 0: Subnet Mask (255.255.255.0).
//     0x00,0x00,0xe7,	    0x09,0x00,0xe7,		0x13,0x00,0xe7,		0x16,0x00,0xe7,		0x21,0x00,0xe7,		0x23,0x59,0xe7,//???????   18
//     0x00,0x00,0xe7,		0x09,0x00,0xe7,     0x13,0x00,0xe7,	    0x16,0x00,0xe7,		0x21,0x00,0xe7,		0x23,0x59,0xe7,//??????   18
// };
const uint8_t DefaultConfig[52]={	//Lee bien desde la app
	192,168, 1,172,     192,168, 1, 1,      255,255,255, 0,//12 IP: Local IP Address (192.168.1.172), 192, 168, 1, 1: Default Gateway (192.168.1.1), 255, 255, 255, 0: Subnet Mask (255.255.255.0).
    0x00,0x00,0xe7,	    0x09,0x00,0xe7,		0x13,0x00,0xe7,		0x16,0x00,0xe7,		0x21,0x00,0xe7,		0x23,0x59,0xe7,//???????   18
    0x00,0x00,0xe7,		0x09,0x00,0xe7,     0x13,0x00,0xe7,	    0x16,0x00,0xe7,		0x21,0x00,0xe7,		0x23,0x59,0xe7,//??????   18
    0x00, 0x00, 0x46, 0x50, //4 Ecuador Timezone (UTC-5) in 4 bytes
};
//static void CreateConfigFile(void)
void CreateConfigFile(void)
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


void Config(void)
{
    while(ReadAndCheckConfigFile() != 1)
    {
        Delay(1000);
    }
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
            printf("%02x ",FileBuf[i]); //Print primera linea de Config.ini en Hexa 
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


void DeleteConfigFile(void) //YO revisar y programar bien
{
// //	char ConfigFile[] = "/sys/Config.ini";
// 	char ConfigFileDir[] = "/sys";

// 	/* Elimina archivos y directorios de prueba, comprobando cada resultado de FatFS. */
// 	FRESULT result;
// //	char FileName[13];
// //	uint8_t i;

//  	/* ????????? */
// 	result = f_mount(&fs, "0:", 0);			/* Mount a logical drive */
// 	if (result != FR_OK)
// 	{
// 		printf("Unidad Montada (%s)\r\n",  FR_Table[result]);
// 	}

// 	#if 0
// 	/* ???????? */
// 	result = f_opendir(&DirInf, "/"); /* ??????????????????????? */
// 	if (result != FR_OK)
// 	{
// 		printf("????????(%s)\r\n",  FR_Table[result]);
// 		return;
// 	}
// 	#endif

// 	/* ?????/Dir1 ?????????????????????????)??????????????????*/
// 	result = f_unlink(ConfigFileDir);
// 	if (result == FR_OK)
// 	{
// 		printf("FR_OK ?????Dir1???\r\n");
// 	}
// 	else if (result == FR_NO_FILE)
// 	{
// 		printf("FR_NO_FILE ??锟斤拷?????????? :%s\r\n", "/Dir1");
// 	}
// 	else
// 	{
// 		printf("ELSE_FR ???Dir1???(??????? = %s) ?????????????\r\n",  FR_Table[result]);
// 	}
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
	// uint8_t day, slot;//para visualizacion
	// const char *day_labels[2] = {"Weekday (Index 0)", "Weekend (Index 1)"};//para visualizacion
	
	memcpy(Time_Volume, pdata, 36);//copia de pdata [Config.ini] los primeros 36 bytes	CHECK//	memcpy(Time_Volume, pdata+4, 36);
    memcpy(system_temp.TimeZone, pdata+36, 4); //memcpy(system_temp.TimeZone, pdata, 4); Toma los 4 bytes de TimeZone desde pdata [Config.ini] y los copia en system_temp.TimeZone
	
    // printf("system_temp.TimeZone = %d \r\n", system_temp.TimeZone); //Imprime en Decimal los 4 bytes de TimeZone
	// printf_fifo_hex(system_temp.TimeZone, 4); //Imprime en Hexa los 4 bytes de TimeZone
	// /////Muestra como se han cargado los valores de Time_Volume desde Config.ini
	// printf("\r\n=================== TIME_VOLUME MATRIX ===================\r\n");
    // for (day = 0; day < 2; day++)
    // {
    //     printf("\r\n--- %s ---\r\n", day_labels[day]);
    //     printf("Slot | Hour (BCD/DEC) | Min (BCD/DEC) | Vol Byte (Hex/Dec)\r\n");
    //     printf("----------------------------------------------------------\r\n");

    //     for (slot = 0; slot < 6; slot++)
    //     {
    //         uint8_t raw_hour = Time_Volume[day][slot][0];
    //         uint8_t raw_min  = Time_Volume[day][slot][1];
    //         uint8_t raw_vol  = Time_Volume[day][slot][2];

    //         printf("  %d  |   0x%02X (%02d)    |   0x%02X (%02d)   |   0x%02X (%3d)\r\n",
    //                slot,
    //                raw_hour, BCD_to_DEC(raw_hour),
    //                raw_min,  BCD_to_DEC(raw_min),
    //                raw_vol,  raw_vol);
    //     }
    // }
    // printf("\r\n==========================================================\r\n\r\n"); // //////
}
