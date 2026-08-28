#include "Systick.h"
#include "hw.h"

/***************************************************************
** 功能：     红外发射端口初始化
** 参数：	  无参数
** 返回值：    无
****************************************************************/
void HW_Init()
{
 	GPIO_InitTypeDef  GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);	//使能PA,PB,PC端口时钟

	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 													//IO口速度为50MHz	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14;						//设置PC10~PC12端口推挽输出
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP; 		 												//上拉输入
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	GPIO_ResetBits(GPIOC,GPIO_Pin_14);

}

/***************************************************************
** 功能：     红外发射子程序
** 参数：	  *s：指向要发送的数据
**             n：数据长度
** 返回值：    无
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
		     if(temp==0)//发射0
		        {
		           RI_TXD=1;
		           Delay_us(85);//延时0.56ms
		           RI_TXD=0;
		           Delay_us(85);//延时0.56ms
		         }
		     if(temp==1)//发射1
		         {
		           RI_TXD=1;
		           Delay_us(85);//延时0.56ms
		           RI_TXD=0;
				       Delay_us(151);
		           Delay_us(85);//延时1.69ms
		
		         }
		  }
	}
    RI_TXD=1;//结束
    Delay_us(85);//延时0.56ms
    RI_TXD=0;//关闭红外发射
}
