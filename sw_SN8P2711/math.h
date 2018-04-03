/*
	数学运算库
*/
//==========================================================
//比较高低字节,不改变原数据
//num1-num2
//>=0: jmp->cjmp_1
//<0 : jmp->cjmp_2
//tmp0
comparison_16b macro  num1_l,num1_h,num2_l,num2_h,cjmp_1,cjmp_2
   	mov  	a,num1_l	//先减低字节
   	sub    	a,num2_l	
   	mov  	a,num1_h	//再减高字节
   	sbc    	a,num2_h	//
   	b0bts0 	FC         	//
   	jmp    	cjmp_1     	//如果等于0就 >=
   	jmp    	cjmp_2     	//如果等于1就 <
   	endm
//==================================================================================================
//交换两个16位数据
swap_16b macro num1_l,num1_h,num2_l,num2_h
	b0mov	a,num1_l
	b0xch	a,num2_l
	b0xch	a,num1_l
	b0mov	a,num1_h
	b0xch	a,num2_h
	b0xch	a,num1_h
	endm
//==================================================================================================
//num ++
cnt_inc_16b macro  num_l,num_h
   	incms	num1_l
   	jmp	$ + 3
   	incms	num_h
   	nop
   	endm
//==================================================================================================
// num1 = num1 + num2
add_16b macro num1_l,num1_h,num2_l,num2_h
   	b0mov	a,num2_l
   	add	num1_l,a
   	b0mov	a,num2_h
   	adc	num1_h,a
   	endm
//==================================================================================================
// num1 = num1 - num2
sub_16b macro num1_l,num1_h,num2_l,num2_h
   	b0mov	a,num1_l
   	sub	a,num2_l
	b0mov	num1_l,a
   	b0mov	a,num1_h
   	sbc	a,num2_h
	b0mov	num1_h,a
   	endm
//==================================================================================================
	__div_u8_u8:
	//8位数除法
	//d_math_input0  = 被除数	
	//d_math_input1  = 除数		
	//d_math_output0 = 商	
	//d_math_output1 = 中间变量(余数)   	
		   clr		_d_math_output0    	//清0商
		   clr		_d_math_output1		//清0中间变量(余数)   
		   clr      _count
		div_u8_u8_0:
		   incms	_count	 			//count加一
		   b0bclr	fc
		   rlcm		_d_math_output0    	//商左移一位至
           	   //b0bclr	fc
		   rlcm		_d_math_input0     	//被除数中间变量左移1位存入中间变量
		   rlcm		_d_math_output1
		   mov		a,_d_math_output1  	//中间变量减去除数
		   sub		a,_d_math_input1
		   b0bts0	fc 	 				//判断是否有借位，有借位被除数大于除数，没借位被除数小于除数
		   jmp		div_u8_u8_1
		   jmp		div_u8_u8_2
		div_u8_u8_1:         	 			//没有借位
		   mov		_d_math_output1,a   //得到余数
		   b0bset	_d_math_output0.0
		   jmp		div_u8_u8_3
		div_u8_u8_2:		 					//如果有借位
		   //b0bclr	_d_math_output0.0
		div_u8_u8_3:
		   mov		a,#8	 			//判断是否循环了8次
		   cmprs	a,_count
		   jmp		div_u8_u8_0
		   ret

//==================================================================================================
/* 	__div_u16_u16:
		//16位数除16位数除法
		//d_math_input0  = 被除数	
		//d_math_input1  = 除数		
		//d_math_output0 = 商	
		//d_math_output1 = 中间变量(余数)   	
		   clr		_d_math_output0    		//清0商
		   clr      	_d_math_output0 + 1
		   clr		_d_math_output1			//清0中间变量(余数)   
		   clr		_d_math_output1 + 1
		   clr      	_count
		div_u16_u16_0:
		   incms	_count	 				//count加一
		   b0bclr	fc
		   rlcm		_d_math_output0    		//商左移1位
		   rlcm     	_d_math_output0 + 1
		   //b0bclr	fc
		   rlcm		_d_math_input0     		//被除数中间变量左移1位存入中间变量
		   rlcm     	_d_math_input0 + 1
		   rlcm		_d_math_output1
		   rlcm     	_d_math_output1 + 1

		   b0mov	a,_d_math_output1  		//中间变量减去除数
		   sub		a,_d_math_input1
		   b0mov    	_math_tmp0,a
		   b0mov    	a,_d_math_output1 + 1
		   sbc      	a,_d_math_input1 + 1
		   //b0mov    	_tmp1,a
		   b0bts0	fc 	 					//判断是否有借位，有借位被除数大于除数，没借位被除数小于除数
		   jmp		div_u16_u16_1
		   jmp		div_u16_u16_2
		div_u16_u16_1:         	 				//没有借位
		   b0mov	_d_math_output1 + 1,a   //得到余数
		   b0mov    	a,_math_tmp0
		   b0mov    	_d_math_output1,a
		   b0bset	_d_math_output0.0
		   jmp		div_u16_u16_3
		div_u16_u16_2:		 					//如果有借位
		   //b0bclr	_d_math_output0.0
		div_u16_u16_3:
		   mov		a,#16	 				//判断是否循环了16次
		   cmprs	a,_count
		   jmp		div_u16_u16_0
		   ret*/
//==================================================================================================
 	__div_u24_u16:
		//24位数除16位数除法
		//d_math_input0  = 被除数	
		//d_math_input1  = 除数		
		//d_math_output0 = 商	
		//d_math_output1 = 中间变量(余数)   	
		   clr          _d_math_input1 + 2		//清除数高位
		   clr		_d_math_output0    		//清0商
		   clr      	_d_math_output0 + 1
		   clr      	_d_math_output0 + 2
		   clr		_d_math_output1			//清0中间变量(余数)   
		   clr		_d_math_output1 + 1		
		   clr		_d_math_output1 + 2
		   clr      	_count
		div_u24_u16_0:
		   incms	_count	 				//count加一
		   b0bclr	fc
		   rlcm		_d_math_output0    		//商左移1位
		   rlcm     	_d_math_output0 + 1
		   rlcm     	_d_math_output0 + 2	
		   //b0bclr	fc
		   rlcm		_d_math_input0     		//被除数中间变量左移1位存入中间变量
		   rlcm     	_d_math_input0 + 1
		   rlcm     	_d_math_input0 + 2
		   rlcm		_d_math_output1
		   rlcm     	_d_math_output1 + 1
	    	   rlcm     	_d_math_output1 + 2

		   b0mov	a,_d_math_output1  		//中间变量减去除数
		   sub		a,_d_math_input1
		   b0mov    	_math_tmp0,a
		   b0mov    	a,_d_math_output1 + 1
		   sbc      	a,_d_math_input1 + 1
		   b0mov    	_math_tmp0 + 1,a
		   b0mov    	a,_d_math_output1 + 2
		   sbc      	a,_d_math_input1 + 2
		   //b0mov    	_tmp1,a
		   b0bts0	fc 	 					//判断是否有借位，有借位被除数大于除数，没借位被除数小于除数
		   jmp		div_u24_u16_1
		   jmp		div_u24_u16_2
		div_u24_u16_1:         	 				//没有借位
		   b0mov	_d_math_output1 + 2,a   //得到余数
		   b0mov    	a,_math_tmp0
		   b0mov    	_d_math_output1,a
    		   b0mov    	a,_math_tmp0 + 1
		   b0mov    	_d_math_output1 + 1,a
		   b0bset	_d_math_output0.0
		   jmp		div_u24_u16_3
		div_u24_u16_2:		 					//如果有借位
		   //b0bclr	_d_math_output0.0
		div_u24_u16_3:
		   mov		a,#24	 				//判断是否循环了24次
		   cmprs	a,_count
		   jmp		div_u24_u16_0
		   ret

//==================================================================================================
	__mul_u16_u8: //2711B没有乘法指令
		//16位数 x 8位数 乘法(结果不超出24位)
		//d_math_input0  = 被乘数	
		//d_math_input1  = 乘数		
		//d_math_output0 = 积
		/*	b0mov	a,_d_math_input0+0
			mul	a,_d_math_input1+0
			b0mov	_d_math_output0,a
			b0mov	a,R
			b0mov	_d_math_output0+1,a
			b0mov	a,_d_math_input0+1
			mul	a,_d_math_input1+0
			add	_d_math_output0+1,a
			b0mov	a,R
			clr	_d_math_output0+2
			adc	_d_math_output0+2,a
			ret*/
			clr		_d_math_input0 + 2     	//清被乘数高位
                        clr             _d_math_input1 + 2	//清乘数高位
			clr             _d_math_input1 + 1	//清乘数中位
		   	clr		_d_math_output0    	//清0积
			clr		_d_math_output0 + 1
			clr		_d_math_output0 + 2
		   	clr     	_count
		mul_u16_u8_0:
		   	incms		_count	 		//count加一

			rrcm    	_d_math_input1      	//乘数右移1位
			b0bts1		fc  				
			jmp		mul_u16_u8_1

			b0mov   	a,_d_math_input0     	//被乘数加到积
			add     	_d_math_output0,a
			b0mov   	a,_d_math_input0 + 1
			adc     	_d_math_output0 + 1,a
			mov		a,_d_math_input0 + 2
			adc		_d_math_output0 + 2,a
		mul_u16_u8_1:
			b0bclr		fc         	 			
		   	rlcm    	_d_math_input0  	//被乘数左移
			rlcm    	_d_math_input0 + 1
			rlcm    	_d_math_input0 + 2
		mul_u16_u8_2:
		   	mov		a,#8	 		//判断是否循环了8次
		   	cmprs		a,_count
		   	jmp		mul_u16_u8_0
			ret
//==================================================================================================
/*		//16位数 x 16位数 乘法(结果不超出32位)
		//d_math_input0  = 被乘数	
		//d_math_input1  = 乘数		
		//d_math_output0 = 积
		//_math_tmp0
	__mul_u16_u16:
		call	__mul_u16_u8
		b0mov	a,_d_math_output0+0
		b0mov	_math_tmp0+0,a
		b0mov	a,_d_math_output0+1
		b0mov	_math_tmp0+1,a
		b0mov	a,_d_math_output0+2
		b0mov	_math_tmp0+2,a
		b0mov	a,_d_math_input1+1
		b0mov	_d_math_input1+0,a
		call	__mul_u16_u8
		b0mov	a,_d_math_output0+2
		b0mov	_d_math_output0+3,a
		b0mov	a,_d_math_output0+1
		b0mov	_d_math_output0+2,a
		b0mov	a,_d_math_output0+0
		b0mov	_d_math_output0+1,a
		b0mov	a,_math_tmp0+0
		b0mov	_d_math_output0+0,a 
		b0mov	a,_math_tmp0+1
		add	_d_math_output0+1,a
		b0mov	a,_math_tmp0+2
		adc	_d_math_output0+2,a
		mov	a,#(0)
		adc	_d_math_output0+3,a	
		ret*/
//==================================================================================================
//end files

