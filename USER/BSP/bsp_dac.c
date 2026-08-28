
#include "stm32f10x.h"
//#include "bsp_dac.h" 

#define DAC_DHR12R1_Address     0x40007408


const uint16_t Sine12bit[32] = {
                      2047, 2447, 2831, 3185, 3498, 3750, 3939, 4056, 4095, 4056,
                      3939, 3750, 3495, 3185, 2831, 2447, 2047, 1647, 1263, 909, 
                      599, 344, 155, 38, 0, 38, 155, 344, 599, 909, 1263, 1647};
/* Configura o actualiza la salida del DAC. */
void bsp_InitDAC(void)
{
//    DAC_GPIOConfig(); 
//    DAC_Ch1_SineWaveConfig();
//    TIM6_Config(); 
	
    DAC_GPIOConfig();
    DAC_Ch1_Config();
}

void Timer4_Init(u16 arr,u16 psc)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	TIM_TimeBaseInitTypeDef    TIM_TimeBaseStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
	
	TIM_TimeBaseStructInit(&TIM_TimeBaseStructure); 
	TIM_TimeBaseStructure.TIM_Period = arr;          
	TIM_TimeBaseStructure.TIM_Prescaler = psc;
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;    
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);

	
	TIM_ClearFlag(TIM4, TIM_FLAG_Update);			// Configura o actualiza el temporizador utilizado por esta rutina.
	TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE);		// Configura o actualiza el temporizador utilizado por esta rutina.
	TIM_ITConfig(TIM4, TIM_IT_Trigger, ENABLE);		// Configura o actualiza el temporizador utilizado por esta rutina.
	TIM_Cmd(TIM4, ENABLE);							// Configura o actualiza el temporizador utilizado por esta rutina.
	

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}


void DAC1_SetData(u16 data)// Configura o actualiza la salida del DAC.
{
    DAC->DHR12R1=data;
    DAC->SWTRIGR|=0x01;
}
void DAC2_SetData(u16 data)// Configura o actualiza la salida del DAC.
{
    DAC->DHR12R2=data;
    DAC->SWTRIGR|=0x02;
}

void DAC_Ch1_Config(void)
{
    DAC_InitTypeDef  DAC_InitStructure;
    
    /* Configura o actualiza la salida del DAC. */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_DAC, ENABLE);
    
    /* Configura o actualiza la salida del DAC. */
 
    /* Configura o actualiza la salida del DAC. */
    DAC_InitStructure.DAC_Trigger = DAC_Trigger_Software;
    DAC_InitStructure.DAC_WaveGeneration = DAC_WaveGeneration_None;
    DAC_InitStructure.DAC_LFSRUnmask_TriangleAmplitude = DAC_LFSRUnmask_Bits8_0;
    DAC_InitStructure.DAC_OutputBuffer = DAC_OutputBuffer_Enable;
    DAC_Init(DAC_Channel_1, &DAC_InitStructure);

    /* Configura o actualiza la salida del DAC. */
    DAC_Cmd(DAC_Channel_1, ENABLE);
    DAC1_SetData(0x000);
}


/* Configura o actualiza el temporizador utilizado por esta rutina. */
void TIM6_Config(void)
{
    TIM_TimeBaseInitTypeDef    TIM_TimeBaseStructure;
    
     /* Configura o actualiza el temporizador utilizado por esta rutina. */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6, ENABLE);
    
    /* Configura o actualiza el temporizador utilizado por esta rutina. */
    
    /* Configura o actualiza el temporizador utilizado por esta rutina. */
    TIM_TimeBaseStructInit(&TIM_TimeBaseStructure); 
    TIM_TimeBaseStructure.TIM_Period = 25;          
    TIM_TimeBaseStructure.TIM_Prescaler = 0;       
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;    
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  
    TIM_TimeBaseInit(TIM6, &TIM_TimeBaseStructure);

    /* Configura o actualiza el temporizador utilizado por esta rutina. */
    TIM_SelectOutputTrigger(TIM6, TIM_TRGOSource_Update);

    /* Configura o actualiza el temporizador utilizado por esta rutina. */
    TIM_Cmd(TIM6, ENABLE);   
}

/* Configura o actualiza la salida del DAC. */
void DAC_Ch1_SineWaveConfig(void)
{
    DAC_InitTypeDef  DAC_InitStructure;
    DMA_InitTypeDef DMA_InitStructure;
      
    /* Configura o actualiza la salida del DAC. */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_DAC, ENABLE);
    

    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA2, ENABLE);
     
    /* Configura o actualiza la salida del DAC. */
    DAC_InitStructure.DAC_Trigger = DAC_Trigger_T6_TRGO;
    DAC_InitStructure.DAC_WaveGeneration = DAC_WaveGeneration_None;
    DAC_InitStructure.DAC_OutputBuffer = DAC_OutputBuffer_Enable;
    DAC_Init(DAC_Channel_1, &DAC_InitStructure);
    

    DMA_DeInit(DMA2_Channel3);
    DMA_InitStructure.DMA_PeripheralBaseAddr = DAC_DHR12R1_Address;
    DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)&Sine12bit;//(uint32_t)&g_Wave;
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
    DMA_InitStructure.DMA_BufferSize = 32;
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;//DMA_Mode_Circular; //
    DMA_InitStructure.DMA_Priority = DMA_Priority_High;
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
    DMA_Init(DMA2_Channel3, &DMA_InitStructure);
 


    DMA_Cmd(DMA2_Channel3, ENABLE);

    /* Configura o actualiza la salida del DAC. */
    DAC_Cmd(DAC_Channel_1, ENABLE);
    DAC1_SetData(0x000);

    DAC_DMACmd(DAC_Channel_1, ENABLE);    
}


void DAC_GPIOConfig(void)
{
    GPIO_InitTypeDef GPIO_InitStructure; 

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);    
}
