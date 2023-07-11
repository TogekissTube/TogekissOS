#include <iostream>
#include <vector>
#include <scsi/sg.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>

#define SCSI_DEVICE_PATH "/dev/sg0"  // SCSI device path

bool readDataSCSI(int sector, int length, std::vector<uint8_t>& buffer) {
    int fd = open(SCSI_DEVICE_PATH, O_RDWR | O_NONBLOCK);
    if (fd == -1) {
        std::cout << "Error opening SCSI device.\n";
        return false;
    }

    sg_io_hdr_t io_hdr{};
    std::vector<uint8_t> cdb(10);  // SCSI read command

    // Configure the SCSI read command
    cdb[0] = 0x28;                 // Read command OpCode
    // Set the LBA (Logical Block Address) and length
    // in the control bytes of the command
    cdb[2] = (sector >> 24) & 0xFF;  
    cdb[3] = (sector >> 16) & 0xFF;
    cdb[4] = (sector >> 8) & 0xFF;
    cdb[5] = sector & 0xFF;
    cdb[7] = (length >> 8) & 0xFF;
    cdb[8] = length & 0xFF;

    io_hdr.interface_id = 'S';
    io_hdr.cmd_len = cdb.size();
    io_hdr.cmdp = cdb.data();
    io_hdr.dxfer_direction = SG_DXFER_FROM_DEV;
    io_hdr.dxfer_len = length * 512;  // Buffer size in bytes
    io_hdr.dxferp = buffer.data();
    io_hdr.timeout = 10000;  // Timeout in milliseconds

    if (ioctl(fd, SG_IO, &io_hdr) < 0 || io_hdr.status != 0) {
        std::cout << "Error during SCSI read operation.\n";
        close(fd);
        return false;
    }

    close(fd);
    return true;
}

bool writeDataSCSI(int sector, int length, const std::vector<uint8_t>& buffer) {
    int fd = open(SCSI_DEVICE_PATH, O_RDWR | O_NONBLOCK);
    if (fd == -1) {
        std::cout << "Error opening SCSI device.\n";
        return false;
    }

    sg_io_hdr_t io_hdr{};
    std::vector<uint8_t> cdb(10);  // SCSI write command

    // Configure the SCSI write command
    cdb[0] = 0x2A;                 // Write command OpCode
    // Set the LBA (Logical Block Address) and length
    // in the control bytes of the command
    cdb[2] = (sector >> 24) & 0xFF;
    cdb[3] = (sector >> 16) & 0xFF;
    cdb[4] = (sector >> 8) & 0xFF;
    cdb[5] = sector & 0xFF;
    cdb[7] = (length >> 8) & 0xFF;
    cdb[8] = length & 0xFF;

    io_hdr.interface_id = 'S';
    io_hdr.cmd_len = cdb.size();
    io_hdr.cmdp = cdb.data();
    io_hdr.dxfer_direction = SG_DXFER_TO_DEV;
    io_hdr.dxfer_len = length * 512;  // Buffer size in bytes
    io_hdr.dxferp = const_cast<uint8_t*>(buffer.data());
    io_hdr.timeout = 10000;  // Timeout in milliseconds

    if (ioctl(fd, SG_IO, &io_hdr) < 0 || io_hdr.status != 0) {
        std::cout << "Error during SCSI write operation.\n";
        close(fd);
        return false;
    }

    close(fd);
    return true;
}

int main() {
    std::vector<uint8_t> buffer(512);  // Buffer for SCSI data

    // Example usage: Read data from SCSI device
    if (readDataSCSI(0, 1, buffer)) {
        std::cout << "Data read successfully.\n";
        // Do something with the read data...
    } else {
        std::cout << "Error reading data from SCSI device.\n";
    }

    // Example usage: Write data to SCSI device
    // std::vector<uint8_t> writeBuffer(512);  // Buffer with data to write
    // Initialize writeBuffer with data to write...
    // if (writeDataSCSI(0, 1, writeBuffer)) {
    //     std::cout << "Data written successfully.\n";
    // } else {
    //     std::cout << "Error writing data to SCSI device.\n";
    // }

    return 0;
}
