#include "Systick.h"
#include "hw.h"

/***************************************************************
****************************************************************/
void HW_Init()
{
 	GPIO_InitTypeDef  GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);	

	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 													
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14;						
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP; 		 												
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	GPIO_ResetBits(GPIOC,GPIO_Pin_14);

}

/***************************************************************
****************************************************************/
void Transmition(u8 *s,int n)
{
    u8 i,j,temp;

    RI_TXD=1;
    Delay_us(1363);
    RI_TXD=0;
    Delay_us(606);
	  Delay_us(85);

	for(i=0;i<n;i++)
	{
		for(j=0;j<8;j++)
		  {
		     temp=(s[i]>>j)&0x01;
		     if(temp==0)
		        {
		           RI_TXD=1;
		           Delay_us(85);
		           RI_TXD=0;
		           Delay_us(85);
		         }
		     if(temp==1)
		         {
		           RI_TXD=1;
		           Delay_us(85);
		           RI_TXD=0;
				       Delay_us(151);
		           Delay_us(85);
		
		         }
		  }
	}
    RI_TXD=1;
    Delay_us(85);
    RI_TXD=0;
}
