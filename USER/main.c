#include "stm32f10x.h" /* Modulo principal de coordinacion del firmware. */

#include "usb_hw.h"
#include "usb_pwr.h"

__IO uint8_t reg1ms_flag; /* Marca generada cada milisegundo por SysTick. */
__IO uint8_t ten_mm_counter; /* Cuenta los milisegundos que forman una ventana de 10 ms. */
__IO uint8_t system_error = 0; /* Indicador global de error del sistema. */
__IO uint16_t reg1ms_count = 1000; /* Contador de milisegundos usado para tareas periodicas de 1 segundo. */

uint8_t StartFlag = 1;

static void SysTick_Init(void);
void Delay(__IO uint32_t nTime);
void SocketProcess(void);
// int contador = 0;
// int i = 0;

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
	
    // //printf("START START ======= Consola Serie iniciada en Keil v5 ======= \r\n"); // // Mensaje inicial por consola
    // while (1) {
    //     printf("Lectura de sistema #%d\r\n", contador++);
    //     for (i = 0; i < 2000000; i++);// Retardo simple de prueba
    //     IWDG_Feed();
    //     LED_Toggle();
    // }

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

/*
 * Procesa una trama recibida por UDP o TCP.
 * UDP se utiliza para descubrir/configurar la red; TCP transporta comandos
 * de la aplicacion. Las respuestas se construyen en Tx_Buffer.
 */
void Process_Socket_Data(SOCKET s)
{
	uint16_t size,i;
    uint8_t CheckSum,result;
	
	size = Read_SOCK_Data_Buffer(s, Rx_Buffer); /* Lee del W5500 la cantidad de bytes disponible en el socket. */
    
	if(s == 0)  /* Socket 0: mensajes UDP de identificacion y configuracion de red. */
	{
		if(Rx_Buffer[8] == 0x55 && Rx_Buffer[9] == 0xbb)    /* 0x55BB solicita la informacion de red y el identificador del equipo. */
        {
            if(Rx_Buffer[0]==Net.IP_Addr[0] && Rx_Buffer[1]==Net.IP_Addr[1] && Rx_Buffer[2]==Net.IP_Addr[2])
            {
                Socket[s].UdpDIPR[0] = Rx_Buffer[0];
                Socket[s].UdpDIPR[1] = Rx_Buffer[1];
                Socket[s].UdpDIPR[2] = Rx_Buffer[2];
                Socket[s].UdpDIPR[3] = Rx_Buffer[3];
                Socket[s].UdpDestPort = 7788;
            }
            else
            {
                Socket[s].UdpDIPR[0] = 255;
                Socket[s].UdpDIPR[1] = 255;
                Socket[s].UdpDIPR[2] = 255;
                Socket[s].UdpDIPR[3] = 255;
                Socket[s].UdpDestPort = 7788;
            }
            Tx_Buffer[0] = 0x55;
            Tx_Buffer[1] = 0xCC;
            Tx_Buffer[2] = Net.IP_Addr[0];
            Tx_Buffer[3] = Net.IP_Addr[1];
            Tx_Buffer[4] = Net.IP_Addr[2];
            Tx_Buffer[5] = Net.IP_Addr[3];
            
            Tx_Buffer[6] = Net.Gateway_IP[0];    
            Tx_Buffer[7] = Net.Gateway_IP[1];
            Tx_Buffer[8] = Net.Gateway_IP[2];
            Tx_Buffer[9] = Net.Gateway_IP[3];
            
            Tx_Buffer[10] = Net.Sub_Mask[0];      
            Tx_Buffer[11] = Net.Sub_Mask[1];
            Tx_Buffer[12] = Net.Sub_Mask[2];
            Tx_Buffer[13] = Net.Sub_Mask[3];
            
            get_cpuid(&Tx_Buffer[14]);
            
            Tx_Buffer[18] = 0x00;
            for(i=2;i<18;i++)
                Tx_Buffer[18] += Tx_Buffer[i];
            Write_SOCK_Data_Buffer(s, Tx_Buffer, 19);
        }
		else if(Rx_Buffer[8] == 0x55 && Rx_Buffer[9] == 0xdd) /* 0x55DD actualiza la configuracion IP si la suma de comprobacion es valida. */
        {
            CheckSum = 0;
            for(i=10;i<22;i++) CheckSum += Rx_Buffer[i];
            if(CheckSum == Rx_Buffer[22])
            {
                WriteConfigFile(&Rx_Buffer[10], 0, 12);
                Config();
                
                Tx_Buffer[0] = 0x55;
                Tx_Buffer[1] = 0xEE;
                Socket[s].UdpDestPort = 7788;
                Write_SOCK_Data_Buffer(s, Tx_Buffer, 2);
                
                W5500_Initialization();
                //printf("new ip = %d.%d.%d.%d\r",Net.IP_Addr[0],Net.IP_Addr[1],Net.IP_Addr[2],Net.IP_Addr[3]);
            }
        }
	}
	else if(s==1)   /* Socket 1: canal TCP para comandos de control. */
	{
		//memcpy(Tx_Buffer, Rx_Buffer, size);
		//if(Check_Ifo(Rx_Buffer, (u8*)"BEEP_ON", strlen("BEEP_ON")))beep_count = 4;
		//else if(Check_Ifo(Rx_Buffer, (u8*)"BEEP_OFF", strlen("BEEP_OFF")))beep_count = 0;
        //Write_SOCK_Data_Buffer(s, Tx_Buffer, size);
        LED_Toggle();
		result = ReceiveProcess(Rx_Buffer, size);/* ReceiveProcess interpreta el comando y devuelve el tipo de respuesta. */
        if(result == 1) // RESPUESTA al recibir AUDIOS
        {
            Tx_Buffer[0] = 'O';
            Tx_Buffer[1] = 'K';
            Write_SOCK_Data_Buffer(s, Tx_Buffer, 2);
            LED_Toggle();
        }
        else if(result == 2) //RESPUESTA al SET.
        {
            Tx_Buffer[0] = 0x32;
            Tx_Buffer[1] = 0x60;
            memset(&Tx_Buffer[2], 0, 13);
            Tx_Buffer[15] = 0x04;
            Tx_Buffer[16] = 0x01;
            Tx_Buffer[17] = 0x00;
            Tx_Buffer[18] = 0x01;
            Tx_Buffer[19] = 0x00;
            Tx_Buffer[20] = 0x00;
            Tx_Buffer[21] = 0x00;
            Tx_Buffer[22] = 0xaa;
            Tx_Buffer[23] = 0xcc;
            Write_SOCK_Data_Buffer(s, Tx_Buffer, 24);
            LED_Toggle();
        }
        else if(result == 3) //RESPPUESTA al READ: ENVIA horarios y reloj
        {
            Tx_Buffer[0] = 0x32;
            Tx_Buffer[1] = 0x60;
            memset(&Tx_Buffer[2], 0, 13);
            Tx_Buffer[15] = 0x03;
            Tx_Buffer[16] = 0x01;
            Tx_Buffer[17] = 0x00;
            Tx_Buffer[18] = 0x01;
            Tx_Buffer[19] = 0x00;
            Tx_Buffer[20] = 47;
            Tx_Buffer[21] = 0x00;
            RtcRead(SYS_RTC);
            memcpy(&Tx_Buffer[22], rtc, 7); //22-28
			memcpy(&Tx_Buffer[29], system_temp.TimeZone, 4); //29-32
            memcpy(&Tx_Buffer[33], Time_Volume, 36); //33-64                //29-64
            Tx_Buffer[65] = 0xaa;//Tx_Buffer[69] = 0xaa;
            Tx_Buffer[66] = 0xcc;//Tx_Buffer[70] = 0xcc;
            Write_SOCK_Data_Buffer(s, Tx_Buffer, 67);//Write_SOCK_Data_Buffer(s, Tx_Buffer, 71);
            LED_Toggle();
			printf_fifo_hex(system_temp.TimeZone, 4);
        }
	}
}

void SocketProcess(void) /* Actualiza sockets, procesa interrupciones del W5500 y entrega tramas recibidas. */
{
    SOCKET n;
    W5500_Socket_Set(0);/* Prepara los sockets de trabajo y atiende sus eventos pendientes. */
    W5500_Socket_Set(1);
    //W5500_Socket_Set(2);
    W5500_Interrupt_Process();
    
    for(n=0;n<8;n++)    /* Recorre los ocho sockets disponibles en el controlador. */
    {
        if((Socket[n].DataState & S_RECEIVE) == S_RECEIVE)
        {
            Socket[n].DataState &= ~S_RECEIVE;  /* La bandera se limpia antes de procesar para evitar repetir la trama. */
            Process_Socket_Data(n);
        }
        else if(W5500_Send_Delay_Counter[n] >= 5000)
        {
            if(Socket[n].State == (S_INIT|S_CONN))
            {
                Socket[n].DataState &= ~S_TRANSMITOK;
                //memcpy(Tx_Buffer, netaddr, strlen(netaddr));	
            }
            W5500_Send_Delay_Counter[n] = 0;
        }
    }
}

static void SysTick_Init(void)  /* Configura SysTick a 1 kHz para generar la base de tiempo del firmware. */
{
    while(SysTick_Config(SystemCoreClock / 1000));
}
