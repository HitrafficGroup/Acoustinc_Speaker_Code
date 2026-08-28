

#include "stm32f10x.h"
#include "usb_hw.h"
#include "usb_lib.h"
#include "usb_desc.h"
#include "usb_pwr.h"
#include "usb_lib.h"
#include "mass_mal.h"

static void IntToUnicode (uint32_t value , uint8_t *pbuf , uint8_t len);
static void USB_Disconnect_Config(void);
static void Set_USBClock(void);
static void USB_Interrupts_Config(void);


void MAL_Config(void)
{
	MAL_Init(0);
}


void InitUsbDev(void)
{	
	USB_Disconnect_Config();

	MAL_Config();			/* MAL configuration */

	Set_USBClock();
	USB_Interrupts_Config();
	USB_Init();
}


void Led_RW_ON(void)
{
	LED_ON();
}


void Led_RW_OFF(void)
{
	LED_OFF();
}


void USB_Configured_LED(void)
{
	LED_ON();
}


void USB_NotConfigured_LED(void)
{
	LED_OFF();
}


void Get_SerialNum(void)
{
	uint32_t Device_Serial0, Device_Serial1, Device_Serial2;
	
	Device_Serial0 = *(__IO uint32_t*)(0x1FFFF7E8);
	Device_Serial1 = *(__IO uint32_t*)(0x1FFFF7EC);
	Device_Serial2 = *(__IO uint32_t*)(0x1FFFF7F0);
	
	Device_Serial0 += Device_Serial2;
	
	if (Device_Serial0 != 0)
	{
		IntToUnicode (Device_Serial0, &MASS_StringSerial[2] , 8);
		IntToUnicode (Device_Serial1, &MASS_StringSerial[18], 4);
	}
}


void USB_Cable_Config (uint8_t NewState)
{

	if (NewState != DISABLE)
	{
		GPIO_SetBits(GPIOB, GPIO_Pin_3);
	}
	else
	{
		GPIO_ResetBits(GPIOB, GPIO_Pin_3);
	}
}


void Enter_LowPowerMode(void)
{

	bDeviceState = SUSPENDED;
}


void Leave_LowPowerMode(void)
{	
	DEVICE_INFO *pInfo = &Device_Info;
	

	if (pInfo->Current_Configuration != 0)
	{

		bDeviceState = CONFIGURED;
	}
	else
	{
		bDeviceState = ATTACHED;
	}
}


static void USB_Disconnect_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
		

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

	USB_Cable_Config(DISABLE);
		

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure); 
}


static void Set_USBClock(void)
{

	RCC_USBCLKConfig(RCC_USBCLKSource_PLLCLK_1Div5);
	

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USB, ENABLE);
}


static void USB_Interrupts_Config(void) 
{
	NVIC_InitTypeDef NVIC_InitStructure;
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
	
	NVIC_InitStructure.NVIC_IRQChannel = USB_LP_CAN1_RX0_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	NVIC_InitStructure.NVIC_IRQChannel = USB_HP_CAN1_TX_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}


static void IntToUnicode (uint32_t value , uint8_t *pbuf , uint8_t len)
{
	uint8_t idx = 0;
	
	for( idx = 0 ; idx < len ; idx ++)
	{
		if( ((value >> 28)) < 0xA )
		{
			pbuf[ 2* idx] = (value >> 28) + '0';
		}
		else
		{
			pbuf[2* idx] = (value >> 28) + 'A' - 10; 
		}
		
		value = value << 4;
		
		pbuf[ 2* idx + 1] = 0;
	}
}

