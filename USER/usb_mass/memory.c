

#include "memory.h"
#include "usb_scsi.h"
#include "usb_bot.h"
#include "usb_regs.h"
#include "usb_mem.h"
#include "usb_conf.h"
#include "usb_hw.h"
#include "mass_mal.h"
#include "usb_lib.h"

#define TXFR_IDLE     0
#define TXFR_ONGOING  1

__IO uint32_t Block_Read_count = 0;
__IO uint32_t Block_offset;
__IO uint32_t Counter = 0;

uint32_t  Idx;
uint8_t Data_Buffer[BULK_MAX_PACKET_SIZE * 64];          /* 64 * 8 = 512 bytes*/
static uint8_t TransferState = TXFR_IDLE;

extern uint8_t Bulk_Data_Buff[BULK_MAX_PACKET_SIZE];    /* data buffer*/
extern uint16_t Data_Len;
extern uint8_t Bot_State;
extern Bulk_Only_CSW CSW;

extern uint32_t Mass_Block_Size;


void Read_Memory(uint8_t lun, uint32_t Memory_Offset, uint32_t Transfer_Length)
{
	static uint32_t Offset, Length;
	
	if (TransferState == TXFR_IDLE )
	{
		Offset = Memory_Offset * Mass_Block_Size;
		Length = Transfer_Length * Mass_Block_Size;
		TransferState = TXFR_ONGOING;
	}

	if (TransferState == TXFR_ONGOING )
	{
		if (!Block_Read_count)
		{
			MAL_Read(lun, Offset, Data_Buffer, Mass_Block_Size);
			
			USB_SIL_Write(EP1_IN, (uint8_t *)Data_Buffer, BULK_MAX_PACKET_SIZE);
			
			Block_Read_count = Mass_Block_Size - BULK_MAX_PACKET_SIZE;
			Block_offset = BULK_MAX_PACKET_SIZE;
		}
		else
		{
			USB_SIL_Write(EP1_IN, (uint8_t *)Data_Buffer + Block_offset, BULK_MAX_PACKET_SIZE);
			
			Block_Read_count -= BULK_MAX_PACKET_SIZE;
			Block_offset += BULK_MAX_PACKET_SIZE;
		}

	    SetEPTxCount(ENDP1, BULK_MAX_PACKET_SIZE);
	    SetEPTxStatus(ENDP1, EP_TX_VALID);

	    Offset += BULK_MAX_PACKET_SIZE;
	    Length -= BULK_MAX_PACKET_SIZE;
	
	    CSW.dDataResidue -= BULK_MAX_PACKET_SIZE;
	    Led_RW_ON();
	}
  
	if (Length == 0)
	{
		Block_Read_count = 0;
		Block_offset = 0;
		Offset = 0;
		Bot_State = BOT_DATA_IN_LAST;
		TransferState = TXFR_IDLE;
		Led_RW_OFF();
	}
}


void Write_Memory (uint8_t lun, uint32_t Memory_Offset, uint32_t Transfer_Length)
{
	static uint32_t W_Offset, W_Length;
	
	uint32_t temp =  Counter + 64;
	
	if (TransferState == TXFR_IDLE )
	{
		W_Offset = Memory_Offset * Mass_Block_Size;
		W_Length = Transfer_Length * Mass_Block_Size;
		TransferState = TXFR_ONGOING;
	}
	
	if(TransferState == TXFR_ONGOING)
	{
		for (Idx = 0 ; Counter < temp; Counter++)
		{
			*((uint8_t *)Data_Buffer + Counter) = Bulk_Data_Buff[Idx++];
		}
		
		W_Offset += Data_Len;
		W_Length -= Data_Len;
		
		if (!(W_Length % Mass_Block_Size))
		{
			Counter = 0;
			MAL_Write(lun, W_Offset - Mass_Block_Size, Data_Buffer, Mass_Block_Size);
		}
		
		CSW.dDataResidue -= Data_Len;
		SetEPRxStatus(ENDP2, EP_RX_VALID);
		
		Led_RW_ON();
	}
	
	if ((W_Length == 0) || (Bot_State == BOT_CSW_Send))
	{
		Counter = 0;
		Set_CSW (CSW_CMD_PASSED, SEND_CSW_ENABLE);
		TransferState = TXFR_IDLE;
		
		Led_RW_OFF();
	}
}

