 *          | PB0 - key1              |
 *          |                         |
 *           -------------------------
 *
**********************************************************************************/
#include "Systick.h"
#include "sys.h"
#include "key.h" 

 /*
 */
 void Delay(__IO u32 nCount)
{
  for(; nCount != 0; nCount--);
} 

 /*
 */

 /*
 */
u8 Key_Scan(GPIO_TypeDef* GPIOx,u16 GPIO_Pin)
{			
   	if(GPIO_ReadInputDataBit(GPIOx,GPIO_Pin) == 0 ) 
	  {	   
	  	Delay(20);	
	   		if(GPIO_ReadInputDataBit(GPIOx,GPIO_Pin) == 0 )  
					{	 
						  	  	Delay(20);	
	   		     if(GPIO_ReadInputDataBit(GPIOx,GPIO_Pin) == 0 )  
					    {	 
						     while(GPIO_ReadInputDataBit(GPIOx,GPIO_Pin) == 1);
			 			    return 	0;	 
					    }
			      else				       
			 			  return 	1;	 
					}
			else
					return 1;
		}
	else
		return 1;
}
