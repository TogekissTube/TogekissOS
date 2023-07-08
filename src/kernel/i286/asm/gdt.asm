// Togekiss OS: Lightweight operating system

// GDT (Global Descriptor Table ASM) 


[bits 16] 

gdt_start:
    ; Descriptor de segmento nulo
    dd 0x00000000
    dd 0x00000000

    ; Descriptor de segmento de código
    dw 0xFFFF  ; Limite bajo (16 bits)
    dw 0x0000  ; Base baja (16 bits)
    db 0x00    ; Base media (8 bits)
    db 0x9A    ; Tipo de segmento (0x9A = código ejecutable y legible)
    db 0xCF    ; Limite alto y atributos (0xCF = límite alto 4KB, granularidad de 32 bits)
    db 0x00    ; Base alta (8 bits)

    ; Descriptor de segmento de datos
    dw 0xFFFF  ; Limite bajo (16 bits)
    dw 0x0000  ; Base baja (16 bits)
    db 0x00    ; Base media (8 bits)
    db 0x92    ; Tipo de segmento (0x92 = datos legibles/escriturables)
    db 0xCF    ; Limite alto y atributos (0xCF = límite alto 4KB, granularidad de 32 bits)
    db 0x00    ; Base alta (8 bits)

gdt_end:

gdt_descriptor:
    dw gdt_end - gdt_start - 1  ; Tamaño de la GDT (16 bits)
    dd gdt_start                ; Dirección base de la GDT (32 bits)
    
    gdtr:
    dw gdt_end - gdt_start - 1   ; Tamaño de la GDT (16 bits)
    dd gdt_start                 ; Dirección base de la GDT (32 bits)


