#ifndef __IO_FUNCTIONS_H
#define __IO_FUNCTIONS_H

#include "stm32f10x.h"
#include "bsp_io.h" /* Inherit system structs like IO_TypeDef and system_temp */

#define DEBUG 9

//#define green_flash_sound_en	0
/***********************************************************************************/
#define STUDY_MODE              0   // Define el modo de operacion STUDY_MODE.
#define FIRST_RED_LAMP_VALID    1
/***********************************************************************************/
#define FILTER_ON_TIME          6	// Normal vino con 6, pero para pruebas de funcionamiento se ha cambiado
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

/* Global state variables exposed to main.c */
extern IO_TypeDef xin;
extern IO_TypeDef ain;
extern uint8_t lamp_state[2];
extern uint8_t lamp_status;
extern uint16_t lamp_chge_flag;
extern uint8_t gre_flash_flag;
extern uint8_t gre_off;

extern uint16_t display_data[2];
extern uint8_t study_lamp_stab_state;

//extern uint8_t green_flash_sound_en; //REvisar si va aqui /* Line 41: Keep this extern declaration */

/* Function Prototypes */
uint16_t get_dir(void);
void down_time_display(void);
void ain_filterAC_DC(void);
void filterAC_DC(void);
void study_mode_filterAC_DC(void);
void study_mode_time_calculation(void);//10ms yici
void workmodejudge(void);
void flash_panel_control(void);

//void Auto_adjust_time(void);

#endif /* __IO_FUNCTIONS_H */
