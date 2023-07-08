// Togekiss OS: Lightweight operating system

// TSS ASM 

[bits 16]


tss_start:
    dw 0x67   ; Señal de resguardo de interrupción de TSS
    dw 0x00   ; Relleno

    dd 0x00000000   ; Puntero de apilamiento TSS (32 bits)
    dw 0x10        ; Segmento de pila TSS (16 bits)
    dw 0x00        ; Relleno

    dd 0x00000000   ; Puntero de apilamiento TSS del nivel 0 (32 bits)
    dw 0x10        ; Segmento de pila TSS del nivel 0 (16 bits)
    dw 0x00        ; Relleno

tss_end:

tss_descriptor:
    dw tss_end - tss_start - 1   ; Tamaño de la TSS (16 bits)
    dd tss_start                ; Dirección base de la TSS (32 bits)

