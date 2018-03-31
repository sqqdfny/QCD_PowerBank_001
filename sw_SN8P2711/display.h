/*
	数码管扫描和显示相关代码
	上电全显2S
	温度数据显示每秒刷新一次
	温度过高显示 "Hi"
	温度过低显示 "Lo"
*/
//==========================================================
//宏定义
	COM_ALL_LOW	macro
		mov	a,#(~(BIT0 | BIT1))
		and	P0,a
		mov	a,#(~(BIT0 | BIT1 | BIT2))
		and	P4,a
		mov	a,#(~(BIT4))
		and	P5,a
		endm
		
	COM_ALL_INPUT	macro
		mov	a,#(~(BIT0 | BIT1))
		and	P0M,a
		mov	a,#(~(BIT0 | BIT1 | BIT2))
		and	P4M,a
		mov	a,#(~(BIT4))
		and	P5M,a
		endm

	COM6_OUT_PP	macro
		b0bset	P0M.1
		endm
	COM6_LOW  	macro	
		b0bclr	P0.1
		endm
	COM6_HIGH  	macro	
		b0bset	P0.1
		endm

	COM5_OUT_PP	macro
		b0bset	P5M.4
		endm
	COM5_LOW  	macro	
		b0bclr	P5.4
		endm
	COM5_HIGH  	macro	
		b0bset	P5.4
		endm

	COM4_OUT_PP	macro
		b0bset	P4M.1
		endm
	COM4_LOW  	macro	
		b0bclr	P4.1
		endm
	COM4_HIGH  	macro	
		b0bset	P4.1
		endm

	COM3_OUT_PP	macro
		b0bset	P4M.0
		endm
	COM3_LOW  	macro	
		b0bclr	P4.0
		endm
	COM3_HIGH  	macro	
		b0bset	P4.0
		endm

	COM2_OUT_PP	macro
		b0bset	P0M.0
		endm
	COM2_LOW  	macro	
		b0bclr	P0.0
		endm
	COM2_HIGH  	macro	
		b0bset	P0.0
		endm

	COM1_OUT_PP	macro
		b0bset	P4M.2
		endm
	COM1_LOW  	macro	
		b0bclr	P4.2
		endm
	COM1_HIGH  	macro	
		b0bset	P4.2
		endm
//==========================================================
//函数

//==========================================================
//此函数在2.5ms中断中调用
	LedScan_ISR macro
		COM_ALL_LOW
		nop
		COM_ALL_INPUT
		
		//if(--_led_scan_index == 0)
		//{
		//	_led_scan_index = 6;
		//}
		decms	_led_scan_index
		jmp	$ + 3
		mov	a,#(6)
		b0mov	_led_scan_index,a
		
		mov	a,_led_scan_index
		sub	a,#(1)
		@JMP_A	6
		jmp	LedScan_ISR_COM1
		jmp	LedScan_ISR_COM2
		jmp	LedScan_ISR_COM3
		jmp	LedScan_ISR_COM4
		jmp	LedScan_ISR_COM5
		jmp	LedScan_ISR_COM6
	LedScan_ISR_COM1:
		b0bts1 _disp_map+0.7
		jmp	$+3
		COM4_HIGH
		COM4_OUT_PP
		
		b0bts1 _disp_map+1.7
		jmp	$+3
		COM5_HIGH
		COM5_OUT_PP
		
		b0bts1 _disp_map+2.7
		jmp	$+3
		COM6_HIGH
		COM6_OUT_PP

		COM3_LOW
		COM3_OUT_PP
		jmp	LedScan_ISR_END
	LedScan_ISR_COM2:
		b0bts1 _disp_map+0.0
		jmp	$+3
		COM2_HIGH
		COM2_OUT_PP
	
		b0bts1 _disp_map+0.1
		jmp	$+3
		COM3_HIGH
		COM3_OUT_PP
	
		b0bts1 _disp_map+0.2
		jmp	$+3
		COM4_HIGH
		COM4_OUT_PP
	
		b0bts1 _disp_map+0.3
		jmp	$+3
		COM5_HIGH
		COM5_OUT_PP
	
		b0bts1 _disp_map+0.4
		jmp	$+3
		COM6_HIGH
		COM6_OUT_PP
		
		COM1_LOW
		COM1_OUT_PP
		jmp	LedScan_ISR_END
	LedScan_ISR_COM3:
		b0bts1 _disp_map+1.0
		jmp	$+3
		COM2_LOW
		COM2_OUT_PP
	
		b0bts1 _disp_map+1.1
		jmp	$+3
		COM3_LOW
		COM3_OUT_PP
	
		b0bts1 _disp_map+1.2
		jmp	$+3
		COM4_LOW
		COM4_OUT_PP
	
		b0bts1 _disp_map+1.3
		jmp	$+3
		COM5_LOW
		COM5_OUT_PP
	
		b0bts1 _disp_map+1.4
		jmp	$+3
		COM6_LOW
		COM6_OUT_PP
		
		COM1_HIGH
		COM1_OUT_PP
		jmp	LedScan_ISR_END
	LedScan_ISR_COM4:
		b0bts1 _disp_map+0.5
		jmp	$+3
		COM3_HIGH
		COM3_OUT_PP

		b0bts1 _disp_map+0.6
		jmp	$+3
		COM4_HIGH
		COM4_OUT_PP

		b0bts1 _disp_map+1.5
		jmp	$+3
		COM5_HIGH
		COM5_OUT_PP

		b0bts1 _disp_map+1.6
		jmp	$+3
		COM6_HIGH
		COM6_OUT_PP
		
		COM2_LOW
		COM2_OUT_PP
		jmp	LedScan_ISR_END
	LedScan_ISR_COM5:
		b0bts1 _disp_map+2.0
		jmp	$+3
		COM3_LOW
		COM3_OUT_PP

		b0bts1 _disp_map+2.1
		jmp	$+3
		COM4_LOW
		COM4_OUT_PP

		b0bts1 _disp_map+2.2
		jmp	$+3
		COM5_LOW
		COM5_OUT_PP

		b0bts1 _disp_map+2.3
		jmp	$+3
		COM6_LOW
		COM6_OUT_PP

		COM2_HIGH
		COM2_OUT_PP
		jmp	LedScan_ISR_END
	LedScan_ISR_COM6:
		b0bts1 _disp_map+2.4
		jmp	$+3
		COM4_LOW
		COM4_OUT_PP

		b0bts1 _disp_map+2.5
		jmp	$+3
		COM5_LOW
		COM5_OUT_PP

		b0bts1 _disp_map+2.6
		jmp	$+3
		COM6_LOW
		COM6_OUT_PP

		COM3_HIGH
		COM3_OUT_PP
	LedScan_ISR_END:
		endm

//==================================================================================================

//==================================================================================================

//==================================================================================================

//==================================================================================================	
//在main中每50ms调用一次
DisplayFuntion	macro
		decms	_disp_swap_dly
		jmp	DisplayFuntion_start_0
		mov	a,#(SYSTEM_TICK * 3)
		b0mov	_disp_swap_dly,a
		
		b0bts1  _input_power_flag
		jmp	$+3
		b0bts1	_disp_voltage_flag
		jmp	$+3
		b0bclr  _disp_voltage_flag
		jmp	$+2
		b0bset	_disp_voltage_flag

	DisplayFuntion_start_0:	
		b0bts0	_lcd_init_flag
		jmp	DisplayFuntion_start
		//if(_disp_update_dly) 
		//{
		//	--_disp_update_dly;
		//	return;
		//}
		mov	a,#(0)
		cmprs	a,_disp_update_dly
		jmp	$ + 2
		jmp	$ + 3
		decms	_disp_update_dly
		jmp	DisplayFuntion_END

		b0bset	_lcd_init_flag
		mov	a,#(20)
		b0mov	_disp_update_dly,a

	DisplayFuntion_start:	
		mov	a,#(0x00)
		b0mov	_disp_map + 0, a
		b0mov	_disp_map + 1, a
		b0mov	_disp_map + 2, a

		b0bts0	_disp_voltage_flag
		jmp	$+3
		mov	a,_cur_batt_capacity
		jmp	$+2
		mov	a,_cur_input_volgate

	DisplayFuntion_bai_wei:
		b0mov	_d_math_input0+0,a
		mov	a,#(100)
		b0mov	_d_math_input1+0,a
		call	__div_u8_u8	
		
		b0mov	Y,#led_num_tab$M
		b0mov	Z,#led_num_tab$L
		b0mov	A,_d_math_output0+0	//百位
		add	Z,A
		movc
		b0mov	_disp_map+0,a
	DisplayFuntion_shi_wei:
		b0mov	a,_d_math_output1+0
		b0mov	_d_math_input0+0,a
		mov	a,#(10)
		b0mov	_d_math_input1+0,a
		call	__div_u8_u8

		b0mov	Y,#led_num_tab$M
		b0mov	Z,#led_num_tab$L
		b0mov	A,_d_math_output0+0	//十位
		add	Z,A
		movc
		b0mov	_disp_map+1,a
	DisplayFuntion_ge_wei:
		b0mov	Z,#led_num_tab$L
		b0mov	A,_d_math_output1+0	//个位
		add	Z,A
		movc
		b0mov	_disp_map+2,a

		b0bts1	_disp_voltage_flag
		jmp	DisplayFuntion_END

		mov	a,#(LCD_DP)
		or	_disp_map+2,a
	DisplayFuntion_END:
		endm
//==================================================================================================
	DisplayInit macro
		mov	a,#(2)
		mov	_led_scan_index,a
		mov	_disp_swap_dly,a

		mov	a,#(SYSTEM_TICK * 2)  //上电全显2S
		b0mov	_disp_update_dly,a
		mov	a,#(0xFF)
		b0mov	_disp_map + 0, a
		b0mov	_disp_map + 1, a
		b0mov	_disp_map + 2, a

		mov	a,#(123)
		b0mov	_cur_input_volgate,a
		mov	a,#(99)
		b0mov	_cur_batt_capacity,a
		endm
//==================================================================================================
/*	_disp_map[3]
	b00->A2  b10->A3  b20->A4
	b01->B2  b11->B3  b21->B4
	b02->C2  b12->C3  b22->C4
	b03->D2  b13->D3  b23->D4
	b04->E2  b14->E3  b24->E4
	b05->F2  b15->F3  b25->F4
	b06->G2  b16->G3  b26->G4
	b07->B1  b17->C1  b27->DP
*/
//==================================================================================================
	LCD_A	equ	0x01
	LCD_B	equ     0x02
	LCD_C	equ     0x04
	LCD_D	equ     0x08
	LCD_E	equ     0x10
	LCD_F	equ     0x20
	LCD_G	equ     0x40
	LCD_H	equ    	0x80
	LCD_DP	equ    	0x80
	led_num_tab:
		dw	LCD_A | LCD_B | LCD_C | LCD_D | LCD_E | LCD_F		/* 0 */
		dw	LCD_B | LCD_C                                          	/* 1 */
		dw	LCD_A | LCD_B | LCD_D | LCD_E | LCD_G                  	/* 2 */
		dw	LCD_A | LCD_B | LCD_C | LCD_D | LCD_G                  	/* 3 */
		dw	LCD_B | LCD_C | LCD_F | LCD_G                         	/* 4 */
		dw	LCD_A | LCD_C | LCD_D | LCD_F | LCD_G                  	/* 5 */
		dw	LCD_A | LCD_C | LCD_D | LCD_E | LCD_F | LCD_G          	/* 6 */
		dw	LCD_A | LCD_B | LCD_C | LCD_F                          	/* 7 */
		dw	LCD_A | LCD_B | LCD_C | LCD_D | LCD_E | LCD_F | LCD_G  	/* 8 */
		dw	LCD_A | LCD_B | LCD_C | LCD_D | LCD_F | LCD_G          	/* 9 */
		//dw	LCD_A | LCD_B | LCD_C | LCD_E | LCD_F | LCD_G          	/* a */
		//dw	LCD_C | LCD_D | LCD_E | LCD_F | LCD_G                  	/* b */
		//dw	LCD_A | LCD_D | LCD_E | LCD_F                          	/* c */
		//dw	LCD_B | LCD_C | LCD_D | LCD_E | LCD_G                  	/* d */
		//dw	LCD_A | LCD_D | LCD_E | LCD_F | LCD_G                  	/* e */
		//dw	LCD_A | LCD_E | LCD_F | LCD_G              		/* f */
//==================================================================================================
//==========================================================
//end files

