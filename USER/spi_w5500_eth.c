#include "stm32f10x.h" /* Modulo principal de coordinacion del firmware. */
#include "spi_w5500_eth.h"

/* Memory allocation for the pack pointer */
PACK_T *pack;

/* External variables referenced across the project */
extern FIle_TRANS_T fileTrans;
extern MP3_T MP3;
extern NET Net;
extern SOCKET_TYPE Socket[8];

extern uint8_t Par[64];
extern uint8_t FileBuf[];
extern unsigned char Rx_Buffer[2048];
extern unsigned char Tx_Buffer[2048];
extern unsigned int W5500_Send_Delay_Counter[8];

extern uint8_t Time_Volume[2][6][3];
extern uint8_t rtc[7];

/* External function prototypes (ensure these match your existing definitions) */
extern void IWDG_Feed(void);
extern void WriteConfigFile(uint8_t* data, uint8_t pos, uint8_t len);
extern void Config(void);
extern void W5500_Initialization(void);
//extern void LED_Toggle(void);
//extern void LED_Toggle();
extern void get_cpuid(uint8_t *pdata);
//extern void RtcRead(void *rtc_struct);
extern void RtcRead(RtcType* Time);
extern void printf_fifo_hex(uint8_t *pdata, uint8_t len);

extern uint16_t Read_SOCK_Data_Buffer(SOCKET s, unsigned char *dat_ptr);
extern void Write_SOCK_Data_Buffer(SOCKET s, unsigned char *dat_ptr, unsigned short size);
extern void W5500_Socket_Set(SOCKET s);
extern void W5500_Interrupt_Process(void);

/* ------------------------------------------------------------------------- */


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
                return 1;//Recibe audio OK
            }
        }
        else if(pack->packType == 0x60)
        {
            if(pack->operaType == 0x01)//??????
            {
                return 3;//Recibe READ OK
            }
            else if(pack->operaType == 0x02)
            {
                memcpy(Par, &pack->data, 47);
                MP3.writeParFlag = 1;
                return 2;//Recibe SET OK
            }
        }
    }
    return 0;
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
        else if(result == 3) //RESPUESTA al READ: ENVIA horarios y reloj
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
            RtcRead(SYS_RTC);//lee 7 bytes
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
