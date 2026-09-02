#include "stm32f10x.h" /* Modulo principal de coordinacion del firmware. */

#include "usb_hw.h"
#include "usb_pwr.h"

#include "spi_w5500_eth.h"
#include "uart_consola.h"

__IO uint8_t reg1ms_flag; /* Marca generada cada milisegundo por SysTick. */
__IO uint8_t ten_mm_counter; /* Cuenta los milisegundos que forman una ventana de 10 ms. */
__IO uint8_t system_error = 0; /* Indicador global de error del sistema. */
__IO uint16_t reg1ms_count = 1000; /* Contador de milisegundos usado para tareas periodicas de 1 segundo. */

uint8_t StartFlag = 1;

static void SysTick_Init(void);
void Delay(__IO uint32_t nTime);
//void SocketProcess(void);

int main(void){ /* Inicializa todos los perifericos y ejecuta el planificador cooperativo. */
    
//    if(FLASH_GetReadOutProtectionStatus() != SET)
//    {
//        FLASH_Unlock();
//        FLASH_ReadOutProtection(ENABLE);
//        FLASH_Lock();
//    }
    
    /* Inicializacion de reloj de sistema y perifericos de la aplicacion. */
    SysTick_Init();
    bsp_GpioInit();
    vs1053_IO_Init();//encargado de el audio sonido
    
    bsp_InitUart();
    
    ADC_Inits();
    bsp_InitI2C();
    bsp_InitSpi1Bus();
    bsp_InitSpiFlash();		//��ʼ��SPI_Flash
    W5500_GPIO_Config(); //Puerto de comunicacion 
    
    bsp_InitSpi2Bus();
    mp3_par_init(); 
    vs1053_HardInit(); 
    
    Config();
    CheckVolume();
    
    W5500_Hardware_Reset();    /* Reinicia y configura el controlador Ethernet W5500. */
	W5500_Initialization();

    bsp_InitIwdg(3000);
	
	nRF24L01ioConfig(); /* Deja el transceptor de radio en un estado conocido antes de recibir. */
	SPI_RW_Reg(FLUSH_RX,0xff);
	SPI_RW_Reg(FLUSH_TX,0xff);
	
	RX_Mode(); /* Configura el nRF24L01 en modo receptor. */
	rf24l01_irq_init();
	pps_irq_init();

    vs1053_TestSine();//si no suena al comienzo no reproduce audios

    while(1)    /* Bucle principal: todas las tareas se ejecutan sin bloquear el sistema. */
    {
        if(reg1ms_flag) /* Las tareas temporizadas se ejecutan una vez por cada tick de 1 ms. */
        {
            reg1ms_flag = 0;
            if(MP3.Writingflag == 0)    /* Durante la escritura de parametros se pausa el procesamiento de entradas. */
            {
                ain_filterAC_DC();
                filterAC_DC();
                if(++ten_mm_counter >= 10)  /* Agrupa diez ticks para las tareas de filtrado y visualizacion de 10 ms. */
                {
                    ten_mm_counter = 0;
                    study_mode_filterAC_DC();
                    study_mode_time_calculation();
                    down_time_display();
                    // if(MP3.PushbuttonCount) // Deshabilito ya que no hay Relay en el sistema de prueba
                    // {
                    //     if(--MP3.PushbuttonCount > 0)
                    //         RELAY_ON();
                    //     else
                    //         RELAY_OFF();
                    // }
                }
            }
            
            if(MP3.writeParFlag)    /* Guarda en Flash los parametros recibidos y actualiza el reloj. */
            {
                //printf("MP3.writeParFlag\r\n");
                MP3.writeParFlag = 0;
                RtcWrite((RtcType*)Par);
				
                WriteConfigFile(&Par[7], 12, 40);//дʱ��+ʱ������
                //Config();
                if(ReadConfigFile())//�������ļ��ɹ�
                    Load_Period_Parameters(&FileBuf[12]); 
                system_temp.timeUpdate = 1;
                MP3.fileChangeFlag = 1;
            }
			
            if((reg1ms_count%10)==0) 
			{
				CheckVolume();
				IWDG_Feed();
			}
			
            if(lamp_chge_flag)  /* Al cambiar el estado de las luces selecciona el audio correspondiente. */
            {
                lamp_chge_flag = 0;
//                if(MP3.fileOpenFlag == 0)
//                {
//                    memset(MP3.filename,0x00,13);
//                    (MP3.dir);
//                    MP3.fileChangeFlag = 1;
//                }
                if(lamp_status == RS)
                {
                    MP3.dir = 0;//MP3.dir = 2;
                }
                else if(lamp_status == GS)
				{
					//if(gre_flash_flag == 0) MP3.dir = 3; else MP3.dir = 4;
					MP3.dir = 2;
				}
				else if(lamp_status == BS)
				{
					MP3.stopFlag = 1;
				}
                memset(MP3.filename,0x00,13);
                get_filename(MP3.dir);
                MP3.fileChangeFlag = 1;
                MP3.lamp_chge_sound_flag = 1;
            }
            fileChange();   /* Aplica los cambios pendientes de archivo y reproduce el aviso necesario. */
            if(++reg1ms_count >= 1000) //Cada 1 segundo
            {
                reg1ms_count = 0;
				MP3.WorkMode = MODEA;
                if(fileTrans.timeCount)
                {
                    if(++fileTrans.timeCount >= 5)
                    {
                        fileTrans.timeCount = 0;
                        MP3.Writingflag = 0;
                        MP3.fileChangeFlag = 1;
                    }
                }
				system_temp.seconds++;
                IWDG_Feed();
                LED_Toggle();

                ////YO imprimir
                printf("> AMAR ST %d \r\n", ain.stab_state); //YO Imprimir estado AIN estable
                printf("> ROJO ST %d \r\n", lamp_state[0]); //YO Imprimir estados rojo estable
                printf("> VERD ST %d \r\n", lamp_state[1]); //YO Imprimir estados verde estable
                if(lamp_status == RS)//YO Imprimir estados BS = 0, RS = 1, GS = 2)
                    printf("> LS=RS >LCF %x \r\n", lamp_chge_flag);//YO Imprimir estados
                else if(lamp_status == GS)
				    printf("> LS=GS >LCF %x \r\n", lamp_chge_flag);//YO Imprimir estados
				else if(lamp_status == BS)
                    printf("> LS=BS >LCF %x \r\n", lamp_chge_flag);//YO Imprimir estados
                printf("> gre_flash_flag %d \r\n", gre_flash_flag);//YO Imprimir estados
                //study_mode_filterAC_DC
                //printf(">>lamp_state[0] %d \r\n", display_data[1] );
                //printf(">>lamp_status %d \r\n", study_lamp_stab_state);
                //printf(">>gre_flash_flag %d \r\n", gre_flash_flag);
                ////
            }
            if(MP3.stopCount)
            {
                if(++MP3.stopCount >= 5)
                {
                    MP3.stopCount = 0;
                    if(MP3.cycleFlag)
                    {
                        if(MP3.fileOpenFlag)
                            MP3.playing = 1;
                        else
                            PlayStart();
                    }
                }
            }
            SocketProcess();    /* Atiende red, panel luminoso y pruebas periodicas del sistema. */
			flash_panel_control();
			
            SYS_TEST();
        }
        Playing();  /* La reproduccion debe atenderse continuamente, incluso entre ticks. */
		if(spi2_busy_flag == 0)
		{
			if(rf_int_flag == 1)    /* Procesa los paquetes de radio cuando SPI2 no esta ocupado. */
			{
				rf_int_flag = 0;
				rf24l01_rx_process(); //funcion que debe procesar informacion que llega por radio frecuencia
				//printf("/");
			}
			if(RF_IRQ()==0) rf24l01_rx_process();
		}
    }
}


static void SysTick_Init(void)  /* Configura SysTick a 1 kHz para generar la base de tiempo del firmware. */
{
    while(SysTick_Config(SystemCoreClock / 1000));
}
