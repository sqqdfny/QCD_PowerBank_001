#include "typedef.h"


#ifndef __NORMAL_H__
#define __NORMAL_H__

#define TEMP_ZERO      40
#define TEMP_DIFF      2

#define LightOn()          (PA_ODR |=  BIT1)
#define LightOff()         (PA_ODR &= ~BIT1)
#define GetLight()         (PA_IDR & BIT1)

#define LedOn()            (PC_ODR |=  BIT6)
#define LedOff()           (PC_ODR &= ~BIT6)
#define GetLed()           (PC_IDR & BIT6)

#define Heat1On()          (PC_ODR |=  BIT5)
#define Heat1Off()         (PC_ODR &= ~BIT5)
#define Heat2On()          (PC_ODR |=  BIT4)
#define Heat2Off()         (PC_ODR &= ~BIT4)
#define Heat3On()          (PA_ODR |=  BIT2)
#define Heat3Off()         (PA_ODR &= ~BIT2)
#define GetHeat1()         (PC_IDR & BIT5)
#define GetHeat2()         (PC_IDR & BIT4)
#define GetHeat3()         (PA_IDR & BIT2)

#define StatusOutOn()      (PA_ODR |=  BIT3)
#define StatusOutOff()     (PA_ODR &= ~BIT3)

#define GetWaterLevel_L()  (GetCurWaterLevel() & BIT0)
#define GetWaterLevel_M()  (GetCurWaterLevel() & BIT1)
#define GetWaterLevel_H()  (GetCurWaterLevel() & BIT2)

extern @tiny u16 cur_time_minute;
extern @tiny u16 cur_time_minute_set_rom;
extern @tiny u8 cur_temp;
extern @tiny u8 cur_temp_set;

extern _bool _on_off_flag;    //0:关机        1:开机
extern _bool _is_top_flag;    //0:无面板      1:有面板
extern _bool _run_flag;       //0:加热关      1:加热开
extern _bool _heat_flag;      //0:不需要加热  1:需要加热 

u8 GetCurWaterLevel(void);

void SystemOnOff(_bool flag);
void SteamOnOff(_bool flag);
void LightOnOff(_bool flag);
void LedOnOff(_bool flag);
void StopForLinkErr(void);
void NormalTimeHook(void);
void SaveTempTimeForUart(u16 minute, u8 temp);
void NormalInit(void);

#endif
//==========================================================
//end files
