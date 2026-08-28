/********************   (C) COPYRIGHT 2013 www.armjishu.com   ********************
**********************************************************************************/
/* Inclusiones ------------------------------------------------------------------*/
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
