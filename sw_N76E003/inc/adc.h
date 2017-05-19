#include "../inc/typedef.h"

#ifndef __ADC_H__
#define __ADC_H__

#define ADC_CH_BATT   7
#define ADC_CH_INPUT  6
#define ADC_CH_REF  8

u16 GetAdc(u8 ch);
void AdcTimeHook(void);
void InitAdc(void);

#endif
