CHIP SN8P2711B
//{{SONIX_CODE_OPTION
	.Code_Option	LVD		LVD_H		; 2.4V Reset Enable LVD36 bit of PFLAG for 3.6V Low Voltage Indicator
	.Code_Option	Reset_Pin	P04
	.Code_Option	Watch_Dog	Always_On	; Watchdog still enable even in Green and Sleep mode
	.Code_Option	High_Clk	IHRC_16M	; Internal 16M RC Oscillator
	.Code_Option	Fcpu		#2     ; Fcpu = Fosc/4
	.Code_Option	Security	Enable
	.Code_Option	Noise_Filter	Disable
//}}SONIX_CODE_OPTION
	INCLUDESTD		MACRO1.H
	INCLUDESTD		MACRO2.H
	INCLUDESTD	 	MACRO3.H
	
.data
	BIT0		equ	0x01
	BIT1		equ	0x02
	BIT2		equ	0x04
	BIT3		equ	0x08
	BIT4		equ	0x10
	BIT5		equ	0x20
	BIT6		equ	0x40
	BIT7		equ	0x80
	SYSTEM_TICK	equ	20	//50ms中断

	flag0			ds	1
	system_idle_flag	equ	flag0.0		//0->正常工作 1->待机模式
	system_tick_flag	equ	flag0.1		//50ms置位一次
	_disp_update_flag	equ	flag0.2		//刷新显示标志
	_disp_voltage_flag	equ	flag0.3		//0->显示电量  1->显示电压
	_input_power_flag	equ	flag0.4		//0->没有外部电源 1->有外部电源
	_led_on_off_flag	equ	flag0.5		//0->LED_OFF	1->LED_ON
	_lcd_init_flag		equ	flag0.6		//此标志为1表示LCD上电全显已执行过
	_is_output_en_flag	equ	flag0.7		//0->升压输出未开启 1->升压输出开启

	flag1			ds	1
	_key_dly_flag		equ	flag1.0

	tmp			ds	1		//临时变量
	_50ms_count		ds	1		//50ms计时
	_enter_idle_dly		ds	1		//按键唤醒后,延时进入IDLE
	
	//math.h
	_d_math_input0		ds	3     //被乘数 被除数
	_d_math_input1		ds	3     //乘数   除数
	_d_math_output0		ds 	3     //积     商
	_d_math_output1		ds	3     //余数 
	_count			ds	1     //循环移位计数器
	_math_tmp0		ds	1

	//display.h
	_disp_swap_dly		ds	1	//电压和电量交替显示间隔计时
	_disp_update_dly	ds	1	//显示更新延时,避免数据刷新过快
	_disp_map		ds	3	//显存映射
	_led_scan_index		ds	1	//当前扫描的COM

	//sensor.h
	_adc_ch_index		ds	1	//当前ADC通道
	_ad_buf_tmp		ds	2	//AD转换的结果缓冲,主要是用于做移位和对齐
	//_ad_buf_l		ds	6	//此数据用于做平均值滤波,去掉最大值与最小值
	//_ad_buf_h		ds	6
	//_ad_buf_l_sort		ds	6	//此数据用于做平均值滤波,去掉最大值与最小值
	//_ad_buf_h_sort		ds	6
	_ad_buf_index		ds	1	//
	_ad_buf_sort_i		ds	1	//用于对多组AD数据进行排序
	_cur_input_volgate	ds	1	//当前输入电压
	_cur_batt_capacity	ds	1	//当前电池电量
	_batt_capacity_tmp	ds	1
	
	_adc_delay_count	ds	1	//转换前的延时计数
	_adc_ch_dly		ds	1	//通道采集切换延时
	
	//key.h
	_key_bit_map		ds	1	//每50MS采集一次按键状态,移位存入,挤掉最旧的位
	_key_dly		ds	1	//检测到一次按键后,延时一段时间再检测按键
	_led_mode		ds	1	//LED模式
	_led_mode_sub		ds	1	//对应模式下的亮灯控制
	_led_dly		ds	1	//LED控制延时

.code
  org 0x00
  jmp reset
  org 0x08
  jmp isr
  org 0x20

	include math.h
	include sensor.h
	include key.h
	include display.h
//==============================================================================
;-------------------------------------------------------------------------------
; ClrRAM
; Use index @YZ to clear RAM 
;-------------------------------------------------------------------------------
__ClrRAM macro
	CLR 		Y
	B0MOV		Z,#0x3f
ClrRAM10:
	CLR 		@YZ
	DECMS		Z
	JMP 		ClrRAM10
	CLR 		@YZ
	endm

reset:
//_IO_Setting:
 	MOV A, #(0x0F)
  	B0MOV P0M, A	//P0 Input-Output value
  	MOV A, #(0x00)
  	B0MOV P0UR, A	//P0 pull up value
  	MOV A, 0x00
  	B0MOV P0, A	//P0 register value

  	MOV A, #(0x07)
  	B0MOV P4M, A		//P4 Input-Output value
  	MOV A, #(0x00)
  	B0MOV P4UR, A		//P4 pull up value
  	MOV A, #(0x08)
  	B0MOV P4CON, A		//P4 Analog-Digital value
  	MOV A, 0x00
  	B0MOV P4, A		//P4 register value

  	MOV A, #(0x18)
  	B0MOV P5M, A		//P5 Input-Output value
  	MOV A, #(0x00)
  	B0MOV P5UR, A		//P5 pull up value
  	MOV A, 0x00
  	B0MOV P5, A	        //P5 register value
//_Timer_Setting:
	MOV A, #0x24
	B0MOV TC0M, A	//TC0M register value, Fcpu: 4.000000MHZ
	B0BCLR FTC0X8 //CLK source: Fcpu
	MOV A, #(0x64)
	B0MOV TC0C, A	//TC0C register value, time: 2.500000ms(400.000000HZ)
	MOV A, #(0x64)
	B0MOV TC0R, A	//TC0R register value
	B0BCLR FTC0IRQ
	B0BSET FTC0IEN	//TC0 Interrupt enable
	B0BCLR FTC0CKS
	B0BSET FTC0ENB	//TC0 enable
	B0BSET FTC0GN
	
	__ClrRAM
	SensorInit
	KeyInit
	DisplayInit

  	B0BSET FGIE
	b0bclr	system_idle_flag
	mov	a,#(SYSTEM_TICK * 5)
	b0mov	_enter_idle_dly,a
public _main
_main:
	MOV 	A, #(0x5A)
  	B0MOV 	WDTR, A
	
	b0bts0  system_idle_flag
	jmp	_main_system_idle_function
//====================================================================
//NORMAL
  	b0bts1	system_tick_flag
	jmp	_main
	b0bclr	system_tick_flag
//================================================
		SensorFuntion
	call	KeyFuntion
		LedFunction
		DisplayFuntion
		//GreenMode
//================================================
		b0bts0	_input_power_flag
		jmp	_main
		b0bts0	_led_on_off_flag
		jmp	_main
		b0bts0	_is_output_en_flag
		jmp	_main

		decms	_enter_idle_dly
		jmp	_main
		
_main_system_normal_function_enter_idle:
		b0bclr	FTC0ENB			//stop timer
		COM_ALL_LOW			//turnoff disp

		mov	a,#(0)
		b0mov	ADM,a

		b0bset	system_idle_flag
		SlowMode
		jmp	_main
//====================================================================
//IDLE
_main_system_idle_function:
        	call	SensorFuntion_IDLE

		b0bts1	KEY_INPUT
		jmp	_main_system_idle_function_exit_idle_key
		b0bts0	_input_power_flag
		jmp	_main_system_idle_function_exit_idle
		b0bts0	_led_on_off_flag
		jmp	_main_system_idle_function_exit_idle
		b0bts0	_is_output_en_flag
		jmp	_main_system_idle_function_exit_idle
		jmp	_main
_main_system_idle_function_exit_idle_key:
		mov	a,#(SYSTEM_TICK * 5)
		b0mov	_enter_idle_dly,a
_main_system_idle_function_exit_idle:
		Slow2Normal
		b0bclr	system_idle_flag
		b0bset 	FTC0ENB			//start timer
  	JMP _main

isr:
	push
	b0bts1	FTC0IRQ
        jmp	isr_exit
	b0bclr  FTC0IRQ
	
	LedScan_ISR
	
	//if(_50ms_count >= 20)
	//{
	//	_50ms_count = 0;
	//	system_tick_flag = 1
	//}
	//else
	//{
	//	++_50ms_count;
	//}
   isr_50ms_start:
	b0mov	a,_50ms_count
	sub	a,#(20)       	//2.5ms * 20 = 50ms
	b0bts1  fc		
        jmp	isr_50ms_1	//<20
	clr	_50ms_count
	b0bset	system_tick_flag
   isr_50ms_1:
	//++_50ms_count
	incms _50ms_count
	nop
   isr_50ms_end:
isr_exit:
	pop
	reti


.ENDP