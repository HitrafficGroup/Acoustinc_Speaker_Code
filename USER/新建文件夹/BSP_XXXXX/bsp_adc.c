#include "stm32f10x.h"


#define SAMP_COUNT	20		/* 样本个数，表示200ms内的采样数据求平均值 */
uint16_t g_usAdcValue[2];	/* ADC 采样值的平均值 */


void ADC_GPIO_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	/* Enable ADC1 and GPIOC clock */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	
	/* Configure PB.0.1  as analog input */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
}

/*
*********************************************************************************************************
*	函 数 名: ADC_Configuration
*	功能说明: 配置ADC, PC4作为ADC通道输入
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
void ADC1_Configuration(void)
{
	ADC_InitTypeDef ADC_InitStructure;

    /* 使能 ADC1 and GPIOC clock */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);

	/* 配置ADC1, 不用DMA, 用软件触发 */
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
	ADC_InitStructure.ADC_ScanConvMode = ENABLE;
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
	ADC_InitStructure.ADC_NbrOfChannel = 1;
	ADC_Init(ADC1, &ADC_InitStructure);

	/* 配置ADC1 规则通道14 channel14 configuration */
	ADC_RegularChannelConfig(ADC1, ADC_Channel_8, 1, ADC_SampleTime_55Cycles5);

	/* 使能ADC1 DMA功能 */
	ADC_DMACmd(ADC1, ENABLE);

	/* 使能 ADC1 */
	ADC_Cmd(ADC1, ENABLE);

	/* 使能ADC1 复位校准寄存器 */
	ADC_ResetCalibration(ADC1);
	/* 检查ADC1的复位寄存器 */
	while(ADC_GetResetCalibrationStatus(ADC1));

	/* 启动ADC1校准 */
	ADC_StartCalibration(ADC1);
	/* 检查校准是否结束 */
	while(ADC_GetCalibrationStatus(ADC1));

	/* 软件启动ADC转换 */
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);
}

/*
*********************************************************************************************************
*	函 数 名: AdcPro
*	功能说明: ADC采样处理，插入1ms systick 中断进行调用
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
void Adc1Pro(void)
{
	static uint16_t buf1[SAMP_COUNT];
	static uint8_t write;
	uint32_t sum;
	uint8_t i;

	buf1[write] = ADC_GetConversionValue(ADC1);
	if (++write >= SAMP_COUNT)
	{
		write = 0;
	}

	/* 下面这段代码采用求平均值的方法进行滤波
		也可以改善下，选择去掉最大和最下2个值，使数据更加精确
	*/
	sum = 0;
	for (i = 0; i < SAMP_COUNT; i++)
	{
		sum += buf1[i];
	}
	g_usAdcValue[0] = sum / SAMP_COUNT;	/* ADC采样值由若干次采样值平均 */

	ADC_SoftwareStartConvCmd(ADC1, ENABLE);	/* 软件启动下次ADC转换 */
}

void Adc2Pro(void)
{
	static uint16_t buf2[SAMP_COUNT];
	static uint8_t write;
	uint32_t sum;
	uint8_t i;

	buf2[write] = ADC_GetConversionValue(ADC2);
	if (++write >= SAMP_COUNT)
	{
		write = 0;
	}

	/* 下面这段代码采用求平均值的方法进行滤波
		也可以改善下，选择去掉最大和最下2个值，使数据更加精确
	*/
	sum = 0;
	for (i = 0; i < SAMP_COUNT; i++)
	{
		sum += buf2[i];
	}
	g_usAdcValue[1] = sum / SAMP_COUNT;	/* ADC采样值由若干次采样值平均 */

	ADC_SoftwareStartConvCmd(ADC2, ENABLE);	/* 软件启动下次ADC转换 */
}

uint16_t GetADC(uint8_t n)
{
	uint16_t ret;

	/* 因为	g_AdcValue 变量在systick中断中改写，为了避免主程序读变量时被中断程序打乱导致数据错误，因此需要
	关闭中断进行保护 */

	/* 进行临界区保护，关闭中断 */
	__set_PRIMASK(1);  /* 关中断 */

	ret = g_usAdcValue[n];

	__set_PRIMASK(0);  /* 开中断 */

	return ret;
}









#define ADC1_DR_Address    ((u32)0x40012400+0x4c)
short int ADC_ConvertedValue[4];

/**
  * @brief  使能ADC1和DMA1的时钟，初始化PC.01
  * @param  无
  * @retval 无
  */
static void ADC1_GPIO_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	/* Enable DMA clock */
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
	
	/* Enable ADC1 and GPIOC clock */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1 | RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB, ENABLE);
	
	/* Configure PC.01  as analog input */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
    
    /* Configure PB.0.1  as analog input */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
}

/**
  * @brief  配置ADC1的工作模式为MDA模式
  * @param  无
  * @retval 无
  */
static void ADC1_Mode_Config(void)
{
	DMA_InitTypeDef DMA_InitStructure;
	ADC_InitTypeDef ADC_InitStructure;
	
	/* DMA channel1 configuration */
	DMA_DeInit(DMA1_Channel1);
	
	DMA_InitStructure.DMA_PeripheralBaseAddr = ADC1_DR_Address;	 			//ADC地址
	DMA_InitStructure.DMA_MemoryBaseAddr = (u32)&ADC_ConvertedValue;	//内存地址
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
	DMA_InitStructure.DMA_BufferSize = 4;
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;	//外设地址固定
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;  				//内存地址固定
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;	//半字
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;										//循环传输
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
	DMA_Init(DMA1_Channel1, &DMA_InitStructure);
	
	/* Enable DMA channel1 */
	DMA_Cmd(DMA1_Channel1, ENABLE);
	
	/* ADC1 configuration */	
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;			//独立ADC模式
	ADC_InitStructure.ADC_ScanConvMode = ENABLE ; 	 			//禁止扫描模式，扫描模式用于多通道采集
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;			//开启连续转换模式，即不停地进行ADC转换
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;	//不使用外部触发转换
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right; 	    //采集数据右对齐
	ADC_InitStructure.ADC_NbrOfChannel = 4;	 					//要转换的通道数目1
	ADC_Init(ADC1, &ADC_InitStructure);
	
	/*配置ADC时钟，为PCLK2的8分频，即9MHz*/
	RCC_ADCCLKConfig(RCC_PCLK2_Div8); 
	/*配置ADC1的通道11为55.	5个采样周期，序列为1 */ 
	ADC_RegularChannelConfig(ADC1, ADC_Channel_0, 1, ADC_SampleTime_7Cycles5); //0电压 ADC_SampleTime_55Cycles5
	ADC_RegularChannelConfig(ADC1, ADC_Channel_1, 2, ADC_SampleTime_7Cycles5); //1光强 
	ADC_RegularChannelConfig(ADC1, ADC_Channel_8, 3, ADC_SampleTime_7Cycles5); //2 MI 
	ADC_RegularChannelConfig(ADC1, ADC_Channel_9, 4, ADC_SampleTime_7Cycles5); //3 XG 

	/* Enable ADC1 DMA */
	ADC_DMACmd(ADC1, ENABLE);
	
	/* Enable ADC1 */
	ADC_Cmd(ADC1, ENABLE);
	
	/*复位校准寄存器 */   
	ADC_ResetCalibration(ADC1);
	/*等待校准寄存器复位完成 */
	while(ADC_GetResetCalibrationStatus(ADC1));
	
	/* ADC校准 */
	ADC_StartCalibration(ADC1);
	/* 等待校准完成*/
	while(ADC_GetCalibrationStatus(ADC1));
	
	/* 由于没有采用外部触发，所以使用软件触发ADC转换 */ 
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);
}

/**
  * @brief  ADC1初始化
  * @param  无
  * @retval 无
  */
void ADC1_Init(void)
{
	ADC1_GPIO_Config();
	ADC1_Mode_Config();
}
/*********************************************END OF FILE**********************/
