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
void Timer0_ISR (void) interrupt 1  //interrupt address is 0x000B
{
    TH0 = u8TH0_Tmp;
    TL0 = u8TL0_Tmp;    
    tick_flag=true;
}

static void TIM0_Init(void)
{
	TIMER0_MODE1_ENABLE;
	
	clr_T0M;	//12分频
	u8TH0_Tmp = 0x3a;	//14999
    u8TL0_Tmp = 0x97; 
	
	TH0 = u8TH0_Tmp;
    TL0 = u8TL0_Tmp;
	
	set_ET0; //enable Timer0 interrupt
	set_EA;  //enable interrupts
    set_TR0; //Timer0 run
}
//==========================================================


//==========================================================
#ifndef DEBUG
	#define IWDG_Feed()   //(set_WDCLR)
	
	static void Iwdg_Init(void)
	{
// 		TA=0xAA;TA=0x55;WDCON=0x07;						//Setting WDT prescale 
// 		set_WDCLR;														//Clear WDT timer
// 		while((WDCON|~SET_BIT6)==0xFF);				//confirm WDT clear is ok before into power down mode
// 		EA = 1;
// 		set_WDTR;															//WDT run	
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
	
	TP5602InitPoweron();
	KeyInit();
	TIM0_Init();
	InitAdc();
	DisplayInitPoweron();
	//_asm("rim");   
	
	//Delayms(1000);
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
		//_asm("wfi");
	#endif
	}
}
//==========================================================

//end files
