#include "../inc/display.h"

//
#define ALL_DISP_PIN_LOW()\
			 {\
			 	clr_P00;clr_P01;clr_P04;\
				clr_P10;clr_P12;clr_P15;\
			 }
			 
#define ALL_DISP_PIN_INPUT(); \
			 {	set_P0M1_0;clr_P0M2_0;\
				set_P0M1_1;clr_P0M2_1;\
				set_P0M1_4;clr_P0M2_4;\
				set_P1M1_0;clr_P1M2_0;\
				set_P1M1_2;clr_P1M2_2;\
				set_P1M1_5;clr_P1M2_5;\
			 }
														
#define DISP_PIN1_OUTPP();  {clr_P0M1_4;set_P0M2_4;}
#define DISP_PIN1_HIGH()   (set_P04)
#define DISP_PIN1_LOW()    (clr_P04)

#define DISP_PIN2_OUTPP();  {clr_P0M1_1;set_P0M2_1;}
#define DISP_PIN2_HIGH()   (set_P01)
#define DISP_PIN2_LOW()    (clr_P01)

#define DISP_PIN3_OUTPP();  {clr_P0M1_0;set_P0M2_0;}
#define DISP_PIN3_HIGH()   (set_P00)
#define DISP_PIN3_LOW()    (clr_P00)

#define DISP_PIN4_OUTPP();  {clr_P1M1_0;set_P1M2_0;}
#define DISP_PIN4_HIGH()   (set_P10)
#define DISP_PIN4_LOW()    (clr_P10)

#define DISP_PIN5_OUTPP();  {clr_P1M1_2;set_P1M2_2;}
#define DISP_PIN5_HIGH()   (set_P12)
#define DISP_PIN5_LOW()    (clr_P12)

#define DISP_PIN6_OUTPP();  {clr_P1M1_5;set_P1M2_5;}
#define DISP_PIN6_HIGH()   (set_P15)
#define DISP_PIN6_LOW()    (clr_P15)

const u8 _led_num_tab[10] = 
{
	LCD_A | LCD_B | LCD_C | LCD_D | LCD_E | LCD_F,          /* 0 */
	LCD_B | LCD_C,                                          /* 1 */
	LCD_A | LCD_B | LCD_D | LCD_E | LCD_G,                  /* 2 */
	LCD_A | LCD_B | LCD_C | LCD_D | LCD_G,                  /* 3 */
	LCD_B | LCD_C | LCD_F | LCD_G,                          /* 4 */
	LCD_A | LCD_C | LCD_D | LCD_F | LCD_G,                  /* 5 */
	LCD_A | LCD_C | LCD_D | LCD_E | LCD_F | LCD_G,          /* 6 */
	LCD_A | LCD_B | LCD_C | LCD_F,                          /* 7 */
	LCD_A | LCD_B | LCD_C | LCD_D | LCD_E | LCD_F | LCD_G,  /* 8 */
	LCD_A | LCD_B | LCD_C | LCD_D | LCD_F | LCD_G,          /* 9 */
	//LCD_A | LCD_B | LCD_C | LCD_E|  LCD_F | LCD_G,          /* a */
	//LCD_C | LCD_D | LCD_E | LCD_F | LCD_G,                  /* b */
	//LCD_A | LCD_D | LCD_E | LCD_F,                          /* c */
	//LCD_B | LCD_C | LCD_D | LCD_E | LCD_G,                  /* d */
	//LCD_A | LCD_D | LCD_E | LCD_F | LCD_G,                  /* e */
	//LCD_A | LCD_E | LCD_F | LCD_G,                          /* f */
};

bool disp_update_flag;
static  DISP_CALLBACK * pDispCallback; //显示回调
static  DISP_TICK_CALLBACK * pDispTickCallback;


/*
	b00->A2  b10->A3  b20->A4
	b01->B2  b11->B3  b21->B4
	b02->C2  b12->C3  b22->C4
	b03->D2  b13->D3  b23->D4
	b04->E2  b14->E3  b24->E4
	b05->F2  b15->F3  b25->F4
	b06->G2  b16->G3  b26->G4
	b07->B1  b17->C1  b27->DP
*/
volatile u8 _led_disp_map[3];
static volatile u8 _led_scan_com;

/************************************************************************************************************
*    TIMER 2 interrupt subroutine
************************************************************************************************************/
void Timer3_ISR (void) interrupt 16 
{
    clr_TF3;

	ALL_DISP_PIN_LOW();
	nop();
	ALL_DISP_PIN_INPUT();
	switch(_led_scan_com)
	{
		case 0:
			DISP_PIN3_LOW();
			if(_led_disp_map[0] & BIT7) 
			{
				DISP_PIN4_HIGH();
				DISP_PIN4_OUTPP();
			}
			
			if(_led_disp_map[1] & BIT7) 
			{
				DISP_PIN5_HIGH();
				DISP_PIN5_OUTPP();
			}
			
			if(_led_disp_map[2] & BIT7) 
			{
				DISP_PIN6_HIGH();
				DISP_PIN6_OUTPP();
			}
			
			DISP_PIN3_OUTPP();
			break;
		case 1:
			DISP_PIN1_LOW();
			if(_led_disp_map[0] & BIT0) 
			{
				DISP_PIN2_HIGH();
				DISP_PIN2_OUTPP();
			}
			
			if(_led_disp_map[0] & BIT1) 
			{
				DISP_PIN3_HIGH();
				DISP_PIN3_OUTPP();
			}
			
			if(_led_disp_map[0] & BIT2) 
			{
				DISP_PIN4_HIGH();
				DISP_PIN4_OUTPP();
			}
			
			if(_led_disp_map[0] & BIT3) 
			{
				DISP_PIN5_HIGH();
				DISP_PIN5_OUTPP();
			}
			
			if(_led_disp_map[0] & BIT4) 
			{
				DISP_PIN6_HIGH();
				DISP_PIN6_OUTPP();
			}
			
			DISP_PIN1_OUTPP();
			break;
		case 2:
			DISP_PIN1_HIGH();
			if(_led_disp_map[1] & BIT0) 
			{
				DISP_PIN2_LOW();
				DISP_PIN2_OUTPP();
			}
			
			if(_led_disp_map[1] & BIT1) 
			{
				DISP_PIN3_LOW();
				DISP_PIN3_OUTPP();
			}
			
			if(_led_disp_map[1] & BIT2) 
			{
				DISP_PIN4_LOW();
				DISP_PIN4_OUTPP();
			}
			
			if(_led_disp_map[1] & BIT3) 
			{
				DISP_PIN5_LOW();
				DISP_PIN5_OUTPP();
			}
			
			if(_led_disp_map[1] & BIT4) 
			{
				DISP_PIN6_LOW();
				DISP_PIN6_OUTPP();
			}
			
		  DISP_PIN1_OUTPP();
			break;
		case 3:
			DISP_PIN2_LOW();
			if(_led_disp_map[0] & BIT5) 
			{
				DISP_PIN3_HIGH();
				DISP_PIN3_OUTPP();
			}
			
			if(_led_disp_map[0] & BIT6) 
			{
				DISP_PIN4_HIGH();
				DISP_PIN4_OUTPP();
			}
			
			if(_led_disp_map[1] & BIT5) 
			{
				DISP_PIN5_HIGH();
				DISP_PIN5_OUTPP();
			}
			
			if(_led_disp_map[1] & BIT6) 
			{
				DISP_PIN6_HIGH();
				DISP_PIN6_OUTPP();
			}
			
			DISP_PIN2_OUTPP();
			break;
		case 4:
			DISP_PIN2_HIGH();
			if(_led_disp_map[2] & BIT0) 
			{
				DISP_PIN3_LOW();
				DISP_PIN3_OUTPP();
			}
			
			if(_led_disp_map[2] & BIT1) 
			{
				DISP_PIN4_LOW();
				DISP_PIN4_OUTPP();
			}
			
			if(_led_disp_map[2] & BIT2) 
			{
				DISP_PIN5_LOW();
				DISP_PIN5_OUTPP();
			}
			
			if(_led_disp_map[2] & BIT3) 
			{
				DISP_PIN6_LOW();
				DISP_PIN6_OUTPP();
			}
			
		  DISP_PIN2_OUTPP();
			break;
		case 5:
			DISP_PIN3_HIGH();
			if(_led_disp_map[2] & BIT4) 
			{
				DISP_PIN4_LOW();
				DISP_PIN4_OUTPP();
			}
			
			if(_led_disp_map[2] & BIT5) 
			{
				DISP_PIN5_LOW();
				DISP_PIN5_OUTPP();
			}
			
			if(_led_disp_map[2] & BIT6) 
			{
				DISP_PIN6_LOW();
				DISP_PIN6_OUTPP();
			}
			
		  DISP_PIN3_OUTPP();
			break;
	}
  if(++ _led_scan_com > 5) _led_scan_com = 0;
}
//=========================================================

//=========================================================
void RefreshDisplay(void)
{
	if(pDispTickCallback)
	{
		pDispTickCallback();
	}
	
	if(0 == disp_update_flag) return;
	disp_update_flag = 0;

	if(pDispCallback)
	{
		pDispCallback();
	}
}

void SetDispCallback(DISP_CALLBACK * pDisp, 
										 DISP_TICK_CALLBACK * pDispTick)
{
	pDispCallback = pDisp;
	pDispTickCallback = pDispTick;
}

bool IsDisplay(DISP_CALLBACK * pDisp)
{
	return ((pDispCallback == pDisp) ? 1 : 0);
}
//=========================================================

//=========================================================
void DisplayEnterSleep(void)
{
	ALL_DISP_PIN_INPUT();
	clr_TR3;    //disable time3
}

void DisplayExitSleep(void)
{	
	set_T3PS0;//8 分频  1M
	set_T3PS1;
	clr_T3PS2;

	RH3 = 0xf2;                       //initial counter values 
    RL3 = 0xfa;    
       
    set_ET3;                                    //enable Timer3 interrupt
    set_EA;                                     //enable interrupts
    set_TR3;                                    //Timer3 run
}
//=========================================================
void DisplayInitPoweron(void)
{
	DISP_PIN6_OUTPP();
	DISP_PIN5_OUTPP();
	DISP_PIN4_OUTPP();
	DISP_PIN3_OUTPP();
	DISP_PIN2_OUTPP();
	DISP_PIN1_OUTPP();

	DISP_PIN6_HIGH();
	DISP_PIN5_HIGH();
	DISP_PIN4_HIGH();
	DISP_PIN3_HIGH();
	DISP_PIN2_HIGH();
	DISP_PIN1_HIGH();

	P0M1  &= ~BIT5;
	clr_P0M1_5;set_P0M2_5;
	clr_P0M1_6;set_P0M2_6;
	clr_P0M1_7;set_P0M2_7;
	clr_P05;
	clr_P06;
	clr_P07;
	
	disp_update_flag = 1;
	pDispCallback = NULL;
	pDispTickCallback = NULL;
	_led_scan_com = 0;
	_led_disp_map[0] = 0xff; //_led_num_tab[1]; //
	_led_disp_map[1] = 0xff; //_led_num_tab[2]; //
	_led_disp_map[2] = 0xff; //_led_num_tab[3]; //
//==========================================================
	//set_T3PS0;//8 分频  1M
	//set_T3PS1;
	set_T3PS0;//8 分频  1M
	set_T3PS1;
	clr_T3PS2;


	RH3 = 0xf2;                       //initial counter values 
    RL3 = 0xfa;    
    
    set_ET3;                                    //enable Timer3 interrupt
    set_EA;                                     //enable interrupts
    set_TR3;                                    //Timer3 run
}
//=========================================================
//end files
