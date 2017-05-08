#include "typedef.h"


#ifndef __DISPLAY_H__
#define __DISPLAY_H__

#define LCD_A   0x02
#define LCD_B   0x08
#define LCD_C   0x40
#define LCD_D   0x20
#define LCD_E   0x10
#define LCD_F   0x04
#define LCD_G   0x80
#define LCD_DP  0x01

#define LCD_C_H   (LCD_B | LCD_C | LCD_E | LCD_F | LCD_G)
#define LCD_C_i   (LCD_E)
#define LCD_C_L   (LCD_D | LCD_E | LCD_F)
#define LCD_C_o   (LCD_C | LCD_D | LCD_E | LCD_G)
#define LCD_C_h   (LCD_C | LCD_E | LCD_F | LCD_G)
#define LCD_C_C   (LCD_A | LCD_D | LCD_E | LCD_F)
#define LCD_C_E   (LCD_A | LCD_D | LCD_E | LCD_F | LCD_G)

#define LCD_MAP_OFF 0

#define UpdateDisplay()	 (led_update_flag = 1)
#define CopyDispMap(_map)\
				{\
					_led_disp_map[0] = _map[0];\
					_led_disp_map[1] = _map[1];\
					_led_disp_map[2] = _map[2];\
				}

typedef void (DISP_CALLBACK)(void);
typedef void (DISP_TICK_CALLBACK)(void);

extern _bool led_update_flag;
extern const u8 _led_num_tab[10];
extern @tiny _led_disp_map[3];

extern void RefreshLed(void);
extern void SetDispCallback(DISP_CALLBACK * pDisp, 
                            DISP_TICK_CALLBACK * pDispTick);
extern _bool IsDisplay(DISP_CALLBACK * pDisp);

void DisplayInitPoweron(void);

#endif