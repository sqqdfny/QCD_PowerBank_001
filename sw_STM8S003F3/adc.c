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

u16 GetAdc(u8 ch)
{
	switch(ch)
	{
		case 2:
			return (ADC_DB2R);
		case 3:
			return (ADC_DB3R);
		default:
			return (0);
	}
}

//==========================================================

void AdcTimeHook(void)
{
	//ADC_CR1 |= BIT0; //start
}

void AdcTriggerConvert(void)
{
}
//==========================================================

//==========================================================
void InitAdc(void)
{
	PC_DDR &= ~(BIT4);
	PC_CR1 &= ~(BIT4);
	PC_CR2 &= ~(BIT4);
	
	PD_DDR &= ~(BIT2);
	PD_CR1 &= ~(BIT2);
	PD_CR2 &= ~(BIT2);
	
	CLK_PCKENR2 |= BIT3;       //ADC
	ADC_CR1 = (7 << 4) | BIT1 | BIT0; //18·ÖÆµ
	ADC_CR2 = BIT3 | BIT1;     //right align É¨ÃèÄ£Ê½
	ADC_CR3 = BIT7;            //data buffer enable
	ADC_CSR = 3;
	ADC_CR1 |= BIT0; //wakeup*/
	ADC_CR1 |= BIT0; //start
}
//==========================================================