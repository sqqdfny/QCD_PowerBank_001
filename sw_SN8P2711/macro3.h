IFNDEF	__MACRO3__
	__MACRO3__	EQU	1

// The follow macro be defined by SONIX
// if you want to define new macro for yourself,
// please defined at other files, and will easy to maintain

// please don't set your file name as MACROx.H,
// because the name be reserved for new SONIX-MACRO file

;-------------------------------------------------------------------
;
;	LJMP	lar_far
;	...
;	LCALL	lar_far
;	...
;	ORG	0x4000
;	...
;	lab_far:

LJMP	macro	adr
	b0mov	pflag, #adr$J
	jmp		adr
	endm

LCALL	macro	adr
	b0mov	pflag, #adr$J
	call	adr
	b0mov	pflag, #(($>>10)!&0xF0)
	endm

;-------------------------------------------------------------------
;
;	...
;	@CHECK_REMAINS			; check the code page
;
;	ORG	0x4000
;	...
;	...
;	@CHECK_REMAINS	0x7000	; define the max code is 0x7000
;
;	ORG	0x7000
;	DW ... // define 0x1000 data
;
;	ORG	0x8000
;	...

@CHECK_REMAINS	macro	val
	IFB	<val>
		temp1	=	($ + 0x3FFF) !& 0xFC000
		temp2	=	temp1 - $
		ECHO	Remain %temp2 space before %temp1 page
	ELSEIF	$ > (val)
		temp1	=	$ - (val)
		ERROR	Over %temp1 space after val
	ELSE
		temp1	=	(val) - $
		ECHO	Remain %temp1 space before val
	ENDIF
	endm

;-------------------------------------------------------------------
;
;	...
;	@ORG	0x4000
;	...
;	...
;
;	@ORG	0x7000
;	DW ... // define 0x1000 data
;
;	@ORG	0x8000

@ORG	macro	val
	@CHECK_REMAINS	<val>
	ORG	val
	endm

;-------------------------------------------------------------------
;
;	mov		a, #n
;	@JMP_A	5		// follow 5 commands of jmp
;	jmp		here_a_0
;	jmp		here_a_1
;	jmp		here_a_2
;	jmp		here_a_3
;	jmp		here_a_4
;

@JMP_A	macro	val
	IF	(($+1) !& 0xff00) !!= (($+(val)) !& 0xff00)
		jmp	($ | 0xff)
		ORG	($ | 0xff)
	ENDIF
	//add	pcl, a
	b0add PCl ,a
	endm

@JMP_A_2ADR	macro	val,adr
  if	(($+4) !& 0xff00) !!= (($+3+(val*2)) !& 0xff00)
		jmp	($ | 0xff)
		ORG	($ | 0xff)
	ENDIF

	b0bclr	FC
	mov adr,a
	RLC adr
	b0add PCl ,a
	endm         
;-------------------------------------------------------------------
;
;	@SWITCH	A,        , here0, here1, here2, here3, ....
;	@SWITCH	A, err_jmp, here0, here1, here2, here3, ....
;
;	@SWITCH	mem,        , here0, here1, here2, here3, ....
;	@SWITCH	mem, err_jmp, here0, here1, here2, here3, ....
;

@SWITCH	expand	mm, ee, lab:VARARG
	IFDIFI	<mm>, <A>
		mov	a, mm
	ENDIF

	temp	=	0
	FOR	ll, <lab>
		temp	=	temp +1
	ENDM

	IFNB	<ee>
		cmprs	a, #temp
		b0bts0	FC
		jmp	ee
	ENDIF

	@JMP_A	temp

	FOR	ll, <lab>
		jmp	ll
	ENDM

	ENDM

;-------------------------------------------------------------------
;
;	XXX		MACRO
;			LOCAL	XX1, XX2
;			...
;		XX1:
;			...
;		XX2:
;			...
;			ENDM
;
;-------------------------------------------------------------------
/*
LOCAL	MACRO	ll:VARARG
		IFNDEF	@MS_LCL_TMP
				@MS_LCL_TMP	=	0
		ENDIF

		FOR		l, <ll>
				l	CATSTR	<@MS_LCL>, %@MS_LCL_TMP
				@MS_LCL_TMP	=	@MS_LCL_TMP + 1
		ENDM
		ENDM
*/
;-------------------------------------------------------------------
;
;	@INIT_PORT	P2, HL10UUII
;		// P2.7 = Out High, (Pull High)
;		// P2.6 = Out Low, (Pull High)
;		// P2.5 = Out High
;		// P2.4 = Out Low
;		// P2.3, P2.2 = Input Pull High
;		// P2.1, P2.0 = Input
;		;
;		; -> be generated
;		;	mov	a, #0F0h
;		;	mov	P2M, a
;		;	mov	a, #0C0h
;		;	mov	P2, a
;		;	mov	a, #0CCh
;		;	mov	P2UR, a

@INIT_PORT	macro	pt, str
	@s_TEMP1	=	0	// Mask
	@s_TEMP2	=	0	// Pull-Up
	@s_TEMP3	=	0	// Data
	FORC	c, <str>
		@s_TEMP1	=	@s_TEMP1 << 1
		@s_TEMP2	=	@s_TEMP2 << 1
		@s_TEMP3	=	@s_TEMP3 << 1

		IFIDNI		<c>, <I>
		ELSEIFIDNI	<c>, <U>
			@s_TEMP2	=	@s_TEMP2 + 1;
		ELSEIFIDNI	<c>, <H>
			@s_TEMP1	=	@s_TEMP1 + 1;
			@s_TEMP2	=	@s_TEMP2 + 1;
			@s_TEMP3	=	@s_TEMP3 + 1;
		ELSEIFIDNI	<c>, <L>
			@s_TEMP1	=	@s_TEMP1 + 1;
			@s_TEMP2	=	@s_TEMP2 + 1;
		ELSEIFIDNI	<c>, <1>
			@s_TEMP1	=	@s_TEMP1 + 1;
			@s_TEMP3	=	@s_TEMP3 + 1;
		ELSEIFIDNI	<c>, <0>
			@s_TEMP1	=	@s_TEMP1 + 1;
		ELSE
			ERROR	error str, only H/L/1/0/I/U can use
			EXITM
		ENDIF
	ENDM

	IF	P0 == pt
		IF	@s_TEMP1
			ERROR	P0 can't set to Output_Mode
		ENDIF
		IF	@s_TEMP2 == 0
			clr	pt&UR
		ELSE
			mov	a, #@s_TEMP2
			b0mov	pt&UR, a
		ENDIF
		EXITM
	ENDIF

	IF	@s_TEMP1 == 0
		clr	pt&M
	ELSE
		mov	a, #@s_TEMP1
		b0mov	pt&M, a
	ENDIF

	IF	@s_TEMP3 == 0
		clr	pt
	ELSEIF	@s_TEMP1 == @s_TEMP3
		b0mov	pt, a
	ELSE
		mov	a, #@s_TEMP3
		b0mov	pt, a
	ENDIF

	IF	@s_TEMP2 == 0
		clr	pt&UR
	ELSEIF	(@s_TEMP2 == @s_TEMP3) || (@s_TEMP2 == @s_TEMP1) !&!& (@s_TEMP3 == 0)
		b0mov	pt&UR, a
	ELSE
		mov	a, #@s_TEMP2
		b0mov	pt&UR, a
	ENDIF

	endm

;-------------------------------------------------------------------
;
;	@s_HCLK	EQU	3580000	// CPU clock
;	...
;	@SET_TIMER1	200	// every 200 uS generate interrupt
;

@SET_TIMER1	macro	time
	IFNDEF	@s_HCLK
		@s_HCLK	EQU	3580000
	ENDIF

	@s_VALUE	=	time * (@s_HCLK / 1000) / 8000
	@s_RATE		=	0x70

	REPEAT	7
		IF	@s_VALUE > 255
			@s_VALUE = @s_VALUE / 2
			@s_RATE	= @s_RATE - 0x10
		ENDIF
	ENDM

	IF	@s_VALUE > 255
		ERROR	time over range
		EXITM
	ELSE
		b0bclr	FTC1IEN

		mov	a, #@s_RATE + 84h	// enable TC1, auto-reload
		b0mov	TC1M, a

		mov	a, #255 - @s_VALUE
		b0mov	TC1R, a

		b0bset  FTC1IEN
	ENDIF
	ENDM

ENDIF