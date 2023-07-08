#include <iostream>
#include <fstream>

// Tamaño del sector de disquete
const int SECTOR_SIZE = 512;

class FloppyDiskDriver {
public:
    FloppyDiskDriver() {
        // Abre el archivo de imagen del disquete en modo binario
        floppyImage.open("floppy.img", std::ios::binary);
    }

    ~FloppyDiskDriver() {
        // Cierra el archivo de imagen del disquete
        floppyImage.close();
    }

    bool readSector(int sectorNumber, char* buffer) {
        if (floppyImage.is_open()) {
            // Calcula la posición del sector en el archivo de imagen del disquete
            std::streampos position = sectorNumber * SECTOR_SIZE;

            // Mueve el puntero de lectura del archivo a la posición del sector
            floppyImage.seekg(position);

            // Lee el sector del archivo y lo almacena en el búfer
            floppyImage.read(buffer, SECTOR_SIZE);

            return true;
        }

        return false;
    }

    bool writeSector(int sectorNumber, const char* buffer) {
        if (floppyImage.is_open()) {
            // Calcula la posición del sector en el archivo de imagen del disquete
            std::streampos position = sectorNumber * SECTOR_SIZE;

            // Mueve el puntero de escritura del archivo a la posición del sector
            floppyImage.seekp(position);

            // Escribe el contenido del búfer en el sector del archivo
            floppyImage.write(buffer, SECTOR_SIZE);

            return true;
        }

        return false;
    }

private:
    std::fstream floppyImage; // Archivo de imagen del disquete
};

int main() {
    FloppyDiskDriver floppyDriver;

    // Leer el sector 0 del disquete
    char sectorData[SECTOR_SIZE];
    if (floppyDriver.readSector(0, sectorData)) {
        std::cout << "Sector 0 read successfully." << std::endl;
        // Realizar acciones con los datos del sector leído...
    } else {
        std::cout << "Error reading sector 0." << std::endl;
    }

    // Escribir en el sector 1 del disquete
    const char newData[SECTOR_SIZE] = "New data";
    if (floppyDriver.writeSector(1, newData)) {
        std::cout << "Sector 1 written successfully." << std::endl;
        // Realizar acciones después de escribir en el sector...
    } else {
        std::cout << "Error writing sector 1." << std::endl;
    }

    return 0;
}
