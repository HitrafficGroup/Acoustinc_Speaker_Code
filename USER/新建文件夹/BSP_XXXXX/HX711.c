/************************************************************************************
							本例程提供自以下店铺：
								Ilovemcu.taobao.com
								epic-mcu.taobao.com
							实验相关外围扩展模块均来自以上店铺
							作者：神秘藏宝室							
*************************************************************************************/
#include "HX711.h"
#include "Systick.h"
#include "sys.h"
#include "stm32f10x.h"

void delay(u32 nCount)
{
    for(;nCount!=0;nCount--);
}

void BufferWrite(void)
{
    if(BufferWptr==(BufferRptr-1))
    {
        return;
    }
    UsartBuffer[BufferWptr]=USART_ReceiveData(USART1);
    BufferWptr++;
    BufferWptr=BufferWptr%BUFFERMAX;
}

void BufferRead(void)
{
    u8 rxdata;	
    if(BufferRptr==BufferWptr)
    {
        return ;
    }
	 
    rxdata=UsartBuffer[BufferRptr];
	 
    if((RxCounter == 0 && rxdata== 0xFF) || RxCounter > 0)
    {
        TEMPBUF[RxCounter++] = rxdata;
    }				
    if (RxCounter >= RXLENGTH)	
    {
        RxCounter = 0;
        processCommand();
    }
    BufferRptr++;
    BufferRptr=BufferRptr%BUFFERMAX;
}

void processCommand()
{
  if(TEMPBUF[0] == 0xFF && (u8)checksum_Test() == 0)
  {
    switch(TEMPBUF[1])
    {    
//     case 0x00:     // Sync Packet
//       Serial.print("sync");
//       Serial.flush();        
//       break;
     case 0x01:    // read digital port  
        TxBuffer[1]=TEMPBUF[1];//ID
        TxBuffer[2]='S';
        TxBuffer[3]='T'; 
        TxBuffer[4]='M'; 
        TxBuffer[5]='3'; 
        TxBuffer[6]='2'; 
        RetMSG();
        break;
    case 0x02:    // Set Relay  
        RelaySet(TEMPBUF[2],TEMPBUF[3]);
   		break;
    default:
        RxCounter =0;
        break;     
 		}
	}
}

void  RelaySet(int RelayIndex,int Value)
{     
    switch (RelayIndex)
    {
        case 255:
            Relayvalue[0]=0;
            Relayvalue[1]=0;
            Relayvalue[2]=0;
            Relayvalue[3]=0;
            Shift595();
            break;
        
        default:
            SetRelayValue(RelayIndex,Value);
            Shift595();
            break;     
    }
}


int checksum_Test(void)
{
    if(checksum_Compute() == TEMPBUF[RXLENGTH-1])
    {
        return 0; 
    }
    else
    {
        return 1;
    }
}

u8 checksum_Compute(void)
{
    u8 checksum = 0;
    u16 i;
    for (i=0; i<(RXLENGTH-1); i++)
    {
        checksum += TEMPBUF[i]; 
    }
    return checksum;
}

void RetMSG()
{    // Serial.println(Value);
    u8 checksum=0; 
    u16 TxCounter = 0;
    u16 i;
    TxBuffer[0]=0xFE;

    for (i=0; i<(TXLENGTH-1); i++)
    {
        checksum += TxBuffer[i]; 
    }
    TxBuffer[TXLENGTH-1]=checksum;
    for (i=0; i< TXLENGTH; i++)
    {
        //USART_SendData(USART1,0xAA);
        USART_SendData(USART1,TxBuffer[TxCounter++]);
        while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET)
        {
        }
    }
}

void HC595Config(void)
{
//  OE595_ON;
//  CLR_OFF;
	  Shift595();
//  OE595_OFF;
//  CLR_ON;
}	

void SetRelayValue(int Relayindex,int value)
{ 
	int Ind595;
	int Bit595;
	char ONConstant;
    
    Ind595 = Relayindex / 8 ;
    Bit595=Relayindex % 8;
    ONConstant=(1<<Bit595);
		 
    if (value > 0)
        Relayvalue[Ind595] = Relayvalue[Ind595] | ONConstant;
    else
        Relayvalue[Ind595] = Relayvalue[Ind595] & (~ONConstant);
}
#define timee  1000   //595传输速度调整
extern u8 A;
extern u8 B;
extern u8 C;
extern u8 D;
void Shift595()
{
	u8 i;
	u8 tmp;

	Latch_595_OFF;
    tmp = A;
    for (i=0;i<8;i++)
    {
        if (tmp & 0x80 )
        {
            SER_595_ON;
            delay(timee);
        }
        else
        {
            SER_595_OFF;
            delay(timee);
        }
        SRCLK_595_OFF;
        delay(timee);
        SRCLK_595_ON;
        delay(timee);
        tmp <<=1;
    }
    tmp = B;
    for (i=0;i<8;i++)
    {
        if (tmp & 0x80 )
        {
            SER_595_ON;
            delay(timee);
        }
        else
        {
            SER_595_OFF;
            delay(timee);
        }
        SRCLK_595_OFF;
        delay(timee);
        SRCLK_595_ON;
        delay(timee);
        tmp <<=1;
    }
//==========================
//		tmp = C;
//	    for (i=0;i<8;i++)
//		{
//			if (tmp & 0x80 )
//			{
//				SER_595_ON;
//			}
//			else
//			{
//				SER_595_OFF;
//			}
//			SRCLK_595_OFF;
//			SRCLK_595_ON;
//			tmp <<=1;
//    }
    Latch_595_ON;
}

