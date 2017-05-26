/* MAIN.C file
 * 
 * Copyright (c) 2002-2005 STMicroelectronics
 系统10ms唤醒一次,所有的计时都在主程序中进行
 10ms中断只做定时唤醒
 */
#include "../inc/main.h"
#include "../inc/key.h"
#include "../inc/adc.h"
#include "../inc/delay.h"
#include "../inc/display.h"
#include "../inc/normal.h"
#include "../inc/standby.h"
#include "../inc/tp5602.h"

static bool tick_flag;
static bool sleep_flag;  //系统休眠
u8 u8TH0_Tmp,u8TL0_Tmp;
//==========================================================
void SystemInit(void)
{
	CKDIV = 1;  //fosc = 16M  fCPU = 8M
	set_CLOEN;
																// when P3.0 keep high, clock out HIRC
	tick_flag = true;
	sleep_flag = false;
}
/************************************************************************************************************
*    TIMER 0 interrupt subroutine
************************************************************************************************************/
void Wkt_ISR(void) interrupt 17  //interrupt address is 
{  
	clr_WKTF;
    tick_flag=true;
}

static void WKT_Init(void)
{
	clr_WKTR;
	clr_WKTF;
	clr_WKPS2;
	clr_WKPS1;
	clr_WKPS0;
	RWK=156;
	set_EWKT;//使能wkt中断
	set_WKTR;
}
//==========================================================
//==========================================================
#ifndef DEBUG
/***********************************************************************
	WDT CONFIG enable 
	warning : this macro is only when ICP not enable CONFIG WDT function
	copy this marco code to you code to enable WDT reset.
************************************************************************/
	void Enable_WDT_Reset_Config(void)
	{
			set_IAPEN;
			IAPAL = 0x04;
			IAPAH = 0x00;
			IAPFD = 0x0F;
			IAPCN = 0xE1;
			set_CFUEN;
			set_IAPGO;                                  //trigger IAP
			while((CHPCON&SET_BIT6)==SET_BIT6);          //check IAPFF (CHPCON.6)
			clr_CFUEN;
			clr_IAPEN;
	}
	#define IWDG_Feed()   set_WDCLR
	
	static void Iwdg_Init(void)
	{
		Enable_WDT_Reset_Config();
 		TA=0xAA;TA=0x55;WDCON=0x07;					//Setting WDT prescale 
 		set_WDCLR;									//Clear WDT timer
 		while((WDCON|~SET_BIT6)==0xFF);				//confirm WDT clear is ok before into power down mode
 		EA = 1;
 		set_WDTR;									//WDT run	
	}
#else
	#define IWDG_Feed()   
	
	static void Iwdg_Init(void)
	{
		
	}
#endif
//==========================================================

//==========================================================
void EnterSleep(void)
{
	DisplayEnterSleep();
	EnterStandby();
}

void ExitSleep(void)
{
	DisplayExitSleep();
	EnterNormal();
}
//==========================================================

//==========================================================
main()
{
	Delayms(100);
	Iwdg_Init();
	SystemInit();

	InitAdc();
	KeyInit();
	WKT_Init();
	DisplayInitPoweron();
	TP5602InitPoweron();

	/*
	clr_P05;
	clr_P06;
	set_P07;
	Delayms(1000);

	clr_P05;
	set_P06;
	clr_P07;
	*/
	EnterNormal();
	
	IWDG_Feed();
	while (1)
	{
		if(tick_flag)
		{
			tick_flag = false;
			IWDG_Feed();
			AdcTimeHook();
			Tp5602Funtion();
			KeyFuntion();
			RefreshDisplay();
		}
	#ifndef DEBUG
	if(IsDispStandby())
	{
		set_PD;
	}else
		{
			set_IDL;
		}
	#endif
	}
}
//==========================================================

//end files
