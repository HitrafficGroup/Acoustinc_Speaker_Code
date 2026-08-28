/********************   (C) COPYRIGHT 2013 www.armjishu.com   ********************
 * 文件名  ：SZ_STM32F103ZE_LIB.h
 * 描述    ：提供STM32F103ZE神舟III号开发板的库函数
 * 实验平台：STM32神舟开发板
 * 作者    ：www.armjishu.com 
**********************************************************************************/
/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
#include "stdio.h"
extern		int r;
extern		int g;
extern		int b;

void DIM_CH4(uint16_t dim);

#ifdef __cplusplus
 
{
  #endif
  void TIM2_Mode_Config(void);
	void TIM3_Mode_Config(void);
  void TIM4_Mode_Config(void);
  void TIM4_Mode(void);  
	void RGB(void);
  #ifdef __cplusplus
}
  #endif
