#include "adc.h"
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
static u16 adc_tmp[3];
u16 GetAdc(u8 ch)
{	
	switch(ch)
	{
		case ADC_CH_BATT:
		{
			return(adc_tmp[0]);
		}
		case ADC_CH_INPUT:
		{
			return(adc_tmp[1]);
		}
		case ADC_CH_REF:
		{
			return(adc_tmp[2]);
		}
		default:
			return 0;
	}
	return 0;
}

//==========================================================

void AdcTimeHook(void)
{
	u16 temp1=0,temp2=0;
	static u8 chang_flag=0;
	if(++chang_flag > 10)
	{
		chang_flag=0;
		
		Enable_ADC_AIN7;
		clr_ADCF;
		set_ADCS;																	// Each time ADC start trig signal
		
		while(ADCF == 0);
		adc_tmp[0]=(u32)(((u32)ADCRH<<4)|ADCRL);
		ADCCON1&=~BIT0;

		Enable_ADC_AIN6;
		clr_ADCF;
		set_ADCS;																	// Each time ADC start trig signal
	    while(ADCF == 0);
		adc_tmp[1]=((ADCRH<<4)|ADCRL);
		ADCCON1&=~BIT0;

		Enable_ADC_BandGap;
		clr_ADCF;
		set_ADCS;																	// Each time ADC start trig signal
		
		while(ADCF == 0);
		adc_tmp[2]=(u32)(((u32)ADCRH<<4)|ADCRL);
		ADCCON1&=~BIT0;
	}
}

//void AdcTriggerConvert(void)
//{
//}
//==========================================================

//==========================================================
void InitAdc(void)
{
	u8 temp=0;
	set_P0M1_3;clr_P0M2_3;
	set_P1M1_1;clr_P1M2_1;
}
//==========================================================
