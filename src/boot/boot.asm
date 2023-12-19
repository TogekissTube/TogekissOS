org 0x7C00  ; Dirección de inicio del sector de arranque

section .text
    ; Punto de entrada del bootloader
    global _start

_start:
    ; Cargar el kernel en memoria
    mov ax, 0x1000  ; Dirección de carga del kernel
    mov es, ax
    mov bx, 0        ; Offset en el segmento es
    mov ah, 0x02     ; Función del BIOS para leer sectores
    mov al, 3        ; Número de sectores a leer
    mov ch, 0        ; Cilindro
    mov cl, 2        ; Sector de inicio
    mov dh, 0        ; Cabeza
    int 0x13         ; Llamada a la interrupción del BIOS

    ; Saltar al kernel
    mov ax, 0x1000
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov esp, 0x10000
    jmp 0x1000:0      ; Salto al kernel

    ; Rellenar el resto del sector de arranque con ceros
    times 510-($-$$) db 0
    dw 0xAA55        ; Firma de arranque

