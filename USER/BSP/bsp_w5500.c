/**********************************************************************************
 * 文件名  ：W5500.c
 * 描述    ：W5500 驱动函数库         
 * 库版本  ：ST_v3.5
**********************************************************************************/

#include "stm32f10x.h"

/***************----- W5500 GPIO定义 -----***************/
#define W5500_SCS_PORT	GPIOC
#define W5500_SCS		GPIO_Pin_4	//定义W5500的CS引脚	 
#define W5500_SCS_Clr()	W5500_SCS_PORT->BRR = W5500_SCS
#define W5500_SCS_Set()	W5500_SCS_PORT->BSRR = W5500_SCS

#define W5500_RST_PORT	GPIOC
#define W5500_RST		GPIO_Pin_5	//定义W5500的RST引脚
#define W5500_RST_Clr()	W5500_RST_PORT->BRR = W5500_RST
#define W5500_RST_Set()	W5500_RST_PORT->BSRR = W5500_RST

#define W5500_INT		GPIO_Pin_0	//定义W5500的INT引脚
#define W5500_INT_PORT	GPIOB



/***************----- 网络参数变量定义 -----***************/
NET             Net;
SOCKET_TYPE     Socket[8];

/***************----- 端口数据缓冲区 -----***************/
unsigned char Rx_Buffer[2048];	//端口接收数据缓冲区 
unsigned char Tx_Buffer[2048];	//端口发送数据缓冲区 

unsigned char W5500_Interrupt;	//W5500中断标志(0:无中断,1:有中断)
unsigned int W5500_Send_Delay_Counter[8] = {0}; //W5500发送延时计数变量(ms)

/*******************************************************************************
* 函数名  : W5500_GPIO_Config
* 描述    : W5500 GPIO初始化配置
* 输入    : 无
* 输出    : 无
* 返回值  : 无
* 说明    : 无
*******************************************************************************/
void W5500_GPIO_Config(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;	
    
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC | RCC_APB2Periph_AFIO, ENABLE);
	
	/* W5500_RST引脚初始化配置(PC5) */
	GPIO_InitStructure.GPIO_Pin  = W5500_RST;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(W5500_RST_PORT, &GPIO_InitStructure);
	W5500_RST_Clr();//复位引脚拉低
	
	/* W5500_INT引脚初始化配置(PB0) */
	GPIO_InitStructure.GPIO_Pin  = W5500_INT;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(W5500_INT_PORT, &GPIO_InitStructure);
    
    /* 初始化CS引脚(PC4) */
	GPIO_InitStructure.GPIO_Pin = W5500_SCS;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;
	GPIO_Init(W5500_SCS_PORT, &GPIO_InitStructure);
    W5500_SCS_Set();
}



/*******************************************************************************
* 函数名  : W5500_Initialization
* 描述    : W5500初始货配置
* 输入    : 无
* 输出    : 无
* 返回值  : 无
* 说明    : 无
*******************************************************************************/
void W5500_Initialization(void)
{
	W5500_Init();		//初始化W5500寄存器函数
	Detect_Gateway(0);	//检查网关服务器 
	Detect_Gateway(1);	//检查网关服务器 
    
	Socket_Init(0);		//指定Socket(0~7)初始化,初始化端口0
	Socket_Init(1);
}

/*******************************************************************************
* 函数名  : W5500_Socket_Set
* 描述    : W5500端口初始化配置
* 输入    : 无
* 输出    : 无
* 返回值  : 无
* 说明    : 分别设置4个端口,根据端口工作模式,将端口置于TCP服务器、TCP客户端或UDP模式.
*			从端口状态字节Socket.State可以判断端口的工作情况
*******************************************************************************/
void W5500_Socket_Set(SOCKET s)
{
	if(Socket[s].State == 0)//端口0初始化配置
	{
		if(Socket[s].Mode == TCP_SERVER)//TCP服务器模式 
		{
            Socket_Init(s);
			if(Socket_Listen(s) == NET_TRUE)
				Socket[s].State = S_INIT;
			else
				Socket[s].State = 0;
		}
		else if(Socket[s].Mode == TCP_CLIENT)//TCP客户端模式 
		{
			if(Socket_Connect(s)==NET_TRUE)
				Socket[s].State = S_INIT;
			else
				Socket[s].State = 0;
		}
		else    //UDP模式 
		{
			if(Socket_UDP(s) == NET_TRUE)
				Socket[s].State = S_INIT | S_CONN;
			else
				Socket[s].State = 0;
		}
	}
}

/*******************************************************************************
* 函数名  : Net_Send_Short
* 描述    : SPI1发送2个字节数据(16位)
* 输入    : dat:待发送的16位数据
* 输出    : 无
* 返回值  : 无
* 说明    : 无
*******************************************************************************/
void Net_Send_Short(unsigned short dat)
{
	Spi1_SendByte(dat>>8);//写数据高位
	Spi1_SendByte(dat);	//写数据低位
}

/*******************************************************************************
* 函数名  : Write_W5500_1Byte
* 描述    : 通过SPI向指定地址寄存器写1个字节数据
* 输入    : reg:16位寄存器地址,dat:待写入的数据
* 输出    : 无
* 返回值  : 无
* 说明    : 无
*******************************************************************************/
void Write_W5500_1Byte(unsigned short reg, unsigned char dat)
{
	W5500_SCS_Clr();

	Net_Send_Short(reg);//通过SPI写16位寄存器地址
	Spi1_SendByte(FDM1|RWB_WRITE|COMMON_R);//通过SPI写控制字节,1个字节数据长度,写数据,选择通用寄存器
	Spi1_SendByte(dat);//写1个字节数据

	W5500_SCS_Set();
}

/*******************************************************************************
* 函数名  : Write_W5500_2Byte
* 描述    : 通过SPI向指定地址寄存器写2个字节数据
* 输入    : reg:16位寄存器地址,dat:16位待写入的数据(2个字节)
* 输出    : 无
* 返回值  : 无
* 说明    : 无
*******************************************************************************/
void Write_W5500_2Byte(unsigned short reg, unsigned short dat)
{
	W5500_SCS_Clr();
		
	Net_Send_Short(reg);//通过SPI写16位寄存器地址
	Spi1_SendByte(FDM2|RWB_WRITE|COMMON_R);//通过SPI写控制字节,2个字节数据长度,写数据,选择通用寄存器
	Net_Send_Short(dat);//写16位数据

	W5500_SCS_Set();
}

/*******************************************************************************
* 函数名  : Write_W5500_nByte
* 描述    : 通过SPI向指定地址寄存器写n个字节数据
* 输入    : reg:16位寄存器地址,*dat_ptr:待写入数据缓冲区指针,size:待写入的数据长度
* 输出    : 无
* 返回值  : 无
* 说明    : 无
*******************************************************************************/
void Write_W5500_nByte(unsigned short reg, unsigned char *dat_ptr, unsigned short size)
{
	unsigned short i;

	W5500_SCS_Clr();
		
	Net_Send_Short(reg);//通过SPI写16位寄存器地址
	Spi1_SendByte(VDM|RWB_WRITE|COMMON_R);//通过SPI写控制字节,N个字节数据长度,写数据,选择通用寄存器

	for(i=0;i<size;i++)//循环将缓冲区的size个字节数据写入W5500
	{
		Spi1_SendByte(*dat_ptr++);//写一个字节数据
	}

    W5500_SCS_Set();
}

/*******************************************************************************
* 函数名  : Write_W5500_SOCK_1Byte
* 描述    : 通过SPI1向指定端口寄存器写1个字节数据
* 输入    : s:端口号,reg:16位寄存器地址,dat:待写入的数据
* 输出    : 无
* 返回值  : 无
* 说明    : 无
*******************************************************************************/
void Write_W5500_SOCK_1Byte(SOCKET s, unsigned short reg, unsigned char dat)
{
	W5500_SCS_Clr();
		
	Net_Send_Short(reg);//通过SPI写16位寄存器地址
	Spi1_SendByte(FDM1|RWB_WRITE|(s*0x20+0x08));//通过SPI写控制字节,1个字节数据长度,写数据,选择端口s的寄存器
	Spi1_SendByte(dat);//写1个字节数据

    W5500_SCS_Set();
}

/*******************************************************************************
* 函数名  : Write_W5500_SOCK_2Byte
* 描述    : 通过SPI向指定端口寄存器写2个字节数据
* 输入    : s:端口号,reg:16位寄存器地址,dat:16位待写入的数据(2个字节)
* 输出    : 无
* 返回值  : 无
* 说明    : 无
*******************************************************************************/
void Write_W5500_SOCK_2Byte(SOCKET s, unsigned short reg, unsigned short dat)
{
	W5500_SCS_Clr();
			
	Net_Send_Short(reg);//通过SPI写16位寄存器地址
	Spi1_SendByte(FDM2|RWB_WRITE|(s*0x20+0x08));//通过SPI写控制字节,2个字节数据长度,写数据,选择端口s的寄存器
	Net_Send_Short(dat);//写16位数据

	W5500_SCS_Set();
}

/*******************************************************************************
* 函数名  : Write_W5500_SOCK_4Byte
* 描述    : 通过SPI向指定端口寄存器写4个字节数据
* 输入    : s:端口号,reg:16位寄存器地址,*dat_ptr:待写入的4个字节缓冲区指针
* 输出    : 无
* 返回值  : 无
* 说明    : 无
*******************************************************************************/
void Write_W5500_SOCK_4Byte(SOCKET s, unsigned short reg, unsigned char *dat_ptr)
{
	W5500_SCS_Clr();//置W5500的SCS为低电平
    
	Net_Send_Short(reg);//通过SPI写16位寄存器地址
	Spi1_SendByte(FDM4|RWB_WRITE|(s*0x20+0x08));//通过SPI写控制字节,4个字节数据长度,写数据,选择端口s的寄存器

	Spi1_SendByte(*dat_ptr++);//写第1个字节数据
	Spi1_SendByte(*dat_ptr++);//写第2个字节数据
	Spi1_SendByte(*dat_ptr++);//写第3个字节数据
	Spi1_SendByte(*dat_ptr++);//写第4个字节数据

	W5500_SCS_Set();//置W5500的SCS为高电平
}

/*******************************************************************************
* 函数名  : Read_W5500_1Byte
* 描述    : 读W5500指定地址寄存器的1个字节数据
* 输入    : reg:16位寄存器地址
* 输出    : 无
* 返回值  : 读取到寄存器的1个字节数据
* 说明    : 无
*******************************************************************************/
unsigned char Read_W5500_1Byte(unsigned short reg)
{
	unsigned char temp;

	W5500_SCS_Clr();//置W5500的SCS为低电平
			
	Net_Send_Short(reg);//通过SPI写16位寄存器地址
	Spi1_SendByte(FDM1 | RWB_READ | COMMON_R);//通过SPI写控制字节,1个字节数据长度,读数据,选择通用寄存器
	temp = Spi1_SendByte(0x00);//发送一个哑数据,读取数据

	W5500_SCS_Set();//置W5500的SCS为高电平
	return temp;//返回读取到的寄存器数据
}

/*******************************************************************************
* 函数名  : Read_W5500_SOCK_1Byte
* 描述    : 读W5500指定端口寄存器的1个字节数据
* 输入    : s:端口号 0-7, reg:16位寄存器地址
* 输出    : 无
* 返回值  : 读取到寄存器的1个字节数据
* 说明    : 无
*******************************************************************************/
unsigned char Read_W5500_SOCK_1Byte(SOCKET s, unsigned short reg)
{
	unsigned char temp;

	W5500_SCS_Clr();//置W5500的SCS为低电平
			
	Net_Send_Short(reg);//通过SPI写16位寄存器地址
	Spi1_SendByte(FDM1 | RWB_READ | (s*0x20+0x08));//通过SPI写控制字节,1个字节数据长度,读数据,选择端口s的寄存器
	temp = Spi1_SendByte(0x00);//发送一个哑数据

	W5500_SCS_Set();//置W5500的SCS为高电平
	return temp;//返回读取到的寄存器数据
}

/*******************************************************************************
* 函数名  : Read_W5500_SOCK_2Byte
* 描述    : 读W5500指定端口寄存器的2个字节数据
* 输入    : s:端口号,reg:16位寄存器地址
* 输出    : 无
* 返回值  : 读取到寄存器的2个字节数据(16位)
* 说明    : 无
*******************************************************************************/
unsigned short Read_W5500_SOCK_2Byte(SOCKET s, unsigned short reg)
{
	unsigned short temp;

	W5500_SCS_Clr();//置W5500的SCS为低电平

	Net_Send_Short(reg);//通过SPI写16位寄存器地址
	Spi1_SendByte(FDM2|RWB_READ|(s*0x20+0x08));//通过SPI写控制字节,2个字节数据长度,读数据,选择端口s的寄存器

	temp = Spi1_SendByte(0x00);//发送一个哑数据
	temp <<= 8;
	temp |= Spi1_SendByte(0x00);//读取低位数据

	W5500_SCS_Set();//置W5500的SCS为高电平
	return temp;//返回读取到的寄存器数据
}

/*******************************************************************************
* 函数名  : Read_SOCK_Data_Buffer
* 描述    : 从W5500接收数据缓冲区中读取数据
* 输入    : s:端口号,*dat_ptr:数据保存缓冲区指针
* 输出    : 无
* 返回值  : 读取到的数据长度,rx_size个字节
* 说明    : 无
*******************************************************************************/
unsigned short Read_SOCK_Data_Buffer(SOCKET s, unsigned char *dat_ptr)
{
	unsigned short rx_size;
	unsigned short offset, offset1;
	unsigned short i;

	rx_size = Read_W5500_SOCK_2Byte(s,Sn_RX_RSR);
	if(rx_size == 0) return 0;//没接收到数据则返回
	if(rx_size > 1460) rx_size = 1460;

	offset = Read_W5500_SOCK_2Byte(s,Sn_RX_RD);
	offset1 = offset;
	offset &= (S_RX_SIZE-1);//计算实际的物理地址

    W5500_SCS_Clr();//置W5500的SCS为低电平

	Net_Send_Short(offset);//写16位地址
	Spi1_SendByte(VDM|RWB_READ|(s*0x20+0x18));//写控制字节,N个字节数据长度,读数据,选择端口s的寄存器
	
	if((offset + rx_size) < S_RX_SIZE)//如果最大地址未超过W5500接收缓冲区寄存器的最大地址
	{
		for(i = 0; i < rx_size; i++)//循环读取rx_size个字节数据
		{
			*dat_ptr = Spi1_SendByte(0x00);//发送一个哑数据
			dat_ptr++;//数据保存缓冲区指针地址自增1
		}
	}
	else//如果最大地址超过W5500接收缓冲区寄存器的最大地址
	{
		offset = S_RX_SIZE - offset;
		for(i = 0; i < offset; i++)//循环读取出前offset个字节数据
		{
			*dat_ptr = Spi1_SendByte(0x00);//发送一个哑数据
			dat_ptr++;//数据保存缓冲区指针地址自增1
		}
		W5500_SCS_Set(); //置W5500的SCS为高电平

		W5500_SCS_Clr();//置W5500的SCS为低电平

		Net_Send_Short(0x00);//写16位地址
		Spi1_SendByte(VDM|RWB_READ|(s*0x20+0x18));//写控制字节,N个字节数据长度,读数据,选择端口s的寄存器

		for( ; i < rx_size; i++)//循环读取后rx_size-offset个字节数据
		{
			*dat_ptr = Spi1_SendByte(0x00);//发送一个哑数据
			dat_ptr++;//数据保存缓冲区指针地址自增1
		}
	}
	W5500_SCS_Set(); //置W5500的SCS为高电平

	offset1 += rx_size;//更新实际物理地址,即下次读取接收到的数据的起始地址
	Write_W5500_SOCK_2Byte(s, Sn_RX_RD, offset1);
	Write_W5500_SOCK_1Byte(s, Sn_CR, RECV);//发送启动接收命令
	return rx_size;//返回接收到数据的长度
}

/*******************************************************************************
* 函数名  : Write_SOCK_Data_Buffer
* 描述    : 将数据写入W5500的数据发送缓冲区
* 输入    : s:端口号,*dat_ptr:数据保存缓冲区指针,size:待写入数据的长度
* 输出    : 无
* 返回值  : 无
* 说明    : 无
*******************************************************************************/
void Write_SOCK_Data_Buffer(SOCKET s, unsigned char *dat_ptr, unsigned short size)
{
	unsigned short offset,offset1;
	unsigned short i;

	//如果是UDP模式,可以在此设置目的主机的IP和端口号
	if((Read_W5500_SOCK_1Byte(s,Sn_MR)&0x0f) != SOCK_UDP)//如果Socket打开失败
	{
		Write_W5500_SOCK_4Byte(s, Sn_DIPR, Socket[s].UdpDIPR);//设置目的主机IP  		
		Write_W5500_SOCK_2Byte(s, Sn_DPORTR, Socket[s].UdpDestPort);//设置目的主机端口号				
	}

	offset  = Read_W5500_SOCK_2Byte(s,Sn_TX_WR);
	offset1 = offset;
	offset &= (S_TX_SIZE-1);//计算实际的物理地址

	W5500_SCS_Clr();

	Net_Send_Short(offset);//写16位地址
	Spi1_SendByte(VDM|RWB_WRITE|(s*0x20+0x10));//写控制字节,N个字节数据长度,写数据,选择端口s的寄存器

	if((offset + size) < S_TX_SIZE)//如果最大地址未超过W5500发送缓冲区寄存器的最大地址
	{
		for(i = 0; i < size; i++)//循环写入size个字节数据
		{
			Spi1_SendByte(*dat_ptr++);//写入一个字节的数据		
		}
	}
	else//如果最大地址超过W5500发送缓冲区寄存器的最大地址
	{
		offset = S_TX_SIZE - offset;
		for(i=0;i<offset;i++)//循环写入前offset个字节数据
		{
			Spi1_SendByte(*dat_ptr++);//写入一个字节的数据
		}
		W5500_SCS_Set();

		W5500_SCS_Clr();

		Net_Send_Short(0x00);//写16位地址
		Spi1_SendByte(VDM|RWB_WRITE|(s*0x20+0x10));//写控制字节,N个字节数据长度,写数据,选择端口s的寄存器

		for(;i<size;i++)//循环写入size-offset个字节数据
		{
			Spi1_SendByte(*dat_ptr++);//写入一个字节的数据
		}
	}
	W5500_SCS_Set();

	offset1+=size;//更新实际物理地址,即下次写待发送数据到发送数据缓冲区的起始地址
	Write_W5500_SOCK_2Byte(s, Sn_TX_WR, offset1);
	Write_W5500_SOCK_1Byte(s, Sn_CR, SEND);//发送启动发送命令
}

/*******************************************************************************
* 函数名  : W5500_Hardware_Reset
* 描述    : 硬件复位W5500
* 输入    : 无
* 输出    : 无
* 返回值  : 无
* 说明    : W5500的复位引脚保持低电平至少500us以上,才能复位W5500
*******************************************************************************/
void W5500_Hardware_Reset(void)
{
	W5500_RST_Clr();//复位引脚拉低
	Delay(50);
	W5500_RST_Set();//复位引脚拉高
	Delay(200);
    
	//while((Read_W5500_1Byte(PHYCFGR) & LINK)==0);//等待以太网连接完成
}

/*******************************************************************************
* 函数名  : W5500_Init
* 描述    : 初始化W5500寄存器函数
* 输入    : 无
* 输出    : 无
* 返回值  : 无
* 说明    : 在使用W5500之前，先对W5500初始化
*******************************************************************************/
void W5500_Init(void)
{
	u8 i=0;

	Write_W5500_1Byte(MR, RST);//软件复位W5500,置1有效,复位后自动清0
	Delay(10);//延时10ms,自己定义该函数

	//设置网关(Gateway)的IP地址,Gateway_IP为4字节unsigned char数组,自己定义 
	//使用网关可以使通信突破子网的局限，通过网关可以访问到其它子网或进入Internet
	Write_W5500_nByte(GAR, Net.Gateway_IP, 4);
			
	//设置子网掩码(MASK)值,SUB_MASK为4字节unsigned char数组,自己定义
	//子网掩码用于子网运算
	Write_W5500_nByte(SUBR,Net.Sub_Mask,4);		
	
	//设置物理地址,PHY_ADDR为6字节unsigned char数组,自己定义,用于唯一标识网络设备的物理地址值
	//该地址值需要到IEEE申请，按照OUI的规定，前3个字节为厂商代码，后三个字节为产品序号
	//如果自己定义物理地址，注意第一个字节必须为偶数
	Write_W5500_nByte(SHAR,Net.Phy_Addr,6);		

	//设置本机的IP地址,IP_ADDR为4字节unsigned char数组,自己定义
	//注意，网关IP必须与本机IP属于同一个子网，否则本机将无法找到网关
	Write_W5500_nByte(SIPR,Net.IP_Addr,4);		
	
	//设置发送缓冲区和接收缓冲区的大小，参考W5500数据手册
	for(i=0;i<8;i++)
	{
		Write_W5500_SOCK_1Byte(i, Sn_RXBUF_SIZE, 0x02);//Socket Rx memory size=2k
		Write_W5500_SOCK_1Byte(i, Sn_TXBUF_SIZE, 0x02);//Socket Tx mempry size=2k
	}

	//设置重试时间，默认为2000(200ms) 
	//每一单位数值为100微秒,初始化时值设为2000(0x07D0),等于200毫秒
	Write_W5500_2Byte(RTR_R, 0x07d0);

	//设置重试次数，默认为8次 
	//如果重发的次数超过设定值,则产生超时中断(相关的端口中断寄存器中的Sn_IR 超时位(TIMEOUT)置“1”)
	Write_W5500_1Byte(RCR_R,8);
}

/*******************************************************************************
* 函数名  : Detect_Gateway
* 描述    : 检查网关服务器
* 输入    : 无
* 输出    : 无
* 返回值  : 成功返回NET_TRUE(0xFF),失败返回NET_FALSE(0x00)
* 说明    : 无
*******************************************************************************/
unsigned char Detect_Gateway(SOCKET s)
{
	unsigned char ip_adde[4];
	ip_adde[0] = Net.IP_Addr[0]+1;
	ip_adde[1] = Net.IP_Addr[1]+1;
	ip_adde[2] = Net.IP_Addr[2]+1;
	ip_adde[3] = Net.IP_Addr[3]+1;

	//检查网关及获取网关的物理地址
	Write_W5500_SOCK_4Byte(s,Sn_DIPR,ip_adde);  //向目的地址寄存器写入与本机IP不同的IP值
	Write_W5500_SOCK_1Byte(s,Sn_MR,MR_TCP);     //设置socket为TCP模式
	Write_W5500_SOCK_1Byte(s,Sn_CR,OPEN);       //打开Socket	
	Delay(5);//延时5ms 	
	
	if(Read_W5500_SOCK_1Byte(s,Sn_SR) != SOCK_INIT)//如果socket打开失败
	{
		Write_W5500_SOCK_1Byte(s,Sn_CR,CLOSE);//打开不成功,关闭Socket
		return NET_FALSE;//返回NET_FALSE(0x00)
	}

	Write_W5500_SOCK_1Byte(s,Sn_CR,CONNECT);//设置Socket为Connect模式						

	do
	{
		u8 j=0;
		j=Read_W5500_SOCK_1Byte(s,Sn_IR);//读取Socket0中断标志寄存器
		if(j!=0)
		Write_W5500_SOCK_1Byte(s,Sn_IR,j);
		Delay(5);//延时5ms 
		if((j&IR_TIMEOUT) == IR_TIMEOUT)
		{
			return NET_FALSE;	
		}
		else if(Read_W5500_SOCK_1Byte(s,Sn_DHAR) != 0xff)
		{
			Write_W5500_SOCK_1Byte(s,Sn_CR,CLOSE);//关闭Socket
			return NET_TRUE;							
		}
	}while(1);
}

/*******************************************************************************
* 函数名  : Socket_Init
* 描述    : 指定Socket(0~7)初始化
* 输入    : s:待初始化的端口
* 输出    : 无
* 返回值  : 无
* 说明    : 无
*******************************************************************************/
void Socket_Init(SOCKET s)
{    
    //设置分片长度，参考W5500数据手册，该值可以不修改	
    Write_W5500_SOCK_2Byte(s, Sn_MSSR, 0x05b4);//最大分片字节数=1460(0x5b4)
    //设置端口0的端口号
    Write_W5500_SOCK_2Byte(s, Sn_PORT, Socket[s].LocalPort);
    
    if(Socket[s].Mode == TCP_CLIENT)
    {
        //设置端口目的(远程)端口号
        Write_W5500_SOCK_2Byte(s, Sn_DPORTR, Socket[s].DestPort);
        //设置端口目的(远程)IP地址
        Write_W5500_SOCK_4Byte(s, Sn_DIPR, Socket[s].DestIP);	
    }
//    else if(Socket[s].Mode == TCP_SERVER)//TCP服务器模式     wcx
//    {
//        if(Socket_Listen(s) == NET_TRUE)
//            Socket[s].State = S_INIT;
//        else
//            Socket[s].State = 0;
//    }
}

/*******************************************************************************
* 函数名  : Socket_Connect
* 描述    : 设置指定Socket(0~7)为客户端与远程服务器连接
* 输入    : s:待设定的端口
* 输出    : 无
* 返回值  : 成功返回NET_TRUE(0xFF),失败返回NET_FALSE(0x00)
* 说明    : 当本机Socket工作在客户端模式时,引用该程序,与远程服务器建立连接
*			如果启动连接后出现超时中断，则与服务器连接失败,需要重新调用该程序连接
*			该程序每调用一次,就与服务器产生一次连接
*******************************************************************************/
unsigned char Socket_Connect(SOCKET s)
{
	Write_W5500_SOCK_1Byte(s,Sn_MR,MR_TCP);//设置socket为TCP模式
	Write_W5500_SOCK_1Byte(s,Sn_CR,OPEN);//打开Socket
	Delay(5);//延时5ms
	if(Read_W5500_SOCK_1Byte(s,Sn_SR)!=SOCK_INIT)//如果socket打开失败
	{
		Write_W5500_SOCK_1Byte(s,Sn_CR,CLOSE);//打开不成功,关闭Socket
		return NET_FALSE;//返回NET_FALSE(0x00)
	}
	Write_W5500_SOCK_1Byte(s,Sn_CR,CONNECT);//设置Socket为Connect模式
	return NET_TRUE;//返回NET_TRUE,设置成功
}

/*******************************************************************************
* 函数名  : Socket_Listen
* 描述    : 设置指定Socket(0~7)作为服务器等待远程主机的连接
* 输入    : s:待设定的端口
* 输出    : 无
* 返回值  : 成功返回NET_TRUE(0xFF),失败返回NET_FALSE(0x00)
* 说明    : 当本机Socket工作在服务器模式时,引用该程序,等等远程主机的连接
*			该程序只调用一次,就使W5500设置为服务器模式
*******************************************************************************/
unsigned char Socket_Listen(SOCKET s)
{
	Write_W5500_SOCK_1Byte(s,Sn_MR,MR_TCP);//设置socket为TCP模式 
	Write_W5500_SOCK_1Byte(s,Sn_CR,OPEN);//打开Socket	
	Delay(5);//延时5ms
	if(Read_W5500_SOCK_1Byte(s,Sn_SR)!=SOCK_INIT)//如果socket打开失败
	{
		Write_W5500_SOCK_1Byte(s,Sn_CR,CLOSE);//打开不成功,关闭Socket
		return NET_FALSE;//返回NET_FALSE(0x00)
	}
	Write_W5500_SOCK_1Byte(s,Sn_CR,LISTEN);//设置Socket为侦听模式	
	Delay(5);//延时5ms
	if(Read_W5500_SOCK_1Byte(s,Sn_SR)!=SOCK_LISTEN)//如果socket设置失败
	{
		Write_W5500_SOCK_1Byte(s,Sn_CR,CLOSE);//设置不成功,关闭Socket
		return NET_FALSE;//返回NET_FALSE(0x00)
	}

	return NET_TRUE;

	//至此完成了Socket的打开和设置侦听工作,至于远程客户端是否与它建立连接,则需要等待Socket中断，
	//以判断Socket的连接是否成功。参考W5500数据手册的Socket中断状态
	//在服务器侦听模式不需要设置目的IP和目的端口号
}

/*******************************************************************************
* 函数名  : Socket_UDP
* 描述    : 设置指定Socket(0~7)为UDP模式
* 输入    : s:待设定的端口
* 输出    : 无
* 返回值  : 成功返回NET_TRUE(0xFF),失败返回NET_FALSE(0x00)
* 说明    : 如果Socket工作在UDP模式,引用该程序,在UDP模式下,Socket通信不需要建立连接
*			该程序只调用一次，就使W5500设置为UDP模式
*******************************************************************************/
unsigned char Socket_UDP(SOCKET s)
{
	Write_W5500_SOCK_1Byte(s,Sn_MR,MR_UDP); //设置Socket为UDP模式*/
	Write_W5500_SOCK_1Byte(s,Sn_CR,OPEN);   //打开Socket*/
	Delay(5);//延时5ms
	if(Read_W5500_SOCK_1Byte(s,Sn_SR)!=SOCK_UDP)//如果Socket打开失败
	{
		Write_W5500_SOCK_1Byte(s,Sn_CR,CLOSE);//打开不成功,关闭Socket
		return NET_FALSE;//返回NET_FALSE(0x00)
	}
	else
		return NET_TRUE;

	//至此完成了Socket的打开和UDP模式设置,在这种模式下它不需要与远程主机建立连接
	//因为Socket不需要建立连接,所以在发送数据前都可以设置目的主机IP和目的Socket的端口号
	//如果目的主机IP和目的Socket的端口号是固定的,在运行过程中没有改变,那么也可以在这里设置
}

/*******************************************************************************
* 函数名  : W5500_Interrupt_Process
* 描述    : W5500中断处理程序框架
* 输入    : 无
* 输出    : 无
* 返回值  : 无
* 说明    : 无
*******************************************************************************/
void W5500_Interrupt_Process(void)
{
	uint8_t SIR_REG, SnIR_REG, n;
IntDispose:
	SIR_REG = Read_W5500_1Byte(SIR);//读取端口中断标志寄存器
	if(SIR_REG == 0) return;
	for(n=0; n<8; n++)
	{
		if((SIR_REG & Socket_Int(n)) == Socket_Int(n))//Socket n事件处理 
		{
			SnIR_REG = Read_W5500_SOCK_1Byte(n,Sn_IR);//读取Socket n中断标志寄存器
			Write_W5500_SOCK_1Byte(n,Sn_IR, SnIR_REG);
			if(SnIR_REG & IR_CON)//在TCP模式下,Socket n成功连接 
			{
                //printf("成功连接 : %d\r\n", n);
				Socket[n].State |= S_CONN;//网络连接状态0x02,端口完成连接，可以正常传输数据
			}
			if(SnIR_REG & IR_DISCON)//在TCP模式下Socket断开连接处理
			{
                //printf("断开连接 : %d\r\n", n);
				Write_W5500_SOCK_1Byte(n,Sn_CR,CLOSE);//关闭端口,等待重新打开连接 
                Socket_Init(n);		//指定Socket(0~7)初始化,初始化端口0
                Socket[n].State = 0;//网络连接状态0x00,端口连接失败
			}
			if(SnIR_REG & IR_SEND_OK)//Socket0数据发送完成,可以再次启动S_tx_process()函数发送数据 
			{
                //printf("发送完成 : %d\r\n", n);
				Socket[n].DataState |= S_TRANSMITOK;//端口发送一个数据包完成 
			}
			if(SnIR_REG & IR_RECV)//Socket接收到数据,可以启动S_rx_process()函数 
			{
                //printf("收到数据 : %d\r\n", n);
				Socket[n].DataState |= S_RECEIVE;//端口接收到一个数据包
			}
			if(SnIR_REG & IR_TIMEOUT)//Socket连接或数据传输超时处理 
			{
                //printf("连接超时 : %d\r\n", n);
				Write_W5500_SOCK_1Byte(n,Sn_CR,CLOSE);// 关闭端口,等待重新打开连接 
                Socket_Init(n);		//指定Socket(0~7)初始化,初始化端口0
                Socket[n].State = 0;//网络连接状态0x00,端口连接失败
			}
		}
	}
	if(Read_W5500_1Byte(SIR) != 0) 
		goto IntDispose;
}

