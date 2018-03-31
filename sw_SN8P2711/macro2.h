IFNDEF	__MACRO2__
	__MACRO2__	EQU	1

;-------------------------------------------------------------------
;
;	when operate between memory to memory, or immediate, the below
;	function are useful.
;
;	*Note* :	ACC will be changed, CF, ZF, DCF may be changed

MOV_	macro	mem, mem_val
	mov	a, mem_val
	mov	mem, a
	endm


OP3_	macro	op
op&_	macro	mem, mem_val, mem_aux
	ifdifi <a>, <mem_val>
		mov	a, mem_val
	endif
	ifb	<mem_aux>
		op	mem, a
	else
		op	a, mem_aux
		ifdifi	<a>, <mem>
			mov	mem, a
		endif
	endif
	endm
	endm

OP3_	ADD
OP3_	ADC
OP3_	AND
OP3_	OR
OP3_	XOR

OP4_	macro	op
op&_	macro	mem, mem_val, mem_aux
	ifb	<mem_aux>
		mov	a, mem
		op	a, mem_val
		mov	mem, a
	else
		ifdifi <a>, <mem_val>
			mov	a, mem_val
		endif
		op	a, mem_aux
		ifdifi	<a>, <mem>
			mov	mem, a
		endif
	endif
	endm
	endm

OP4_	SUB
OP4_	SBC

XCH_	macro	mem1, mem2
	mov	a, mem2
	xch	a, mem1
	mov	mem2, a
	endm

NOT_	macro	mem
	IFIDNI	<mem>, <A>
		xor	a, #0xff
	ELSE
		mov	a, #0xff
		xor	mem, a
	ENDIF
	endm

NEG_	macro	mem
	IFIDNI	<mem>, <A>
		xor	a, #0xff
		add	a, #1
	ELSE
		mov	a, #0
		sub	mem, a
	ENDIF
	endm

;-------------------------------------------------------------------
;
;	when compare between memory to memory, or immediate, the below
;	function are useful.
;
;	CJAE_/CJBE_/CJA_/CJB_/CJE_/CJNE_	mem1, mem2, addr_match_jump
;
;	Note :	ACC will be changed; C, Z flag will be changed
;				adr may be 'RET'

CJAE_	macro	v1, v2, adr
	mov	a, v1
	cmprs	a, v2
	b0bts0	FC

	Jmp_Ret	<adr>
	endm

CJA_	macro	v1, v2, adr
	mov	a, v2
	sub	a, v1
	b0bts1	FC

	Jmp_Ret	<adr>
	endm

CJBE_	macro	v1, v2, adr
	CJAE_	<v2>, <v1>, <adr>
	endm

CJB_	macro	v1, v2, adr
	CJA_	<v2>, <v1>, <adr>
	endm

CJNE_	macro	v1, v2, adr
	mov	a, v1
	cmprs	a, v2

	Jmp_Ret	<adr>
	endm

CJE_	macro	v1, v2, adr
	mov	a, v1
	cmprs	a, v2
	jmp	$+2

	Jmp_Ret	<adr>
	endm

;-------------------------------------------------------------------
;
;	To check ACC in some range, example : #30h - #39h,
;	use BOUND_	30h, 39h, over_jmp_addr
;
;	Note :	ACC will be changed; C, DC, Z flag will be changed

BOUND_	expand	sm, la, adr
	add	a, #(0xFF - (la))
	b0bts0	FC
	Jmp_Ret	<adr>
	IFNB	<sm>
		IF	(sm) > 0
			add	a, #((la) - (sm) + 1)
			b0bts1	FC
			Jmp_Ret	<adr>
		ENDIF
	ENDIF
	endm

ENDIF