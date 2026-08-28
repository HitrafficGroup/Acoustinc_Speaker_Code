/* Controlador del transceptor inalambrico nRF24L01. */
#include "stm32f10x.h"

u8 Buffer[16]  ; // Define a static TX address
u8 TX_ADDRESS[TX_ADR_WIDTH]= {0x34,0x43,0x10,0x12,0x01};	
u8 RX_ADDRESS[RX_ADR_WIDTH]= {0x34,0x43,0x10,0x12,0x01};	

void nRF24L01ioConfig(void)//LCD initial IO.
{
 	nRF24L01_CE_L;	
	nRF24L01_CSN_H;	
	
	nRF24L01_CE_L;
	SPI_Write_Buf(RFWRITE_REG + TX_ADDR, TX_ADDRESS, TX_ADR_WIDTH);    
	SPI_Write_Buf(RFWRITE_REG + RX_ADDR_P0, RX_ADDRESS, RX_ADR_WIDTH); 

	SPI_RW_Reg(RFWRITE_REG + EN_AA, 0x01);      
	SPI_RW_Reg(RFWRITE_REG + EN_RXADDR, 0x01);  
	SPI_RW_Reg(RFWRITE_REG + RF_CH, 40);        
	SPI_RW_Reg(RFWRITE_REG + RX_PW_P0, RX_PLOAD_WIDTH); 
	SPI_RW_Reg(RFWRITE_REG + RF_SETUP, 0x07);   
	
	nRF24L01_CE_H;
}



u8 SPI_Read(u8 reg)
{
	u8 reg_val;	
	nRF24L01_CSN_L;                
    Spi2_SendByte(reg);
	reg_val = Spi2_SendByte(0);    
	nRF24L01_CSN_H;               
	return(reg_val);        
}
   




u8 SPI_RW_Reg(u8 reg, u8 value)
{
	u8 status;	
	nRF24L01_CSN_L;                   // CSN low, init SPI transaction
	status = Spi2_SendByte(reg);
    Spi2_SendByte(value);
	nRF24L01_CSN_H;                // CSN high again	
	return(status);            // return nRF24L01 status uchar
}





u8 SPI_Read_Buf(u8 reg, u8 *pBuf,u8 nchars)
{
	u8 status,uchar_ctr;	
	nRF24L01_CSN_L;                   		// Set CSN low, init SPI tranaction
	status = Spi2_SendByte(reg);
	for(uchar_ctr=0;uchar_ctr<nchars;uchar_ctr++)		  
    pBuf[uchar_ctr] = Spi2_SendByte(0); 				  
	nRF24L01_CSN_H;                          	
	return status;                    // return nRF24L01 status uchar
}




u8 SPI_Write_Buf(u8 reg, u8 *pBuf, u8 nchars)
{
	u8 status,uchar_ctr;	
	nRF24L01_CSN_L;            
	  status = Spi2_SendByte(reg);
	for(uchar_ctr=0; uchar_ctr<nchars; uchar_ctr++) 
		Spi2_SendByte(*pBuf++);						
	nRF24L01_CSN_H;           
	return(status);    // 
}



u8 nRF24L01_RxPacket(u8* rx_buf)
{	 
	u8 sta;
	sta=SPI_Read(STATUS);	    
	SPI_RW_Reg(RFWRITE_REG+STATUS,sta);   
	if(sta&RX_OK)				
	{
		SPI_Read_Buf(RD_RX_PLOAD,rx_buf,TX_PLOAD_WIDTH);// read receive payload from RX_FIFO buffer
		SPI_RW_Reg(FLUSH_RX,0xff);
		return 0; 
	}
	return 1;
}




u8 nRF24L01_TxPacket(u8 * tx_buf)
{	 
    u8 st;
	nRF24L01_CE_L;	
	SPI_Write_Buf(WR_TX_PLOAD, tx_buf, TX_PLOAD_WIDTH); 			 

	nRF24L01_CE_H;	
	while(RF_IRQ()!=0) IWDG_Feed();
	st=	SPI_Read(STATUS);			  
	SPI_RW_Reg(RFWRITE_REG+STATUS,st);  
	
	if(st&MAX_TX)
	{
		SPI_RW_Reg(FLUSH_TX,0xff);
		return MAX_TX; 
	}
	
	if(st&TX_OK)				 
	{
		return TX_OK;
	}
	return 0xff;
}

void RX_Mode(void)
{
	nRF24L01_CE_L;	
	SPI_RW_Reg(RFWRITE_REG + CONFIG, 0x0f);   	     // Set PWR_UP bit, enable CRC(2 bytes) & Prim:RX. RX_DR enabled..
  	nRF24L01_CE_H;
}

void TX_Mode(void)
{
	nRF24L01_CE_L;
	SPI_RW_Reg(RFWRITE_REG + CONFIG, 0x0e);     // Set PWR_UP bit, enable CRC(2 bytes) & Prim:TX. MAX_RT & TX_DS enabled.
	nRF24L01_CE_H;
}

void rf24l01_irq_init(void)  
{
    EXTI_InitTypeDef EXTI_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
    
    
  	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource12);
    
  	EXTI_InitStructure.EXTI_Line = EXTI_Line12;
  	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;	
  	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
  	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    
  	EXTI_Init(&EXTI_InitStructure);
	
    
  	NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;
    
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00;	//
    
  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x01;		
    
  	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;	
    
  	NVIC_Init(&NVIC_InitStructure);
}

void rf24l01_rx_process(void)
{
	uint8_t rf_buf[32];
	if(nRF24L01_RxPacket(rf_buf)==0)	
	{
		printf_fifo_hex(rf_buf, 32);
	}
}

void EXTI15_10_IRQHandler(void) //genera interrupcion para cuando recibe un dato por radio frecuencia
{
	if(EXTI_GetITStatus(EXTI_Line12) != RESET)//lora_irq PC9
	{
        EXTI_ClearFlag(EXTI_Line12);
		//rf24l01_rx_process();
		rf_int_flag = 1;
		printf("RF24L01_IRQ\n");
	}
}
