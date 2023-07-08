#include <iostream>
#include <conio.h>

struct Color {
    unsigned char red;
    unsigned char green;
    unsigned char blue;
};

const Color palette[] = {
    { 0, 0, 0 },        // Color 0: Negro
    { 0, 0, 170 },      // Color 1: Azul
    { 0, 170, 0 },      // Color 2: Verde
    { 0, 170, 170 }     // Color 3: Cian
};

// Constantes para el controlador CGA
const int SCREEN_WIDTH = 320;
const int SCREEN_HEIGHT = 200;
const int SCREEN_SIZE = SCREEN_WIDTH * SCREEN_HEIGHT;

// Puntero al búfer de video
unsigned char* videoBuffer = (unsigned char*)0xB8000;

// Función para configurar el modo de video CGA
void setVideoMode() {
    asm {
        mov ax, 0x0013
        int 0x10
    }
}

void setPixel(int x, int y, unsigned char colorIndex) {
    int offset = y * SCREEN_WIDTH + x;
    videoBuffer[offset * 2] = colorIndex;
}

void drawRectangle(int x, int y, int width, int height, unsigned char colorIndex) {
    for (int i = y; i < y + height; i++) {
        for (int j = x; j < x + width; j++) {
            setPixel(j, i, colorIndex);
        }
    }
}


// Función principal
int main() {
    setVideoMode();

   drawRectangle(50, 50, 100, 50, 1);  // Dibuja un rectángulo usando el color rojo de la paleta (color 1)


    getch();  // Espera a que el usuario presione una tecla

    return 0;
}
