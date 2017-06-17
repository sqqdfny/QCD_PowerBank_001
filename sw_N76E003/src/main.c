/* MAIN.C file
 * 
 * Copyright (c) 2002-2005 STMicroelectronics
 系统10ms唤醒一次,所有的计时都在主程序中进行
 10ms中断只做定时唤醒
 */
 
 /*
           b7        b6        b5        b4        b3        b2         b1         b0
 config0   CBS       Reserved  OCDPWM    OCDEN     Reserved  RPD        LOCK       Reserved     0xf9   RST_PIN->P2.0  LOCK_EN
 config1   Reserved  Reserved  Reserved  Reserved  Reserved  LDSIZE[2]  LDSIZE[1]  LDSIZE[0]    0xff
 config2   CBODEN    Reserved  CBOV[1]   CBOV[0]   BOIAP     CBORST     Reserved   Reserved     0xef   2.7V
 config3   Reserved  Reserved  Reserved  Reserved  Reserved  Reserved   Reserved   Reserved     0xff
 config4   WDTEN[3]  WDTEN[2]  WDTEN[1]  WDTEN[0]  Reserved  Reserved   Reserved   Reserved     0xff
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

static void BOD_Init(void)
{
#pragma asm
  MOV TA, #0AAH
	MOV TA, #55H
	MOV BODCON0, #0A4H    //BOEN 2.7V BORST
	
	MOV TA, #0AAH
	MOV TA, #55H
	MOV BODCON1, #07H    //25.6ms BODFLT
#pragma endasm
}
//==========================================================
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
	RWK=156;        //10MS
  WKCON = 0x08;   //1分频  
	set_EWKT;//使能wkt中断
}
//==========================================================

//==========================================================
#if(0)   //#ifndef   DEBUG
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
	BOD_Init();
  
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
