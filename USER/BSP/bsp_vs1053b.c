

#include "stm32f10x.h" 
/*
		PB13/SPI1_SCK
		PB14/SPI1_MISO
		PB15/SPI1_MOSI

		VS1053_DREQ     PB11    PA8
		VS1053_XDCS     PB2     PC7
		VS1053_XCS      PB12    PC6
*/

#define VS1053_RST_0()	GPIOB->BRR = GPIO_Pin_2
#define VS1053_RST_1()	GPIOB->BSRR = GPIO_Pin_2

#define VS1053_CS_0()	GPIOC->BRR = GPIO_Pin_6
#define VS1053_CS_1()	GPIOC->BSRR = GPIO_Pin_6

#define VS1053_DS_0()	GPIOC->BRR = GPIO_Pin_7
#define VS1053_DS_1()	GPIOC->BSRR = GPIO_Pin_7





/* VS1053_DREQ = PA8 */
#define VS1053_IS_BUSY()	((GPIOA->IDR & GPIO_Pin_8) == 0)

#define DUMMY_BYTE    0xFF

//uint8_t vs1053ram[5]={0,0,0,0,250};


//const uint16_t plugin[605] = { /* Compressed plugin */
//  0x0007, 0x0001, 0x8300, 0x0006, 0x01f2, 0xb080, 0x0024, 0x0007, /*    0 */
//  0x9257, 0x3f00, 0x0024, 0x0030, 0x0297, 0x3f00, 0x0024, 0x0006, /*    8 */
//  0x0017, 0x3f10, 0x0024, 0x3f00, 0x0024, 0x0000, 0xf6d7, 0xf400, /*   10 */
//  0x55c0, 0x0000, 0x0817, 0xf400, 0x57c0, 0x0000, 0x004d, 0x000a, /*   18 */
//  0x708f, 0x0000, 0xc44e, 0x280f, 0xe100, 0x0006, 0x2016, 0x0000, /*   20 */
//  0x028d, 0x0014, 0x1b01, 0x2800, 0xc795, 0x0015, 0x59c0, 0x0000, /*   28 */
//  0xfa0d, 0x0039, 0x324f, 0x0000, 0xd20e, 0x2920, 0x41c0, 0x0000, /*   30 */
//  0x0024, 0x000a, 0x708f, 0x0000, 0xc44e, 0x280a, 0xcac0, 0x0000, /*   38 */
//  0x028d, 0x6fc2, 0x0024, 0x000c, 0x0981, 0x2800, 0xcad5, 0x0000, /*   40 */
//  0x18c2, 0x290c, 0x4840, 0x3613, 0x0024, 0x290c, 0x4840, 0x4086, /*   48 */
//  0x184c, 0x6234, 0x0024, 0x0000, 0x0024, 0x2800, 0xcad5, 0x0030, /*   50 */
//  0x0317, 0x3f00, 0x0024, 0x280a, 0x71c0, 0x002c, 0x9d40, 0x3613, /*   58 */
//  0x0024, 0x3e10, 0xb803, 0x3e14, 0x3811, 0x3e11, 0x3805, 0x3e00, /*   60 */
//  0x3801, 0x0007, 0xc390, 0x0006, 0xa011, 0x3010, 0x0444, 0x3050, /*   68 */
//  0x4405, 0x6458, 0x0302, 0xff94, 0x4081, 0x0003, 0xffc5, 0x48b6, /*   70 */
//  0x0024, 0xff82, 0x0024, 0x42b2, 0x0042, 0xb458, 0x0003, 0x4cd6, /*   78 */
//  0x9801, 0xf248, 0x1bc0, 0xb58a, 0x0024, 0x6de6, 0x1804, 0x0006, /*   80 */
//  0x0010, 0x3810, 0x9bc5, 0x3800, 0xc024, 0x36f4, 0x1811, 0x36f0, /*   88 */
//  0x9803, 0x283e, 0x2d80, 0x0fff, 0xffc3, 0x003e, 0x2d4f, 0x2800, /*   90 */
//  0xe380, 0x0000, 0xcb4e, 0x3413, 0x0024, 0x2800, 0xd405, 0xf400, /*   98 */
//  0x4510, 0x2800, 0xd7c0, 0x6894, 0x13cc, 0x3000, 0x184c, 0x6090, /*   a0 */
//  0x93cc, 0x38b0, 0x3812, 0x3004, 0x4024, 0x0000, 0x0910, 0x3183, /*   a8 */
//  0x0024, 0x3100, 0x4024, 0x6016, 0x0024, 0x000c, 0x8012, 0x2800, /*   b0 */
//  0xd711, 0xb884, 0x104c, 0x6894, 0x3002, 0x2939, 0xb0c0, 0x3e10, /*   b8 */
//  0x93cc, 0x4084, 0x9bd2, 0x4282, 0x0024, 0x0000, 0x0041, 0x2800, /*   c0 */
//  0xd9c5, 0x6212, 0x0024, 0x0000, 0x0040, 0x2800, 0xdec5, 0x000c, /*   c8 */
//  0x8390, 0x2a00, 0xe240, 0x34c3, 0x0024, 0x3444, 0x0024, 0x3073, /*   d0 */
//  0x0024, 0x3053, 0x0024, 0x3000, 0x0024, 0x6092, 0x098c, 0x0000, /*   d8 */
//  0x0241, 0x2800, 0xe245, 0x32a0, 0x0024, 0x6012, 0x0024, 0x0000, /*   e0 */
//  0x0024, 0x2800, 0xe255, 0x0000, 0x0024, 0x3613, 0x0024, 0x3001, /*   e8 */
//  0x3844, 0x2920, 0x0580, 0x3009, 0x3852, 0xc090, 0x9bd2, 0x2800, /*   f0 */
//  0xe240, 0x3800, 0x1bc4, 0x000c, 0x4113, 0xb880, 0x2380, 0x3304, /*   f8 */
//  0x4024, 0x3800, 0x05cc, 0xcc92, 0x05cc, 0x3910, 0x0024, 0x3910, /*  100 */
//  0x4024, 0x000c, 0x8110, 0x3910, 0x0024, 0x39f0, 0x4024, 0x3810, /*  108 */
//  0x0024, 0x38d0, 0x4024, 0x3810, 0x0024, 0x38f0, 0x4024, 0x34c3, /*  110 */
//  0x0024, 0x3444, 0x0024, 0x3073, 0x0024, 0x3063, 0x0024, 0x3000, /*  118 */
//  0x0024, 0x4080, 0x0024, 0x0000, 0x0024, 0x2839, 0x53d5, 0x4284, /*  120 */
//  0x0024, 0x3613, 0x0024, 0x2800, 0xe585, 0x6898, 0xb804, 0x0000, /*  128 */
//  0x0084, 0x293b, 0x1cc0, 0x3613, 0x0024, 0x000c, 0x8117, 0x3711, /*  130 */
//  0x0024, 0x37d1, 0x4024, 0x4e8a, 0x0024, 0x0000, 0x0015, 0x2800, /*  138 */
//  0xe845, 0xce9a, 0x0024, 0x3f11, 0x0024, 0x3f01, 0x4024, 0x000c, /*  140 */
//  0x8197, 0x408a, 0x9bc4, 0x3f15, 0x4024, 0x2800, 0xea85, 0x4284, /*  148 */
//  0x3c15, 0x6590, 0x0024, 0x0000, 0x0024, 0x2839, 0x53d5, 0x4284, /*  150 */
//  0x0024, 0x0000, 0x0024, 0x2800, 0xd2d8, 0x458a, 0x0024, 0x2a39, /*  158 */
//  0x53c0, 0x3009, 0x3851, 0x3e14, 0xf812, 0x3e12, 0xb817, 0x0006, /*  160 */
//  0xa057, 0x3e11, 0x9fd3, 0x0023, 0xffd2, 0x3e01, 0x0024, 0x0006, /*  168 */
//  0x0011, 0x3111, 0x0024, 0x6498, 0x07c6, 0x868c, 0x2444, 0x3901, /*  170 */
//  0x8e06, 0x0030, 0x0551, 0x3911, 0x8e06, 0x3961, 0x9c44, 0xf400, /*  178 */
//  0x44c6, 0xd46c, 0x1bc4, 0x36f1, 0xbc13, 0x2800, 0xf615, 0x36f2, /*  180 */
//  0x9817, 0x002b, 0xffd2, 0x3383, 0x188c, 0x3e01, 0x8c06, 0x0006, /*  188 */
//  0xa097, 0x468c, 0xbc17, 0xf400, 0x4197, 0x2800, 0xf304, 0x3713, /*  190 */
//  0x0024, 0x2800, 0xf345, 0x37e3, 0x0024, 0x3009, 0x2c17, 0x3383, /*  198 */
//  0x0024, 0x3009, 0x0c06, 0x468c, 0x4197, 0x0006, 0xa052, 0x2800, /*  1a0 */
//  0xf544, 0x3713, 0x2813, 0x2800, 0xf585, 0x37e3, 0x0024, 0x3009, /*  1a8 */
//  0x2c17, 0x36f1, 0x8024, 0x36f2, 0x9817, 0x36f4, 0xd812, 0x2100, /*  1b0 */
//  0x0000, 0x3904, 0x5bd1, 0x2a00, 0xeb8e, 0x3e11, 0x7804, 0x0030, /*  1b8 */
//  0x0257, 0x3701, 0x0024, 0x0013, 0x4d05, 0xd45b, 0xe0e1, 0x0007, /*  1c0 */
//  0xc795, 0x2800, 0xfd95, 0x0fff, 0xff45, 0x3511, 0x184c, 0x4488, /*  1c8 */
//  0xb808, 0x0006, 0x8a97, 0x2800, 0xfd45, 0x3009, 0x1c40, 0x3511, /*  1d0 */
//  0x1fc1, 0x0000, 0x0020, 0xac52, 0x1405, 0x6ce2, 0x0024, 0x0000, /*  1d8 */
//  0x0024, 0x2800, 0xfd41, 0x68c2, 0x0024, 0x291a, 0x8a40, 0x3e10, /*  1e0 */
//  0x0024, 0x2921, 0xca80, 0x3e00, 0x4024, 0x36f3, 0x0024, 0x3009, /*  1e8 */
//  0x1bc8, 0x36f0, 0x1801, 0x2808, 0x9300, 0x3601, 0x5804, 0x0007, /*  1f0 */
//  0x0001, 0x802e, 0x0006, 0x0002, 0x2800, 0xf700, 0x0007, 0x0001, /*  1f8 */
//  0x8050, 0x0006, 0x0028, 0x3e12, 0x3800, 0x2911, 0xf140, 0x3e10, /*  200 */
//  0x8024, 0xf400, 0x4595, 0x3593, 0x0024, 0x35f3, 0x0024, 0x3500, /*  208 */
//  0x0024, 0x0021, 0x6d82, 0xd024, 0x44c0, 0x0006, 0xa402, 0x2800, /*  210 */
//  0x1815, 0xd024, 0x0024, 0x0000, 0x0000, 0x2800, 0x1815, 0x000b, /*  218 */
//  0x6d57, 0x3009, 0x3c00, 0x36f0, 0x8024, 0x36f2, 0x1800, 0x2000, /*  220 */
//  0x0000, 0x0000, 0x0024, 0x0007, 0x0001, 0x8030, 0x0006, 0x0002, /*  228 */
//  0x2800, 0x1400, 0x0007, 0x0001, 0x8064, 0x0006, 0x001c, 0x3e12, /*  230 */
//  0xb817, 0x3e14, 0xf812, 0x3e01, 0xb811, 0x0007, 0x9717, 0x0020, /*  238 */
//  0xffd2, 0x0030, 0x11d1, 0x3111, 0x8024, 0x3704, 0xc024, 0x3b81, /*  240 */
//  0x8024, 0x3101, 0x8024, 0x3b81, 0x8024, 0x3f04, 0xc024, 0x2808, /*  248 */
//  0x4800, 0x36f1, 0x9811, 0x0007, 0x0001, 0x8028, 0x0006, 0x0002, /*  250 */
//  0x2a00, 0x190e, 0x000a, 0x0001, 0x0300,
//};


void vs1053_IO_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;


	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC | RCC_APB2Periph_AFIO, ENABLE);


	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA,&GPIO_InitStructure);


	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOC,&GPIO_InitStructure);	


	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOC,&GPIO_InitStructure);	


	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOB,&GPIO_InitStructure);	
    VS1053_RST_1();
	VS1053_CS_1();
	VS1053_DS_1();

}


static void vs1053_SetCS(uint8_t _level)
{
	if (_level == 0)
	{
        VS1053_CS_0();
		spi2_busy_flag = 1;
	}
	else
	{
        VS1053_CS_1();
		spi2_busy_flag = 0;
	}
}



static void vs1053_SetDS(uint8_t _level)
{
	if (_level == 0)
	{
        VS1053_DS_0();
	}
	else
	{
        VS1053_DS_1();
	}
}


void vs1053_WriteCmd(uint8_t _ucAddr, uint16_t _usData)
{

	if (vs1053_WaitTimeOut())
	{
		return;
	}

	vs1053_SetCS(0);

	Spi2_SendByte(VS_WRITE_COMMAND);
	Spi2_SendByte(_ucAddr);
	Spi2_SendByte(_usData >> 8);
	Spi2_SendByte(_usData);
	
	vs1053_SetCS(1);
}


uint8_t vs1053_ReqNewData(void)
{
	if (VS1053_IS_BUSY())
	{
		return 0;
	}
	else
	{
		return 1;
	}
}


void vs1053_PreWriteData(void)
{
	VS1053_CS_1();
	VS1053_DS_0();
}


void vs1053_WriteData(uint8_t _ucData)
{
	vs1053_SetDS(0);
	Spi2_SendByte(_ucData);
	vs1053_SetDS(1);
}

void vs1053_WriteDatas(uint8_t *databuf, uint8_t n)
{
	vs1053_SetDS(0);
    while (n--)
    {
        Spi2_SendByte(*databuf++);
    }
	vs1053_SetDS(1);
}


uint16_t vs1053_ReadReg(uint8_t _ucAddr)
{
	uint16_t usTemp;


	if (vs1053_WaitTimeOut())
	{
		return 0;
	}

	vs1053_SetCS(0);
	
	Spi2_SendByte(VS_READ_COMMAND);
	Spi2_SendByte(_ucAddr);
	usTemp = Spi2_SendByte(DUMMY_BYTE) << 8;
	usTemp += Spi2_SendByte(DUMMY_BYTE);
	
	vs1053_SetCS(1);
	return usTemp;
}


uint8_t vs1053_ReadChipID(void)
{
	uint16_t usStatus;
    char *pModel;
	
	usStatus = vs1053_ReadReg(SCI_STATUS);
	usStatus = ((usStatus >> 4) & 0x000F);
    

    switch (usStatus)
    {
        case VS1001:    pModel = "VS1001";      break;
        case VS1011:    pModel = "VS1011";      break;
        case VS1002:    pModel = "VS1002";      break;
        case VS1003:    pModel = "VS1003";      break;
        case VS1053:    pModel = "VS1053";      break;
        case VS1033:    pModel = "VS1033";      break;
        case VS1103:    pModel = "VS1103";      break;
        default:        pModel = "unknow";      break;
    }
    printf("Ω‚¬Î–æ∆¨–Õ∫≈ : %s\r\n", pModel);
    return usStatus;
}

void vs1053_HardInit(void)
{

    VS_HD_Reset();
    //vs1053_SoftReset();
    vs1053_ReadChipID();    


}

uint8_t VS_HD_Reset(void)
{
	uint8_t retry=0;
	VS1053_RST_0();
	Delay(20);
	VS1053_DS_1();
	VS1053_CS_1();
	VS1053_RST_1();
	while(VS1053_IS_BUSY()&&retry<200)
	{
		retry++;
		Delay(1);
	};
	Delay(20);
	if(retry>=200)return 1;
	else return 0;	    		 
}



uint8_t vs1053_WaitTimeOut(void)
{
	uint32_t i;

	for (i = 0; i < 4000000; i++)
	{
		if (!VS1053_IS_BUSY())
		{
			break;
		}
	}

	if (i >= 4000000)
	{
		return 1;
	}

	return 0;
}

//void LoadUserPatch(void)
//{
//	int i = 0;

//	while (i < sizeof(plugin) / sizeof(plugin[0]))
//	{
//		unsigned short addr, n, val;

//		addr = plugin[i++];
//		n = plugin[i++];
//		if (n & 0x8000U)
//		{
//			/* RLE run, replicate n samples */
//			n &= 0x7FFF;
//			val = plugin[i++];
//			while (n--)
//			{
//				vs1053_WriteCmd(addr, val);
//			}
//		}
//		else
//		{
//			/* Copy run, copy n samples */
//			while (n--)
//			{
//				val = plugin[i++];
//				vs1053_WriteCmd(addr, val);
//			}
//		}
//	}

//	if (vs1053_WaitTimeOut())
//	{
//		return;
//	}
//}


uint8_t vs1053_TestRam(void)
{
	uint16_t usRegValue;

 	vs1053_WriteCmd(SCI_MODE, 0x0820);	


	if (vs1053_WaitTimeOut())
	{
		return 0;
	}

	vs1053_SetDS(0);
	
	Spi2_SendByte(0x4d);
	Spi2_SendByte(0xea);
	Spi2_SendByte(0x6d);
	Spi2_SendByte(0x54);
	Spi2_SendByte(0x00);
	Spi2_SendByte(0x00);
	Spi2_SendByte(0x00);
	Spi2_SendByte(0x00);
	
	vs1053_SetDS(1);


	if (vs1053_WaitTimeOut())
	{
		return 0;
	}

	usRegValue = vs1053_ReadReg(SCI_HDAT0); 

	if (usRegValue == 0x807F)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}


void vs1053_TestSine(void)
{
	

	vs1053_WriteCmd(0x0b,0x2020);	  	
 	vs1053_WriteCmd(SCI_MODE, 0x0820);	


	if (vs1053_WaitTimeOut())
	{
		return;
	}

 	
	vs1053_SetDS(0);
	Spi2_SendByte(0x53);
	Spi2_SendByte(0xef);
	Spi2_SendByte(0x6e);
	Spi2_SendByte(0x24);	/* 0x24 or 0x44 */
	Spi2_SendByte(0x00);
	Spi2_SendByte(0x00);
	Spi2_SendByte(0x00);
	Spi2_SendByte(0x00);
	vs1053_SetDS(1);


//    vs1053_SetDS(0);
//	Spi2_SendByte(0x45);
//	Spi2_SendByte(0x78);
//	Spi2_SendByte(0x69);
//	Spi2_SendByte(0x74);
//	Spi2_SendByte(0x00);
//	Spi2_SendByte(0x00);
//	Spi2_SendByte(0x00);
//	Spi2_SendByte(0x00);
//	vs1053_SetDS(1);
}

void vs1053_TestSineExit(void)
{
	
    vs1053_SetDS(0);
	Spi2_SendByte(0x45);
	Spi2_SendByte(0x78);
	Spi2_SendByte(0x69);
	Spi2_SendByte(0x74);
	Spi2_SendByte(0x00);
	Spi2_SendByte(0x00);
	Spi2_SendByte(0x00);
	Spi2_SendByte(0x00);
	vs1053_SetDS(1);
}

void vs1053_SoftReset(void)
{
	uint8_t retry = 0;
    uint16_t RSCI_HDAT0,RSCI_HDAT1;
    RESET:

    while(VS1053_IS_BUSY()==RESET)
    {
        if(++retry<5) Delay(1);
        else break;
    }
    /* A quick sanity check: write to two registers, then test if we
     get the same results. Note that if you use a too high SPI
     speed, the MSB is the most likely to fail when read again. */
    vs1053_WriteCmd(SCI_HDAT0, 0xABAD);
    vs1053_WriteCmd(SCI_HDAT1, 0x55aa);
    RSCI_HDAT0 = vs1053_ReadReg(SCI_HDAT0);
    RSCI_HDAT1 = vs1053_ReadReg(SCI_HDAT1);
    if (RSCI_HDAT0 != 0xABAD || RSCI_HDAT1 != 0x55aa)
    {
        printf("There is something wrong with VS10xx\n");
        VS_HD_Reset();
		MP3.ucVolume = 1;
        goto RESET;
    }
    
	Spi2_SendByte(0X00);
	retry = 0;
	while(vs1053_ReadReg(SCI_MODE) != 0x0804)
	{
		
		vs1053_WriteCmd(SCI_MODE, 0x0804);


        while(VS1053_IS_BUSY()==RESET);
        if (retry++>5)
        {
            break;
        }
	}
    //SM_CANCEL P49
#if 0
	vs1053_WriteCmd(SCI_CLOCKF,0x9800);
	vs1053_WriteCmd(SCI_AUDATA,0xBB81); 

	vs1053_WriteCmd(SCI_BASS, 0x0000);	/* */
    vs1053_WriteCmd(SCI_VOL, 0x2020); 	

	ResetDecodeTime();


    VS1053_DS_0();
	vs1053_WriteByte(0xFF);
	vs1053_WriteByte(0xFF);
	vs1053_WriteByte(0xFF);
	vs1053_WriteByte(0xFF);
	VS1053_DS_1();
#else
	/* Set clock register, doubler etc. */
	vs1053_WriteCmd(SCI_CLOCKF, 0xC000);





    while(VS1053_IS_BUSY()==RESET);
    //LoadUserPatch();
#endif
}

void vs1053_SetVolume(uint8_t _ucVol)
{

	if (_ucVol == 0)
	{
		_ucVol = 254;
	}
	else if (_ucVol == 255)
	{
		_ucVol = 0;
	}
	else
	{
		_ucVol = 254 - _ucVol;
	}

	vs1053_WriteCmd(SCI_VOL, (_ucVol << 8) | _ucVol);
}


void vs1053_SetBASS(int8_t _cHighAmp, uint16_t _usHighFreqCut, uint8_t _ucLowAmp, uint16_t _usLowFreqCut)
{
	uint16_t usValue;




	if (_cHighAmp < -8)
	{
		_cHighAmp = -8;
	}
	else if (_cHighAmp > 7)
	{
		_cHighAmp = 7;
	}
	usValue = _cHighAmp << 12;


	if (_usHighFreqCut < 1000)
	{
		_usHighFreqCut = 1000;
	}
	else if (_usHighFreqCut > 15000)
	{
		_usHighFreqCut = 15000;
	}
	usValue  += ((_usHighFreqCut / 1000) << 8);


	if (_ucLowAmp > 15)
	{
		_ucLowAmp = 15;
	}
	usValue  += (_ucLowAmp << 4);


	if (_usLowFreqCut < 20)
	{
		_usLowFreqCut = 20;
	}
	else if (_usLowFreqCut > 150)
	{
		_usLowFreqCut = 150;
	}
	usValue  += (_usLowFreqCut / 10);

	vs1053_WriteCmd(SCI_BASS, usValue);
}


void ResetDecodeTime(void)
{
	vs1053_WriteCmd(SCI_DECODE_TIME, 0x0000);
}



#if 0


void VsRamTest(void)
{
	uint16_t u16 regvalue ;

	Mp3Reset();
 	vs1053_CMD_Write(SPI_MODE,0x0820);
	while ((GPIOC->IDR&MP3_DREQ)==0);
 	MP3_DCS_SET(0);	       			  
	SPI1_ReadWriteByte(0x4d);
	SPI1_ReadWriteByte(0xea);
	SPI1_ReadWriteByte(0x6d);
	SPI1_ReadWriteByte(0x54);
	SPI1_ReadWriteByte(0x00);
	SPI1_ReadWriteByte(0x00);
	SPI1_ReadWriteByte(0x00);
	SPI1_ReadWriteByte(0x00);
	delay_ms(50);
	MP3_DCS_SET(1);
	regvalue=vs1053_REG_Read(SPI_HDAT0);
	printf("regvalueH:%x\n",regvalue>>8);
	printf("regvalueL:%x\n",regvalue&0xff);
}

//FOR WAV HEAD0 :0X7761 HEAD1:0X7665
//FOR MIDI HEAD0 :other info HEAD1:0X4D54
//FOR WMA HEAD0 :data speed HEAD1:0X574D
//FOR MP3 HEAD0 :data speed HEAD1:ID

const uint16_t bitrate[2][16]=
{
	{0,8,16,24,32,40,48,56,64,80,96,112,128,144,160,0},
	{0,32,40,48,56,64,80,96,112,128,160,192,224,256,320,0}
};


uint16_t GetHeadInfo(void)
{
	unsigned int HEAD0;
	unsigned int HEAD1;

    HEAD0=vs1053_REG_Read(SPI_HDAT0);
    HEAD1=vs1053_REG_Read(SPI_HDAT1);
    switch(HEAD1)
    {
        case 0x7665:return 0;
        case 0X4D54:return 1;
        case 0X574D:
        {
            HEAD1=HEAD0*2/25;
            if((HEAD1%10)>5)return HEAD1/10+1;
            else return HEAD1/10;
        }
        default:
        {
            HEAD1>>=3;
            HEAD1=HEAD1&0x03;
            if(HEAD1==3)HEAD1=1;
            else HEAD1=0;
            return bitrate[HEAD1][HEAD0>>12];
        }
    }
}


uint16_t GetDecodeTime(void)
{
    return vs1053_REG_Read(SPI_DECODE_TIME);
}

void LoadPatch(void)
{
	uint16_t i;

	for (i=0;i<943;i++)vs1053_CMD_Write(atab[i],dtab[i]);
	delay_ms(10);
}

void GetSpec(u8 *p)
{
	u8 byteIndex=0;
	u8 temp;
	vs1053_CMD_Write(SPI_WRAMADDR,0x1804);
	for (byteIndex=0;byteIndex<14;byteIndex++)
	{
		temp=vs1053_REG_Read(SPI_WRAM)&0x63;
		*p++=temp;
	}
}


void set1003(void)
{
    uint8 t;
    uint16_t bass=0;
    uint16_t volt=0;
    uint8_t vset=0;

    vset=255-vs1053ram[4];
    volt=vset;
    volt<<=8;
    volt+=vset;
     //0,henh.1,hfreq.2,lenh.3,lfreq
    for(t=0;t<4;t++)
    {
        bass<<=4;
        bass+=vs1053ram[t];
    }
	vs1053_CMD_Write(SPI_BASS, 0x0000);//BASS
    vs1053_CMD_Write(SPI_VOL, 0x0000);
}

#endif
