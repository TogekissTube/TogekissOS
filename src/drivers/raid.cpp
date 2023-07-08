#include <iostream>
#include <vector>

// Estructura para representar un disco en el RAID
struct Disk {
    int id;
    bool isOnline;
    std::vector<std::string> dataBlocks; // Vector de bloques de datos para RAID 0 y 2
};

// Clase del controlador RAID
class RaidController {
public:
    RaidController() {
        // Inicializar discos
        for (int i = 0; i < MAX_DISKS; i++) {
            disks[i].id = i;
            disks[i].isOnline = true;
        }
    }

    void readData(int diskId, int sector) {
        if (disks[diskId].isOnline) {
            std::cout << "Reading data from disk " << diskId << ", sector " << sector << ": " << disks[diskId].dataBlocks[sector] << std::endl;
        } else {
            std::cout << "Disk " << diskId << " is offline. Unable to read data." << std::endl;
        }
    }

    void writeData(int diskId, int sector, const std::string& data) {
        if (disks[diskId].isOnline) {
            disks[diskId].dataBlocks[sector] = data;
            std::cout << "Writing data to disk " << diskId << ", sector " << sector << ": " << data << std::endl;
        } else {
            std::cout << "Disk " << diskId << " is offline. Unable to write data." << std::endl;
        }
    }

private:
    static const int MAX_DISKS = 4;
    Disk disks[MAX_DISKS];
};

int main() {
    RaidController raidController;

    // Leer datos del disco 2, sector 10 (RAID 0)
    raidController.readData(2, 10);

    // Escribir datos en el disco 0, sector 5 (RAID 0)
    raidController.writeData(0, 5, "Hello, RAID!");

    // Leer datos del disco 1, sector 5 (RAID 1)
    raidController.readData(1, 5);

    // Escribir datos en el disco 1, sector 5 (RAID 1)
    raidController.writeData(1, 5, "Hello again, RAID!");

    // Leer datos del disco 3, sector 2 (RAID 2)
    raidController.readData(3, 2);

    // Escribir datos en el disco 3, sector 2 (RAID 2)
    raidController.writeData(3, 2, "RAID 2 data");

    // Leer datos del disco 2, sector 7 (RAID 3)
    raidController.readData(2, 7);

    // Escribir datos en el disco 2, sector 7 (RAID 3)
    raidController.writeData(2, 7, "RAID 3 data");

    return 0;
}
