#ifndef __BSP_IO_H
#define __BSP_IO_H
//#include <stdint.h>

#define DEBUG 9

/* Moved from bsp_io.c */
//#define SW4()   		(GPIOC->IDR & 0x0008)
#define SW1_3()	(GPIOC->IDR & 0x000f)	//Read 4bit Switch
#define RIN()	((GPIOC->IDR & 0x2000)? 0:1)	//#define RIN()	(GPIOC->IDR & 0x2000)	// 0:1
#define GIN()   ((GPIOC->IDR & 0x4000)? 0:1)	//#define GIN() (GPIOC->IDR & 0x4000)	// 0:1
#define AIN()   ((GPIOC->IDR & 0x8000)? 0:1)	//#define AIN() ((GPIOC->IDR & 0x8000)? 0:1)
#define GET_MUTE_STATE()   (GPIOC->IDR & 0x8000)
#define GET_LAMP_STATE()   ((GPIOC->IDR>>13) & 0x0003)

#define GPS_ON()        GPIO_SetBits(GPIOA,GPIO_Pin_0)   //salida que controla pin para encender o apagar modulo GPS
#define GPS_OFF()       GPIO_ResetBits(GPIOA,GPIO_Pin_0)
#define PPS_IN()   		(GPIOA->IDR & 0x0002)

#define PA_ON()       	GPIO_SetBits(GPIOA,GPIO_Pin_11) //Amplificador de potencia ON
#define PA_OFF()     	GPIO_ResetBits(GPIOA,GPIO_Pin_11) //Amplificador de potencia OFF

#define ADC_ON()        GPIO_ResetBits(GPIOA,GPIO_Pin_12)
#define ADC_OFF()       GPIO_SetBits(GPIOA,GPIO_Pin_12)

#define LED_ON()        GPIO_ResetBits(GPIOA,GPIO_Pin_15)
#define LED_OFF()       GPIO_SetBits(GPIOA,GPIO_Pin_15)
#define LED_Toggle()    GPIOA->ODR ^= GPIO_Pin_15

#define DR1_OFF()     	GPIO_ResetBits(GPIOC,GPIO_Pin_10)
#define DR1_ON()       	GPIO_SetBits(GPIOC,GPIO_Pin_10)
#define DR1_Toggle()    GPIOC->ODR ^= GPIO_Pin_10
#define DR2_OFF()    	GPIO_ResetBits(GPIOC,GPIO_Pin_11)
#define DR2_ON()       	GPIO_SetBits(GPIOC,GPIO_Pin_11)
#define DR2_Toggle()    GPIOC->ODR ^= GPIO_Pin_11
#define DR3_OFF()     	GPIO_ResetBits(GPIOC,GPIO_Pin_12)
#define DR3_ON()       	GPIO_SetBits(GPIOC,GPIO_Pin_12)
#define DR3_Toggle()    GPIOC->ODR ^= GPIO_Pin_12

/////////////////////////////////
#define RELAY_OFF()     GPIO_ResetBits(GPIOB,GPIO_Pin_9)
#define RELAY_ON()      GPIO_SetBits(GPIOB,GPIO_Pin_9)

//#define Out1_OFF()		GPIO_ResetBits(GPIOB,GPIO_Pin_11)
//#define Out1_ON()		GPIO_SetBits(GPIOB,GPIO_Pin_11)
//#define Out1_Toggle()	GPIOB->ODR ^= GPIO_Pin_11
//#define Out2_OFF()		GPIO_ResetBits(GPIOB,GPIO_Pin_15)
//#define Out2_ON()		GPIO_SetBits(GPIOB,GPIO_Pin_15
//#define Out2_Toggle()	GPIOB->ODR ^= GPIO_Pin_15
//#define Out3_OFF()		GPIO_ResetBits(GPIOB,GPIO_Pin_12)
//#define Out3_ON()		GPIO_SetBits(GPIOB,GPIO_Pin_12)
//#define Out3_Toggle()	GPIOB->ODR ^= GPIO_Pin_12


typedef struct
{
    uint8_t temp_state;                 /*  */
	uint8_t stab_state;			        /*  */
	uint8_t study_stab_state;           /*  */
	uint8_t lamp_chge_counter;          /*  */
    uint8_t study_chge_counter;         /*  */
    uint16_t current_study_counter;     /*  */
    uint16_t previous_study_counter;    /*  */
    uint8_t data;
}IO_TypeDef;

typedef struct  //Variables para guardar los datos de GPS obtenidos
{
    RtcType utc;
    RtcType local;
    char time_str[16];//UTCtime
    char year_str[8];
    char month_str[8];
    char day_str[8];
	char Latitude[16];
	char NS[2];
	char Longitude[16];
	char EW[2];
    char FS[2];
    char numSv[4];
	char Altitude[4];
	char Speed[8];
}GpsType; //revisar a donde moverla

typedef struct
{
    uint8_t data;
	uint8_t timeUpdate;
	
	uint32_t seconds;
    uint32_t gps_seconds;
	uint32_t gps1ms;
	GpsType	Gps;
	uint8_t gps_flag;
	uint8_t gps_count;
	uint8_t sync_with_gps_flag;
	
	uint8_t  pps_flag;
	uint16_t pps_1ms;
	uint16_t pps_count;

	RtcType	rtc_bcd;
	RtcType	rtc_dec;
	uint8_t TimeZone[4];
}SYSTEM_TEMP_TypeDef; //Revisar si debe estar qui

typedef struct
{
    uint8_t temp_state;                 /*  */
	uint8_t stab_state;			        /*  */
	uint8_t study_stab_state;           /*  */
	uint8_t lamp_chge_counter;          /*  */
    uint8_t study_chge_counter;         /*  */
    uint16_t current_study_counter;     /*  */
    uint16_t previous_study_counter;    /*  */
    uint8_t data;
}FLASHER_TypeDef; //revisar si debe estar aqui


extern SYSTEM_TEMP_TypeDef system_temp;
extern uint8_t spi2_busy_flag;
extern uint8_t rf_int_flag;


void bsp_GpioInit(void);
void pps_irq_init(void);

#endif 
