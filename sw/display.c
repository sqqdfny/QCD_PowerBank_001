#include "display.h"

//
#define ALL_DISP_PIN_LOW()\
			 {PA_ODR &= ~BIT3;\
			  PC_ODR &= ~(BIT3 | BIT5 | BIT6 | BIT7);\
			  PD_ODR &= ~BIT3;\
			 }
			 
#define ALL_DISP_PIN_INPUT_F(); \
			 {PA_DDR &= ~BIT3;PA_CR1 &= ~BIT3;\
				PC_DDR &= ~(BIT3 | BIT5 | BIT6 | BIT7);\
				PC_CR1 &= ~(BIT3 | BIT5 | BIT6 | BIT7);\
				PD_DDR &= ~BIT3;PD_CR1 &= ~BIT3;}
														
#define DISP_PIN6_OUTPP();  {PA_DDR |=  BIT3;PA_CR1 |=  BIT3;}
#define DISP_PIN6_HIGH()   (PA_ODR |=  BIT3)
#define DISP_PIN6_LOW()    (PA_ODR &= ~BIT3)

#define DISP_PIN5_OUTPP();  {PC_DDR |=  BIT3;PC_CR1 |=  BIT3;}
#define DISP_PIN5_HIGH()   (PC_ODR |=  BIT3)
#define DISP_PIN5_LOW()    (PC_ODR &= ~BIT3)

#define DISP_PIN4_OUTPP();  {PC_DDR |=  BIT5;PC_CR1 |=  BIT5;}
#define DISP_PIN4_HIGH()   (PC_ODR |=  BIT5)
#define DISP_PIN4_LOW()    (PC_ODR &= ~BIT5)

#define DISP_PIN3_OUTPP();  {PC_DDR |=  BIT6;PC_CR1 |=  BIT6;}
#define DISP_PIN3_HIGH()   (PC_ODR |=  BIT6)
#define DISP_PIN3_LOW()    (PC_ODR &= ~BIT6)

#define DISP_PIN2_OUTPP();  {PC_DDR |=  BIT7;PC_CR1 |=  BIT7;}
#define DISP_PIN2_HIGH()   (PC_ODR |=  BIT7)
#define DISP_PIN2_LOW()    (PC_ODR &= ~BIT7)

#define DISP_PIN1_OUTPP();  {PD_DDR |=  BIT3;PD_CR1 |=  BIT3;}
#define DISP_PIN1_HIGH()   (PD_ODR |=  BIT3)
#define DISP_PIN1_LOW()    (PD_ODR &= ~BIT3)

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
static @near DISP_CALLBACK * pDispCallback; //显示回调
static @near DISP_TICK_CALLBACK * pDispTickCallback;


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
volatile @tiny _led_disp_map[3];
volatile static @tiny _led_scan_com;
//==========================================================
//TIM2 150Hz 中断 
@far @interrupt void TIM2_UPD_OVF_IRQHandler (void)
{
	TIM2_SR1 = 0;  //clear interrupt flag

	ALL_DISP_PIN_LOW();
	nop();
	ALL_DISP_PIN_INPUT_F();
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
	
	if(0 == disp_update_flag) return ;
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
	ALL_DISP_PIN_INPUT_F();
	TIM2_CR1 &= ~BIT0;    //enable time2
	CLK_PCKENR1 &= ~BIT5;  //TIM2
}

void DisplayExitSleep(void)
{
	CLK_PCKENR1 |= BIT5;  //TIM2
	
	TIM2_PSCR = 0x03;   //8 分频  1M
	TIM2_ARRH = (3333 >> 8) & 0xff;    // 300hz
	TIM2_ARRL = (3333 & 0xff);
	TIM2_IER = 0x01;    //enable interrupt
	TIM2_CR1 = 0x01;    //enable time2
	TIM2_EGR = 0x01;
}
//=========================================================
void DisplayInitPoweron(void)
{
	u8 tmp;
	PA_DDR |=  (BIT3);
	PA_CR1 |=  (BIT3);
	PA_CR2 &= ~(BIT3);
	PA_ODR |=  (BIT3);
	
	PC_DDR |=  (BIT3 | BIT5 | BIT6 | BIT7);
	PC_CR1 |=  (BIT3 | BIT5 | BIT6 | BIT7);
	PC_CR2 &= ~(BIT3 | BIT5 | BIT6 | BIT7);
	PC_ODR |=  (BIT3 | BIT5 | BIT6 | BIT7);
	
	PD_DDR |=  (BIT3);
	PD_CR1 |=  (BIT3);
	PD_CR2 &= ~(BIT3);
	PD_ODR |=  (BIT3);
	
	PD_DDR |=  (BIT4 | BIT5 | BIT6);
	PD_CR1 |=  (BIT4 | BIT5 | BIT6);
	PD_CR2 &= ~(BIT4 | BIT5 | BIT6);
	PD_ODR &= ~(BIT4 | BIT5 | BIT6);
	
	disp_update_flag = 1;
	pDispCallback = NULL;
	pDispTickCallback = NULL;
	_led_scan_com = 0;
	_led_disp_map[0] = 0xff; //_led_num_tab[1]; //
	_led_disp_map[1] = 0xff; //_led_num_tab[2]; //
	_led_disp_map[2] = 0xff; //_led_num_tab[3]; //
//==========================================================
	CLK_PCKENR1 |= BIT5;  //TIM2
	
	TIM2_PSCR = 0x03;   //8 分频  1M
	TIM2_ARRH = (3333 >> 8) & 0xff;    // 300hz
	TIM2_ARRL = (3333 & 0xff);
	TIM2_IER = 0x01;    //enable interrupt
	TIM2_CR1 = 0x01;    //enable time2
	TIM2_EGR = 0x01;
	
	tmp = ITC_SPR4;
	tmp = (tmp & 0xf3) | (0 << 2);  //prio = 2 b00
	ITC_SPR4 = tmp;
}
//=========================================================
//end files
