#include "typedef.h"

#ifndef __ADC_H__
#define __ADC_H__

#define ADC_CH_BATT   2
#define ADC_CH_INPUT  3

u16 GetAdc(u8 ch);
void AdcTimeHook(void);
void InitAdc(void);

#endif