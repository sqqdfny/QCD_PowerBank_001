/*
	°´¼üÉ¨ÃèºÍÖ´ÐÐ
*/
//==========================================================
	KeyFuntion macro
		rlcm	_key_bit_map  //×óÒÆÎ»
		b0bts1	P0.4
		jmp	$+3
		b0bset	_key_bit_map.0
		jmp	$+2
		b0bclr	_key_bit_map.0
		
		incs	_key_bit_map
		jmp	$+2
		jmp	KeyFuntion_NOKEY

		mov	a#(0xF0)
		cmprs	a,_key_bit_map
		jmp	$+2
		jmp	KeyFuntion_CLICK
		/*mov	a,#(11111010b)
		cmprs	a,_key_bit_map
		jmp	$+2
		jmp	KeyFuntion_DOUBLE
		mov	a,#(1110110b)
		cmprs	a,_key_bit_map
		jmp	$+2
		jmp	KeyFuntion_DOUBLE
		mov	a,#(11101110b)
		cmprs	a,_key_bit_map
		jmp	$+2
		jmp	KeyFuntion_DOUBLE*/
		
		mov	a,#(11110010b)
		cmprs	a,_key_bit_map
		jmp	$+2
		jmp	KeyFuntion_DOUBLE
		mov	a,#(1100110b)
		cmprs	a,_key_bit_map
		jmp	$+2
		jmp	KeyFuntion_DOUBLE
		mov	a,#(11001110b)
		cmprs	a,_key_bit_map
		jmp	$+2
		jmp	KeyFuntion_DOUBLE

		/*mov	a,#(11100010b)
		cmprs	a,_key_bit_map
		jmp	$+2
		jmp	KeyFuntion_DOUBLE*/
		mov	a,#(11000110b)
		cmprs	a,_key_bit_map
		jmp	$+2
		jmp	KeyFuntion_DOUBLE
		mov	a,#(10001110b)
		cmprs	a,_key_bit_map
		jmp	$+2
		jmp	KeyFuntion_DOUBLE
		jmp	KeyFuntion_NOKEY

		mov	a,#(00011110b)
		cmprs	a,_key_bit_map
		jmp	$+2
		jmp	KeyFuntion_DOUBLE
		mov	a,#(10011110b)
		cmprs	a,_key_bit_map
		jmp	$+2
		jmp	KeyFuntion_DOUBLE
		mov	a,#(00001110b)
		cmprs	a,_key_bit_map
		jmp	$+2
		jmp	KeyFuntion_DOUBLE	
	KeyFuntion_CLICK:	//µ¥»÷
		b0bts1	_led_on_off_flag
		jmp	KeyFuntion_Exit
		decms	_led_mode
		jmp	KeyFuntion_Exit
		mov	a,#(5)
		b0mov	_led_mode,a
		b0mov	_led_dly,a
		jmp	KeyFuntion_Exit
	KeyFuntion_DOUBLE:	//Ë«»÷
		b0bts0	_led_on_off_flag
		jmp	KeyFuntion_DOUBLE_1
		b0bset	_led_on_off_flag
		mov	a,#(5)
		b0mov	_led_mode,a
		b0mov	_led_dly,a
		jmp	KeyFuntion_Exit
	KeyFuntion_DOUBLE_1:
		b0bclr	_led_on_off_flag
		jmp	KeyFuntion_Exit
	KeyFuntion_NOKEY:	
	KeyFuntion_Exit:
		endm
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

	LedFunction:
		mov	a,_led_mode
		sub	a,#(1)
		@JMP_A	5
		jmp	LedFunction_R_FLASH
		jmp	LedFunction_RB 
		jmp	LedFunction_R
		jmp	LedFunction_B
		jmp	LedFunction_W
	LedFunction_R_FLASH:
		LED_W_OFF
		decms	_led_dly
		jmp	LedFunction_Exit
		mov	a,#(SYSTEM_TICK)
		b0mov	_led_dly,a

		jmp	LedFunction_Exit
	LedFunction_RB:
		LED_W_OFF
		decms	_led_dly
		jmp	LedFunction_Exit
		mov	a,#(SYSTEM_TICK)
		b0mov	_led_dly,a

		b0bts0	LED_R
		jmp	LedFunction_RB_B_ON
	  LedFunction_RB_R_ON:
		LED_R_ON
		LED_B_OFF
		jmp	LedFunction_Exit
	  LedFunction_RB_B_ON:
		LED_R_OFF
		LED_B_ON
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
		ret
//==========================================================
	KeyInit macro
		mov	a,#(1)
		b0mov	_led_mode,a
		endm
//==========================================================