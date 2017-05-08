#include "adc.h"
#include "normal.h"

//==========================================================
/*static @tiny u8 _buf_index;
static @near u16 _buf[16];

static u16 GetAverage(void)
{
	u8 i, j;
	u16 value, buf[16];
	
	for(i = 0; i < 16; i ++)
	{
		buf[i] = _buf[i];
	}
	//sort
	for(i = 0; i < 16; i ++)
	{
		for(j = i; j < 15; j ++)
		{
			if(buf[j] > buf[j+1])
			{
				value = buf[j];
				buf[j] = buf[j+1];
				buf[j+1] = value;
			}
		}
	}
	
	value = 0;
	for(i = 4; i < 12; i ++)
	{
		value += _buf[i];
	}
	
	return (value >> 1);
}*/
//==========================================================

//==========================================================
//ch: 0~15
/*u16 GetAdc(u8 ch)
{
	ADC_CSR = ch;
	//ADC_CR1 |= BIT0; //wakeup
	ADC_CR1 |= BIT0; //start
	while(0 == ADC_CSR & BIT7)
	{
		//nop();
	}
	return ( ((u16)ADC_DRL) | (((u16)ADC_DRH) << 8) );
}*/

//==========================================================

void AdcTimeHook(void)
{
	/*if(0 == (ADC_CSR & BIT7)) return;
	
	_buf[_buf_index] = (((u16)ADC_DB4RL) | (((u16)ADC_DB4RH) << 8) ) & 0x3ff;
	if(++ _buf_index > 15)
	{
		cur_temp = GetCurTemperatureFromAd();
		_buf_index = 0;
	}*/
}

void InitAdc(void)
{
	PD_DDR &= ~(BIT3);
	PD_CR1 &= ~(BIT3);
	PD_CR2 &= ~(BIT3);
	
	CLK_PCKENR2 |= BIT3;       //ADC
	ADC_CR1 = (7 << 4) |  BIT1 | BIT0; //18·ÖÆµ
	ADC_CR2 = BIT3;            //right align
	ADC_CR3 = BIT7;            //data buffer enable
	ADC_CSR = 4;
	ADC_CR1 |= BIT0; //wakeup
	ADC_CR1 |= BIT0; //start
}
//==========================================================