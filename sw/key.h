#include "typedef.h"


#ifndef __KEY_H__
#define __KEY_H__


typedef enum
{
	KEY_VALUE_NOKEY  = 0x01,
	KEY_VALUE_ON_OFF = 0x00
}enumKeyValue;

typedef enum
{
	KEY_STATUS_NOPRESS = 0,
	KEY_STATUS_PRESS,     //按下
	KEY_STATUS_KEEP,      //保持
	KEY_STATUS_KEEPED     //长按已响应,避免重复响应
}enumKeyStatus;

//返回非0表示按键已经响应过,可以进入下一个状态
typedef char (KEY_CALLBACK)(enumKeyValue value, enumKeyStatus status, u8 second);

#define GetCurKeyValue() keyValue
#define GetCurHc165Map() _hc165_map

extern @tiny enumKeyValue keyValue;       //当前键值
extern @tiny u8 _hc165_map;

void KeyFuntion(void);
void KeyTimeHook(void);
void SetKeyCallback(KEY_CALLBACK * pCallback);
void KeyInit(void);



#endif

//==========================================================

//end files
