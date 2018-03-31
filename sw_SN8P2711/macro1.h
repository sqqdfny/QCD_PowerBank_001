IFNDEF	__MACRO1__
	__MACRO1__	EQU	1

;-------------------------------------------------------------------
;
;	CLC	:	clear Carry Flag
;	STC	:	set Carry Flag
;	CLRWDT	:	reset Watch-Dog Count
;	NOT A	:	A = ~A
;	NEG A	:	A = -A
;
;	Note	:	the function as Alias-Name

CLC	macro
	b0bclr	FC
	endm

STC	macro
	b0bset	FC
	endm

RSTWDT	macro
	b0bset	fwdrst
	endm

NOT	macro	acc
	IFIDNI	<acc>, <A>
		xor	a, #0xff
	ELSE
		ERROR	please use NOT_ mem
	ENDIF
	endm

NEG	macro	acc
	IFIDNI	<acc>, <A>
		xor	a, #0xff
		add	a, #1
	ELSE
		ERROR	please use NEG_ mem
	ENDIF
	endm

;-------------------------------------------------------------------
;
;	Compare ACC with memory or immediate, if Above/Below/Equ then
;	jump to new address
;
;	CJxx	A, mem_imm, adr
;	CJxx	mem_imm, A, adr
;
;	*Note*	:	CF, ZF will be cahnge
;				adr may be 'RET'

Jmp_Ret	macro	adr
	IFIDNI	<adr>, <ret>
		ret
	ELSE
		jmp	adr
	ENDIF
	endm

CJAE	macro	v1, v2, adr
	IFIDNI	<v1>, <A>
		cmprs	a, v2
		b0bts0	FC
	ELSEIFIDNI	<v2>, <A>
		cmprs	a, v1
		b0bts1	FC
	ELSE
		ERROR	please use CJAE_/CJBE_ mem, mem, address
	ENDIF
	Jmp_Ret	<adr>
	endm

CJA	macro	v1, v2, adr
	IFIDNI	<v1>, <A>
		cmprs	a, v2
		b0bts1	FC
		jmp	$+2
	ELSEIFIDNI	<v2>, <A>
		cmprs	a, v1
		b0bts0	FC
		jmp	$+2
	ELSE
		ERROR	please use CJA_/CJB_ mem, mem, address
	ENDIF
	Jmp_Ret	<adr>
	endm

CJBE	macro	v1, v2, adr
	CJAE	<v2>, <v1>, <adr>
	endm

CJB	macro	v1, v2, adr
	CJA	<v2>, <v1>, <adr>
	endm

CJNE	macro	v1, v2, adr
	IFIDNI	<v1>, <A>
		cmprs	a, v2
	ELSEIFIDNI	<v2>, <A>
		cmprs	a, v1
	ELSE
		ERROR	please use CJNE_ mem, mem, address
	ENDIF
	Jmp_Ret	<adr>
	endm

CJE	macro	v1, v2, adr
	IFIDNI	<v1>, <A>
		cmprs	a, v2
	ELSEIFIDNI	<v2>, <A>
		cmprs	a, v1
	ELSE
		ERROR	please use CJE_ mem, mem, address
	ENDIF
	jmp	$+2
	Jmp_Ret	<adr>
	endm

;-------------------------------------------------------------------
;
;	Jxx	addr		:	Jump to address if condiction match.
;
;	DJNZ/IJNZ mem, addr	:	dec/inc memory, if mem not zero,
;					jump to another address.
;
;	*Note*			:	C, DC, Z not be changed,
;					2 cycle if no jump,
;					3 Cycle if jump to another address.
;					adr may be 'RET'

JZ	macro	adr
	b0bts0	FZ
	Jmp_Ret	<adr>
	endm

JNZ	macro	adr
	b0bts1	FZ
	Jmp_Ret	<adr>
	endm

JC	macro	adr
	b0bts0	FC
	Jmp_Ret	<adr>
	endm

JNC	macro	adr
	b0bts1	FC
	Jmp_Ret	<adr>
	endm

JDC	macro	adr
	b0bts0	FDC
	Jmp_Ret	<adr>
	endm

JNDC	macro	adr
	b0bts1	FDC
	Jmp_Ret	<adr>
	endm

JB1	macro	f, adr
	bts0	f
	Jmp_Ret	<adr>
	endm

JB0	macro	f, adr
	bts1	f
	Jmp_Ret	<adr>
	endm

DJNZ	macro	val, adr
	decms	val
	Jmp_Ret	<adr>
	endm

IJNZ	macro	val, adr
	incms	val
	Jmp_Ret	<adr>
	endm

DJZ	macro	val, adr
	decms	val
	jmp		$+2
	Jmp_Ret	<adr>
	endm

IJZ	macro	val, adr
	incms	val
	jmp		$+2
	Jmp_Ret	<adr>
	endm

;-------------------------------------------------------------------
;
;	RETxx		:	Ret if condiction match.
;
;	*Note*			:	C, DC, Z not be changed,
;					2 cycle if no ret,
;					3 Cycle if ret to another address.

RETZ	macro
	b0bts0	FZ
	ret
	endm

RETC	macro
	b0bts0	FC
	ret
	endm

RETNZ	macro
	b0bts1	FZ
	ret
	endm

RETNC	macro
	b0bts1	FC
	ret
	endm

;-------------------------------------------------------------------
;
;	SHL/SHR	mem	:	shift left/right memory
;	ROL/ROR mem	:	rotate left/right memory, bit0 <--> bit7
;	B2B/B2P		:	get one bit status, and set to another bit
;	*Note*		:	C flag will be changed, 2 - 4 Cycle

SHL	macro	mem
	b0bclr	FC
	rlcm	mem
	endm

SHR	macro	mem
	b0bclr	FC
	rrcm	mem
	endm

B2B	macro	f1, f2
	b0bclr	f2
	b0bts0	f1
	b0bset	f2
	endm

B2NB macro	f1, f2
	b0bclr	f2
	b0bts1	f1
	b0bset	f2
	endm

B2P	macro	f1, p1
	b0bts0	f1
	b0bset	p1
	b0bts1	f1
	b0bclr	p1
	endm

B2NP	macro	f1, p1
	b0bts1	f1
	b0bset	p1
	b0bts0	f1
	b0bclr	p1
	endm

ROL	macro	mem
	rlcm	mem
	B2B	FC, mem.0
	endm

ROR	macro	mem
	rrcm	mem
	B2B	FC, mem.7
	endm

RCL	macro	mem
	rlcm	mem
	endm

RCR	macro	mem
	rrcm	mem
	endm

;-------------------------------------------------------------------
;
;	DINT			// Global Interrupt Disable
;	DINT	P00/P01/P02	// General Interrupt Disable
;	DINT	TC0/TC1/T0/SIO	//
;
;	EINT	...		// Same as DINT
;
;	Note :	C, DC, Z not be changed, 1 or 2 Cycle

DINT	macro	int
	IFB	<int>
		b0bclr	fgie
	ELSE
		b0bclr	f&int&ien
		b0bclr	f&int&irq
	ENDIF
	endm

EINT	macro	int
	IFB	<int>
		b0bset	fgie
	ELSE
		b0bclr	f&int&irq
		b0bset	f&int&ien
	ENDIF
	endm

ENDIF
