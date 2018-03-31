/*

*/
//========================================================================================
	//结果存入 _ad_buf_tmp
	SensorADConvert_CH	macro	ch
		mov	a,#(ch | BIT4 | BIT7)
		b0mov	ADM,a
		call	SensorADConvert
		endm
//========================================================================================
	AdcDelay:
		mov	a,#(50)
		b0mov	_adc_delay_count,a
		decms	_adc_delay_count
		jmp	$ - 1
		ret
//========================================================================================
	//启动一次ADC转换
	//结果存入 _ad_buf_tmp
	SensorADConvert:
		MOV A, #(0x50)
  		B0MOV ADR, A	//ADC clock: 2MHZ,ADC resolution: -1
  		MOV A, #(0x00)
  		B0MOV VREFH, A	//Internal VREFH: 2V

		call	AdcDelay
		b0bset	FADS			//start adc
	SensorADConvert_wait_adc:
		b0bts1	FEOC			//检查ADC状态标志
   		jmp	SensorADConvert_wait_adc	//eco=0：AD转换中
		b0mov	a,ADB
		b0mov	_ad_buf_tmp + 1,a
		b0mov	a,ADR
		b0mov	_ad_buf_tmp + 0,a
		rlcm	_ad_buf_tmp + 0		//低位带进位左移
		rlcm	_ad_buf_tmp + 0		//低位带进位左移
		rlcm	_ad_buf_tmp + 0		//低位带进位左移
		rlcm	_ad_buf_tmp + 0		//低位带进位左移
		rrcm	_ad_buf_tmp + 1		//高位带进位右移
		rrcm	_ad_buf_tmp + 0		//低位带进位右移
		rrcm	_ad_buf_tmp + 1		//高位带进位右移
		rrcm	_ad_buf_tmp + 0		//低位带进位右移
		rrcm	_ad_buf_tmp + 1		//高位带进位右移
		rrcm	_ad_buf_tmp + 0		//低位带进位右移
		rrcm	_ad_buf_tmp + 1		//高位带进位右移
		rrcm	_ad_buf_tmp + 0		//低位带进位右移
		mov	a,#(0x0f)
		and	_ad_buf_tmp + 1,a	//把高位的高4位清零
		ret
//========================================================================================
/*	//对同一个通道6次采样求平均(去掉最大值和最小值)
	//结果存入 _ad_buf_tmp
	SensorAdAverage:
		//if(++_ad_buf_index < 6) return
		incms	_ad_buf_index
		nop
		b0mov	a,_ad_buf_index
		sub	a,#(6)
		b0bts1	fc
		jmp	$+2  		//<6
		clr	_ad_buf_index
		
		call	SensorADConvert
		//_ad_buf_l[_ad_buf_index] = _ad_buf_tmp+0
		b0mov	Y,#(0)
		b0mov	Z,#_ad_buf_l$L
		b0mov	A,_ad_buf_index
		add	Z,A
		b0mov	a,_ad_buf_tmp+0
		b0mov	@YZ,a

		//_ad_buf_h[_ad_buf_index] = _ad_buf_tmp+1
		//b0mov	Y,#(0)
		b0mov	Z,#_ad_buf_h$L
		b0mov	A,_ad_buf_index
		add	Z,A
		b0mov	a,_ad_buf_tmp+1
		b0mov	@YZ,a
	//======================================================================
	//复制数据到排序缓冲区
		b0mov	a,_ad_buf_l+0
		b0mov	_ad_buf_l_sort+0,a
		b0mov	a,_ad_buf_l+1
		b0mov	_ad_buf_l_sort+1,a
		b0mov	a,_ad_buf_l+2
		b0mov	_ad_buf_l_sort+2,a
		b0mov	a,_ad_buf_l+3
		b0mov	_ad_buf_l_sort+3,a
		b0mov	a,_ad_buf_l+4
		b0mov	_ad_buf_l_sort+4,a
		b0mov	a,_ad_buf_l+5
		b0mov	_ad_buf_l_sort+5,a

		b0mov	a,_ad_buf_h+0
		b0mov	_ad_buf_h_sort+0,a
		b0mov	a,_ad_buf_h+1
		b0mov	_ad_buf_h_sort+1,a
		b0mov	a,_ad_buf_h+2
		b0mov	_ad_buf_h_sort+2,a
		b0mov	a,_ad_buf_h+3
		b0mov	_ad_buf_h_sort+3,a
		b0mov	a,_ad_buf_h+4
		b0mov	_ad_buf_h_sort+4,a
		b0mov	a,_ad_buf_h+5
		b0mov	_ad_buf_h_sort+5,a
	//======================================================================
	//排序
		mov	a,#(5)
		b0mov	_ad_buf_sort_i,a
   	SensorAdAverage_comp_01:
		comparison_16b	_ad_buf_l_sort+0,_ad_buf_h_sort+0,_ad_buf_l_sort+1,_ad_buf_h_sort+1,SensorAdAverage_swap_01,SensorAdAverage_comp_12
	SensorAdAverage_swap_01:
		swap_16b	_ad_buf_l_sort+0,_ad_buf_h_sort+0,_ad_buf_l_sort+1,_ad_buf_h_sort+1
	SensorAdAverage_comp_12:
		comparison_16b	_ad_buf_l_sort+1,_ad_buf_h_sort+1,_ad_buf_l_sort+2,_ad_buf_h_sort+2,SensorAdAverage_swap_12,SensorAdAverage_comp_23
	SensorAdAverage_swap_12:
		swap_16b	_ad_buf_l_sort+1,_ad_buf_h_sort+1,_ad_buf_l_sort+2,_ad_buf_h_sort+2
	SensorAdAverage_comp_23:
		comparison_16b	_ad_buf_l_sort+2,_ad_buf_h_sort+2,_ad_buf_l_sort+3,_ad_buf_h_sort+3,SensorAdAverage_swap_23,SensorAdAverage_comp_34
	SensorAdAverage_swap_23:
		swap_16b	_ad_buf_l_sort+2,_ad_buf_h_sort+2,_ad_buf_l_sort+3,_ad_buf_h_sort+3
	SensorAdAverage_comp_34:
		comparison_16b	_ad_buf_l_sort+3,_ad_buf_h_sort+3,_ad_buf_l_sort+4,_ad_buf_h_sort+4,SensorAdAverage_swap_34,SensorAdAverage_comp_45
	SensorAdAverage_swap_34:
		swap_16b	_ad_buf_l_sort+3,_ad_buf_h_sort+3,_ad_buf_l_sort+4,_ad_buf_h_sort+4
	SensorAdAverage_comp_45:
		comparison_16b	_ad_buf_l_sort+4,_ad_buf_h_sort+4,_ad_buf_l_sort+5,_ad_buf_h_sort+5,SensorAdAverage_swap_45,SensorAdAverage_comp_56
	SensorAdAverage_swap_45:
		swap_16b	_ad_buf_l_sort+4,_ad_buf_h_sort+4,_ad_buf_l_sort+5,_ad_buf_h_sort+5
	SensorAdAverage_comp_56:
		decms	_ad_buf_sort_i
		jmp	SensorAdAverage_comp_01
	//======================================================================
	//求中间四次的平均值,去掉最大的1个和最小的1个数
		clr	_ad_buf_tmp+0	
		clr	_ad_buf_tmp+1
		add_16b	_ad_buf_tmp+0,_ad_buf_tmp+1,_ad_buf_l_sort+1,_ad_buf_h_sort+1
		add_16b	_ad_buf_tmp+0,_ad_buf_tmp+1,_ad_buf_l_sort+2,_ad_buf_h_sort+2
		add_16b	_ad_buf_tmp+0,_ad_buf_tmp+1,_ad_buf_l_sort+3,_ad_buf_h_sort+3
		add_16b	_ad_buf_tmp+0,_ad_buf_tmp+1,_ad_buf_l_sort+4,_ad_buf_h_sort+4
	
		rrcm	_ad_buf_tmp+1	;高位带进位右移
   		rrcm	_ad_buf_tmp+0	;低位带进位右移
   		rrcm	_ad_buf_tmp+1	;高位带进位右移
   		rrcm	_ad_buf_tmp+0	;低位带进位右移
   		mov	a,#(0x0F) 	;把高位的左4为置0
   		and	_ad_buf_tmp+1,a
		ret*/
//========================================================================================
//计算电池电量
/*	//AD = 256 * V(0.1V) / 5  
	//4096 = 256 * 80 / 5
	batt_capacity_tab_add:
		dw	2150	//4.2
		dw	2110
		dw	2070
		dw	2030
		dw	1990
		dw	1950
		dw	1910
		dw	1870	
		dw	1830
		dw	1790
		dw	1750	//3.42
	batt_capacity_tab_dec:
		dw	2090	//4.08
		dw	2040
		dw	1990
		dw	1940
		dw	1890
		dw	1840
		dw	1790
		dw	1740
		dw	1690
		dw	1640
		dw	1590	//3.1
*/
	SensorMathBattCapacity:
		b0bts1	_input_power_flag
		jmp	SensorMathBattCapacity_DEC
//==================================================================================================
		comparison_16b	#(2155&0xff),#((2155>>8)&0xff),_ad_buf_tmp+0,_ad_buf_tmp+1,SensorMathBattCapacity_ADD_nofull,SensorMathBattCapacity_ADD_full
	SensorMathBattCapacity_ADD_full:
		mov	a,#(100)
		b0mov	_cur_batt_capacity,a
		jmp	SensorMathBattCapacity_Exit
	SensorMathBattCapacity_ADD_nofull:
		comparison_16b	#(2150&0xff),#((2150>>8)&0xff),_ad_buf_tmp+0,_ad_buf_tmp+1,SensorMathBattCapacity_ADD_90,SensorMathBattCapacity_ADD_99
	SensorMathBattCapacity_ADD_99:
		mov	a,#(99)
		b0mov	_batt_capacity_tmp,a
		jmp	SensorMathBattCapacity_ADD_End
	SensorMathBattCapacity_ADD_90:
		comparison_16b	#(2110&0xff),#((2110>>8)&0xff),_ad_buf_tmp+0,_ad_buf_tmp+1,SensorMathBattCapacity_ADD_80,SensorMathBattCapacity_ADD_90_more
	SensorMathBattCapacity_ADD_90_more:
		mov	a,#(90)
		b0mov	_batt_capacity_tmp,a
		jmp	SensorMathBattCapacity_ADD_End
	SensorMathBattCapacity_ADD_80:
		comparison_16b	#(2070&0xff),#((2070>>8)&0xff),_ad_buf_tmp+0,_ad_buf_tmp+1,SensorMathBattCapacity_ADD_70,SensorMathBattCapacity_ADD_80_more
	SensorMathBattCapacity_ADD_80_more:
		mov	a,#(80)
		b0mov	_batt_capacity_tmp,a
		jmp	SensorMathBattCapacity_ADD_End
	SensorMathBattCapacity_ADD_70:
		comparison_16b	#(2030&0xff),#((2030>>8)&0xff),_ad_buf_tmp+0,_ad_buf_tmp+1,SensorMathBattCapacity_ADD_60,SensorMathBattCapacity_ADD_70_more
	SensorMathBattCapacity_ADD_70_more:
		mov	a,#(70)
		b0mov	_batt_capacity_tmp,a
		jmp	SensorMathBattCapacity_ADD_End
	SensorMathBattCapacity_ADD_60:
		comparison_16b	#(1990&0xff),#((1990>>8)&0xff),_ad_buf_tmp+0,_ad_buf_tmp+1,SensorMathBattCapacity_ADD_50,SensorMathBattCapacity_ADD_60_more
	SensorMathBattCapacity_ADD_60_more:
		mov	a,#(60)
		b0mov	_batt_capacity_tmp,a
		jmp	SensorMathBattCapacity_ADD_End
	SensorMathBattCapacity_ADD_50:
		comparison_16b	#(1950&0xff),#((1950>>8)&0xff),_ad_buf_tmp+0,_ad_buf_tmp+1,SensorMathBattCapacity_ADD_40,SensorMathBattCapacity_ADD_50_more
	SensorMathBattCapacity_ADD_50_more:
		mov	a,#(50)
		b0mov	_batt_capacity_tmp,a
		jmp	SensorMathBattCapacity_ADD_End
	SensorMathBattCapacity_ADD_40:
		comparison_16b	#(1910&0xff),#((1910>>8)&0xff),_ad_buf_tmp+0,_ad_buf_tmp+1,SensorMathBattCapacity_ADD_30,SensorMathBattCapacity_ADD_40_more
	SensorMathBattCapacity_ADD_40_more:
		mov	a,#(40)
		b0mov	_batt_capacity_tmp,a
		jmp	SensorMathBattCapacity_ADD_End
	SensorMathBattCapacity_ADD_30:
		comparison_16b	#(1870&0xff),#((1870>>8)&0xff),_ad_buf_tmp+0,_ad_buf_tmp+1,SensorMathBattCapacity_ADD_20,SensorMathBattCapacity_ADD_30_more
	SensorMathBattCapacity_ADD_30_more:
		mov	a,#(30)
		b0mov	_batt_capacity_tmp,a
		jmp	SensorMathBattCapacity_ADD_End
	SensorMathBattCapacity_ADD_20:
		comparison_16b	#(1830&0xff),#((1830>>8)&0xff),_ad_buf_tmp+0,_ad_buf_tmp+1,SensorMathBattCapacity_ADD_10,SensorMathBattCapacity_ADD_20_more
	SensorMathBattCapacity_ADD_20_more:
		mov	a,#(20)
		b0mov	_batt_capacity_tmp,a
		jmp	SensorMathBattCapacity_ADD_End
	SensorMathBattCapacity_ADD_10:
		comparison_16b	#(1790&0xff),#((1790>>8)&0xff),_ad_buf_tmp+0,_ad_buf_tmp+1,SensorMathBattCapacity_ADD_0,SensorMathBattCapacity_ADD_10_more
	SensorMathBattCapacity_ADD_10_more:
		mov	a,#(10)
		b0mov	_batt_capacity_tmp,a
		jmp	SensorMathBattCapacity_ADD_End
	SensorMathBattCapacity_ADD_0:
		comparison_16b	#(1750&0xff),#((1750>>8)&0xff),_ad_buf_tmp+0,_ad_buf_tmp+1,SensorMathBattCapacity_ADD_0_less,SensorMathBattCapacity_ADD_0_more
	SensorMathBattCapacity_ADD_0_more:
		mov	a,#(1)
		b0mov	_batt_capacity_tmp,a
		jmp	SensorMathBattCapacity_ADD_End
	SensorMathBattCapacity_ADD_0_less:
		mov	a,#(0)
		b0mov	_batt_capacity_tmp,a
	SensorMathBattCapacity_ADD_End:
		b0mov	a,_batt_capacity_tmp
		sub	a,_cur_batt_capacity
		b0bts1	fc
		jmp	SensorMathBattCapacity_Exit
		b0mov	a,_batt_capacity_tmp
		b0mov	_cur_batt_capacity,a
		jmp	SensorMathBattCapacity_Exit
//==================================================================================================
	SensorMathBattCapacity_DEC:
	comparison_16b	#(2095&0xff),#((2095>>8)&0xff),_ad_buf_tmp+0,_ad_buf_tmp+1,SensorMathBattCapacity_DEC_nofull,SensorMathBattCapacity_DEC_full
	SensorMathBattCapacity_DEC_full:
		mov	a,#(100)
		b0mov	_batt_capacity_tmp,a
		jmp	SensorMathBattCapacity_DEC_End
	SensorMathBattCapacity_DEC_nofull:
		comparison_16b	#(2090&0xff),#((2090>>8)&0xff),_ad_buf_tmp+0,_ad_buf_tmp+1,SensorMathBattCapacity_DEC_90,SensorMathBattCapacity_DEC_99
	SensorMathBattCapacity_DEC_99:
		mov	a,#(99)
		b0mov	_batt_capacity_tmp,a
		jmp	SensorMathBattCapacity_DEC_End
	SensorMathBattCapacity_DEC_90:
		comparison_16b	#(2040&0xff),#((2040>>8)&0xff),_ad_buf_tmp+0,_ad_buf_tmp+1,SensorMathBattCapacity_DEC_80,SensorMathBattCapacity_DEC_90_more
	SensorMathBattCapacity_DEC_90_more:
		mov	a,#(90)
		b0mov	_batt_capacity_tmp,a
		jmp	SensorMathBattCapacity_DEC_End
	SensorMathBattCapacity_DEC_80:
		comparison_16b	#(1990&0xff),#((1990>>8)&0xff),_ad_buf_tmp+0,_ad_buf_tmp+1,SensorMathBattCapacity_DEC_70,SensorMathBattCapacity_DEC_80_more
	SensorMathBattCapacity_DEC_80_more:
		mov	a,#(80)
		b0mov	_batt_capacity_tmp,a
		jmp	SensorMathBattCapacity_DEC_End
	SensorMathBattCapacity_DEC_70:
		comparison_16b	#(1940&0xff),#((1940>>8)&0xff),_ad_buf_tmp+0,_ad_buf_tmp+1,SensorMathBattCapacity_DEC_60,SensorMathBattCapacity_DEC_70_more
	SensorMathBattCapacity_DEC_70_more:
		mov	a,#(70)
		b0mov	_batt_capacity_tmp,a
		jmp	SensorMathBattCapacity_DEC_End
	SensorMathBattCapacity_DEC_60:
		comparison_16b	#(1890&0xff),#((1890>>8)&0xff),_ad_buf_tmp+0,_ad_buf_tmp+1,SensorMathBattCapacity_DEC_50,SensorMathBattCapacity_DEC_60_more
	SensorMathBattCapacity_DEC_60_more:
		mov	a,#(60)
		b0mov	_batt_capacity_tmp,a
		jmp	SensorMathBattCapacity_DEC_End
	SensorMathBattCapacity_DEC_50:
		comparison_16b	#(1840&0xff),#((1840>>8)&0xff),_ad_buf_tmp+0,_ad_buf_tmp+1,SensorMathBattCapacity_DEC_40,SensorMathBattCapacity_DEC_50_more
	SensorMathBattCapacity_DEC_50_more:
		mov	a,#(50)
		b0mov	_batt_capacity_tmp,a
		jmp	SensorMathBattCapacity_DEC_End
	SensorMathBattCapacity_DEC_40:
		comparison_16b	#(1790&0xff),#((1790>>8)&0xff),_ad_buf_tmp+0,_ad_buf_tmp+1,SensorMathBattCapacity_DEC_30,SensorMathBattCapacity_DEC_40_more
	SensorMathBattCapacity_DEC_40_more:
		mov	a,#(40)
		b0mov	_batt_capacity_tmp,a
		jmp	SensorMathBattCapacity_DEC_End
	SensorMathBattCapacity_DEC_30:
		comparison_16b	#(1740&0xff),#((1740>>8)&0xff),_ad_buf_tmp+0,_ad_buf_tmp+1,SensorMathBattCapacity_DEC_20,SensorMathBattCapacity_DEC_30_more
	SensorMathBattCapacity_DEC_30_more:
		mov	a,#(30)
		b0mov	_batt_capacity_tmp,a
		jmp	SensorMathBattCapacity_DEC_End
	SensorMathBattCapacity_DEC_20:
		comparison_16b	#(1690&0xff),#((1690>>8)&0xff),_ad_buf_tmp+0,_ad_buf_tmp+1,SensorMathBattCapacity_DEC_10,SensorMathBattCapacity_DEC_20_more
	SensorMathBattCapacity_DEC_20_more:
		mov	a,#(20)
		b0mov	_batt_capacity_tmp,a
		jmp	SensorMathBattCapacity_DEC_End
	SensorMathBattCapacity_DEC_10:
		comparison_16b	#(1640&0xff),#((1640>>8)&0xff),_ad_buf_tmp+0,_ad_buf_tmp+1,SensorMathBattCapacity_DEC_0,SensorMathBattCapacity_DEC_10_more
	SensorMathBattCapacity_DEC_10_more:
		mov	a,#(10)
		b0mov	_batt_capacity_tmp,a
		jmp	SensorMathBattCapacity_DEC_End
	SensorMathBattCapacity_DEC_0:
		comparison_16b	#(1590&0xff),#((1590>>8)&0xff),_ad_buf_tmp+0,_ad_buf_tmp+1,SensorMathBattCapacity_DEC_0_less,SensorMathBattCapacity_DEC_0_more
	SensorMathBattCapacity_DEC_0_more:
		mov	a,#(1)
		b0mov	_batt_capacity_tmp,a
		jmp	SensorMathBattCapacity_DEC_End
	SensorMathBattCapacity_DEC_0_less:
		mov	a,#(0)
		b0mov	_batt_capacity_tmp,a
	SensorMathBattCapacity_DEC_End:
		b0mov	a,_batt_capacity_tmp
		sub	a,_cur_batt_capacity
		b0bts0	fc
		jmp	SensorMathBattCapacity_Exit
		b0mov	a,_batt_capacity_tmp
		b0mov	_cur_batt_capacity,a	
//==================================================================================================
	SensorMathBattCapacity_Exit:		
		ret
//==================================================================================================
//
SensorFuntion macro
		
	
		endm
//========================================================================================
	SensorInit macro
		endm
//========================================================================================
//end files



