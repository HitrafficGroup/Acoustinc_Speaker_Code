/*    _______    ______     _____      ____       ___        __         _
 * |_|       |__|      |___|     |____|    |_____|   |______|  |_______| |________|
 */

#include "PWM.h"

void TIM4_GPIO_Config(void) 
{
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE); 
    GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_6|GPIO_Pin_7|GPIO_Pin_8|GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;		    
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;  
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    
}

uint16_t DIM[4];


void TIM4_Mode_Config(void)
{
    TIM_OCInitTypeDef  TIM_OCInitStructure;
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE); 					
    TIM4_GPIO_Config(); 
    
    
    TIM_TimeBaseStructure.TIM_Period = 999;       							
    TIM_TimeBaseStructure.TIM_Prescaler = 719;	     						
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1 ;			    
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  	        
    TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure); 
    //
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; 				
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;  	    
    
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;	
    TIM_OCInitStructure.TIM_Pulse = 100;							
    TIM_OC1Init(TIM4, &TIM_OCInitStructure);	 				    
    TIM_OC1PreloadConfig(TIM4, TIM_OCPreload_Enable);			    
    
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;	
    TIM_OCInitStructure.TIM_Pulse = 100;							
    TIM_OC2Init(TIM4, &TIM_OCInitStructure);	 				    
    TIM_OC2PreloadConfig(TIM4, TIM_OCPreload_Enable);			    
    
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;	
    TIM_OCInitStructure.TIM_Pulse = 100;							
    TIM_OC3Init(TIM4, &TIM_OCInitStructure);	 				    
    TIM_OC3PreloadConfig(TIM4, TIM_OCPreload_Enable);			    
    
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;	
    TIM_OCInitStructure.TIM_Pulse = 100;							
    TIM_OC4Init(TIM4, &TIM_OCInitStructure);	 			        
    TIM_OC4PreloadConfig(TIM4, TIM_OCPreload_Enable);		        

    TIM_CtrlPWMOutputs(TIM4,ENABLE);

    TIM_ARRPreloadConfig(TIM4, ENABLE);			 				    
    TIM_Cmd(TIM4, ENABLE);                   					    
}

void DIM_CH4(uint16_t dim)
{
    TIM_OCInitTypeDef  TIM_OCInitStructure;
    
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; 				
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;  	    
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;	
    TIM_OCInitStructure.TIM_Pulse = dim;							
    TIM_OC4Init(TIM4, &TIM_OCInitStructure);	 				    
    TIM_OC4PreloadConfig(TIM4, TIM_OCPreload_Enable);			    
}
void TIM3_GPIO_Config(void) 
{
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE); 
    GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_0|GPIO_Pin_1;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;		    
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;  
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE); 
    GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_6|GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;		    
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;  
    GPIO_Init(GPIOA, &GPIO_InitStructure);
}

void TIM3_Mode_Config(void)
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    TIM_OCInitTypeDef  TIM_OCInitStructure;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE); 					

    
    TIM3_GPIO_Config(); 	
    TIM_TimeBaseStructure.TIM_Period =65535;       							  
    TIM_TimeBaseStructure.TIM_Prescaler = 0;	    							
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1 ;			
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  	
    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);              //
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;	    				
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;  	  
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;	
    TIM_OCInitStructure.TIM_Pulse = 0;										  			
    TIM_OC1Init(TIM3, &TIM_OCInitStructure);	 									
    TIM_OC1PreloadConfig(TIM3, TIM_OCPreload_Enable);						
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;	
    TIM_OCInitStructure.TIM_Pulse = 0;										  			
    TIM_OC2Init(TIM3, &TIM_OCInitStructure);	 									
    TIM_OC2PreloadConfig(TIM3, TIM_OCPreload_Enable);						
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;	
    TIM_OCInitStructure.TIM_Pulse = 256;										  			
    TIM_OC3Init(TIM3, &TIM_OCInitStructure);	 									
    TIM_OC3PreloadConfig(TIM3, TIM_OCPreload_Enable);						
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;	
    TIM_OCInitStructure.TIM_Pulse = 0;										  			
    TIM_OC4Init(TIM3, &TIM_OCInitStructure);	 									
    TIM_OC4PreloadConfig(TIM3, TIM_OCPreload_Enable);						
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    TIM_ARRPreloadConfig(TIM3, ENABLE);			 										
    TIM_Cmd(TIM3, ENABLE);                   										
}

void TIM2_GPIO_Config(void) 
{
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE); 
    GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;		    
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;  
    GPIO_Init(GPIOA, &GPIO_InitStructure);
}

void TIM2_Mode_Config(void)
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    TIM_OCInitTypeDef  TIM_OCInitStructure;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE); 					

    
    TIM2_GPIO_Config(); 	
    TIM_TimeBaseStructure.TIM_Period =65535;       							  
    TIM_TimeBaseStructure.TIM_Prescaler = 0;	    							
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1 ;			
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  	
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);              //
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;	    				
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;  	  
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;	
    TIM_OCInitStructure.TIM_Pulse = 0;										  			
    TIM_OC1Init(TIM2, &TIM_OCInitStructure);	 									
    TIM_OC1PreloadConfig(TIM2, TIM_OCPreload_Enable);						
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;	
    TIM_OCInitStructure.TIM_Pulse = 0;										  			
    TIM_OC2Init(TIM2, &TIM_OCInitStructure);	 									
    TIM_OC2PreloadConfig(TIM2, TIM_OCPreload_Enable);						
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;	
    TIM_OCInitStructure.TIM_Pulse = 0;										  			
    TIM_OC3Init(TIM2, &TIM_OCInitStructure);	 									
    TIM_OC3PreloadConfig(TIM2, TIM_OCPreload_Enable);						
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;	
    TIM_OCInitStructure.TIM_Pulse = 0;										  			
    TIM_OC4Init(TIM2, &TIM_OCInitStructure);	 									
    TIM_OC4PreloadConfig(TIM2, TIM_OCPreload_Enable);						
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    TIM_ARRPreloadConfig(TIM2, ENABLE);			 										
    TIM_Cmd(TIM2, ENABLE);                   										
}
