#include "typedef.h"


#ifndef __DISPLAY_H__
#define __DISPLAY_H__

#define DISP_TICK      SYSTEM_TICK

#define LCD_A   BIT0
#define LCD_B   BIT1
#define LCD_C   BIT2
#define LCD_D   BIT3
#define LCD_E   BIT4
#define LCD_F   BIT5
#define LCD_G   BIT6
#define LCD_DP  BIT7

#define LCD_C_H   (LCD_B | LCD_C | LCD_E | LCD_F | LCD_G)
#define LCD_C_i   (LCD_E)
#define LCD_C_L   (LCD_D | LCD_E | LCD_F)
#define LCD_C_o   (LCD_C | LCD_D | LCD_E | LCD_G)
#define LCD_C_h   (LCD_C | LCD_E | LCD_F | LCD_G)
#define LCD_C_C   (LCD_A | LCD_D | LCD_E | LCD_F)
#define LCD_C_E   (LCD_A | LCD_D | LCD_E | LCD_F | LCD_G)


#define LED_W_ON()    (PD_ODR |=  BIT4)
#define LED_W_OFF()   (PD_ODR &= ~BIT4)
#define LED_B_ON()    (PD_ODR |=  BIT5)
#define LED_B_OFF()   (PD_ODR &= ~BIT5)
#define LED_R_ON()    (PD_ODR |=  BIT6)
#define LED_R_OFF()   (PD_ODR &= ~BIT6)


#define UpdateDisplay()	 (disp_update_flag = 1)
#define RefreshDispDataToDispMap(_map)\
				{\
					_led_disp_map[0] = _map[0];\
					_led_disp_map[1] = _map[1];\
					_led_disp_map[2] = _map[2];\
				}

typedef void (DISP_CALLBACK)(void);
typedef void (DISP_TICK_CALLBACK)(void);

extern bool disp_update_flag;
extern const u8 _led_num_tab[10];

/*
	b00->A2  b10->A3  b20->A4
	b01->B2  b11->B3  b21->B4
	b02->C2  b12->C3  b22->C4
	b03->D2  b13->D3  b23->D4
	b04->E2  b14->E3  b24->E4
	b05->F2  b15->F3  b25->F4
	b06->G2  b16->G3  b26->G4
	b07->B1  b17->C1  b27->DP
*/
extern @tiny _led_disp_map[3];

void RefreshDisplay(void);
void SetDispCallback(DISP_CALLBACK * pDisp, 
                            DISP_TICK_CALLBACK * pDispTick);
bool IsDisplay(DISP_CALLBACK * pDisp);

void DisplayEnterSleep(void);
void DisplayExitSleep(void);

void DisplayInitPoweron(void);

#endif