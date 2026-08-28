/*    _______    ______     _____      ____       ___        __         _
 * |_|       |__|      |___|     |____|    |_____|   |______|  |_______| |________|
 */

#include "PWM.h"

void TIM4_GPIO_Config(void) 
{
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE); 
    GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_6|GPIO_Pin_7|GPIO_Pin_8|GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;		    // 复用推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;  
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    
}

uint16_t DIM[4];


void TIM4_Mode_Config(void)
{
    TIM_OCInitTypeDef  TIM_OCInitStructure;
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE); 					//使能TIM3时钟
    TIM4_GPIO_Config(); 
    
    // 基本定时器配置 
    TIM_TimeBaseStructure.TIM_Period = 999;       							//当定时器从0计数到255，即为266次，为一个定时周期
    TIM_TimeBaseStructure.TIM_Prescaler = 719;	     						//设置预分频：
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1 ;			    //设置时钟分频系数：不分频(这里用不到)
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  	        //向上计数模式
    TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure); 
    //
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; 				//配置为PWM模式1
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;  	    //当定时器计数值小于CCR1_Val时为低电平
    //通道1 
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;	//使能输出
    TIM_OCInitStructure.TIM_Pulse = 100;							//设置初始PWM脉冲宽度为0	
    TIM_OC1Init(TIM4, &TIM_OCInitStructure);	 				    //使能通道1
    TIM_OC1PreloadConfig(TIM4, TIM_OCPreload_Enable);			    //使能预装载	
    //通道2 
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;	//使能输出
    TIM_OCInitStructure.TIM_Pulse = 100;							//设置初始PWM脉冲宽度为0	
    TIM_OC2Init(TIM4, &TIM_OCInitStructure);	 				    //使能通道2
    TIM_OC2PreloadConfig(TIM4, TIM_OCPreload_Enable);			    //使能预装载
    //通道3 
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;	//使能输出
    TIM_OCInitStructure.TIM_Pulse = 100;							//设置初始PWM脉冲宽度为0	
    TIM_OC3Init(TIM4, &TIM_OCInitStructure);	 				    //使能通道1
    TIM_OC3PreloadConfig(TIM4, TIM_OCPreload_Enable);			    //使能预装载	
    //通道4 
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;	//使能输出
    TIM_OCInitStructure.TIM_Pulse = 100;							//设置初始PWM脉冲宽度为0	
    TIM_OC4Init(TIM4, &TIM_OCInitStructure);	 			        //使能通道2
    TIM_OC4PreloadConfig(TIM4, TIM_OCPreload_Enable);		        //使能预装载

    TIM_CtrlPWMOutputs(TIM4,ENABLE);

    TIM_ARRPreloadConfig(TIM4, ENABLE);			 				    //使能TIM1重载寄存器ARR
    TIM_Cmd(TIM4, ENABLE);                   					    //使能定时器1
}

void DIM_CH4(uint16_t dim)
{
    TIM_OCInitTypeDef  TIM_OCInitStructure;
    
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; 				//配置为PWM模式1
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;  	    //当定时器计数值小于CCR1_Val时为低电平
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;	//使能输出
    TIM_OCInitStructure.TIM_Pulse = dim;							//设置初始PWM脉冲宽度为0	
    TIM_OC4Init(TIM4, &TIM_OCInitStructure);	 				    //使能通道1
    TIM_OC4PreloadConfig(TIM4, TIM_OCPreload_Enable);			    //使能预装载	
}
void TIM3_GPIO_Config(void) 
{
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE); 
    GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_0|GPIO_Pin_1;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;		    // 复用推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;  
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE); 
    GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_6|GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;		    // 复用推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;  
    GPIO_Init(GPIOA, &GPIO_InitStructure);
}

void TIM3_Mode_Config(void)
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    TIM_OCInitTypeDef  TIM_OCInitStructure;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE); 					//使能TIM3时钟

    // 基本定时器配置 
    TIM3_GPIO_Config(); 	
    TIM_TimeBaseStructure.TIM_Period =65535;       							  //当定时器从0计数到255，即为266次，为一个定时周期
    TIM_TimeBaseStructure.TIM_Prescaler = 0;	    							//设置预分频：
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1 ;			//设置时钟分频系数：不分频(这里用不到)
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  	//向上计数模式
    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);              //
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;	    				//配置为PWM模式1
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;  	  //当定时器计数值小于CCR1_Val时为低电平
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;	//使能输出
    TIM_OCInitStructure.TIM_Pulse = 0;										  			//设置初始PWM脉冲宽度为0	
    TIM_OC1Init(TIM3, &TIM_OCInitStructure);	 									//使能通道1
    TIM_OC1PreloadConfig(TIM3, TIM_OCPreload_Enable);						//使能预装载	
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;	//使能输出
    TIM_OCInitStructure.TIM_Pulse = 0;										  			//设置初始PWM脉冲宽度为0	
    TIM_OC2Init(TIM3, &TIM_OCInitStructure);	 									//使能通道2
    TIM_OC2PreloadConfig(TIM3, TIM_OCPreload_Enable);						//使能预装载
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;	//使能输出
    TIM_OCInitStructure.TIM_Pulse = 256;										  			//设置初始PWM脉冲宽度为0	
    TIM_OC3Init(TIM3, &TIM_OCInitStructure);	 									//使能通道1
    TIM_OC3PreloadConfig(TIM3, TIM_OCPreload_Enable);						//使能预装载	
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;	//使能输出
    TIM_OCInitStructure.TIM_Pulse = 0;										  			//设置初始PWM脉冲宽度为0	
    TIM_OC4Init(TIM3, &TIM_OCInitStructure);	 									//使能通道2
    TIM_OC4PreloadConfig(TIM3, TIM_OCPreload_Enable);						//使能预装载
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    TIM_ARRPreloadConfig(TIM3, ENABLE);			 										//使能TIM1重载寄存器ARR
    TIM_Cmd(TIM3, ENABLE);                   										//使能定时器1	
}

void TIM2_GPIO_Config(void) 
{
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE); 
    GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;		    // 复用推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;  
    GPIO_Init(GPIOA, &GPIO_InitStructure);
}

void TIM2_Mode_Config(void)
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    TIM_OCInitTypeDef  TIM_OCInitStructure;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE); 					//使能TIM3时钟

    // 基本定时器配置 
    TIM2_GPIO_Config(); 	
    TIM_TimeBaseStructure.TIM_Period =65535;       							  //当定时器从0计数到255，即为266次，为一个定时周期
    TIM_TimeBaseStructure.TIM_Prescaler = 0;	    							//设置预分频：
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1 ;			//设置时钟分频系数：不分频(这里用不到)
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  	//向上计数模式
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);              //
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;	    				//配置为PWM模式1
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;  	  //当定时器计数值小于CCR1_Val时为低电平
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;	//使能输出
    TIM_OCInitStructure.TIM_Pulse = 0;										  			//设置初始PWM脉冲宽度为0	
    TIM_OC1Init(TIM2, &TIM_OCInitStructure);	 									//使能通道1
    TIM_OC1PreloadConfig(TIM2, TIM_OCPreload_Enable);						//使能预装载	
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;	//使能输出
    TIM_OCInitStructure.TIM_Pulse = 0;										  			//设置初始PWM脉冲宽度为0	
    TIM_OC2Init(TIM2, &TIM_OCInitStructure);	 									//使能通道2
    TIM_OC2PreloadConfig(TIM2, TIM_OCPreload_Enable);						//使能预装载
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;	//使能输出
    TIM_OCInitStructure.TIM_Pulse = 0;										  			//设置初始PWM脉冲宽度为0	
    TIM_OC3Init(TIM2, &TIM_OCInitStructure);	 									//使能通道1
    TIM_OC3PreloadConfig(TIM2, TIM_OCPreload_Enable);						//使能预装载	
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;	//使能输出
    TIM_OCInitStructure.TIM_Pulse = 0;										  			//设置初始PWM脉冲宽度为0	
    TIM_OC4Init(TIM2, &TIM_OCInitStructure);	 									//使能通道2
    TIM_OC4PreloadConfig(TIM2, TIM_OCPreload_Enable);						//使能预装载
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    TIM_ARRPreloadConfig(TIM2, ENABLE);			 										//使能TIM1重载寄存器ARR
    TIM_Cmd(TIM2, ENABLE);                   										//使能定时器1		
}
