#include "../inc/tp5602.h"
#include "../inc/adc.h"
#include "../inc/main.h"


static u16 curBattVoltage;
static u8 curBattCapacity;
static u8 curInputVoltage;

static u8 second_count;
static bool tp5602_key_press_flag;

code u16 voltageadd[]=
{
	420,//100
	410,//90
	400,//80
	392,//70
	387,//60
	382,//50
	379,//40 
	377,//30
	374,//20
	368,//10
	345,//5
	300//0
};

code u16 voltagedec[]=
{
	388,//100
	380,//90	
	370,//80	
	369,//70	
	346,//60
	345,//50
	335,//40
	330,//30
	320,//20
	310,//10
	305,//5
	300//0
};

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
	if(curBattVoltage<300) return;
	P30=1;
	tp5602_key_press_flag = false;
}
void CurBattCapacitySwitch(u8 i)
{
	switch(i)
	{
		case 0:
		{
			curBattCapacity=100;
			break;
		}
		case 1:
		{
			curBattCapacity=90;
			break;
		}		
		case 2:
		{
			curBattCapacity=80;
			break;
		}		
		case 3:
		{
			curBattCapacity=70;
			break;
		}		
		case 4:
		{
			curBattCapacity=60;
			break;
		}		
		case 5:
		{
			curBattCapacity=50;
			break;
		}		
		case 6:
		{
			curBattCapacity=40;
			break;
		}		
		case 7:
		{
			curBattCapacity=30;
			break;
		}		
		case 8:
		{
			curBattCapacity=20;
			break;
		}		
		case 9:
		{
			curBattCapacity=10;
			break;
		}		
		case 10:
		{
			curBattCapacity=5;
			break;
		}		
		case 11:
		{
			curBattCapacity=0;
			break;
		}
		default:
			break;
	}
}

//==========================================================
//计算电池当前剩余电量
static void CalculationCurBattCapacity(void)
{
	u8 i;
	static u8 lastBattCapacityflag=0xff;
	static u8 poweronoffflag=0;

	if(IsConnectedInputPower())
	{				
		for(i=0;i<12;i++)
		{
			if(GetBattVoltage()>=voltageadd[i]) break;
		}
		if((lastBattCapacityflag>i)||(0xff==lastBattCapacityflag)||(0==poweronoffflag))
		{
			CurBattCapacitySwitch(i);
			lastBattCapacityflag=i;
		}
		poweronoffflag=1;
	}else
		{
			for(i=0;i<12;i++)
			{
				if(GetBattVoltage()>=voltagedec[i]) break;	
			}			
			if((lastBattCapacityflag<i)||(0xff==lastBattCapacityflag)||(1==poweronoffflag))
			{
				CurBattCapacitySwitch(i);
				lastBattCapacityflag=i;
			}
			poweronoffflag=0;

		}
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
