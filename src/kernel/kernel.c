// kernel.c

void print_string(const char* str) {
    // Función simple para imprimir una cadena de caracteres en pantalla
    // Esta función asume que estás trabajando en un entorno donde puedes
    // enviar caracteres a la pantalla, como el modo de texto de VGA.

    // Puntero al registro de video (modo de texto de VGA)
    volatile char* video_memory = (char*)0xB8000;

    // Bucle para imprimir la cadena de caracteres en la memoria de video
    while (*str != '\0') {
        *video_memory++ = *str++;  // Carácter
        *video_memory++ = 0x07;    // Atributos (color blanco sobre fondo negro)
    }
}

void kernel_main() {
    // Función principal del kernel
    // Puedes agregar tu lógica del kernel aquí

    // En este ejemplo, simplemente imprimimos un mensaje
    print_string("Hola desde el kernel!");

    // En un sistema operativo real, aquí agregarías más lógica y manejarías interrupciones,
    // configurarías la gestión de memoria, realizarías operaciones de entrada/salida, etc.

    // En este ejemplo, simplemente detenemos la ejecución (bucle infinito)
    while (1) {
        // Bucle infinito
    }
}

