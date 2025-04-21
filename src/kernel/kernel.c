/* kernel.c */

/* Definición de la función principal del kernel */
void main() {
    /* Puntero a la dirección de memoria de video */
    char* video_memory = (char*) 0xB8000;

    /* Mensaje de saludo */
    char* message = "Hello world!";

    /* Color celeste en formato de video de texto */
    char color = 0x03;

    /* Pintar el mensaje en la memoria de video con color celeste */
    for(int i = 0; message[i] != '\0'; ++i) {
        /* Carácter ASCII */
        video_memory[i*2] = message[i];
        /* Formato de color */
        video_memory[i*2+1] = color;
    }
}
