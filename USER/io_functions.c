#include "io_functions.h"
#include <stdio.h>
/* Include the header that defines the MP3 struct and MODEA for workmodejudge() */
#include "spi_flash_fatfs_mp3.h" 

/* State Variables Moved from bsp_io.c */
//uint8_t green_flash_sound_en = 0;

/* Functions */
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

/* State Variables Moved from bsp_io.c */
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

uint16_t lamp_chge_counter[2] = {0};//20MS锟剿诧拷锟斤拷锟斤拷锟斤拷
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

void down_time_display(void)	//10ms yi ci
{
	if(display_data[0] != 0)//red
	{
		if(red_displaying_flag == 0)//锟斤拷始锟斤拷锟斤拷时
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
		if(gre_displaying_flag == 0)//锟斤拷始锟斤拷锟斤拷时
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


void ain_filterAC_DC(void)
{
	DR2_Toggle();//Solo pruebas
    ain.temp_state = AIN();
    if(ain.stab_state != ain.temp_state)
    {
        if(ain.temp_state)
        {
            if(++ain.lamp_chge_counter >= FILTER_ON_TIME)
            {
                ain.stab_state = ain.temp_state;
                ain.lamp_chge_counter = 0;
            }
        }
        else
        {
            if(++ain.lamp_chge_counter >= FILTER_OFF_TIME)
            {
                ain.stab_state = ain.temp_state;
                ain.lamp_chge_counter = 0;
            }
        }
    }
	else
	{
		ain.lamp_chge_counter = 0;
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
        else//锟斤拷锟斤拷-锟斤拷锟脚猴拷锟斤拷锟斤拷
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


void study_mode_filterAC_DC(void)//10锟斤拷锟斤拷锟斤拷锟揭伙拷锟絚alculation
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


void study_mode_time_calculation(void)	//10ms yici
{
	unsigned char i;
	unsigned char temp_var1;
	unsigned int temp_var2;
	study_lamp_stab_state &= 0x03;
	temp_var1 = 0x01;
	for(i=0;i<2;i++)
	{
		if(study_lamp_stab_state==((~temp_var1)&0x03))//锟叫灯伙拷 锟斤拷为锟斤拷锟斤拷锟斤拷锟斤拷锟斤拷锟窖澳Ｊ斤拷陆锟斤拷锟斤拷锟绞�
		{
			if((study_time_flag&temp_var1)==0)//没 锟斤拷锟斤拷学习锟斤拷锟斤拷 锟狡刚碉拷锟斤拷
			{
				study_time_flag |= temp_var1;
				current_study_counter[i]=0;
                if((study_time_valid_flag&temp_var1)==temp_var1)//学习锟斤拷锟斤拷锟斤拷OK
                {
                    if(previous_study_counter[i] >= 60000) display_data[i] = 0;
                    else 
                    {
                        display_data[i] = (previous_study_counter[i]+18)/100;   //锟斤拷坪锟斤拷痰频锟窖笆憋拷浯ワ拷锟�
                        if(i==0)red_displaying_flag = 0;//锟斤拷锟铰匡拷始锟斤拷锟斤拷时
                        if(i==1)gre_displaying_flag = 0;
                    }
                }
			}
			else
			{
				if(++current_study_counter[i] >= 60000) current_study_counter[i] = 60000;
			}
		}
		else//锟斤拷锟斤拷10 01 11锟斤拷锟斤拷锟斤拷状态时
		{
			if((study_time_flag&temp_var1)==temp_var1)// 一锟斤拷状态锟斤拷锟斤拷,锟斤拷锟斤拷状态锟斤拷效
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
					if(STUDY_MODE == 0)//锟斤拷锟轿拷锟斤拷锟斤拷锟侥Ｊ�
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
					else//为双锟斤拷锟斤拷模式
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
			lamp_counter_valid_flag |= temp_var1;//锟斤拷坪锟斤拷痰频锟斤拷锟叫ё刺� //锟斤拷前状态锟斤拷一锟斤拷锟斤拷锟斤拷锟斤拷状态,锟斤拷锟斤拷状态锟叫伙拷锟斤拷锟阶刺拷锟斤拷锟揭伙拷锟斤拷锟斤拷锟斤拷锟阶刺�,锟斤拷锟斤拷锟斤拷为锟斤拷锟较碉拷时状态锟侥诧拷锟斤拷锟斤拷
		}
		temp_var1 <<= 1; 
	}
}


extern __IO uint16_t reg1ms_count;
void flash_panel_control(void)//1ms
{
	if(system_temp.pps_flag)
	{
		if(++system_temp.pps_1ms >= 1000)	//3S没锟斤拷锟秸碉拷PPS锟脚号ｏ拷锟斤拷0 pps_flag
		{
			system_temp.pps_1ms = 0;
		}
		
		if(++system_temp.pps_count >= 3000)	 //3S没锟斤拷锟秸碉拷PPS锟脚号ｏ拷锟斤拷0 pps_flag
		{
			system_temp.pps_count = 0;
			system_temp.pps_flag = 0;
		}
	}
//锟斤拷锟斤拷锟绞憋拷慰锟斤拷锟�
	if(system_temp.gps_flag)//GPS时锟斤拷锟斤拷效
	{
		system_temp.gps1ms++;
		if(system_temp.pps_1ms < 300) 
		{
			//DR1_ON(); DR2_ON(); //Apago DR para pruebas estables
		}
		else 
		{
			//DR1_OFF(); DR2_OFF(); //Apago DR para pruebas estables
		}
		//if((system_temp.gps1ms%3000) < 300)  //Apago DR para pruebas estables
			//DR3_ON(); //Apago DR para pruebas estables
		//else //Apago DR para pruebas estables
			//DR3_OFF();  //Apago DR para pruebas estables
	}
	else//
	{
		//if(reg1ms_count < 300) DR1_ON(); else DR1_OFF();  //Apago DR para pruebas estables
		//if(reg1ms_count >= 500 && reg1ms_count < 800) DR2_ON(); else DR2_OFF(); //Apago DR para pruebas estables
		//DR3_OFF(); //Apago DR para pruebas estables
	}
}
