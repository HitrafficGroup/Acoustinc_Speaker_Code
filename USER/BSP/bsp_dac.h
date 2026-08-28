/*
*********************************************************************************************************
*
*	模块名称 : DAC驱动模块
*	文件名称 : bsp_dac.c
*	版    本 : V1.0
*	说    明 : 实现DAC输出噪声
*
*********************************************************************************************************
*/

#ifndef __BSP_DAC_H
#define __BSP_DAC_H


void DAC1_SetData(u16 data);
void DAC2_SetData(u16 data);
void Timer4_Init(u16 arr,u16 psc);


void bsp_InitDAC(void);
void DAC_GPIOConfig(void); 
void TIM6_Config(void);

void DAC_Ch1_Config(void);
void DAC_Ch1_SineWaveConfig(void);
void DAC_Ch1_AmplitudeConfig(uint32_t _DAC_Amplitude);

#endif
