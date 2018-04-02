/*
	°´¼üÉ¨ÃèºÍÖ´ÐÐ
*/

	KEY_INPUT	equ	P0.4
//==========================================================
	KeyFuntion :
		rlcm	_key_bit_map  //×óÒÆÎ»
		b0bts1	P0.4
		jmp	$+3
		b0bset	_key_bit_map.0
		jmp	$+2
		b0bclr	_key_bit_map.0
		
		incs	_key_bit_map	
		jmp	$+2
		jmp	KeyFuntion_NOKEY

	//----------------------------------------
	/*	b0mov	a,_key_dly
		cmprs	a,#(0)
		jmp	KeyFuntion_DLY
		jmp	KeyFuntion_READY_KEY
	KeyFuntion_DLY:
		decms	_key_dly
		jmp	KeyFuntion_NOKEY
	KeyFuntion_READY_KEY:
		mov	a,#(SYSTEM_TICK)
		b0mov	_key_dly,a*/
        //----------------------------------------

		mov	a#(11100001b)
		cmprs	a,_key_bit_map
		jmp	$+2
		jmp	KeyFuntion_CLICK

		mov	a,#(11100110b)
		cmprs	a,_key_bit_map
		jmp	KeyFuntion_NOKEY
		jmp	KeyFuntion_DOUBLE
	KeyFuntion_CLICK:	//µ¥»÷
		b0bts1	_led_on_off_flag
		jmp	KeyFuntion_Exit

		decms	_led_mode
		jmp	KeyFuntion_CLICK_END_MOD2
		mov	a,#(5)
		b0mov	_led_mode,a

	KeyFuntion_CLICK_END_MOD2:
		mov	a,#(1)
		b0mov	_led_dly,a

		//_led_mode == 2(LedFunction_RB)
		mov	a,#(2)       
		cmprs	a,_led_mode  
		jmp	KeyFuntion_CLICK_END_MOD1
		mov	a,#(12)
		b0mov	_led_mode_sub,a
		jmp	KeyFuntion_Exit
	KeyFuntion_CLICK_END_MOD1:
		//_led_mode == 1(LedFunction_R_FLASH)
		mov	a,#(1)       
		cmprs	a,_led_mode  
		jmp	KeyFuntion_Exit
		mov	a,#(10)
		b0mov	_led_mode_sub,a
		jmp	KeyFuntion_Exit

	KeyFuntion_DOUBLE:	//Ë«»÷
		b0bts0	_led_on_off_flag
		jmp	KeyFuntion_DOUBLE_1
		b0bset	_led_on_off_flag
		mov	a,#(5)
		b0mov	_led_mode,a
		mov	a,#(1)
		b0mov	_led_dly,a
		jmp	KeyFuntion_Exit
	KeyFuntion_DOUBLE_1:
		b0bclr	_led_on_off_flag
//		LED_W_OFF
//		LED_R_OFF
//		LED_B_OFF
		jmp	KeyFuntion_Exit
	KeyFuntion_NOKEY:	
	KeyFuntion_Exit:
		ret
//==========================================================
	LED_W_ON	macro
		b0bset	P5.3
		endm
	LED_W_OFF	macro
		b0bclr	P5.3
		endm

	LED_R	equ	P0.2
	LED_R_ON	macro
		b0bset	P0.2
		endm
	LED_R_OFF	macro
		b0bclr	P0.2
		endm

	LED_B	equ	P0.3
	LED_B_ON	macro
		b0bset	P0.3
		endm
	LED_B_OFF	macro
		b0bclr	P0.3
		endm

	LedFunction macro
		b0bts1	_led_on_off_flag
		jmp	LedFunction_Exit

		b0mov	a,_led_mode
		sub	a,#(1)
		@JMP_A	5
		jmp	LedFunction_R_FLASH
		jmp	LedFunction_RB 
		jmp	LedFunction_R
		jmp	LedFunction_B
		jmp	LedFunction_W
	LedFunction_R_FLASH:
		LED_W_OFF
		LED_B_OFF
		//if(--_led_dly != 0)
		//{
		//	jmp	LedFunction_Exit
		//}
		decms	_led_dly
		jmp	LedFunction_Exit
		LED_R_OFF

		b0mov	a,_led_mode_sub
		sub	a,#(1)
		@JMP_A	10
		jmp	LedFunction_R_FLASH_OFF_S_2
		jmp	LedFunction_R_FLASH_ON_S_2
		jmp	LedFunction_R_FLASH_OFF_S_1
		jmp	LedFunction_R_FLASH_ON_S_1
		jmp	LedFunction_R_FLASH_OFF_L_3
		jmp	LedFunction_R_FLASH_ON_L_3
		jmp	LedFunction_R_FLASH_OFF_L_2
		jmp	LedFunction_R_FLASH_ON_L_2
		jmp	LedFunction_R_FLASH_OFF_L_1
		jmp	LedFunction_R_FLASH_ON_L_1
	  LedFunction_R_FLASH_ON_L_1:
	  LedFunction_R_FLASH_ON_L_2:
	  LedFunction_R_FLASH_ON_L_3:
		LED_R_ON
		mov	a,#(10)
		jmp	LedFunction_R_FLASH_END
	  LedFunction_R_FLASH_OFF_L_1:
	  LedFunction_R_FLASH_OFF_L_2:
	  LedFunction_R_FLASH_OFF_L_3:
		mov	a,#(10)
		jmp	LedFunction_R_FLASH_END
	  LedFunction_R_FLASH_ON_S_1:
	  LedFunction_R_FLASH_ON_S_2:
		LED_R_ON
		mov	a,#(3)
		jmp	LedFunction_R_FLASH_END
	  LedFunction_R_FLASH_OFF_S_1:
	  LedFunction_R_FLASH_OFF_S_2:
		mov	a,#(3)
		//jmp	LedFunction_R_FLASH_END
          LedFunction_R_FLASH_END:
		b0mov	_led_dly,a

		//if(--_led_mode_sub == 0)
		//{
		//	_led_mode_sub = 10;
		//}
		decms	_led_mode_sub
		jmp	LedFunction_Exit
		mov	a,#(10)
		b0mov	_led_mode_sub,a
		jmp	LedFunction_Exit
	LedFunction_RB:
		LED_W_OFF
		//if(--_led_dly != 0)
		//{
		//	jmp	LedFunction_Exit
		//}
		decms	_led_dly
		jmp	LedFunction_Exit
		LED_R_OFF
		LED_B_OFF

		b0mov	a,_led_mode_sub
		sub	a,#(1)
		@JMP_A	12
		jmp	LedFunction_RB_B_OFF3
		jmp	LedFunction_RB_B_ON3
		jmp	LedFunction_RB_B_OFF2
		jmp	LedFunction_RB_B_ON2
		jmp	LedFunction_RB_B_OFF1
		jmp	LedFunction_RB_B_ON1
		jmp	LedFunction_RB_R_OFF3
		jmp	LedFunction_RB_R_ON3
		jmp	LedFunction_RB_R_OFF2
		jmp	LedFunction_RB_R_ON2
		jmp	LedFunction_RB_R_OFF1
		jmp	LedFunction_RB_R_ON1
	  LedFunction_RB_R_ON1:
	  LedFunction_RB_R_ON2:
	  LedFunction_RB_R_ON3:
		LED_R_ON
		mov	a,#(3)
		jmp	LedFunction_RB_END
	  LedFunction_RB_R_OFF1:
	  LedFunction_RB_R_OFF2:
		mov	a,#(3)
		jmp	LedFunction_RB_END
	  LedFunction_RB_R_OFF3:
		mov	a,#(10)
		jmp	LedFunction_RB_END
	  LedFunction_RB_B_ON1:
          LedFunction_RB_B_ON2:
          LedFunction_RB_B_ON3:
		LED_B_ON
		mov	a,#(3)
		jmp	LedFunction_RB_END
	  LedFunction_RB_B_OFF1:
	  LedFunction_RB_B_OFF2:
		mov	a,#(3)
		jmp	LedFunction_RB_END
	  LedFunction_RB_B_OFF3:
		mov	a,#(10)
		jmp	LedFunction_RB_END
	  LedFunction_RB_END:
		b0mov	_led_dly,a

		//if(--_led_mode_sub == 0)
		//{
		//	_led_mode_sub = 12;
		//}
		decms	_led_mode_sub
		jmp	LedFunction_Exit
		mov	a,#(12)
		b0mov	_led_mode_sub,a

		jmp	LedFunction_Exit
	LedFunction_R:
		LED_W_OFF
		LED_R_ON
		LED_B_OFF
		jmp	LedFunction_Exit
	LedFunction_B:
		LED_W_OFF
		LED_R_OFF
		LED_B_ON
		jmp	LedFunction_Exit
	LedFunction_W:
		LED_W_ON
		LED_R_OFF
		LED_B_OFF
		jmp	LedFunction_Exit
	LedFunction_Exit:
		endm
//==========================================================
	KeyInit macro
		mov	a,#(0xff)
		b0mov	_key_bit_map,a
		endm
//==========================================================