#ifndef __BSP_IO_H
#define __BSP_IO_H
#include <stdint.h>

#define DEBUG 9

#define green_flash_sound_en	0
/***********************************************************************************/
#define STUDY_MODE              0   // Define el modo de operacion STUDY_MODE.
#define FIRST_RED_LAMP_VALID    1
/***********************************************************************************/
#define FILTER_ON_TIME          6
#define FILTER_OFF_TIME         21
#define DEVIATION_TIME          300
#define AHEAD_END_TIME          2 
#define LOST_DELAY_TIME         3
/***********************************************************************************/





#define LongCycleTime       30000
#define RedCycleTime        800//
#define GreCycleTime        3//10
#define GreFlashCycleTime   3//800 3

#define BS  0
#define RS  1
#define GS  2

#define SW4()   		(GPIOC->IDR & 0x0008)

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

#define ADC_ON()        GPIO_ResetBits(GPIOA,GPIO_Pin_12)
#define ADC_OFF()       GPIO_SetBits(GPIOA,GPIO_Pin_12)

#define RELAY_OFF()     GPIO_ResetBits(GPIOB,GPIO_Pin_9)
#define RELAY_ON()      GPIO_SetBits(GPIOB,GPIO_Pin_9)

#define GPS_ON()        GPIO_SetBits(GPIOA,GPIO_Pin_0)   //salida que controla pin para encender o apagar modulo GPS
#define GPS_OFF()       GPIO_ResetBits(GPIOA,GPIO_Pin_0)
#define PPS_IN()   		(GPIOA->IDR & 0x0002)

#define PA_ON()       	GPIO_SetBits(GPIOA,GPIO_Pin_11)
#define PA_OFF()     	GPIO_ResetBits(GPIOA,GPIO_Pin_11)

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
}GpsType;


typedef struct
{
    uint8_t data;
	uint8_t timeUpdate;
	
	uint32_t            seconds;
    uint32_t            gps_seconds;
	uint32_t		gps1ms;
	GpsType	Gps;
	uint8_t gps_flag;
	uint8_t gps_count;
	uint8_t sync_with_gps_flag;
	
	uint8_t  pps_flag;
	uint16_t pps_1ms;
	uint16_t pps_count;

	
	RtcType		rtc_bcd;
	RtcType		rtc_dec;
	uint8_t TimeZone[4];
}SYSTEM_TEMP_TypeDef;


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
}FLASHER_TypeDef;



extern SYSTEM_TEMP_TypeDef system_temp;
extern uint8_t spi2_busy_flag;
extern uint8_t rf_int_flag;

extern IO_TypeDef xin;
extern IO_TypeDef ain;
extern uint8_t lamp_state[2];
extern uint8_t lamp_status;
extern uint16_t lamp_chge_flag;
extern uint8_t gre_flash_flag;
extern uint8_t gre_off;

uint16_t get_dir(void);

void bsp_GpioInit(void);

void down_time_display(void);
void ain_filterAC_DC(void);
void filterAC_DC(void);
void study_mode_filterAC_DC(void);
void study_mode_time_calculation(void);//10ms yici
void workmodejudge(void);
void flash_panel_control(void);

void Auto_adjust_time(void);

void pps_irq_init(void);

#endif 
