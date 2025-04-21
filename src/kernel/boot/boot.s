bits 32

section .text
	align 4
	dd 0x1BADB002
	dd 0x000000000
	dd - (0x1BADB002 + 0x000000000)

global start
extern main

start:
	cli
	mov esp, stack_space
	call main
	hlt

Done:
	cli
	hlt
	jmp Done

section .bss

resb 8192

stack_space:
