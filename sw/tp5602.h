#include "typedef.h"


#ifndef __TP5602_H__
#define __TP5602_H__

//电池电压
u8 GetBattVoltage(void);

//电池电量
u8 GetBattCapacity(void);

//车辆电瓶电压
u8 GetInputVoltage(void);

//是否连接了外部电源
bool IsConnectedInputPower(void);

//升压输出是否正在工作
bool IsBoostOut(void);

//向TP5602按键PIN 输出一个低脉冲
//10~20 ms
void Tp5602KeyPress(void);

void Tp5602Funtion(void);
void TP5602InitPoweron(void);

#endif //__TP5602_H__
//==========================================================
//end files
