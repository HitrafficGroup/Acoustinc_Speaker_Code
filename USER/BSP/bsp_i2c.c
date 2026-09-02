//RTC I2C

#include "stm32f10x.h" 

void RtcIrqConfig(void);
void RtcFreqConfig(void);

uint8_t rtc[7];
RtcType* SYS_RTC;

void bsp_Init_RTCIRQ(void)  
{
    EXTI_InitTypeDef EXTI_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
    

  	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB,GPIO_PinSource5);

  	EXTI_InitStructure.EXTI_Line = EXTI_Line5;
  	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;	
  	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
  	EXTI_InitStructure.EXTI_LineCmd = ENABLE;

  	EXTI_Init(&EXTI_InitStructure);
	

  	NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;

  	//NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02;	

  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x01;		

  	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;	

  	NVIC_Init(&NVIC_InitStructure);
}


void bsp_InitI2C(void)  
{
    GPIO_InitTypeDef GPIO_InitStructure;   
    I2C_InitTypeDef I2C_InitStructure;   

    
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1,ENABLE);
    
    /* PB5-RTC_IRQ */  
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;   
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;   
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;   
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    /* PB6-I2C1_SCL PB7-I2C1_SDA */  
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    
    I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
    I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;   
    I2C_InitStructure.I2C_OwnAddress1 = 0XA0;
    I2C_InitStructure.I2C_Ack = I2C_Ack_Enable; // Configura o ejecuta una transaccion I2C o RTC.
    I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;   
    I2C_InitStructure.I2C_ClockSpeed = 50000;//50000
    
    I2C_Init(I2C1, &I2C_InitStructure);
    
    I2C_Cmd(I2C1,ENABLE);
    I2C_AcknowledgeConfig(I2C1, ENABLE);
    
    SYS_RTC = (RtcType*)rtc;
    bsp_Init_RTCIRQ();
    RtcIrqConfig();
    //RtcFreqConfig();
}


void RTC_WriteReg(uint8_t slave_addr, uint8_t WriteRegAddr, uint8_t WriteValue)
{ 
    I2C_GenerateSTART(I2C1, ENABLE);
    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT));
    
    I2C_Send7bitAddress(I2C1, slave_addr, I2C_Direction_Transmitter);
    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));
    
    /* 3.Write Data Address */
    I2C_SendData(I2C1, WriteRegAddr);
    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
    
    I2C_SendData(I2C1, WriteValue);
    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
    
    I2C_GenerateSTOP(I2C1, ENABLE);
    while((I2C1->CR1 & TIM_CR1_CKD_1) == TIM_CR1_CKD_1);
}

uint8_t RTC_BufferRead(uint8_t slave_addr, uint8_t DataAddress, uint8_t* pBuffer, uint8_t NumByteToRead) 
{
    while(I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY));  
    I2C_AcknowledgeConfig(I2C1, ENABLE);
    /* 1.Start */
    I2C_GenerateSTART(I2C1, ENABLE);
    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT));
    /* 2.Write Slave Address */
    I2C_Send7bitAddress(I2C1, slave_addr, I2C_Direction_Transmitter);
    while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));
    /* 3.Write Data Address */
    I2C_SendData(I2C1, DataAddress);
    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
    
    /* 4.Start*/
    I2C_GenerateSTART(I2C1, ENABLE);
    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT));
    /* 5.Write Slave Address */
    I2C_Send7bitAddress(I2C1, slave_addr, I2C_Direction_Receiver);
    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED));

    /* 6.conturn Write data */
    while(NumByteToRead)
    {
        if(NumByteToRead==1)
        {
            I2C_AcknowledgeConfig(I2C1, DISABLE);//6.noack
            I2C_GenerateSTOP(I2C1, ENABLE);//STOP
        }

        while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_RECEIVED));  /* EV7 */
        *pBuffer++ = I2C_ReceiveData(I2C1);
        NumByteToRead--;
    }

    I2C_AcknowledgeConfig(I2C1, ENABLE);
    return 1;
}

uint8_t RTC_BufferWrite(uint8_t slave_addr, uint8_t DataAddress, uint8_t* pBuffer, uint8_t NumByteToWrite)
{
    I2C_GenerateSTART(I2C1, ENABLE);
    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT));

    I2C_Send7bitAddress(I2C1, slave_addr, I2C_Direction_Transmitter);
    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));

    /* 3.Write Data Address */
    I2C_SendData(I2C1, DataAddress);
    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
    
    while(NumByteToWrite--)
    {
      I2C_SendData(I2C1, *pBuffer);
      while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
      pBuffer++;
    }

    I2C_GenerateSTOP(I2C1, ENABLE);
    while((I2C1->CR1 & TIM_CR1_CKD_1) == TIM_CR1_CKD_1);
    return 0;
}

void RTC_WriteEnable(void)
{
    RTC_WriteReg(ISL1208_ADDR, SR_Reg, 0x10);
}

void RTC_WriteDisable(void)
{
    RTC_WriteReg(ISL1208_ADDR, SR_Reg, 0x00);
}

void RtcRead(RtcType* Time)
{
	RTC_BufferRead(ISL1208_ADDR, 0x00, (uint8_t*)Time, 0x07);
    Time->hour &=0x7f;
}

void RtcWrite(RtcType* Time)
{
    Time->hour |= 0x80;
	RTC_WriteEnable();
    RTC_BufferWrite(ISL1208_ADDR, 0x00, (uint8_t*)Time, 0x07);
	RTC_WriteDisable();
}

void RtcIrqConfig(void)
{
    RTC_WriteReg(ISL1208_ADDR, SecAlarm, 0x80);
    RTC_WriteReg(ISL1208_ADDR, MinAlarm, 0x00);
    RTC_WriteReg(ISL1208_ADDR, HourAlarm, 0x00);
    RTC_WriteReg(ISL1208_ADDR, DayAlarm, 0x00);
    RTC_WriteReg(ISL1208_ADDR, MonAlarm, 0x00);
    RTC_WriteReg(ISL1208_ADDR, WeekAlarm, 0x00);
    
    RTC_WriteReg(ISL1208_ADDR, INT_Reg, 0xC0);      //
}

void RtcFreqConfig(void)
{
    RTC_WriteReg(ISL1208_ADDR, INT_Reg, 0x0A);
}

void EXTI9_5_IRQHandler(void)
{

    if(EXTI_GetITStatus(EXTI_Line5) != RESET)
	{
        //printf("E5_IRQ\r\n");
        system_temp.timeUpdate = 1;

        EXTI_ClearITPendingBit(EXTI_Line5);
    }
}


