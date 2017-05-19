#include "../inc/tp5602.h"
#include "../inc/adc.h"
#include "../inc/main.h"


static u16 curBattVoltage;
static u8 curBattCapacity;
static u8 curInputVoltage;

static u8 second_count;
static bool tp5602_key_press_flag;
//==========================================================
//电池电压
u16 GetBattVoltage(void)
{
	return (curBattVoltage);
}

//电池电量
u8 GetBattCapacity(void)
{
	return (curBattCapacity);
}

//车辆电瓶电压
u8 GetInputVoltage(void)
{
	return (curInputVoltage);
}

bool IsConnectedInputPower(void)
{
	return ((curInputVoltage > 90) ? true : false);
}

bool IsBoostOut(void)
{
	return ((P17) ? true : false);
}
//==========================================================
//==========================================================
void Tp5602KeyPress(void)
{
	P30=1;
	tp5602_key_press_flag = false;
}
//==========================================================
//计算电池当前剩余电量
static void CalculationCurBattCapacity(void)
{
	
}
//==========================================================
//在主循环中每 10ms 调用一次
void Tp5602Funtion(void)
{
	if(P30)
	{
		if(false == tp5602_key_press_flag)
		{
			tp5602_key_press_flag = true;
		}
		else
		{
			P30=0;
		}
	}
	
	if((second_count ++) > 10)
	{
		second_count = 0;
		curBattVoltage = (u16)(((u32)GetAdc(ADC_CH_BATT)*180)/GetAdc(ADC_CH_REF));
		CalculationCurBattCapacity();
		
		curInputVoltage = (u16)(((u32)GetAdc(ADC_CH_INPUT)*72)/GetAdc(ADC_CH_REF))+8;
	}
}
//==========================================================
void TP5602InitPoweron(void)
{
	clr_P3M1_0;set_P3M2_0;
	clr_P30;
	set_P1M1_7;clr_P1M2_7;

	
	curBattVoltage = 0;
	curBattCapacity = 0;
	curInputVoltage = 0;
	second_count = 0xfe;
}
//==========================================================
//end files
