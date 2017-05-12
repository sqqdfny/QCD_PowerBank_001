/* MAIN.C file
 * 
 * Copyright (c) 2002-2005 STMicroelectronics
 系统10ms唤醒一次,所有的计时都在主程序中进行
 10ms中断只做定时唤醒
 */
#include "main.h"
#include "key.h"
#include "adc.h"
#include "delay.h"
#include "display.h"
#include "normal.h"
#include "standby.h"
#include "tp5602.h"

static bool tick_flag;
static bool sleep_flag;  //系统休眠
//==========================================================
void SystemInit(void)
{
	CLK_CKDIVR = (1<<3) | 0;  //fMASTER = 8M  fCPU = 8M
  ITC_SPR1 = 0;
	ITC_SPR2 = 0;
	ITC_SPR3 = 0;
	ITC_SPR4 = 0;
	ITC_SPR5 = (0x3 << 4);  //uart1_rx
	ITC_SPR6 = 0;
	ITC_SPR7 = 0;
	
	tick_flag = true;
	sleep_flag = false;
}
//==========================================================
//TIM1 10ms 中断 
@far @interrupt void TIM1_UPD_OVF_IRQHandler (void)
{
	TIM1_SR1 = 0;  //clear interrupt flag
	tick_flag = true;
}

static void TIM1_Init(void)
{
	CLK_PCKENR1 |= BIT7;  // TIM1
	TIM1_PSCRH = 0;
	TIM1_PSCRL = 7;   //8 分频  1M
	TIM1_ARRH = (10000 >> 8) & 0xff;    // 10ms
	TIM1_ARRL = (10000 & 0xff);
	TIM1_IER = 0x01;    //enable interrupt
	TIM1_CR1 = 0x01;    //enable time1
	TIM1_EGR = 0x01;
}
//==========================================================


//==========================================================
#ifndef DEBUG
	#define IWDG_Feed()   (IWDG_KR = 0xaa)
	
	static void Iwdg_Init(void)
	{
		IWDG_KR = 0xcc;
		IWDG_KR = 0x55;
		IWDG_PR = 2;      //63.7ms
		IWDG_RLR = 0xff;
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
	Iwdg_Init();
	SystemInit();
	
	TP5602InitPoweron();
	KeyInit();
	TIM1_Init();
	InitAdc();
	DisplayInitPoweron();
	_asm("rim");   
	
	Delayms(1000);
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
		_asm("wfi");
	#endif
	}
}
//==========================================================

//end files
