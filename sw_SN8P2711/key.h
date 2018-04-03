/*
	按键检测与执行
	每50ms执行一次
	采用差值消抖,连续两次采样不一样认为是抖动

	LED灯控制，5种LED模式
*/
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

	//键值定义
	KEY_VALUE_NOKEY		equ	0x10
	KEY_VALUE_OK		equ	0x00

	//按键状态定义
	KEY_STATUS_NOPRESS	equ	0
	KEY_STATUS_PRESS	equ	1
	KEY_STATUS_KEEP		equ	2
	KEY_STATUS_KEEPED	equ	3
	KEY_STATUS_UP		equ	4
	
	KEY_STEP_TICK		equ	SYSTEM_TICK
	KEY_LINK_TICK		equ	(SYSTEM_TICK/3)

	KEY_INPUT		equ	P0.4
//=========================================================
KeyPressFunction macro
	KeyPressFunction_END:
		endm

KeyKeepFunction macro
		b0mov	a,key_hold_tick
		sub	a,#(SYSTEM_TICK*2)
		b0bts0	fc
		jmp	KeyKeepFunction_Long
	KeyKeepFunction_Short:
		jmp	KeyKeepFunction_END
	KeyKeepFunction_Long:
		b0bts0	_key_long_flag
		jmp	KeyKeepFunction_END
		b0bset	_key_long_flag
		b0bts0	_led_on_off_flag
		jmp	KeyKeepFunction_Long_1

		b0bset	_led_on_off_flag
		mov	a,#(5)
		b0mov	_led_mode,a
		jmp	KeyKeepFunction_END
	KeyKeepFunction_Long_1:
		b0bclr	_led_on_off_flag
		LED_W_OFF
		LED_R_OFF
		LED_B_OFF
	KeyKeepFunction_END:
		endm

KeyUpFunction	macro
		//key_hold_tick < SYSTEM_TICK 为短按 否则为长按
		b0mov	a,key_hold_tick
		sub	a,#(SYSTEM_TICK)
		b0bts0	fc
		jmp	KeyUpFunction_long
	KeyUpFunction_Short:
		b0bts1	_led_on_off_flag
		jmp	KeyUpFunction_END
		
		decms	_led_mode
		jmp	KeyUpFunction_Short_MOD1
		mov	a,#(5)
		b0mov	_led_mode,a
	KeyUpFunction_Short_MOD1:
		mov	a,#(1)
		b0mov	_led_dly,a

		//_led_mode == 1(LedFunction_R_FLASH)
		mov	a,#(1)       
		cmprs	a,_led_mode  
		jmp	KeyUpFunction_Short_MOD2

		mov	a,#(10)
		b0mov	_led_mode_sub,a
		jmp	KeyUpFunction_END
	KeyUpFunction_Short_MOD2:
		//_led_mode == 2(LedFunction_RB)
		mov	a,#(2)       
		cmprs	a,_led_mode  
		jmp	KeyUpFunction_END

		mov	a,#(12)
		b0mov	_led_mode_sub,a
		jmp	KeyUpFunction_END
	KeyUpFunction_Long:
	KeyUpFunction_END:
		endm
//==========================================================
	KeyFuntion macro
		b0mov	a,P0
		and	a,#(0x10)
		cmprs	a,key_value_old  	//key_value_old == A ?
		jmp	$+2
		jmp	$+3
		b0mov	key_value_old,a		//key_value_old = A
		jmp	KeyFuntion_END
		
		cmprs	a,#(KEY_VALUE_NOKEY)	//KEY_VALUE_NOKEY == A ?
		jmp	KeyScan_Press
		jmp	KeyScan_Up
	KeyScan_Press:
		b0mov	a,key_value_old
		cmprs	a,key_value_cur		//key_value_cur != key_value_old ?
		jmp	$+2
		jmp	KeyScan_Keep_dly
		b0mov	key_value_cur,a		//key_value_cur = key_value_old
		mov	a,#(KEY_STATUS_PRESS)
		b0mov	key_state_cur,a		//key_state_cur = KEY_STATUS_PRESS
		clr	key_hold_tick		//key_hold_tick = 0
		mov	a,#(KEY_STEP_TICK)	
		b0mov	key_repeat_tick,a	//key_repeat_tick = KEY_STEP_TICK
		 	KeyPressFunction
		jmp	KeyFuntion_END
	KeyScan_Keep_dly:
		//if(++key_hold_time == 0)
		//{
		//	--key_hold_time;
		//}
		incms	key_hold_tick
		jmp	$+3
		decms	key_hold_tick
		nop
		
		//if(--key_repeat_tick != 0)
		//{
		//	return;
		//}
		decms	key_repeat_tick
		jmp	KeyFuntion_END
	KeyScan_Keep:
		mov	a,#(KEY_LINK_TICK)
		b0mov	key_repeat_tick,a	//key_repeat_tick = KEY_LINK_TICK
		b0mov	a,key_state_cur
		cmprs	a,#(KEY_STATUS_PRESS)	//key_state_cur == KEY_STATUS_PRESS ?
		jmp	KeyScan_Keeped
		mov	a,#(KEY_STATUS_KEEP)
		b0mov	key_state_cur,a		//key_state_cur = KEY_STATUS_KEEP
	KeyScan_Keeped:
		 	KeyKeepFunction
		jmp	KeyFuntion_END
	KeyScan_Up:
		b0mov	a,key_value_cur
		cmprs	a,#(KEY_VALUE_NOKEY)	//KEY_VALUE_NOKEY == key_value_cur ?
		jmp	$+2
		jmp	KeyFuntion_END
		mov	a,#(KEY_STATUS_UP)
		b0mov	key_state_cur,a		//key_state_cur = KEY_STATUS_UP
		 	KeyUpFunction
		mov	a,#(KEY_VALUE_NOKEY)
		b0mov	key_value_cur,a
		mov	a,#(KEY_STATUS_NOPRESS)
		b0mov	key_state_cur,a		//key_state_cur = KEY_STATUS_NOPRESS
		clr	key_repeat_tick
		clr	key_hold_tick
		b0bclr	_key_long_flag
	KeyFuntion_END:
		endm
//==========================================================
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
		mov	a,#(3)
		jmp	LedFunction_R_FLASH_END
	  LedFunction_R_FLASH_OFF_S_2:
		mov	a,#(10)
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
		mov	a,#(KEY_VALUE_NOKEY)
		b0mov	key_value_tmp,a
		b0mov	key_value_old,a
		b0mov	key_value_cur,a
		endm
//==========================================================