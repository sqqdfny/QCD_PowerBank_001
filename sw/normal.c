#include "normal.h"
#include "key.h"
#include "eeprom.h"
#include "buzzer.h"
#include "adc.h"
#include "setup1.h"
#include "uart.h"



#define MAX_TEMP_SET       110
#define MIN_TEMP_SET       20
#define DEFAULT_TEMP_SET   55

#define MAX_TIME_SET       999
#define MIN_TIME_SET       0
#define DEFAULT_TIME_SET   60


#define _WaterInOn()       (PD_ODR |=  BIT2)
#define WaterInOff()       (PD_ODR &= ~BIT2)
#define GetWaterIn()       (PD_IDR & BIT2)

//5分钟水没排完,认为是排水故障
#define WaterOutOn()       {PC_ODR |=  BIT7;_water_out_second_err = 60 * 5;}
#define WaterOutOff()      (PC_ODR &= ~BIT7)
#define GetWaterOut()      (PC_IDR & BIT7)


typedef enum
{
	WATER_OUT_NO 		 = 0,
	WATER_OUT_FIRST  = 1,
	WATER_OUT_IN     = 2,
	WATER_OUT_SECOND = 3,
	WATER_OUT_ERR    = 4
}enumWaterOut;

@tiny u16 cur_time_minute;
@tiny u16 cur_time_minute_set_rom;
static @tiny u16 cur_time_minute_set;

@tiny u8 cur_temp;
@tiny u8 cur_temp_set;

_bool _on_off_flag;    //0:关机        1:开机
_bool _is_top_flag;    //0:无面板      1:有面板
_bool _run_flag;       //0:加热关      1:加热开
_bool _heat_flag;      //0:不需要加热  1:需要加热 

static _bool _water_in_first;
static @tiny u16 _water_in_second;

static @tiny enumWaterOut _water_out_flag;
static @tiny u16 _water_out_second;         //关机后排水前的等待时间
static @tiny u16 _water_out_second_err;     //排水超时

static @tiny u8 _heat_dly_second;
static @tiny u8 _second_count;
static @tiny u8 _minute_count;
//==========================================================
static u16 CalculateTimeSet(u8 _switch)
{
	u8 tmp = 0;
	
	if(_switch & BIT0) tmp |= BIT3;
	if(_switch & BIT1) tmp |= BIT2;
	if(_switch & BIT2) tmp |= BIT1;
	if(_switch & BIT3) tmp |= BIT0;
	return ((u16)tmp * 30);
}

//当前水位
u8 GetCurWaterLevel(void) 
{
	/*u8 result = 0;
	u8 tmp8 = ((~(GetCurHc165Map() >> 1)) & 0x7);
	
	if(tmp8 & BIT2) result |= BIT0;
	if(tmp8 & BIT1) result |= BIT1;
	if(tmp8 & BIT0) result |= BIT2;*/
	
	return ((~(GetCurHc165Map() >> 1)) & 0x7);
}
//==========================================================
//flag: 0.主板按键开机    1.串口命令开机
static void Start(void)
{
	if(_temp_sensor_state) return; //温度传感器故障
	
	StatusOutOn();
	WaterOutOff();  //如果正在排水
	
	//开机时检测是否连接面板
	if(_is_top_flag)
	{//有面板
		cur_time_minute_set = cur_time_minute_set_rom;
	}
	else
	{//没有面板
		cur_time_minute_set = CalculateTimeSet((GetCurHc165Map() >> 4) & 0xf);
	}
	
	_minute_count = 0;
	cur_time_minute = cur_time_minute_set;
	_water_in_first = 1;  //开机将水加满
	_water_out_flag = WATER_OUT_NO;
	_run_flag = 1;
	
	
	if(cur_temp < cur_temp_set)
	{
		_heat_flag = 1;
	}
	HostSendStateReq();
}

static void Stop(void)
{
	_run_flag = 0;
	cur_time_minute = cur_time_minute_set;   //关蒸气后,显示设置时间 20151216
	_water_out_flag = WATER_OUT_FIRST;    
	_water_out_second = 8 * 60;  //关机延时30分钟之后开始排水
	WaterInOff();                 //如果正在进水
	StatusOutOff();
	
	HostSendStateReq();
}

static void StopForTimer(void)
{
	Stop();
}

void StopForLinkErr(void)
{
	Stop();
}
//警报关机
static void StopForAlarm(void)
{
	Heat1Off();
	Heat2Off();
	Heat3Off();
	//_run_flag = 0;
	//_water_out_flag = WATER_OUT_FIRST;    
	//_water_out_second = 10 * 60;  //关机延时10分钟之后开始排水
	//WaterInOff();                 //如果正在进水
	//StatusOutOff();
}
//==========================================================
//flag
//0:关机 
//1:开机
void SystemOnOff(_bool flag)
{
	if(_on_off_flag == flag) return;
	_on_off_flag = flag;
	if(_on_off_flag)
	{
		LightOn();
	}
	else
	{
		if(_run_flag) Stop();
		LightOff();
		LedOff();
	}
	HostSendStateReq();
}

//flag
//0:关蒸气 
//1:开蒸气
void SteamOnOff(_bool flag)
{
	if(flag)
	{//主机为关机状态,不响应开蒸气指令
		if(!_on_off_flag) return;
	}
	
	if(_run_flag == flag) return;
	
	_run_flag = flag;
	if(_run_flag)
	{
		Start();
	}
	else
	{
		Stop();
	}
	HostSendStateReq();
}

//flag
//0:关灯
//1:开灯
void LightOnOff(_bool flag)
{
	if(flag)
	{//主机为关机状态,不响应开灯指令
		if(!_on_off_flag) return;
	}
	
	if(flag) LightOn();
	else     LightOff();
	
	HostSendStateReq();
}

//flag
//0:关LED
//1:开LED 
void LedOnOff(_bool flag)
{
	if(flag)
	{//主机为关机状态,不响应开LED指令
		if(!_on_off_flag) return;
	}
	
	if(flag) LedOn();
	else     LedOff();
	
	HostSendStateReq();
}
//==========================================================
//定时工作
//call in TimeHook() per minute once
static void HeatTimer(void)
{
	if(_run_flag)
	{//定时关机
		if(cur_time_minute_set)
		{//定时关机时间为0时,无定时关机功能
			if(cur_time_minute)
			{
				cur_time_minute --;
				if(0 == cur_time_minute)
				{
					StopForTimer();
				}
			}
		}
	}
	//else
	//{//预约开机
	//}
}
//==========================================================

//==========================================================
//进水控制
void WaterInOn(void)
{
	_WaterInOn();
	_water_in_second = 0;
}
//call in TimeHook() per second once
static void WaterInControl(void)
{
	u8 _water_level;
	if(0 == _on_off_flag)
	{//如果没有开机(不是开蒸汽),不进水
		return;
	}
	if(0 == GetWaterOut())
	{//当前未处于排水状态
		_water_level = GetCurWaterLevel();
		if(GetWaterIn())
		{//正在进水
			if(0x07 == _water_level)
			{//水满
				WaterInOff();
				if(_water_in_first)
				{//第一次进水
					_water_in_first = 0;
				}
			}
			else
			{
				if(++_water_in_second > 600)
				{//从开始进水超过10分钟还未进满,关机
					Stop();
				}
			}
		}
		else
		{
			if(_water_level < 0x07)
			{//只要水位没满就进水
				WaterInOn();
			}
		}
	}
}

//排水控制
//call in TimeHook() per second once
static void WaterOutControl(void)
{
	u8 _water_level;
	if(_run_flag) return;  //关机状态下才排水
	if(_water_out_flag && (_water_out_flag < WATER_OUT_ERR))
	{
		if(_water_out_second) 
		{
			_water_out_second --;
			return;
		}
		
		_water_level = GetCurWaterLevel();
		if(WATER_OUT_IN == _water_out_flag) 
		{//正在冲洗,等待水满
			if(_water_level == 0x07)
			{
				_water_out_flag = WATER_OUT_SECOND;
			}
		}
		else
		{
			if(_water_level > 0 && 0 == GetWaterOut())
			{//当前有水,并且未处于排水状态
				WaterOutOn();
			}
			else
			{
				if(0 == _water_level)
				{//水已排完
					WaterOutOff(); 
					if(WATER_OUT_FIRST == _water_out_flag)
					{//第一次排水
						_water_out_flag = WATER_OUT_IN;
					}
					else
					{
						_water_out_flag = WATER_OUT_NO;
					}
				}
			}
		}
		
		if(GetWaterOut())
		{//正在排水
			if(_water_out_second_err) 
			{
				_water_out_second_err --;
			}
			else
			{//排水超时
				_water_out_flag = WATER_OUT_ERR;
				WaterOutOff(); 
			}
		}
	}
}
//==========================================================
//call in TimeHook() per second once
static void HeatOutCOntrol(void)
{
	u8 _water_level;
	
	if(_run_flag)
	{
		_water_level = GetCurWaterLevel();
		
		if((_water_level & 0x03) < 0x03)
		{//缺水/高水位有水低水位有一条没水,紧急停机
			StopForAlarm();
			return;
		}
	}

	if(_heat_dly_second)
	{
		if((-- _heat_dly_second))
		{
			return;
		}
	}
	_heat_dly_second = 2;
	
	if( _run_flag && _heat_flag )
	{//开机,并且需要加热
		if(_water_in_first)
		{
			if(_water_level < 0x7) return; //等水加满才开始加热
		}
		
		if(0 == GetHeat1())
		{
			Heat1On();
			return;
		}
		
		if(0 == GetHeat2())
		{
			Heat2On();
			return;
		}
		
		if(0 == GetHeat3())
		{
			Heat3On();
			return;
		}
	}
	else
	{
		if(GetHeat1())
		{
			Heat1Off();
			return;
		}
		
		if(GetHeat2())
		{
			Heat2Off();
			return;
		}
		
		if(GetHeat3())
		{
			Heat3Off();
			return;
		}
	}
}
//==========================================================
static void NormalKeyOnOff(enumKeyStatus status)
{
  if(KEY_STATUS_PRESS == status)
	{
		if(_run_flag)
		{
		  _on_off_flag = 0;
			Stop();
			LightOff();
			LedOff();
		}
		else
		{
			_on_off_flag = 1;
			LightOn();
			Start();
		}
  }
}

static char Key (enumKeyValue value, enumKeyStatus status, u8 second)
{
	u8 flag = 0;
	switch(value)
  {
		case KEY_VALUE_ON_OFF  :
 		  NormalKeyOnOff(status);
 			break;
 	}
  return (flag);
}

static void CheckHeatFlag(void)
{
	if(_run_flag & (0 == _temp_sensor_state))
	{
		if(_heat_flag) 
		{
			if(cur_temp > cur_temp_set)
			{
				_heat_flag = 0;
			}
		}
		else
		{
			if((cur_temp + TEMP_DIFF) < cur_temp_set)
			{
				_heat_flag = 1;
			}
		}
	}
	else
	{
		_heat_flag = 0;
	}
}

//call per system_tick (10ms)
void NormalTimeHook(void)
{
	if(++ _second_count > (SYSTEM_TICK - 1)) 
	{
		_second_count = 0;
		CheckHeatFlag();
		WaterInControl();
		WaterOutControl();
		HeatOutCOntrol();
		if(++ _minute_count > 59)
		{
			_minute_count = 0;
			HeatTimer();
		}
	}
}

//保存串口命令设置的温度和时间
void SaveTempTimeForUart(u16 minute, u8 temp)
{
	if(cur_time_minute_set_rom != minute)
	{
		cur_time_minute_set_rom = minute;
		EepromProgramWord((u8*)TIME_SET_ADDR, cur_time_minute_set_rom);
		cur_time_minute_set = cur_time_minute_set_rom;  //20160828
	}
	
	if(cur_temp_set != temp)
	{
		cur_temp_set = temp;
		EepromProgramByte((u8*)TEMP_SET_ADDR, cur_temp_set);
	}
	
	if((cur_time_minute > cur_time_minute_set_rom) || (0 == _run_flag))
	{//当前为关机状态(20160828),或者当前值大于设置值
		cur_time_minute = cur_time_minute_set_rom;
	}
	
	/*if(cur_temp < cur_temp_set)
	{
		_heat_flag = 1;
	}*/
	
	HostSendStateReq();
}

void NormalInit(void)
{
	//out 1 ~ 7
	PA_ODR &= ~(BIT1 | BIT2);
	PD_ODR &= ~(BIT2);
	PC_ODR &= ~(BIT7 | BIT6 | BIT5 | BIT4);
	PA_DDR |=  (BIT1 | BIT2);
	PA_CR1 |=  (BIT1 | BIT2);
	PA_CR2 &= ~(BIT1 | BIT2);
	PD_DDR |=  (BIT2);
	PD_CR1 |=  (BIT2);
	PD_CR2 &= ~(BIT2);
	PC_DDR |=  (BIT7 | BIT6 | BIT5 | BIT4);
	PC_CR1 |=  (BIT7 | BIT6 | BIT5 | BIT4);
	PC_CR2 &= ~(BIT7 | BIT6 | BIT5 | BIT4);
	
	PA_DDR |=  (BIT3);
	PA_CR1 |=  (BIT3);
	PA_CR2 &= ~(BIT3);
	StatusOutOff();
	
	_on_off_flag = 0;
	_run_flag = 0;
	_is_top_flag = 0;
	_heat_flag = 0;
	_water_in_first = 1;
	_water_out_flag = WATER_OUT_NO;
	_heat_dly_second = 0;
	_second_count = 0;
	_minute_count = 0;
	
	cur_time_minute_set = CalculateTimeSet((GetCurHc165Map() >> 4) & 0xf);
	cur_time_minute_set_rom = EepromReadWord((u8*)TIME_SET_ADDR);
	if(cur_time_minute_set_rom > MAX_TIME_SET)
	{
		cur_time_minute_set_rom = DEFAULT_TIME_SET;
	}
	cur_time_minute = cur_time_minute_set_rom;
	
	cur_temp_set = EepromReadByte((u8*)TEMP_SET_ADDR);
	if((cur_temp_set > MAX_TEMP_SET) || (cur_temp_set < MIN_TEMP_SET))
	{
		cur_temp_set = DEFAULT_TEMP_SET;
	}
	cur_temp = 25;
	
	SetKeyCallback(Key);
}
//==========================================================
//end files

