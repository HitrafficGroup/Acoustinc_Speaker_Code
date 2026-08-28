/* Controlador del watchdog independiente. */
#include "stm32f10x.h"


void IWDG_Feed(void)
{
	IWDG_ReloadCounter();
}

void bsp_InitIwdg(uint32_t _ulIWDGTime)
{

	if (RCC_GetFlagStatus(RCC_FLAG_IWDGRST) != RESET)
	{		

		RCC_ClearFlag();
	}

	IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);
	

	IWDG_SetPrescaler(IWDG_Prescaler_32);
	
	IWDG_SetReload(_ulIWDGTime);
	

	IWDG_ReloadCounter();
	

	IWDG_Enable();
}
