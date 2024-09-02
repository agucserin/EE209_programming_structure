# mydc.s / assignment 4 / 20190377 / Yang Jin Woo

	.equ ARRAYSIZE, 20	# size of buffer
	.equ EOF, -1				# End of File
.section ".rodata"
	line:								# makes new line
		.asciz "\n"
	cScanFormat:				# makes new line
		.asciz "%d"
	ScanFormat:
		.asciz "%s"
	pScanFormat:
		.asciz "%p"
	plus:
		.asciz "+"
	minus:
		.asciz "-"
	multi:
		.asciz "*"
	remainder:
		.asciz "%"
	division:
		.asciz "/"
	exponen:
		.asciz "^"
	quitcos:
		.asciz "q"
	print:
		.asciz "p"
	negative:
		.asciz "_"
	lifo:
		.asciz "f"
	reverse:
		.asciz "r"
	delete:
		.asciz "c"
	duplicate:
		.asciz "d"
	error1:
		.asciz "dc: stack empty\n"
.section ".bss"
	buffer:
		.skip ARRAYSIZE
	
.section ".data"
	## global variables which help caculating
	mul:	.int	0
	mul2:	.int	0
	mul3:	.int	0
	mul4:	.int 	0
	mul5:	.int  0
.section ".text"
	.equ	IBASE,	-4	
	
	.global main
	.type main,@function

	.global	power
	.type power,@function

# int main(void) : Runs desk calculator program.
#									 Returns 0.
main:
	pushl %ebp
	movl %esp, %ebp

input:
	pushl	$buffer
	pushl	$ScanFormat
	call	scanf
	addl  $8, %esp
	
	cmp	$EOF, %eax
	je	quit
	
	movzx buffer(, %ebx, 1), %eax	
	pushl	%eax
	cmpl	$'_',	%eax
	je		n
	call	isdigit
	addl	$4,	%esp
	pushl	%eax

	cmpl	$0,	%eax
	popl	%eax
	je		notint
	
	pushl 	$buffer
	call	atoi
	addl	$4, %esp

	pushl 	%eax
	jmp		 	input
	
	notint: //수가 아닌 문자일 경우
		pushl $buffer
		pushl $plus
		call	strcmp
		addl	$8, %esp
	
		cmp	$0,	%eax
		je a 
	
		pushl $buffer
		pushl $minus
		call	strcmp
		addl	$8, %esp
	
		cmp	$0,	%eax
		je b

		pushl $buffer
		pushl $multi
		call	strcmp
		addl	$8, %esp
	
		cmp	$0,	%eax
		je c
	
		pushl $buffer
		pushl $remainder
		call	strcmp
		addl	$8, %esp
	
		cmp	$0,	%eax
		je d

		pushl $buffer
		pushl $division
		call	strcmp
		addl	$8, %esp
	
		cmp	$0,	%eax
		je e
	
		pushl $buffer
		pushl $exponen
		call	strcmp
		addl	$8, %esp
	
		cmp	$0,	%eax
		je f
		
		pushl	$buffer
		pushl	$quitcos
		call	strcmp
		addl	$8,	%esp
		
		cmp	$0,	%eax
		je quit
		
		pushl	$buffer
		pushl	$print
		call	strcmp
		addl	$8,	%esp
		
		cmp	$0,	%eax
		je g

		pushl	$buffer
		pushl	$lifo
		call	strcmp
		addl	$8,	%esp
		
		cmp	$0,	%eax
		je h

		pushl	$buffer
		pushl	$duplicate
		call	strcmp
		addl	$8,	%esp
		
		cmp	$0,	%eax
		je i

		pushl	$buffer
		pushl	$delete
		call	strcmp
		addl	$8,	%esp
		
		cmp	$0,	%eax
		je j

		pushl	$buffer
		pushl	$reverse
		call	strcmp
		addl	$8,	%esp
		
		cmp	$0,	%eax
		je k

		jmp input
	a: //더하기
		cmpl	%esp,	%ebp //첫번째 값 확인
		je		input0.3
		popl	%eax
		cmpl	%esp,	%ebp //두번쨰 값 확인
		je		input0.5
		popl	%ecx
		
		addl	%ecx, %eax
		
		pushl	%eax
		
		jmp		input
		
		input0.5:
			pushl	%eax
		input0.3:
			pushl	$error1
			call	printf
			addl	$4,	%esp
			jmp		input
		
			
	b: //빼기
		cmpl	%esp,	%ebp
		je		input0.3
		popl		%eax
		cmpl	%esp,	%ebp
		je		input0.5
		popl		%ecx
		
		subl	%eax, %ecx
		pushl	%ecx

		jmp		input
	c: //곱하기
		cmpl	%esp,	%ebp
		je		input0.3
		popl	%eax
		cmpl	%esp,	%ebp
		je		input0.5
		popl	%ecx
		
		movl	$1, %edx
		movl	$0,		mul
		addl	%eax,	mul
		
		cmpl	$0,	%ecx
		je		endloop1.0
		jl		loop1.2
		cmpl	$0,	%eax
		je		endloop1.0
	
		loop1:
			cmpl	%edx, %ecx
			je		endloop1
			addl	mul, %eax
			inc		%edx
			jmp		loop1
		loop1.2:		
			movl	$0,	mul2
			addl	%ecx,	mul2
			subl	mul2,	%ecx
			subl	mul2,	%ecx
			loop1.4:
				cmpl	%edx, %ecx
				je		endloop1.2
				addl	mul, %eax
				inc		%edx
				jmp		loop1.4
		endloop1.2:
			movl	$0,	mul
			addl	%eax,	mul
			subl	mul, %eax
			subl	mul, %eax
		endloop1:
			pushl	%eax
		jmp		input
		endloop1.0:
			movl	$0,	%eax
			pushl %eax
			jmp	input
	d: //나머지
		cmpl	%esp,	%ebp
		je		input0.3
		popl	%eax
		cmpl	%esp,	%ebp
		je		input0.5
		popl	%ecx

		movl	%eax,	mul
		movl	%ecx,	mul2
		movl	$0,		mul3

		cmpl	$0,	%ecx
		jl		loop2.2
		##je		input0
		
		cmpl	$0, %eax
		jg		loop2
		subl	%eax,	mul
		subl	%eax,	mul		
		jmp 	loop2

		loop2.2:
			subl	$1,		mul3
			subl	mul2,	%ecx
			subl	mul2,	%ecx
			cmpl	$0,		%eax
			jg		loop2			
			subl	%eax,	mul
			subl	%eax,	mul

		loop2:
			cmpl	mul,	%ecx
			jl		endloop2
			subl	mul, %ecx
			jmp		loop2
	
		endloop2:
			cmpl	$0,	mul3
			jl endloop2.5
			jge endendloop2
			endloop2.5:
				movl	%ecx,	mul
				subl	mul,	%ecx
				subl	mul,	%ecx
		endendloop2:
			pushl	%ecx
			jmp		input
	e: //나누기
		cmpl	%esp,	%ebp
		je		input0.3
		popl	%eax
		cmpl	%esp,	%ebp
		je		input0.5
		popl	%ecx

		movl	$0,	%edx
		movl	%eax,	mul
		movl	%ecx,	mul2
		movl	$0,		mul3
		movl	$0,		mul4

		cmpl	$0,	%ecx
		jl		loop3.2
		cmpl	$0,	%eax
		jl		loop3.4
		jg		loop3

		loop3.4:
			subl	$1,	mul4
			subl	%eax,	mul
			subl	%eax,	mul
			jmp		loop3

		loop3.2:
			subl	$1,	mul3
			subl	mul2,	%ecx
			subl	mul2,	%ecx
			cmpl	$0,	%eax
			jg		loop3
			subl	$1, mul4
			subl	%eax,	mul
			subl	%eax,	mul	

		loop3:
			cmpl	mul,	%ecx
			jl		endloop3
			subl	mul, %ecx
			inc		%edx
			jmp		loop3
	
		endloop3:
			subl	%ecx,	%ecx
			addl	mul4,	%ecx
			addl	%ecx,	mul3

			cmpl	$-1,	mul3
			je		endendloop3
			pushl	%edx
			jmp		input
		endendloop3:
			movl	%edx,	mul4
			subl	mul4,	%edx
			subl	mul4,	%edx
			pushl %edx
			jmp		input
	f: //제곱
		cmpl	%esp,	%ebp
		je		input0.35
		popl	%ecx
		cmpl	$0,		%ecx
		jl		input0.57
		cmpl	%esp,	%ebp
		je		input0.55
		popl	%eax

		call 	power
		pushl	%eax
		jmp		input

		input0.35:
			pushl	$error1
			call	printf
			addl	$4,	%esp
			jmp		input
		input0.55:
			pushl	%ecx
			pushl	$error1
			call	printf
			addl	$4,	%esp
		jmp		input
			input0.57:
			pushl	%ecx
			jmp		input
	g: //프린트
		cmpl	%esp, %ebp
		je		input0.3

		pushl	$cScanFormat
		call	printf
		addl	$4, %esp

		pushl 	$line
		call	printf
		addl	$4, %esp

		jmp		input
	n:
		addl	$4,	%esp
		movzx (buffer+1)(, %ebx, 1), %eax
		pushl	%eax
		call	isdigit
		cmpl	$0,	%eax
		je		input0.9
		
		
		addl	$4,	%esp
		
		
		pushl	$(buffer+1)
		call	atoi				
		addl	$4, %esp

		movl	$0,		mul
		addl	%eax,	mul
		subl	mul,	%eax
		subl	mul,	%eax
		pushl	%eax

		jmp		input

		input0.9:
			addl	$4,	%esp
			jmp		input
	h: //전부 출력
		movl	%esp,	%ecx
		movl	%ebp,	%eax

		cmpl	%esp,	%ebp
		je		input

		movl	$0,		mul
		loop6:
			subl	$4,	%eax
			pushl	(%eax)
			addl	$1,	mul
			cmpl	%eax,	%ecx
			je		endloop6
			jmp		loop6

		endloop6:
			pushl	$cScanFormat
			call	printf
			addl	$4,	%esp
			popl	%eax

			pushl	$line
			pushl	$ScanFormat
			call	printf
			addl	$8,	%esp
			subl	$1,	mul
			cmpl	$0,	mul
			je		input
			jmp 	endloop6
	i: //복사
		cmpl	%esp,	%ebp
		je		input0.3
		popl	%eax
		pushl	%eax
		pushl	%eax
		jmp		input
	j: //삭제
		loop7:
			cmpl	%esp,	%ebp
			je		input
			popl	%eax		
			jmp		loop7
	k: //리버스
		cmpl	%esp,	%ebp
		je		input0.3
		popl	%eax
		cmpl	%esp,	%ebp
		je		input0.5
		popl	%ecx
		pushl	%eax
		pushl	%ecx
		jmp		input
	quit:
		movl    $0, %eax
		movl    %ebp, %esp
		popl    %ebp
		ret

# int power(void) : implement power arithmetic operator
#										returns integer
power:
	cmpl	$0,	%ecx
	je		endloop4.7
	cmpl	$0,	%eax
	je		endloop4.5

	subl	%edx,%edx
	movl	%eax, mul5
	movl	mul5,	%ebx

	movl	$1,	mul2

	loop4:
		cmpl	mul2,	%ecx
		je		endloop4
		addl	$1,	mul2
		jmp		c2
	endloop4:
		movl	$0,	%ebx
		cmpl	$0, %eax
		jl		check
		ret

	endloop4.7:
		movl	$1,	%eax
		cmpl	$0,	%eax
		jl    check
		ret

	endloop4.5:
		movl	$0,	%eax
		ret
	
	check:
		loopcheck:
			cmpl $2, %ecx
			jl	endloopcheck
			subl $2, %ecx
			jmp	loopcheck
		endloopcheck:
			cmpl $1, %ecx
			je   endcheck
			movl %eax, mul
			subl mul,	 %eax
			subl mul,  %eax
			ret
	endcheck:
		ret
	c2:
		movl	$1, %edx

		movl	%eax,	mul

		cmpl	$0,		%ebx
		jl		loop5.2

		loop5:
			cmpl	%edx, %ebx
			je		endloop5
			addl	mul, %eax
			inc		%edx
			jmp		loop5
	
		endloop5:	
			jmp		loop4
		loop5.2:
			movl	%ebx,	mul4
			subl	mul4,	%ebx
			subl	mul4,	%ebx
			jmp		loop5

