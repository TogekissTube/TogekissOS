section .text
    global fat12_read_file

%define ENDL 0x0D, 0x0A

; Estructura de la entrada de directorio en un sistema de archivos FAT12
struc fat12_directory_entry
    .filename   resb 8     ; Nombre del archivo
    .extension  resb 3     ; Extensión del archivo
    .attributes resb 1     ; Atributos del archivo
    .reserved   resb 10    ; Reservado
    .time       resw 1     ; Hora de creación/último acceso
    .date       resw 1     ; Fecha de creación/último acceso
    .cluster    resw 1     ; Número de cluster inicial
    .size       resd 1     ; Tamaño del archivo en bytes
endstruc

; Estructura de la tabla de archivos FAT12
struc fat12_boot_sector
    .oem_id         resb 8     ; ID OEM
    .bytes_per_sec  resw 1     ; Bytes por sector
    .sec_per_clust  resb 1     ; Sectores por clúster
    .reserved_secs  resw 1     ; Sectores reservados antes del sistema de archivos
    .fat_count      resb 1     ; Número de tablas FAT
    .dir_entries    resw 1     ; Número de entradas de directorio
    .total_secs     resw 1     ; Número total de sectores (si es 0, usar total_secs_large)
    .media_type     resb 1     ; Tipo de medio
    .secs_per_fat   resw 1     ; Sectores por FAT
    .secs_per_track resw 1     ; Sectores por pista
    .heads          resw 1     ; Número de cabezas en el dispositivo
    .hidden_secs    resd 1     ; Sectores ocultos antes del inicio de la partición
    .total_secs_large resd 1   ; Número total de sectores para particiones mayores a 32 MB
    .drive_number   resb 1     ; Número de unidad de disco
    .reserved       resb 1     ; Reservado (para DOS)
    .boot_signature resb 1     ; Firma de arranque
    .volume_id      resd 1     ; Número de serie del volumen
    .volume_label   resb 11    ; Etiqueta del volumen
    .file_system    resb 8     ; Sistema de archivos (FAT12)
endstruc

; Función para leer un archivo desde un sistema de archivos FAT12
; Parámetros:
;   - drive: Número de unidad de disco
;   - filename: Nombre del archivo a leer
;   - buffer: Buffer donde se almacenará el contenido del archivo
; Retorna:
;   - 0 si la operación fue exitosa, otro valor si hubo un error
fat12_read_file:
    push ebp
    mov ebp, esp

    ; Parámetros de la función
    mov edx, [ebp + 8]     ; Número de unidad de disco
    mov ebx, [ebp + 12]    ; Puntero al nombre del archivo
    mov ecx, [ebp + 16]    ; Puntero al buffer de destino

    ; Leer el sector de arranque (boot sector)
    call read_sector
    mov edi, ecx            ; Puntero al buffer actualizado

    ; Obtener la dirección de inicio del sistema de archivos FAT12
    mov eax, [edi + fat12_boot_sector.total_secs_large - fat12_boot_sector.oem_id]
    add eax, 1              ; Añadir 1 al número total de sectores para incluir el sector de arranque
    mov ecx, [edi + fat12_boot_sector.fat_count] ; Número de tablas FAT
    imul ecx, [edi + fat12_boot_sector.secs_per_fat] ; Sectores por FAT
    add eax, ecx            ; Sumar sectores de la FAT
    mov ecx, [edi + fat12_boot_sector.reserved_secs] ; Sectores reservados
    add eax, ecx            ; Sumar sectores reservados

    ; Leer la tabla FAT
    call read_sectors
    mov esi, ecx            ; Puntero a la tabla FAT

    ; Buscar el archivo en el directorio raíz
    mov edx, 0              ; Número de entrada de directorio
    .search_root:
        ; Calcular la dirección de la entrada de directorio actual
        mov eax, edx
        imul eax, fat12_directory_entry.size
        add eax, ebx
        ; Leer la entrada de directorio
        mov eax, [eax + fat12_directory_entry.filename]
        cmp eax, 0            ; ¿Entrada de directorio vacía?
        je .not_found
        ; Comparar nombre de archivo
        mov eax, edx
        imul eax, fat12_directory_entry.size
        add eax, ebx
        mov eax, [eax + fat12_directory_entry.filename]
        call compare_filename
        cmp eax, 1            ; ¿Coincide el nombre del archivo?
        je .file_found
        ; Siguiente entrada de directorio
        inc edx
        jmp .search_root

    .not_found:
        mov eax, 1            ; Error: archivo no encontrado
        jmp .end

    .file_found:
        ; Calcular la dirección de inicio del archivo
        movzx eax, word [edx + fat12_directory_entry.cluster]
        imul eax, [edi + fat12_boot_sector.sec_per_clust] ; Sectores por clúster
        add eax, esi            ; Sumar sectores de la FAT
        mov esi, eax            ; Puntero a la entrada de la tabla FAT

        ; Leer el contenido del archivo
        .read_file:
            mov eax, [esi]
            cmp eax, 0xFF8        ; Último clúster
            je .end_read_file
            call read_cluster
            add esi, 2            ; Siguiente entrada de la FAT
            jmp .read_file

        .end_read_file:

    .end:
        pop ebp
        ret

; Función para comparar dos cadenas de caracteres (case-insensitive)
; Parámetros:
;   - esi: Puntero a la primera cadena
;   - edi: Puntero a la segunda cadena
; Retorna:
;   - 0 si las cadenas no son iguales, 1 si son iguales
compare_filename:
    .compare_loop:
        mov al, [esi]
        mov bl, [edi]
        cmp al, bl
        je .check_null
        ; Convierte a mayúsculas si es necesario
        cmp al, 'a'
        jb .check_null
        cmp al, 'z'
        ja .check_null
        sub al, 0x20
        .check_null:
            cmp al, 0
            je .equal
            inc esi
            inc edi
            jmp .compare_loop
    .equal:
        mov eax, 1
        ret

; Función para leer un sector desde el disco
; Parámetros:
;   - edx: Número de unidad de disco
;   - ecx: Número de sector
;   - edi: Puntero al buffer de destino
; Retorna:
;   - Ninguno
read_sector:
    mov eax, 0x2            ; Número de sectores a leer (1 sector)
    mov ebx, edi            ; Puntero al buffer de destino
    call read_sectors
    ret

; Función para leer múltiples sectores desde el disco
; Parámetros:
;   - edx: Número de unidad de disco
;   - ecx: Número de sector inicial
;   - eax: Número de sectores a leer
;   - ebx: Puntero al buffer de destino
; Retorna:
;   - Ninguno
read_sectors:
    mov ah, 0x02            ; Función de lectura de sectores
    mov al, eax             ; Número de sectores a leer
    mov ch, 0               ; Cilindro (bits 8-9)
    mov cl, ecx             ; Número de sector (bits 0-5)
    mov dh, 0               ; Cabeza
    int 0x13                ; Llamada a la interrupción del BIOS
    jc .read_error          ; Si el carry está configurado, hubo un error
    ret

    .read_error:
        ; Manejar el error aquí
        ret

; Función para leer un clúster desde el disco
; Parámetros:
;   - esi: Puntero a la entrada de la tabla FAT
; Retorna:
;   - Ninguno
read_cluster:
    mov eax, [esi]
    mov ecx, eax
    sub ecx, 2               ; Ajustar para el desplazamiento
    imul ecx, [edi + fat12_boot_sector.sec_per_clust]
    add ecx, esi             ; Calcular la dirección de inicio del clúster
    call read_sectors        ; Leer el clúster
    ret

