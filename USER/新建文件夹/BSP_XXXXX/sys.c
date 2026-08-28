#include "sys.h"
#include "stm32f10x.h"
#include "Systick.h"
/**
  * @brief  复位和时钟控制 配置
  * @param  无
  * @retval 无
  */
void RCC_Configuration(void)
{
  ErrorStatus HSEStartUpStatus;//定义外部高速晶体启动状态枚举变量
  RCC_DeInit();//复位RCC外部设备寄存器到默认值
  RCC_HSEConfig(RCC_HSE_ON);//打开外部高速晶振
  HSEStartUpStatus = RCC_WaitForHSEStartUp();//等待外部高速时钟准备好
  if(HSEStartUpStatus == SUCCESS)//外部高速时钟已经准别好
  {
    FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);//开启FLASH预读缓冲功能，加速FLASH的读取。所有程序中必须的用法.位置：RCC初始化子函数里面，时钟起振之后
    FLASH_SetLatency(FLASH_Latency_2);//flash操作的延时
      	
    RCC_HCLKConfig(RCC_SYSCLK_Div1);//配置AHB(HCLK)时钟等于==SYSCLK
    RCC_PCLK2Config(RCC_HCLK_Div1);//配置APB2(PCLK2)钟==AHB时钟
    RCC_PCLK1Config(RCC_HCLK_Div2);//配置APB1(PCLK1)钟==AHB1/2时钟
         
    RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_9);//配置PLL时钟 == 外部高速晶体时钟 * 9 = 72MHz
    RCC_PLLCmd(ENABLE);//使能PLL时钟
   
    while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET);//等待PLL时钟就绪
    
    RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);//配置系统时钟 = PLL时钟
    while(RCC_GetSYSCLKSource() != 0x08);//检查PLL时钟是否作为系统时钟    
  }
}

void GPIO_Config(void) 
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOC|RCC_APB2Periph_GPIOD, ENABLE);	//使能PA,PB,PC端口时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
    GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable , ENABLE); //GPIO_Remap_SWJ_JTAGDisable, JTAG-DP ?? + SW-DP ??
  
	
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	    //IO口速度为50MHz	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;			    //设置PC10~PC12端口推挽输出
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP; 	    //上拉输入
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_ResetBits(GPIOA,GPIO_Pin_11);
	
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	    //IO口速度为50MHz	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3|GPIO_Pin_5;	//设置PC10~PC12端口推挽输出
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP; 	    //上拉输入
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	GPIO_ResetBits(GPIOB,GPIO_Pin_3);
	GPIO_ResetBits(GPIOB,GPIO_Pin_5);

	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	    //IO口速度为50MHz	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11|GPIO_Pin_12;  //设置PC10~PC12端口推挽输出
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;       //上拉输入
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	GPIO_SetBits(GPIOC,GPIO_Pin_11);
	GPIO_SetBits(GPIOC,GPIO_Pin_12);

	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;   //IO口速度为50MHz	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;		    //设置PC10~PC12端口推挽输出
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;   //上拉输入
	GPIO_Init(GPIOD, &GPIO_InitStructure);
	GPIO_SetBits(GPIOD,GPIO_Pin_2);
}

void Timer2_init()	//T4 10ms时钟
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

    TIM_TimeBaseStructure.TIM_Period = 10000; //计数个数     //100us*10=1000us=10ms
    TIM_TimeBaseStructure.TIM_Prescaler = 6000;//分频值   	    
    TIM_TimeBaseStructure.TIM_ClockDivision = 0x0; 	//分割时钟			
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; //向上计数
    TIM_TimeBaseInit(TIM2, & TIM_TimeBaseStructure); 
    TIM_Cmd(TIM2, ENABLE); 	 //使能定时器2

     /*以下定时器4中断初始化*/
    TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE); //向上计数溢出产生中断
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
