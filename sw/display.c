#include "display.h"

#define DISP_PIN1_HIGH()   ()
#define DISP_PIN1_LOW()    ()


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

_bool led_update_flag;
static @near DISP_CALLBACK * pDispCallback; //显示回调
static @near DISP_TICK_CALLBACK * pDispTickCallback;
static @tiny _led_scan_com;
@tiny _led_disp_map[3];
//==========================================================
//TIM2 150Hz 中断 
@far @interrupt void TIM2_UPD_OVF_IRQHandler (void)
{
	TIM2_SR1 = 0;  //clear interrupt flag

	/*ALL_COM_OFF();
	switch(_led_scan_com)
	{
		case 0:
			SendDat2Hc595(_led_disp_map[0]);
			COM2_ON();
			break;
		case 1:
		  SendDat2Hc595(_led_disp_map[1]);
			COM3_ON();
			break;
		case 2:
		  SendDat2Hc595(_led_disp_map[2]);
			COM1_ON();
			break;
	}
  if(++ _led_scan_com > 2) _led_scan_com = 0;*/
}
//=========================================================


//=========================================================
void DisplayInitPoweron(void)
{
	u8 tmp;
	PC_DDR |=  (BIT3 | BIT4 | BIT5 | BIT6 | BIT7);
	PC_CR1 |=  (BIT3 | BIT4 | BIT5 | BIT6 | BIT7);
	PC_CR2 |=  (BIT3 | BIT4 | BIT5 | BIT6 | BIT7);
	PC_ODR |=  (BIT3 | BIT4 | BIT5 | BIT6 | BIT7);
	
	PB_DDR |=  (BIT4 | BIT5);
	PB_CR1 |=  (BIT4 | BIT5);
	PB_CR2 |=  (BIT4 | BIT5);
	PB_ODR |=  (BIT4 | BIT5);
	
	led_update_flag = 1;
	pDispCallback = NULL;
	pDispTickCallback = NULL;
	_led_scan_com = 0;
	_led_disp_map[0] = 0xff;//_led_num_tab[5];
	_led_disp_map[1] = 0xff;//_led_num_tab[6];
	_led_disp_map[2] = 0xff;//_led_num_tab[7];
//==========================================================
	CLK_PCKENR1 |= BIT5;  //TIM2
	
	TIM2_PSCR = 0x03;   //8 分频  1M
	TIM2_ARRH = (6666 >> 8) & 0xff;    // 150hz
	TIM2_ARRL = (6666 & 0xff);
	TIM2_IER = 0x01;    //enable interrupt
	TIM2_CR1 = 0x01;    //enable time2
	TIM2_EGR = 0x01;
	
	tmp = ITC_SPR4;
	tmp = (tmp & 0xf3) | (0 << 2);  //prio = 2 b00
	ITC_SPR4 = tmp;
}
//=========================================================
//end files
