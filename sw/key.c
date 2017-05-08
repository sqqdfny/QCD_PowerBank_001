#include "key.h"
#include "delay.h"

#define Set165ssHigh()     (PC_ODR |=  BIT3)
#define Clr165ssLow()      (PC_ODR &= ~BIT3)  
#define Set165clkHigh()    (PB_ODR |=  BIT4)  
#define Clr165clkLow()     (PB_ODR &= ~BIT4)  
#define Get165dat()        (PB_IDR & BIT5) 


static @tiny u8 keyShakeTime;          //消抖时间
static @tiny u8 keyRepeatTime;         //连动时间

static @near u8 keyTimeSecond;            //按键按下的时间(S)
static @near enumKeyStatus keyStatus;     //按键状态
static @near KEY_CALLBACK * pKeyCallback; //按键回调

@tiny enumKeyValue keyValue;              //当前键值
@tiny u8 _hc165_map, _hc165_map_tmp[3];
//==========================================================
static u8 Get74hc165Data(void)
{
	u8 i, result=0;

  nop();nop();
	Set165ssHigh();  //锁定输入数据
	for(i = 0; i < 8; i ++)
	{
		nop();nop();
		result <<= 1;
		if(Get165dat())	result ++;
		Clr165clkLow();
		nop();nop();
		Set165clkHigh();
	}
	Clr165ssLow();	//重新输入并行数据

	return (result);
}

static void KeyScan(void)
{
	u8 tmp;
	for(tmp = 0; tmp < 3; tmp ++)
	{
		_hc165_map_tmp[tmp] = Get74hc165Data();
	}
	
	if((_hc165_map_tmp[0] == _hc165_map_tmp[1]) && (_hc165_map_tmp[1] == _hc165_map_tmp[2]))
	{
		_hc165_map = _hc165_map_tmp[0];
	}
	
	tmp = (_hc165_map & 0x1);
	if(KEY_VALUE_NOKEY != tmp)
	{
		if(0 == keyShakeTime)
		{
			keyShakeTime = 0x80 + 2;  //20ms
		}
		else
		if(0x80 == keyShakeTime)
		{
			if(keyValue != tmp)
			{
				keyValue = tmp;
				keyStatus = KEY_STATUS_PRESS;
				keyRepeatTime = 5;
				keyTimeSecond = 0;
			}
		}
	}
	else
	{
		if(KEY_VALUE_NOKEY != keyValue)
		{
			keyShakeTime = 0;
			keyRepeatTime = 0;
			keyValue = KEY_VALUE_NOKEY;
			keyStatus = KEY_STATUS_NOPRESS;
		}
	}
}

void KeyFuntion(void)
{
	KeyScan();
	if(KEY_VALUE_NOKEY == keyValue) return;
	if(keyRepeatTime & 0x7f) return;
				
	if(0 == keyRepeatTime)
	{//step
		keyRepeatTime = 0x80 + 100;  //1S
		if(pKeyCallback) 
		{
			pKeyCallback(keyValue, keyStatus, 0);
		}
		
		keyStatus = KEY_STATUS_KEEP;
		keyTimeSecond = 3;
	}
	else
	{//link
		keyRepeatTime = 0x80 + 25;  //0.25S
	  if(keyTimeSecond < 0xff) keyTimeSecond ++;
		
		if(pKeyCallback) 
		{
			if( pKeyCallback(keyValue, keyStatus, keyTimeSecond >> 2))
			{
				keyStatus = KEY_STATUS_KEEPED;
			}
		}
	}
}
 
void KeyTimeHook()
{
	if(keyShakeTime & 0x7f)   keyShakeTime --;
	if(keyRepeatTime & 0x7f)  keyRepeatTime --;
}

void SetKeyCallback(KEY_CALLBACK * pCallback)
{
	pKeyCallback = pCallback;
}

void KeyInit(void)
{
	PA_DDR &= ~(BIT3 | BIT2);
	PA_CR1 |=  (BIT3 | BIT2);
	PA_CR2 &= ~(BIT3 | BIT2);
	
	//HC165 sspl 
	PC_DDR |=  (BIT3);
	PC_CR1 |=  (BIT3);
	PC_CR2 |=  (BIT3);
	Clr165ssLow();
	
	//HC165 sclk
	PB_DDR |=  (BIT4);
	PB_CR1 |=  (BIT4);
	PB_CR2 |=  (BIT4);
	PB_ODR |=  (BIT4);
	
	//HC165 dat
	PB_DDR &= ~(BIT5);
	PB_CR1 &= ~(BIT5);
	PB_CR2 &= ~(BIT5);
	
	keyShakeTime = 0;
	keyRepeatTime = 0;
	keyTimeSecond = 0;
	keyStatus = KEY_STATUS_NOPRESS;
	keyValue = KEY_VALUE_NOKEY;
	pKeyCallback = NULL;
	
	_hc165_map = Get74hc165Data();
}
//==========================================================

//end files

