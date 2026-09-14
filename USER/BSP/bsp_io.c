#include "stm32f10x.h"	/* Controlador de entradas, salidas y estados de las luces. */

uint8_t spi2_busy_flag = 0; //Buscar en que file esta para moverla ya que esta extern
uint8_t rf_int_flag = 0; //Buscar en que file esta para moverla ya que esta extern
SYSTEM_TEMP_TypeDef system_temp; //Buscar en que file esta para moverla ya que no se usa aqui


void bsp_GpioInit(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOC|RCC_APB2Periph_GPIOD|RCC_APB2Periph_AFIO, ENABLE);
    
    GPIO_PinRemapConfig(GPIO_Remap_SWJ_Disable, ENABLE);
    // GPIO_Remap_SWJ_Disable SWJ JTAG+SW-DP)
    GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable , ENABLE);
    // GPIO_Remap_SWJ_JTAGDisable ,JTAG-DP + SW-DP
    
    //**********************************************************************************************************************/
    //PA0_GPS_ON/OFF	PA15_LED	PA11_PA
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_11 | GPIO_Pin_15;//
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPS_ON(); //GPS_OFF(); //Cuando inicia enciende el GPS
    LED_ON();
    PA_ON(); //Amplificador de potencia
	
    //PA1_GPS_1PPS		//No usado SW5-SW8 PA0-PA3 锟斤拷为GPS锟斤拷锟斤拷 PA0_ON/OFF
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IN_FLOATING;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
    GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_1;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	//PA12_AD_CON	 //锟剿匡拷锟斤拷锟斤拷锟斤拷锟�
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
    ADC_OFF();
	
	//PC0_SW1 - PC1_SW2 - PC2_SW3 - PC3_SW4,	PC13_RIN, PC14_GIN, PC15_AIN
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

    //PC10_DR1 PC11_DR2 PC12_DR3 MOS锟斤拷锟斤拷锟斤拷       //锟剿匡拷锟斤拷锟斤拷锟斤拷锟�
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	DR1_OFF(); DR2_OFF(); DR3_OFF();

	// PB10 PB11 PB12 PB13 PB14 PB15 para poder usar el conector cuadrado como In y Out

	//PB8_IN删锟斤拷    PB4_XIN删锟斤拷 // No se usan
    // GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IN_FLOATING;
    // GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
    // GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_4 | GPIO_Pin_8;
    // GPIO_Init(GPIOB, &GPIO_InitStructure);

	//PB10_RF_CE 使锟杰匡拷锟斤拷  PB11_RF_CSN 片选		//No usado PB9_RELAY 删锟斤拷  GPIO_Pin_9 | 
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
  RELAY_OFF();

	//PB12_IRQ 锟叫讹拷 // Pin de Radio frecuencia
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	//PB11_Out1, PB15_Out2, PB12_Out3	// 3 salidas
	// GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
	// GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
	// GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11 | GPIO_Pin_15 | GPIO_Pin_12;
	// GPIO_Init(GPIOB, &GPIO_InitStructure);

	//PB10_In1, PB13_In2, PB14_In3	// 3 entradas
    // GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IN_FLOATING;
    // GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
    // GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_10 | GPIO_Pin_15 | GPIO_Pin_14;
    // GPIO_Init(GPIOB, &GPIO_InitStructure);
}


void pps_irq_init(void)
{
    EXTI_InitTypeDef EXTI_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
    
  	/* PC9 锟叫讹拷锟斤拷锟斤拷锟斤拷 IRQ0 */
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource1);
	/* PC9 锟叫断筹拷始锟斤拷锟斤拷锟斤拷 */
  	EXTI_InitStructure.EXTI_Line = EXTI_Line1;
  	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;	
  	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
  	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    /*锟斤拷锟斤拷EXTI_InitStruct锟斤拷指锟斤拷锟侥诧拷锟斤拷锟斤拷始锟斤拷锟斤拷锟斤拷EXTI锟侥达拷*/
  	EXTI_Init(&EXTI_InitStructure);
	
    /*使锟杰帮拷锟斤拷锟斤拷锟节碉拷锟解部锟叫讹拷通锟斤拷*/
  	NVIC_InitStructure.NVIC_IRQChannel = EXTI1_IRQn;
    /*锟斤拷锟斤拷锟斤拷占锟斤拷锟饺硷拷锟斤拷锟斤拷占锟斤拷锟饺硷拷锟斤拷为2*/	
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00;	//
    /*锟斤拷锟斤拷锟斤拷锟斤拷锟饺硷拷锟斤拷锟斤拷锟斤拷锟饺硷拷锟斤拷为2*/
  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x02;		
    /*使锟斤拷锟解部锟叫讹拷通*/
  	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;	
    /*锟斤拷锟斤拷NVIC_InitStruct锟斤拷指锟斤拷锟侥诧拷锟斤拷锟斤拷始锟斤拷锟斤拷锟斤拷NVIC锟侥达拷锟斤拷*/	
  	NVIC_Init(&NVIC_InitStructure);
}


void EXTI1_IRQHandler(void)
{
	if(EXTI_GetITStatus(EXTI_Line1) != RESET)//PPS_irq PA1
	{
        EXTI_ClearFlag(EXTI_Line1);
		system_temp.pps_count = 0;
		system_temp.pps_1ms = 0;
		system_temp.pps_flag = 1;
		printf("PPS_IRQ\n");
	}
}
