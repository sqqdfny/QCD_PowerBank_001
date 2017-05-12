#include "standby.h"
#include "key.h"
#include "display.h"
#include "tp5602.h"
#include "main.h"
//==========================================================

//==========================================================
static void KeyOnOff(enumKeyStatus status)
{
	if(KEY_STATUS_PRESS == status)
	{
		Tp5602KeyPress();
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
	if((IsConnectedInputPower()) \
	|| (IsBoostOut()))
	{
		ExitSleep();
	}
}

static void Display(void)
{
	
}

void EnterStandby(void)
{
 	SetKeyCallback(Key);
 	SetDispCallback(Display, TimeHook);
 	UpdateDisplay();
}
//==========================================================
//end files

