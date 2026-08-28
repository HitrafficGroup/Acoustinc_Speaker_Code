/* Controlador de modulacion por ancho de pulso. */
#include "stm32f10x.h"

void TIM4_GPIO_Config(void) 
{
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE); 
    GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_6 | GPIO_Pin_8 | GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;  
    GPIO_Init(GPIOB, &GPIO_InitStructure);
}

void TIM4_Mode_Config(void)
{
    TIM_OCInitTypeDef  TIM_OCInitStructure;
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
    
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE); 					// Configura o actualiza el temporizador utilizado por esta rutina.
    TIM4_GPIO_Config(); 
    
    // Configura o actualiza el temporizador utilizado por esta rutina.
    TIM_TimeBaseStructure.TIM_Period = 999;       							// Configura o actualiza el temporizador utilizado por esta rutina.
    TIM_TimeBaseStructure.TIM_Prescaler = 719;	     						// Configura o actualiza el temporizador utilizado por esta rutina.
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1 ;			    // Configura o actualiza el temporizador utilizado por esta rutina.
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  	        // Configura o actualiza el temporizador utilizado por esta rutina.
    TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure); 

    // Configura o actualiza el temporizador utilizado por esta rutina.
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; 				// Configura o actualiza el temporizador utilizado por esta rutina.
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;	// Configura o actualiza el temporizador utilizado por esta rutina.
    TIM_OCInitStructure.TIM_Pulse = 0;							    // Configura o actualiza el temporizador utilizado por esta rutina.
    
    TIM_OCInitStructure.TIM_OCPolarity =TIM_OCPolarity_Low ;  	    // Configura o actualiza el temporizador utilizado por esta rutina.
    TIM_OC1Init(TIM4, &TIM_OCInitStructure);	 				    // Configura o actualiza el temporizador utilizado por esta rutina.
    
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;  	    // Configura o actualiza el temporizador utilizado por esta rutina.
    TIM_OC3Init(TIM4, &TIM_OCInitStructure); 				        // Configura o actualiza el temporizador utilizado por esta rutina.
    TIM_OC4Init(TIM4, &TIM_OCInitStructure);	 			        // Configura o actualiza el temporizador utilizado por esta rutina.
    
    TIM_OC1PreloadConfig(TIM4, TIM_OCPreload_Enable);			    // Configura o actualiza el temporizador utilizado por esta rutina.
    TIM_OC3PreloadConfig(TIM4, TIM_OCPreload_Enable);			    // Configura o actualiza el temporizador utilizado por esta rutina.
    TIM_OC4PreloadConfig(TIM4, TIM_OCPreload_Enable);		        // Configura o actualiza el temporizador utilizado por esta rutina.
    
    TIM_CtrlPWMOutputs(TIM4,ENABLE);

    TIM_ARRPreloadConfig(TIM4, ENABLE);			 				    // Configura o actualiza el temporizador utilizado por esta rutina.
    //TIM_ARRPreloadConfig(TIM4, DISABLE);
	
    TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE);//TIM_IT_CC4
    
    TIM_Cmd(TIM4, ENABLE); // Configura o actualiza el temporizador utilizado por esta rutina.
    
    NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}
void DIM_CH1_SHUZI(uint16_t dim)
{
    TIM_SetCompare1(TIM4,dim);
}

void DIM_CH3_xiangao(uint16_t dim)
{
    TIM_SetCompare3(TIM4,dim);	
}

void DIM_CH4_MI(uint16_t dim)
{
    TIM_SetCompare4(TIM4,dim);
}

void diming(uint16_t dim)
{
    DIM_CH1_SHUZI(dim);
    DIM_CH3_xiangao(dim);
    DIM_CH4_MI(dim);
}

uint8_t adc_count = 0;
uint8_t adc_count1 = 0;
uint8_t adc[8];

uint8_t reload_flag;


void TIM4_IRQHandler(void)
{
    if(TIM_GetITStatus(TIM4, TIM_IT_Update) != RESET)//TIM_IT_CC4
    {
        TIM_ClearITPendingBit(TIM4,TIM_IT_Update);
        reload_flag = 1;
        if(++adc_count >= 100)
        {
            adc_count = 0;
            //read_detect(); 

            adc[4] = ADC_ConvertedValue[2]>>8;
            adc[5] = ADC_ConvertedValue[2];
            adc[6] = ADC_ConvertedValue[3]>>8;
            adc[7] = ADC_ConvertedValue[3];   
            //UartSendBuf(adc+4,4);
        }
        display_data();
    }
}
