/* Controlador de entradas y salidas del sistema. */
#include "stm32f10x.h"

uint8_t spi2_busy_flag = 0;
uint8_t rf_int_flag = 0;
SYSTEM_TEMP_TypeDef system_temp;

#define SW1_3()       (GPIOC->IDR & 0x000f) //Read Switch

#define RIN()	((GPIOC->IDR & 0x2000)? 0:1)	//#define RIN()	(GPIOC->IDR & 0x2000)
#define GIN()   ((GPIOC->IDR & 0x4000)? 0:1)	//#define GIN()   (GPIOC->IDR & 0x4000)
#define AIN()   ((GPIOC->IDR & 0x8000)? 0:1)	

#define GET_MUTE_STATE()   (GPIOC->IDR & 0x8000)

#define GET_LAMP_STATE()   ((GPIOC->IDR>>13) & 0x0003)


void bsp_GpioInit(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOC|RCC_APB2Periph_GPIOD|RCC_APB2Periph_AFIO, ENABLE);
    
    GPIO_PinRemapConfig(GPIO_Remap_SWJ_Disable, ENABLE);
    // GPIO_Remap_SWJ_Disable SWJ JTAG+SW-DP)
    GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable , ENABLE);
    // GPIO_Remap_SWJ_JTAGDisable ,JTAG-DP + SW-DP 
    
    /****************************************************/
    //SW1-SW4,   RIN,GIN,AIN
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
    
	//PA0_GPS_ON/OFF	PA15_LED	PA11_PA
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;       
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_11 | GPIO_Pin_15;//
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPS_ON(); //Cuando inicia enciende el GPS
	//GPS_OFF();
    LED_ON();
    PA_ON();
    
	
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;       
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
    ADC_OFF();
    
	
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;       
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	DR1_OFF(); DR2_OFF(); DR3_OFF();
    /****************************************************/
	
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;       
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
    RELAY_OFF();
    
//    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IN_FLOATING;
//    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
//    GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_4 | GPIO_Pin_8;
//    GPIO_Init(GPIOB, &GPIO_InitStructure);
    
    /****************************************************/
    
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IN_FLOATING;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
    GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_1;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    /****************************************************/
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;     
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
}

uint16_t get_dir(void) //switch function
{
    uint16_t temp, count, sw, dir;

    for(count=0; count<5; )
    {
        temp = SW1_3();
        if(sw == temp)
        {
            count++;
        }
        else 
        {
            sw = temp;
            count = 0;
        }
    }
    dir = 0;
    temp = 0x0001;
    for(count = 0; count < 3; count++ )
    {
        if((sw & temp)==0)
        {
            dir = count;
            break;
        }
        temp <<= 1;
    }
    return dir;
}

IO_TypeDef xin;
IO_TypeDef ain;

uint8_t last8second_flag = 0;
uint8_t gre_flash_flag = 0;
uint8_t have_gre_flash_flag = 0;

uint8_t lamp_on_flag = 0;
uint8_t lamp_off_flag = 0;

uint8_t lamp_status = BS;
uint8_t lamp_state[2] = {0};
uint8_t lamp_stab_state = 0x03;
uint8_t lamp_temp_state = 0x03;
uint8_t study_lamp_stab_state = 0x03;

uint16_t lamp_chge_counter[2] = {0};
uint16_t lamp_chge_flag = 0;
uint16_t lamp_off_count = 0;
uint8_t lamp_state_chg_count = 0;

uint8_t gre_start = 0;
uint8_t gre_off = 0;

uint8_t study_time_flag;
uint8_t study_time_valid_flag;
uint8_t lamp_counter_valid_flag;

uint8_t study_chge_counter[2];
uint16_t current_study_counter[2];
uint16_t previous_study_counter[2];

uint8_t displaying_second_counter;
uint8_t red_displaying_flag;
uint8_t gre_displaying_flag;
uint16_t display_data[2];
uint16_t display_data_backup = 0;

void down_time_display(void)//10ms yi ci
{
	if(display_data[0] != 0)//red
	{
		if(red_displaying_flag == 0)
		{
			red_displaying_flag = 1;
			gre_displaying_flag = 0;
			display_data[1] = 0;
            display_data_backup = 0;
			displaying_second_counter = 0;
            printf("R--%d\r\n",display_data[0]);
		}
	}
	if(display_data[1] != 0)//gre
	{
		if(gre_displaying_flag == 0)
		{
			gre_displaying_flag = 1;
			red_displaying_flag = 0;
			display_data[0] = 0;
            display_data_backup = display_data[1];
			displaying_second_counter = 0;
            printf("G--%d\r\n",display_data[1]);
		}
	}
    
	if(red_displaying_flag == 1)
	{
		displaying_second_counter++;
		if(displaying_second_counter >= 100)
		{
			displaying_second_counter = 0;
            if(display_data[0] <= 1) 
            {
                red_displaying_flag = 0;
                display_data[0] = 0;	
            }
            else
            {
                display_data[0]--;
                if(display_data[0] == 1) displaying_second_counter = AHEAD_END_TIME;
            }
            printf("R:%d\r\n",display_data[0]);
		}
	}
	if(gre_displaying_flag == 1)
	{
		displaying_second_counter++;
		if(displaying_second_counter >= 100)
		{
			displaying_second_counter = 0;
            if(display_data[1] <= 1)
            {
                gre_displaying_flag = 0;
                display_data[1] = 0;
                last8second_flag = 0;
            }
            else 
            {
                display_data[1]--;
                if(display_data[1] == 1) displaying_second_counter = AHEAD_END_TIME;	
            }
            printf("G:%d\r\n",display_data[1]);
		}
        if(gre_displaying_flag == 1)
        {
            if(display_data_backup > 8)
            {
                if(last8second_flag == 0  && display_data[1] <= 8)
                {
                    last8second_flag = 1;
                    printf("G_speed_up\r\n");
                }
            }
            else last8second_flag = 1;
        }
	}
}

void workmodejudge(void)
{
	if(MP3.WorkMode == MODEA)//mode A
	{
		if(lamp_status == RS)
		{
			MP3.CycleTime = RedCycleTime;
		}
		else if(lamp_status == GS)
		{
			if(gre_flash_flag)
				MP3.CycleTime = GreFlashCycleTime;
			else
				MP3.CycleTime = GreCycleTime;
		}
	}
}


/*
*** void ain_filterAC_DC(void)


[ Muestreo: ain.temp_state = AIN() ]
                  |
             /          \
          /                \
       /                      \
++lamp_chge_counter      ++lamp_chge_counter
     |                        |
     |                        |
(Actualiza stab_state)   (Actualiza stab_state)
*/
void ain_filterAC_DC(void)
{
    ain.temp_state = AIN();
    if(ain.stab_state != ain.temp_state)
    {
        if(ain.temp_state)
        {
            if(++ain.lamp_chge_counter >= FILTER_ON_TIME)
            {
                ain.stab_state = ain.temp_state;
                ain.lamp_chge_counter = 0;
				printf("\r\n==AinFilter ==STBL#%d ==TMP#%d\r\n", ain.stab_state, ain.temp_state);
            }
        }
        else
        {
            if(++ain.lamp_chge_counter >= FILTER_OFF_TIME)
            {
                ain.stab_state = ain.temp_state;
                ain.lamp_chge_counter = 0;
				printf("\r\n==AinFilter ==STBL#%d ==TMP#%d\r\n", ain.stab_state, ain.temp_state);
            }
        }
    }
	else
	{
		ain.lamp_chge_counter = 0;
		//printf("\r\n==AinFilter ==STBL#%d ==TMP#%d\r\n", ain.stab_state, ain.temp_state);
	}
	
}

void filterAC_DC(void)
{
	uint8_t i;
	uint8_t temp_var1;
	uint8_t temp_var2;
	lamp_temp_state = GET_LAMP_STATE();
	temp_var1 = lamp_temp_state ^ lamp_stab_state;
	if(temp_var1 != 0)
	{
		temp_var2 = 0x01;
		for(i=0;i<2;i++)
		{
			if((temp_var1 & temp_var2)==temp_var2)
			{
				if((lamp_stab_state & temp_var2)==temp_var2)//mei dao you
				{
					if(++lamp_chge_counter[i] >= FILTER_ON_TIME)
					{
						lamp_stab_state ^= 	temp_var2;
                        lamp_state[i] = 1;
						lamp_chge_counter[i] = 0;
					}
				}
				else//you dao mei
				{
					if(++lamp_chge_counter[i] >= FILTER_OFF_TIME)
					{
						lamp_stab_state ^= 	temp_var2;
                        lamp_state[i] = 0;
						lamp_chge_counter[i] = 0;
					}
				}
			}
			else
			{
				lamp_chge_counter[i] = 0;	
			}
			temp_var2 <<= 1;	
		}
	}
	else
	{
		lamp_chge_counter[0] = 0;
		lamp_chge_counter[1] = 0;
	}
    
    if(lamp_state[0]==1 && lamp_state[1] == 0)//R	RS
    {
        if(lamp_status != RS)
        {
            if(++lamp_state_chg_count > 3)
            {
                lamp_state_chg_count = 0;
                lamp_chge_flag = 1;
                lamp_status = RS;
                if(gre_flash_flag) gre_flash_flag = 0;
                else have_gre_flash_flag = 0;
                last8second_flag = 0;
            }
        }
        lamp_off_count = 0;
    }
    else if(lamp_state[0]==0 && lamp_state[1] == 1)//G    GS
    {
        if(lamp_status != GS)
        {
            if(++lamp_state_chg_count > 3)
            {
                lamp_state_chg_count = 0;
                lamp_chge_flag = 1;
                lamp_status = GS;
                gre_start = 1;
                gre_off = 0;
            }
        }
        else
        {
            if(gre_off == 1)
            {
				if(gre_start == 1)
				{
					gre_start = 0;
					gre_off = 0;
					//lamp_chge_flag = 1;//
					gre_flash_flag = 1;
					have_gre_flash_flag = 1;
					//printf("gre_flash_flag");
				}
				else gre_off = 0;
            }
        }
        lamp_off_count = 0;
    }
    else if(lamp_state[0]==0 && lamp_state[1] == 0)//B     BS Black State 
    {
        if(++lamp_off_count > 10)//300 3
        {
            //lamp_off_count = 0;
            if(lamp_status == GS)
            {
                gre_off = 1;
            }
            if(lamp_off_count == 1000)
            {
                lamp_off_count = 0;
                lamp_state_chg_count = 0;
                lamp_chge_flag = 1;
                lamp_status = BS;
                if(gre_flash_flag) gre_flash_flag = 0;
                else have_gre_flash_flag = 0;
                last8second_flag = 0;
            }
        }
    }
	
}

void study_mode_filterAC_DC(void)
{
	unsigned char i;
	unsigned char temp_var1;
	unsigned char temp_var2;
	unsigned char temp_var3;

	temp_var1 = 0x01;
	lamp_stab_state &= 0x03;
	temp_var2 = lamp_stab_state ^ study_lamp_stab_state;

	if(temp_var2 != 0)
	{
		for(i=0;i<2;i++)
		{
			if((temp_var2&temp_var1)==temp_var1)
			{
				study_chge_counter[i]++;
				if((study_lamp_stab_state&temp_var1)==temp_var1)//mei dao you
				{
					if(study_chge_counter[i] >= 10)
					{
						study_chge_counter[i] = 0;
						study_lamp_stab_state ^= temp_var1;
					}
				}
				else
				{
					temp_var3 = (~temp_var1)&0x03;
					if((study_lamp_stab_state&temp_var3) == temp_var3)
					{
						if(study_chge_counter[i] >= 75)
						{
							study_chge_counter[i] = 0;
							study_lamp_stab_state ^= temp_var1;			
						}	
					}
					else
					{
						study_chge_counter[i] = 0;
						study_lamp_stab_state ^= temp_var1;	
					}
				}
			}
			else
			{
				study_chge_counter[i] = 0;	
			}
			temp_var1 <<= 1;			
		}
	}
	else
	{
		study_chge_counter[0] = 0;
		study_chge_counter[1] = 0;
	}
}

void study_mode_time_calculation(void)//10ms yici
{
	unsigned char i;
	unsigned char temp_var1;
	unsigned int temp_var2;
	study_lamp_stab_state &= 0x03;
	temp_var1 = 0x01;
	for(i=0;i<2;i++)
	{
		if(study_lamp_stab_state==((~temp_var1)&0x03))
		{
			if((study_time_flag&temp_var1)==0)
			{
				study_time_flag |= temp_var1;
				current_study_counter[i]=0;
                if((study_time_valid_flag&temp_var1)==temp_var1)
                {
                    if(previous_study_counter[i] >= 60000) display_data[i] = 0;
                    else 
                    {
                        display_data[i] = (previous_study_counter[i]+18)/100;   
                        if(i==0)red_displaying_flag = 0;
                        if(i==1)gre_displaying_flag = 0;
                    }
                }
			}
			else
			{
				if(++current_study_counter[i] >= 60000) current_study_counter[i] = 60000;
			}
		}
		else
		{
			if((study_time_flag&temp_var1)==temp_var1)
			{
				study_time_flag &= ~temp_var1;

				if(previous_study_counter[i] >= current_study_counter[i])
				{
					temp_var2 = previous_study_counter[i]-current_study_counter[i];
				}
				else
				{
					temp_var2 = current_study_counter[i]-previous_study_counter[i];
				}
				if((lamp_counter_valid_flag & temp_var1)==temp_var1)
				{
					if(STUDY_MODE == 0)
					{
//						if(temp_var2 > DEVIATION_TIME)
//						{
//							if(previous_study_counter[i] != 0)
//							{
//								previous_study_counter[0] = 0;	
//								previous_study_counter[1] = 0;
//                                display_data[0] = 0;
//                                display_data[1] = 0;
//								study_time_valid_flag = 0;
//							}
//						}
                        previous_study_counter[i] = current_study_counter[i];
                        study_time_valid_flag |= temp_var1;	
//                        if(temp_var2 > DEVIATION_TIME)
//                        {
//                            study_time_valid_flag &= (~temp_var1);	
//                        }
					}
					else
					{
						if(temp_var2 <= DEVIATION_TIME)
						{
							study_time_valid_flag |= temp_var1;	
							previous_study_counter[i] = current_study_counter[i];
						}
                        else
                        {
                            study_time_valid_flag &= (~temp_var1);	
                        }
//						else
//						{
//							if(previous_study_counter[i] != 0)
//							{
//								previous_study_counter[0] = 0;	
//								previous_study_counter[1] = 0;
//                                display_data[0] = 0;
//                                display_data[1] = 0;
//							}
//							previous_study_counter[i] = current_study_counter[i];//STUDY_MODE	
//							study_time_valid_flag = 0;
//						}
					}
				}
			}
		}
		if(((study_lamp_stab_state&temp_var1)==temp_var1)&&(study_lamp_stab_state!=0x03)) 
		{
			lamp_counter_valid_flag |= temp_var1;
		}
		temp_var1 <<= 1; 
	}
}

extern __IO uint16_t reg1ms_count;

void flash_panel_control(void)//1ms
{
	if(system_temp.pps_flag)
	{
		if(++system_temp.pps_1ms >= 1000) 
		{
			system_temp.pps_1ms = 0;
		}
		
		if(++system_temp.pps_count >= 3000) 
		{
			system_temp.pps_count = 0;
			system_temp.pps_flag = 0;
		}
	}
	
	if(system_temp.gps_flag)
	{
		system_temp.gps1ms++;
		if(system_temp.pps_1ms < 300) 
		{
			//DR1_ON(); DR2_ON();
		}
		else 
		{
			//DR1_OFF(); DR2_OFF();
		}
		//if((system_temp.gps1ms%3000) < 300) 
			//DR3_ON();
		//else
			//DR3_OFF(); 
	}
	else//
	{
		//if(reg1ms_count < 300) DR1_ON(); else DR1_OFF(); 
		//if(reg1ms_count >= 500 && reg1ms_count < 800) DR2_ON(); else DR2_OFF();
		//DR3_OFF();
	}
}

void Auto_adjust_time(void)
{
    if(system_temp.sync_with_gps_flag)//wcxmask
    {
        RtcWrite(&system_temp.Gps.local);
		system_temp.timeUpdate = 1;
        system_temp.sync_with_gps_flag = 0;
        #if DEBUG > 2
        printf("sync_with_gps, Seconds = %d, gps_seconds = %d\n", system_temp.seconds, system_temp.gps_seconds);
        #endif
    }
}

void pps_irq_init(void)
{
    EXTI_InitTypeDef EXTI_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
    
    
  	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource1);
    
  	EXTI_InitStructure.EXTI_Line = EXTI_Line1;
  	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;	
  	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
  	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    
  	EXTI_Init(&EXTI_InitStructure);
	
    
  	NVIC_InitStructure.NVIC_IRQChannel = EXTI1_IRQn;
    
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00;	//
    
  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x02;		
    
  	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;	
    
  	NVIC_Init(&NVIC_InitStructure);
}

void EXTI1_IRQHandler(void)
{
	if(EXTI_GetITStatus(EXTI_Line1) != RESET)//PPS_irq PA1
	{
        EXTI_ClearFlag(EXTI_Line1);
		system_temp.pps_count = 0;
		system_temp.pps_1ms = 0;
		system_temp.pps_flag = 1;
		printf("PPS_IRQ\n");
	}
}


