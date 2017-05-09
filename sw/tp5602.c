#include "tp5602.h"
#include "adc.h"


static u8 curBattVoltage;
static u8 curBattCapacity;
static u8 curInputVoltage;

static u8 second_count;
static bool tp5602_key_press_flag;
//==========================================================
//电池电压
u8 GetBattVoltage(void)
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
	return ((PA_IDR & BIT2) ? true : false);
}
//==========================================================
void Tp5602KeyPress(void)
{
	PA_ODR |= BIT1;
	tp5602_key_press_flag = false;
}
//==========================================================
//在主循环中每 10ms 调用一次
void Tp5602Funtion(void)
{
	if(PA_ODR & BIT1)
	{
		if(false == tp5602_key_press_flag)
		{
			tp5602_key_press_flag = true;
		}
		else
		{
			PA_ODR &= ~(BIT1);
		}
	}
	
	if((second_count ++) > 10)
	{
		second_count = 0;
		curBattVoltage = (u8)((((u32)GetAdc(ADC_CH_BATT)) * 99) >> 11);
		curInputVoltage = (u8)((((u32)GetAdc(ADC_CH_INPUT)) * 99) >> 9);
	}
}
//==========================================================
void TP5602InitPoweron(void)
{
	PA_DDR |=  (BIT1);
	PA_CR1 |=  (BIT1);
	PA_CR2 &= ~(BIT1);
	PA_ODR &= ~(BIT1);
	
	PA_DDR &= ~(BIT2);
	PA_CR1 &= ~(BIT2);
	PA_CR2 &= ~(BIT2);
	
	curBattVoltage = 0;
	curBattCapacity = 0;
	curInputVoltage = 0;
	second_count = 0xfe;
}
//==========================================================
//end files
