/* Controlador del controlador Ethernet W5500. */
/**********************************************************************************
**********************************************************************************/

#include "stm32f10x.h"


#define W5500_SCS_PORT	GPIOC
#define W5500_SCS		GPIO_Pin_4	
#define W5500_SCS_Clr()	W5500_SCS_PORT->BRR = W5500_SCS
#define W5500_SCS_Set()	W5500_SCS_PORT->BSRR = W5500_SCS

#define W5500_RST_PORT	GPIOC
#define W5500_RST		GPIO_Pin_5	
#define W5500_RST_Clr()	W5500_RST_PORT->BRR = W5500_RST
#define W5500_RST_Set()	W5500_RST_PORT->BSRR = W5500_RST

#define W5500_INT		GPIO_Pin_0	
#define W5500_INT_PORT	GPIOB




NET             Net;
SOCKET_TYPE     Socket[8];


unsigned char Rx_Buffer[2048];	
unsigned char Tx_Buffer[2048];	

unsigned char W5500_Interrupt;	
unsigned int W5500_Send_Delay_Counter[8] = {0}; 

/*******************************************************************************
*******************************************************************************/
void W5500_GPIO_Config(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;	
    
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC | RCC_APB2Periph_AFIO, ENABLE);
	
	
	GPIO_InitStructure.GPIO_Pin  = W5500_RST;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(W5500_RST_PORT, &GPIO_InitStructure);
	W5500_RST_Clr();
	
	
	GPIO_InitStructure.GPIO_Pin  = W5500_INT;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(W5500_INT_PORT, &GPIO_InitStructure);
    
    
	GPIO_InitStructure.GPIO_Pin = W5500_SCS;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;
	GPIO_Init(W5500_SCS_PORT, &GPIO_InitStructure);
    W5500_SCS_Set();
}



/*******************************************************************************
*******************************************************************************/
void W5500_Initialization(void)
{
	W5500_Init();		
	Detect_Gateway(0);	
	Detect_Gateway(1);	
    
	Socket_Init(0);		
	Socket_Init(1);
}

/*******************************************************************************
*******************************************************************************/
void W5500_Socket_Set(SOCKET s)
{
	if(Socket[s].State == 0)
	{
		if(Socket[s].Mode == TCP_SERVER)
		{
            Socket_Init(s);
			if(Socket_Listen(s) == NET_TRUE)
				Socket[s].State = S_INIT;
			else
				Socket[s].State = 0;
		}
		else if(Socket[s].Mode == TCP_CLIENT)
		{
			if(Socket_Connect(s)==NET_TRUE)
				Socket[s].State = S_INIT;
			else
				Socket[s].State = 0;
		}
		else    
		{
			if(Socket_UDP(s) == NET_TRUE)
				Socket[s].State = S_INIT | S_CONN;
			else
				Socket[s].State = 0;
		}
	}
}

/*******************************************************************************
*******************************************************************************/
void Net_Send_Short(unsigned short dat)
{
	Spi1_SendByte(dat>>8);
	Spi1_SendByte(dat);	
}

/*******************************************************************************
*******************************************************************************/
void Write_W5500_1Byte(unsigned short reg, unsigned char dat)
{
	W5500_SCS_Clr();

	Net_Send_Short(reg);
	Spi1_SendByte(FDM1|RWB_WRITE|COMMON_R);
	Spi1_SendByte(dat);

	W5500_SCS_Set();
}

/*******************************************************************************
*******************************************************************************/
void Write_W5500_2Byte(unsigned short reg, unsigned short dat)
{
	W5500_SCS_Clr();
		
	Net_Send_Short(reg);
	Spi1_SendByte(FDM2|RWB_WRITE|COMMON_R);
	Net_Send_Short(dat);

	W5500_SCS_Set();
}

/*******************************************************************************
*******************************************************************************/
void Write_W5500_nByte(unsigned short reg, unsigned char *dat_ptr, unsigned short size)
{
	unsigned short i;

	W5500_SCS_Clr();
		
	Net_Send_Short(reg);
	Spi1_SendByte(VDM|RWB_WRITE|COMMON_R);

	for(i=0;i<size;i++)
	{
		Spi1_SendByte(*dat_ptr++);
	}

    W5500_SCS_Set();
}

/*******************************************************************************
*******************************************************************************/
void Write_W5500_SOCK_1Byte(SOCKET s, unsigned short reg, unsigned char dat)
{
	W5500_SCS_Clr();
		
	Net_Send_Short(reg);
	Spi1_SendByte(FDM1|RWB_WRITE|(s*0x20+0x08));
	Spi1_SendByte(dat);

    W5500_SCS_Set();
}

/*******************************************************************************
*******************************************************************************/
void Write_W5500_SOCK_2Byte(SOCKET s, unsigned short reg, unsigned short dat)
{
	W5500_SCS_Clr();
			
	Net_Send_Short(reg);
	Spi1_SendByte(FDM2|RWB_WRITE|(s*0x20+0x08));
	Net_Send_Short(dat);

	W5500_SCS_Set();
}

/*******************************************************************************
*******************************************************************************/
void Write_W5500_SOCK_4Byte(SOCKET s, unsigned short reg, unsigned char *dat_ptr)
{
	W5500_SCS_Clr();
    
	Net_Send_Short(reg);
	Spi1_SendByte(FDM4|RWB_WRITE|(s*0x20+0x08));

	Spi1_SendByte(*dat_ptr++);
	Spi1_SendByte(*dat_ptr++);
	Spi1_SendByte(*dat_ptr++);
	Spi1_SendByte(*dat_ptr++);

	W5500_SCS_Set();
}

/*******************************************************************************
*******************************************************************************/
unsigned char Read_W5500_1Byte(unsigned short reg)
{
	unsigned char temp;

	W5500_SCS_Clr();
			
	Net_Send_Short(reg);
	Spi1_SendByte(FDM1 | RWB_READ | COMMON_R);
	temp = Spi1_SendByte(0x00);

	W5500_SCS_Set();
	return temp;
}

/*******************************************************************************
*******************************************************************************/
unsigned char Read_W5500_SOCK_1Byte(SOCKET s, unsigned short reg)
{
	unsigned char temp;

	W5500_SCS_Clr();
			
	Net_Send_Short(reg);
	Spi1_SendByte(FDM1 | RWB_READ | (s*0x20+0x08));
	temp = Spi1_SendByte(0x00);

	W5500_SCS_Set();
	return temp;
}

/*******************************************************************************
*******************************************************************************/
unsigned short Read_W5500_SOCK_2Byte(SOCKET s, unsigned short reg)
{
	unsigned short temp;

	W5500_SCS_Clr();

	Net_Send_Short(reg);
	Spi1_SendByte(FDM2|RWB_READ|(s*0x20+0x08));

	temp = Spi1_SendByte(0x00);
	temp <<= 8;
	temp |= Spi1_SendByte(0x00);

	W5500_SCS_Set();
	return temp;
}

/*******************************************************************************
*******************************************************************************/
unsigned short Read_SOCK_Data_Buffer(SOCKET s, unsigned char *dat_ptr)
{
	unsigned short rx_size;
	unsigned short offset, offset1;
	unsigned short i;

	rx_size = Read_W5500_SOCK_2Byte(s,Sn_RX_RSR);
	if(rx_size == 0) return 0;
	if(rx_size > 1460) rx_size = 1460;

	offset = Read_W5500_SOCK_2Byte(s,Sn_RX_RD);
	offset1 = offset;
	offset &= (S_RX_SIZE-1);

    W5500_SCS_Clr();

	Net_Send_Short(offset);
	Spi1_SendByte(VDM|RWB_READ|(s*0x20+0x18));
	
	if((offset + rx_size) < S_RX_SIZE)
	{
		for(i = 0; i < rx_size; i++)
		{
			*dat_ptr = Spi1_SendByte(0x00);
			dat_ptr++;
		}
	}
	else
	{
		offset = S_RX_SIZE - offset;
		for(i = 0; i < offset; i++)
		{
			*dat_ptr = Spi1_SendByte(0x00);
			dat_ptr++;
		}
		W5500_SCS_Set(); 

		W5500_SCS_Clr();

		Net_Send_Short(0x00);
		Spi1_SendByte(VDM|RWB_READ|(s*0x20+0x18));

		for( ; i < rx_size; i++)
		{
			*dat_ptr = Spi1_SendByte(0x00);
			dat_ptr++;
		}
	}
	W5500_SCS_Set(); 

	offset1 += rx_size;
	Write_W5500_SOCK_2Byte(s, Sn_RX_RD, offset1);
	Write_W5500_SOCK_1Byte(s, Sn_CR, RECV);
	return rx_size;
}

/*******************************************************************************
*******************************************************************************/
void Write_SOCK_Data_Buffer(SOCKET s, unsigned char *dat_ptr, unsigned short size)
{
	unsigned short offset,offset1;
	unsigned short i;

	
	if((Read_W5500_SOCK_1Byte(s,Sn_MR)&0x0f) != SOCK_UDP)
	{
		Write_W5500_SOCK_4Byte(s, Sn_DIPR, Socket[s].UdpDIPR);
		Write_W5500_SOCK_2Byte(s, Sn_DPORTR, Socket[s].UdpDestPort);
	}

	offset  = Read_W5500_SOCK_2Byte(s,Sn_TX_WR);
	offset1 = offset;
	offset &= (S_TX_SIZE-1);

	W5500_SCS_Clr();

	Net_Send_Short(offset);
	Spi1_SendByte(VDM|RWB_WRITE|(s*0x20+0x10));

	if((offset + size) < S_TX_SIZE)
	{
		for(i = 0; i < size; i++)
		{
			Spi1_SendByte(*dat_ptr++);
		}
	}
	else
	{
		offset = S_TX_SIZE - offset;
		for(i=0;i<offset;i++)
		{
			Spi1_SendByte(*dat_ptr++);
		}
		W5500_SCS_Set();

		W5500_SCS_Clr();

		Net_Send_Short(0x00);
		Spi1_SendByte(VDM|RWB_WRITE|(s*0x20+0x10));

		for(;i<size;i++)
		{
			Spi1_SendByte(*dat_ptr++);
		}
	}
	W5500_SCS_Set();

	offset1+=size;
	Write_W5500_SOCK_2Byte(s, Sn_TX_WR, offset1);
	Write_W5500_SOCK_1Byte(s, Sn_CR, SEND);
}

/*******************************************************************************
*******************************************************************************/
void W5500_Hardware_Reset(void)
{
	W5500_RST_Clr();
	Delay(50);
	W5500_RST_Set();
	Delay(200);
    
	
}

/*******************************************************************************
*******************************************************************************/
void W5500_Init(void)
{
	u8 i=0;

	Write_W5500_1Byte(MR, RST);
	Delay(10);

	
	
	Write_W5500_nByte(GAR, Net.Gateway_IP, 4);
			
	
	
	Write_W5500_nByte(SUBR,Net.Sub_Mask,4);		
	
	
	
	
	Write_W5500_nByte(SHAR,Net.Phy_Addr,6);		

	
	
	Write_W5500_nByte(SIPR,Net.IP_Addr,4);		
	
	
	for(i=0;i<8;i++)
	{
		Write_W5500_SOCK_1Byte(i, Sn_RXBUF_SIZE, 0x02);//Socket Rx memory size=2k
		Write_W5500_SOCK_1Byte(i, Sn_TXBUF_SIZE, 0x02);//Socket Tx mempry size=2k
	}

	
	
	Write_W5500_2Byte(RTR_R, 0x07d0);

	
	
	Write_W5500_1Byte(RCR_R,8);
}

/*******************************************************************************
*******************************************************************************/
unsigned char Detect_Gateway(SOCKET s)
{
	unsigned char ip_adde[4];
	ip_adde[0] = Net.IP_Addr[0]+1;
	ip_adde[1] = Net.IP_Addr[1]+1;
	ip_adde[2] = Net.IP_Addr[2]+1;
	ip_adde[3] = Net.IP_Addr[3]+1;

	
	Write_W5500_SOCK_4Byte(s,Sn_DIPR,ip_adde);  
	Write_W5500_SOCK_1Byte(s,Sn_MR,MR_TCP);     
	Write_W5500_SOCK_1Byte(s,Sn_CR,OPEN);       
	Delay(5);
	
	if(Read_W5500_SOCK_1Byte(s,Sn_SR) != SOCK_INIT)
	{
		Write_W5500_SOCK_1Byte(s,Sn_CR,CLOSE);
		return NET_FALSE;
	}

	Write_W5500_SOCK_1Byte(s,Sn_CR,CONNECT);

	do
	{
		u8 j=0;
		j=Read_W5500_SOCK_1Byte(s,Sn_IR);
		if(j!=0)
		Write_W5500_SOCK_1Byte(s,Sn_IR,j);
		Delay(5);
		if((j&IR_TIMEOUT) == IR_TIMEOUT)
		{
			return NET_FALSE;	
		}
		else if(Read_W5500_SOCK_1Byte(s,Sn_DHAR) != 0xff)
		{
			Write_W5500_SOCK_1Byte(s,Sn_CR,CLOSE);
			return NET_TRUE;							
		}
	}while(1);
}

/*******************************************************************************
*******************************************************************************/
void Socket_Init(SOCKET s)
{    
    
    Write_W5500_SOCK_2Byte(s, Sn_MSSR, 0x05b4);
    
    Write_W5500_SOCK_2Byte(s, Sn_PORT, Socket[s].LocalPort);
    
    if(Socket[s].Mode == TCP_CLIENT)
    {
        
        Write_W5500_SOCK_2Byte(s, Sn_DPORTR, Socket[s].DestPort);
        
        Write_W5500_SOCK_4Byte(s, Sn_DIPR, Socket[s].DestIP);	
    }

//    {
//        if(Socket_Listen(s) == NET_TRUE)
//            Socket[s].State = S_INIT;
//        else
//            Socket[s].State = 0;
//    }
}

/*******************************************************************************
*******************************************************************************/
unsigned char Socket_Connect(SOCKET s)
{
	Write_W5500_SOCK_1Byte(s,Sn_MR,MR_TCP);
	Write_W5500_SOCK_1Byte(s,Sn_CR,OPEN);
	Delay(5);
	if(Read_W5500_SOCK_1Byte(s,Sn_SR)!=SOCK_INIT)
	{
		Write_W5500_SOCK_1Byte(s,Sn_CR,CLOSE);
		return NET_FALSE;
	}
	Write_W5500_SOCK_1Byte(s,Sn_CR,CONNECT);
	return NET_TRUE;
}

/*******************************************************************************
*******************************************************************************/
unsigned char Socket_Listen(SOCKET s)
{
	Write_W5500_SOCK_1Byte(s,Sn_MR,MR_TCP);
	Write_W5500_SOCK_1Byte(s,Sn_CR,OPEN);
	Delay(5);
	if(Read_W5500_SOCK_1Byte(s,Sn_SR)!=SOCK_INIT)
	{
		Write_W5500_SOCK_1Byte(s,Sn_CR,CLOSE);
		return NET_FALSE;
	}
	Write_W5500_SOCK_1Byte(s,Sn_CR,LISTEN);
	Delay(5);
	if(Read_W5500_SOCK_1Byte(s,Sn_SR)!=SOCK_LISTEN)
	{
		Write_W5500_SOCK_1Byte(s,Sn_CR,CLOSE);
		return NET_FALSE;
	}

	return NET_TRUE;

	
	
	
}

/*******************************************************************************
*******************************************************************************/
unsigned char Socket_UDP(SOCKET s)
{
	Write_W5500_SOCK_1Byte(s,Sn_MR,MR_UDP); 
	Write_W5500_SOCK_1Byte(s,Sn_CR,OPEN);   
	Delay(5);
	if(Read_W5500_SOCK_1Byte(s,Sn_SR)!=SOCK_UDP)
	{
		Write_W5500_SOCK_1Byte(s,Sn_CR,CLOSE);
		return NET_FALSE;
	}
	else
		return NET_TRUE;

	
	
	
}

/*******************************************************************************
*******************************************************************************/
void W5500_Interrupt_Process(void)
{
	uint8_t SIR_REG, SnIR_REG, n;
IntDispose:
	SIR_REG = Read_W5500_1Byte(SIR);
	if(SIR_REG == 0) return;
	for(n=0; n<8; n++)
	{
		if((SIR_REG & Socket_Int(n)) == Socket_Int(n))
		{
			SnIR_REG = Read_W5500_SOCK_1Byte(n,Sn_IR);
			Write_W5500_SOCK_1Byte(n,Sn_IR, SnIR_REG);
			if(SnIR_REG & IR_CON)
			{
                
				Socket[n].State |= S_CONN;
			}
			if(SnIR_REG & IR_DISCON)
			{
                
				Write_W5500_SOCK_1Byte(n,Sn_CR,CLOSE);
                Socket_Init(n);		
                Socket[n].State = 0;
			}
			if(SnIR_REG & IR_SEND_OK)
			{
                
				Socket[n].DataState |= S_TRANSMITOK;
			}
			if(SnIR_REG & IR_RECV)
			{
                
				Socket[n].DataState |= S_RECEIVE;
			}
			if(SnIR_REG & IR_TIMEOUT)
			{
                
				Write_W5500_SOCK_1Byte(n,Sn_CR,CLOSE);
                Socket_Init(n);		
                Socket[n].State = 0;
			}
		}
	}
	if(Read_W5500_1Byte(SIR) != 0) 
		goto IntDispose;
}

