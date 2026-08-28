#ifndef __BSP_PWM_H
#define __BSP_PWM_H

#include <stdint.h>



void TIM4_Mode_Config(void);

void DIM_CH1_SHUZI(uint16_t dim);
void DIM_CH3_xiangao(uint16_t dim);
void DIM_CH4_MI(uint16_t dim);

void diming(uint16_t dim);

#endif
