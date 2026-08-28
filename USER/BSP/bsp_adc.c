/* Controlador de conversion analogica. */
#include "stm32f10x.h"
//#include "bsp_adc.h"

#define SAMP_COUNT	20
uint16_t g_usAdcValue[2];
uint16_t adc_max = 0;

void ADC_GPIO_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	/* Enable ADC1 and GPIOC clock */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	
	/* Configure PB.1  as analog input */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
}

/* Configura o consulta la conversion del ADC. */
void ADC_Configuration(void)
{
	ADC_InitTypeDef ADC_InitStructure;

    /* Configura o consulta la conversion del ADC. */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);

	/* Configura o consulta la conversion del ADC. */
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
	ADC_InitStructure.ADC_ScanConvMode = ENABLE;
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
	ADC_InitStructure.ADC_NbrOfChannel = 1;
	ADC_Init(ADC1, &ADC_InitStructure);

	/* Configura o consulta la conversion del ADC. */
	ADC_RegularChannelConfig(ADC1, ADC_Channel_9, 1, ADC_SampleTime_55Cycles5);


	ADC_DMACmd(ADC1, ENABLE);

	/* Configura o consulta la conversion del ADC. */
	ADC_Cmd(ADC1, ENABLE);

	/* Configura o consulta la conversion del ADC. */
	ADC_ResetCalibration(ADC1);

	while(ADC_GetResetCalibrationStatus(ADC1));

	/* Configura o consulta la conversion del ADC. */
	ADC_StartCalibration(ADC1);

	while(ADC_GetCalibrationStatus(ADC1));

	/* Configura o consulta la conversion del ADC. */
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);
}

void ADC_Inits(void)
{
	ADC_GPIO_Config();
	ADC_Configuration();
}




void Adc1Pro(void)
{
	uint16_t temp = ADC_GetConversionValue(ADC1);
    if(temp > adc_max) adc_max = temp;
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);	/* Configura o consulta la conversion del ADC. */
}












uint16_t GetADC(uint8_t n)
{
	uint16_t ret;




	__set_PRIMASK(1);

	ret = g_usAdcValue[n];

	__set_PRIMASK(0);

	return ret;
}



#define ADC1_DR_Address    ((u32)0x40012400+0x4c)
short int ADC_ConvertedValue[4];


static void ADC1_GPIO_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	/* Enable DMA clock */
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
	
	/* Enable ADC1 and GPIOC clock */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1 | RCC_APB2Periph_GPIOB, ENABLE);
    
    /* Configure PB.1  as analog input */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
}

/* Configura o consulta la conversion del ADC. */
static void ADC1_Mode_Config(void)
{
	DMA_InitTypeDef DMA_InitStructure;
	ADC_InitTypeDef ADC_InitStructure;
	
	/* DMA channel1 configuration */
	DMA_DeInit(DMA1_Channel1);
	
	DMA_InitStructure.DMA_PeripheralBaseAddr = ADC1_DR_Address;
	DMA_InitStructure.DMA_MemoryBaseAddr = (u32)&ADC_ConvertedValue;
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
	DMA_InitStructure.DMA_BufferSize = 4;
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
	DMA_Init(DMA1_Channel1, &DMA_InitStructure);
	
	/* Enable DMA channel1 */
	DMA_Cmd(DMA1_Channel1, ENABLE);
	
	/* ADC1 configuration */	
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;			// Configura o consulta la conversion del ADC.
	ADC_InitStructure.ADC_ScanConvMode = ENABLE ; 	 			// Configura o consulta la conversion del ADC.
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;			// Configura o consulta la conversion del ADC.
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;	// Configura o consulta la conversion del ADC.
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right; 	    // Configura o consulta la conversion del ADC.
	ADC_InitStructure.ADC_NbrOfChannel = 4;	 					// Configura o consulta la conversion del ADC.
	ADC_Init(ADC1, &ADC_InitStructure);
	
	/* Configura o consulta la conversion del ADC. */
	RCC_ADCCLKConfig(RCC_PCLK2_Div8); 
	/* Configura o consulta la conversion del ADC. */
	ADC_RegularChannelConfig(ADC1, ADC_Channel_0, 1, ADC_SampleTime_7Cycles5); // Configura o consulta la conversion del ADC.
	ADC_RegularChannelConfig(ADC1, ADC_Channel_1, 2, ADC_SampleTime_7Cycles5); // Configura o consulta la conversion del ADC.
	ADC_RegularChannelConfig(ADC1, ADC_Channel_8, 3, ADC_SampleTime_7Cycles5); //2 MI 
	ADC_RegularChannelConfig(ADC1, ADC_Channel_9, 4, ADC_SampleTime_7Cycles5); //3 XG 

	/* Enable ADC1 DMA */
	ADC_DMACmd(ADC1, ENABLE);
	
	/* Enable ADC1 */
	ADC_Cmd(ADC1, ENABLE);
	
	/* Configura o consulta la conversion del ADC. */
	ADC_ResetCalibration(ADC1);

	while(ADC_GetResetCalibrationStatus(ADC1));
	
	/* Configura o consulta la conversion del ADC. */
	ADC_StartCalibration(ADC1);

	while(ADC_GetCalibrationStatus(ADC1));
	
	/* Configura o consulta la conversion del ADC. */
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);
}

/* Configura o consulta la conversion del ADC. */
void ADC1_Init(void)
{
	ADC1_GPIO_Config();
	ADC1_Mode_Config();
}
/*********************************************END OF FILE**********************/
