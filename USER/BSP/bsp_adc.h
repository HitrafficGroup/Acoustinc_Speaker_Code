#ifndef __ADC_H
#define	__ADC_H

#include "stm32f10x.h"

void ADC1_Init(void);



extern short int ADC_ConvertedValue[4];
extern uint16_t adc_max;

void ADC_Inits(void);


void ADC_GPIO_Config(void);
void ADC1_Configuration(void);
void Adc1Pro(void);
void Adc2Pro(void);
uint16_t GetADC(uint8_t n);

#endif /* __ADC_H */

