#include "../inc/normal.h"
#include "../inc/key.h"
#include "../inc/adc.h"
#include "../inc/display.h"
#include "../inc/tp5602.h"
#include "../inc/main.h"


#define LED_R_B_SWITCH_DELAY     10   /*ÉÁË¸¼ä¸ô*/
#define LED_R_B_SWITCH_INTERVAL  30   /*ºìÀ¶½»Ìæ¼ä¸ô*/


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

static u8 _second_count;
static u8 show_voltage_second;
static bool show_input_voltage_flag;
//==========================================================
bool IsLedStateOn(void)
{
	return (led_state > LED_STATE_OFF ? true : false);
}
//==========================================================

//==========================================================
//ºìÀ¶ÉÁË¸¿ØÖÆ
static u8 led_r_b_switch_delay;
static u8 led_r_b_switch_index;
static void Led_R_B_Switch(void)
{
	if(LED_STATE_R_B_SWITCH == led_state)
	{
		if(led_r_b_switch_delay --) 
		{
			return;
		}
		
		switch(led_r_b_switch_index)
		{
			case 0:
				LED_R_ON();
				led_r_b_switch_delay = LED_R_B_SWITCH_DELAY;
				led_r_b_switch_index = 1;
				break;
			case 1:
				LED_R_OFF();
				led_r_b_switch_delay = LED_R_B_SWITCH_DELAY;
				led_r_b_switch_index = 2;
			  break;
			case 2:
			  LED_R_ON();
				led_r_b_switch_delay = LED_R_B_SWITCH_DELAY;
				led_r_b_switch_index = 3;
				break;
			case 3:
			  LED_R_OFF();
				led_r_b_switch_delay = LED_R_B_SWITCH_DELAY;
				led_r_b_switch_index = 4;
				break;
			case 4:
			  LED_R_ON();
				led_r_b_switch_delay = LED_R_B_SWITCH_DELAY;
				led_r_b_switch_index = 5;
				break;
			case 5:
			  LED_R_OFF();
				led_r_b_switch_delay = LED_R_B_SWITCH_INTERVAL;
				led_r_b_switch_index = 6;
				break;
				
			case 6:
				LED_B_ON();
				led_r_b_switch_delay = LED_R_B_SWITCH_DELAY;
				led_r_b_switch_index = 7;
				break;
			case 7:
				LED_B_OFF();
				led_r_b_switch_delay = LED_R_B_SWITCH_DELAY;
				led_r_b_switch_index = 8;
			  break;
			case 8:
			  LED_B_ON();
				led_r_b_switch_delay = LED_R_B_SWITCH_DELAY;
				led_r_b_switch_index = 9;
				break;
			case 9:
			  LED_B_OFF();
				led_r_b_switch_delay = LED_R_B_SWITCH_DELAY;
				led_r_b_switch_index = 10;
				break;
			case 10:
			  LED_B_ON();
				led_r_b_switch_delay = LED_R_B_SWITCH_DELAY;
				led_r_b_switch_index = 11;
				break;
			case 11:
			  LED_B_OFF();
				led_r_b_switch_delay = LED_R_B_SWITCH_INTERVAL;
				led_r_b_switch_index = 0;
				break;
			default:
			{
				LED_R_OFF();
				LED_B_OFF();
			  led_r_b_switch_index = 0;
				led_r_b_switch_delay = 0;
			}
		}
	}
}
//==========================================================

//==========================================================
static void KeyOnOff(enumKeyStatus status)
{
	if((false == IsConnectedInputPower()) \
	&& (false == IsBoostOut()))
	{
		if(KEY_STATUS_PRESS == status)
		{
			Tp5602KeyPress();
		}
	}
	
	if(false == IsLedStateOn())
	{
		if(KEY_STATUS_DOUBLE == status)
		{
			led_state = LED_STATE_W_ON;
			LED_W_ON();
		}
	}
	else
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
					LED_R_OFF();
					LED_B_OFF();
					led_r_b_switch_index = 0;
					led_r_b_switch_delay = 0;
					break;
				default:
				{
					led_state = LED_STATE_OFF;
					LED_W_OFF();
					LED_R_OFF();
					LED_B_OFF();
				}
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
static void TimeHook(void)
{
	if((false == IsConnectedInputPower()) \
	&& (false == IsBoostOut()) \
	&& (false == IsLedStateOn()))
	{//½øÈë´ý»úÄ£Ê½
		EnterSleep();
		return ;
	}
		
	if(++ _second_count > (SYSTEM_TICK - 1)) 
	{
		_second_count = 0;
		
		if(IsConnectedInputPower())
		{
			if(++ show_voltage_second > 3)
			{
				show_voltage_second = 0;
				show_input_voltage_flag = !show_input_voltage_flag;
			}
		}
		UpdateDisplay();
	}
	
	Led_R_B_Switch();
}

static void Display(void)
{
	u8 disp_map[3];
	u16 tmp;
	
	if(IsConnectedInputPower() && show_input_voltage_flag)
	{
		tmp	= GetInputVoltage();
		disp_map[0] = _led_num_tab[tmp / 100];
		tmp = tmp % 100;
		disp_map[1] = _led_num_tab[tmp / 10];
		disp_map[2] = _led_num_tab[tmp % 10] | LCD_DP; 
	}
	else
	{
		tmp	= GetBattVoltage();
		disp_map[0] = _led_num_tab[tmp / 100]; 
		tmp = tmp % 100;
		disp_map[1] = _led_num_tab[tmp / 10]; 
		disp_map[2] = _led_num_tab[tmp % 10];
	}
	RefreshDispDataToDispMap(disp_map);
}

void EnterNormal(void)
{
	_second_count = 0;
	show_voltage_second = 0;
	show_input_voltage_flag = false;
 	SetKeyCallback(Key);
 	SetDispCallback(Display, TimeHook);
 	UpdateDisplay();
}

bool IsDispNormal(void)
{
	return (IsDisplay(Display));
}
//==========================================================
//end files

