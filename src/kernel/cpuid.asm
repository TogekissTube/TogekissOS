section .data
    cpuid_string db "CPUID: ", 0

section .text
    global _start

_start:
   
    mov eax, 4
    mov ebx, 1
    mov ecx, cpuid_string
    mov edx, 8
    int 0x80

   
    xor eax, eax
    cpuid

    mov eax, 1
    xor ebx, ebx
    xor ecx, ecx
    xor edx, edx
    cpuid

 


    mov eax, 1
    xor ebx, ebx
    int 0x80

