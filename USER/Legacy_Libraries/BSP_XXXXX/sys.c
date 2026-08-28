#include "sys.h"
#include "stm32f10x.h"
#include "Systick.h"
/**
  */
void RCC_Configuration(void)
{
  ErrorStatus HSEStartUpStatus;
  RCC_DeInit();
  RCC_HSEConfig(RCC_HSE_ON);
  HSEStartUpStatus = RCC_WaitForHSEStartUp();
  if(HSEStartUpStatus == SUCCESS)
  {
    FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);
    FLASH_SetLatency(FLASH_Latency_2);
      	
    RCC_HCLKConfig(RCC_SYSCLK_Div1);
    RCC_PCLK2Config(RCC_HCLK_Div1);
    RCC_PCLK1Config(RCC_HCLK_Div2);
         
    RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_9);
    RCC_PLLCmd(ENABLE);
   
    while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET);
    
    RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);
    while(RCC_GetSYSCLKSource() != 0x08);
  }
}

void GPIO_Config(void) 
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOC|RCC_APB2Periph_GPIOD, ENABLE);	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
    GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable , ENABLE); //GPIO_Remap_SWJ_JTAGDisable, JTAG-DP ?? + SW-DP ??
  
	
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	    
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;			    
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP; 	    
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_ResetBits(GPIOA,GPIO_Pin_11);
	
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	    
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3|GPIO_Pin_5;	
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP; 	    
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	GPIO_ResetBits(GPIOB,GPIO_Pin_3);
	GPIO_ResetBits(GPIOB,GPIO_Pin_5);

	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	    
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11|GPIO_Pin_12;  
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;       
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	GPIO_SetBits(GPIOC,GPIO_Pin_11);
	GPIO_SetBits(GPIOC,GPIO_Pin_12);

	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;   
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;		    
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;   
	GPIO_Init(GPIOD, &GPIO_InitStructure);
	GPIO_SetBits(GPIOD,GPIO_Pin_2);
}

void Timer2_init()	
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
    RCC_APB1PeriphClockCmd (RCC_APB1Periph_TIM2,ENABLE);

    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
    NVIC_InitStructure.NVIC_IRQChannel =TIM2_IRQn;// TIM4_IRQChannel; 
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority =1; 
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1; 
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; 
    NVIC_Init (&NVIC_InitStructure); 

    TIM_TimeBaseStructure.TIM_Period = 10000; 
    TIM_TimeBaseStructure.TIM_Prescaler = 6000;
    TIM_TimeBaseStructure.TIM_ClockDivision = 0x0; 	
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; 
    TIM_TimeBaseInit(TIM2, & TIM_TimeBaseStructure); 
    TIM_Cmd(TIM2, ENABLE); 	 

    TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE); 
}

void EXTI_Config(void)
{
    EXTI_InitTypeDef EXTI_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
    //============================================================B1
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);	
    NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource5);
    EXTI_InitStructure.EXTI_Line = EXTI_Line5;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;  
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);
}
