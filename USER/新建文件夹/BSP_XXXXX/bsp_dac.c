/*
*********************************************************************************************************
*
*	模块名称 : DAC驱动模块
*	文件名称 : bsp_dac.c
*	版    本 : V1.0
*	说    明 : 实现DAC输出正弦波
*	修改记录 :
*		版本号    日期        作者     说明
*		V1.0    2013-12-30   armfly   正式发布
*
*	Copyright (C), 2013-2014, 安富莱电子 www.armfly.com
*
*********************************************************************************************************
*/
#include "bsp_dac.h" 

#define DAC_DHR12R1_Address     0x40007408

/* 正弦波参考点 */
const uint16_t Sine12bit[32] = {
                      2047, 2447, 2831, 3185, 3498, 3750, 3939, 4056, 4095, 4056,
                      3939, 3750, 3495, 3185, 2831, 2447, 2047, 1647, 1263, 909, 
                      599, 344, 155, 38, 0, 38, 155, 344, 599, 909, 1263, 1647};
/*
*********************************************************************************************************
*	函 数 名: bsp_InitDAC
*	功能说明: DAC初始化
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
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

	
	TIM_ClearFlag(TIM4, TIM_FLAG_Update);					//清中断标志
	TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE);		//TIM3->DIER|=1<<0;//允许更新中断
	TIM_ITConfig(TIM4, TIM_IT_Trigger, ENABLE);		//TIM3->DIER|=1<<6;//允许触发中断
	TIM_Cmd(TIM4, ENABLE);												//TIM3->CR1|=0x01; //使能定时器
	

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}


void DAC1_SetData(u16 data)//12位右对齐
{
    DAC->DHR12R1=data;
    DAC->SWTRIGR|=0x01;
}
void DAC2_SetData(u16 data)//12位右对齐
{
    DAC->DHR12R2=data;
    DAC->SWTRIGR|=0x02;
}

void DAC_Ch1_Config(void)
{
    DAC_InitTypeDef  DAC_InitStructure;
    
    /* DAC 外设时钟使能 */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_DAC, ENABLE);
    
    /* DMA1 时钟使能 */
 
    /* DAC通道1配置 */
    DAC_InitStructure.DAC_Trigger = DAC_Trigger_Software;
    DAC_InitStructure.DAC_WaveGeneration = DAC_WaveGeneration_None;
    DAC_InitStructure.DAC_LFSRUnmask_TriangleAmplitude = DAC_LFSRUnmask_Bits8_0;
    DAC_InitStructure.DAC_OutputBuffer = DAC_OutputBuffer_Enable;
    DAC_Init(DAC_Channel_1, &DAC_InitStructure);

    /* 使能DAC通道 */
    DAC_Cmd(DAC_Channel_1, ENABLE);
    DAC1_SetData(0x000);
}


/*
*********************************************************************************************************
*	函 数 名: TIM6_Config
*	功能说明: 配置定时器6，用于触发DAC，每TIM6CLK/26更新一次
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
void TIM6_Config(void)
{
    TIM_TimeBaseInitTypeDef    TIM_TimeBaseStructure;
    
     /* 使能TIM6时钟 */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6, ENABLE);
    
    /* --------------------------------------------------------
    system_stm32f2xx.c 文件中 void SetSysClock(void) 函数对时钟的配置如下：

    HCLK  = SYSCLK / 1     (AHB1Periph)	= 120M
    PCLK2 = HCLK   / 2     (APB2Periph) =  60M
    PCLK1 = HCLK   / 4     (APB1Periph) =  30M

    因为APB1 prescaler != 1, 所以 APB1上的TIMxCLK = PCLK1 x 2 = SystemCoreClock / 2;
    因为APB2 prescaler != 1, 所以 APB2上的TIMxCLK = PCLK2 x 2 = SystemCoreClock;

    APB1 定时器有 TIM2, TIM3 ,TIM4, TIM5, TIM6, TIM7, TIM12, TIM13, TIM14
    APB2 定时器有 TIM1, TIM8 ,TIM9, TIM10, TIM11
          
    TIM6 更新周期是 = TIM6CLK / （TIM_Period + 1）/（TIM_Prescaler + 1）= TIM6CLK/256
    ----------------------------------------------------------- */
    
    /* 配置TIM6 */
    TIM_TimeBaseStructInit(&TIM_TimeBaseStructure); 
    TIM_TimeBaseStructure.TIM_Period = 25;          
    TIM_TimeBaseStructure.TIM_Prescaler = 0;       
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;    
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  
    TIM_TimeBaseInit(TIM6, &TIM_TimeBaseStructure);

    /* TIM6 TRGO 选择 */
    TIM_SelectOutputTrigger(TIM6, TIM_TRGOSource_Update);

    /* 使能TIM6 */
    TIM_Cmd(TIM6, ENABLE);   
}

/*
*********************************************************************************************************
*	函 数 名: DAC_Ch1_SineWaveConfig
*	功能说明: DAC通道1正弦波输出
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
void DAC_Ch1_SineWaveConfig(void)
{
    DAC_InitTypeDef  DAC_InitStructure;
    DMA_InitTypeDef DMA_InitStructure;
      
    /* DAC 外设时钟使能 */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_DAC, ENABLE);
    
    /* DMA1 时钟使能 */
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA2, ENABLE);
     
    /* DAC通道1配置 */
    DAC_InitStructure.DAC_Trigger = DAC_Trigger_T6_TRGO;
    DAC_InitStructure.DAC_WaveGeneration = DAC_WaveGeneration_None;
    DAC_InitStructure.DAC_OutputBuffer = DAC_OutputBuffer_Enable;
    DAC_Init(DAC_Channel_1, &DAC_InitStructure);
    
    /* DMA2_Channel3 通道 13 配置 **************************************/  
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
 

    /* 使能 DMA2_Channel3 */
    DMA_Cmd(DMA2_Channel3, ENABLE);

    /* 使能DAC通道 */
    DAC_Cmd(DAC_Channel_1, ENABLE);
    DAC1_SetData(0x000);
    /* 使能DAC的DMA通道 */
    DAC_DMACmd(DAC_Channel_1, ENABLE);    
}

/*
*********************************************************************************************************
*	函 数 名: DAC_GPIOConfig
*	功能说明: DAC通道一的PA4引脚配置
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
void DAC_GPIOConfig(void)
{
    GPIO_InitTypeDef GPIO_InitStructure; 
    /*  使能引脚时钟 */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    /* DAC通道1输出引脚配置（PA.4） */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);    
}
