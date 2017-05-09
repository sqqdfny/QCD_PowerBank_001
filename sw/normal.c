#include "normal.h"
#include "key.h"
#include "adc.h"
#include "display.h"
#include "tp5602.h"

static u8 _second_count;

typedef enum
{
	LED_STATE_OFF = 0,      //¹Ø±Õ
	LED_STATE_W_ON,         //¿ª°×µÆ
	LED_STATE_R_ON,         //¿ªºìµÆ
	LED_STATE_B_ON,         //¿ªÀ¶µÆ
	LED_STATE_R_B_SWITCH,   //ºìÀ¶µÆÇÐ»»
	LED_STATE_MAX = LED_STATE_R_B_SWITCH
}enumLedState;

static enumLedState led_state = LED_STATE_OFF;
//==========================================================

//==========================================================

//==========================================================

//==========================================================
static void KeyOnOff(enumKeyStatus status)
{
	if((false == IsConnectedInputPower()) || (false == IsBoostOut()))
	{
		Tp5602KeyPress();
		return ;
	}
	
	if(led_state == LED_STATE_OFF)
	{
		if(KEY_STATUS_DOUBLE == status)
		{
			led_state = LED_STATE_W_ON;
			LED_W_ON();
		}
		return;
	}
	
	
	{
		if(KEY_STATUS_PRESS == status)
		{
			if(++led_state > LED_STATE_MAX)
			{
				led_state = LED_STATE_W_ON;
			}
			LED_W_OFF();
			LED_R_OFF();
			LED_B_OFF();
			switch(led_state)
			{
				case LED_STATE_W_ON:         //¿ª°×µÆ
					LED_W_ON();
					break;
				case LED_STATE_R_ON:         //¿ªºìµÆ
					LED_R_ON();
					break;
				case LED_STATE_B_ON:         //¿ªÀ¶µÆ
				  LED_B_ON();
					break;
				case LED_STATE_R_B_SWITCH:   //ºìÀ¶µÆÇÐ»» 
					LED_R_ON();
					LED_B_ON();
					break;
				default:;
			}
		}
		else
		if(KEY_STATUS_DOUBLE == status)
		{
			led_state = LED_STATE_OFF;
			LED_W_OFF();
			LED_R_OFF();
			LED_B_OFF();
		}
	}
}

static char Key(enumKeyValue value, enumKeyStatus status, u8 second)
{
	u8 flag = 0;
	switch(value)
  {
		case KEY_VALUE_ON_OFF  :
 		  KeyOnOff(status);
 			break;
 	}
  return (flag);
}
//==========================================================

//==========================================================
//call per system_tick (10ms)
void TimeHook(void)
{
	if(++ _second_count > (SYSTEM_TICK - 1)) 
	{
		_second_count = 0;
		
		UpdateDisplay();
	}
}

void Display(void)
{
	u8 disp_map[3];
	u8 tmp = GetBattVoltage();
	disp_map[0] = _led_num_tab[0]; //0xff; //
	disp_map[1] = _led_num_tab[tmp / 10]; //0xff; //
	disp_map[2] = _led_num_tab[tmp % 10] | LCD_DP; //0xff; // 
	RefreshDispDataToDispMap(disp_map);
}

void EnterNormal(void)
{
	_second_count = 0;
 	SetKeyCallback(Key);
 	SetDispCallback(Display, TimeHook);
 	UpdateDisplay();
}
//==========================================================
//end files

