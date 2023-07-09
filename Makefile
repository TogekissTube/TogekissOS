ASM=nasm

SRC_DIR=src
BUILD_DIR=build

$(BUILD_DIR)/Disk1.img: $(BUILD_DIR)/bootloader.bin
	cp $(BUILD_DIR)/bootloader.bin $(BUILD_DIR)/Disk1.img
	truncate -s 1440k $(BUILD_DIR)/Disk1.img
    
$(BUILD_DIR)/bootloader.bin: $(SRC_DIR)/bootloader.asm
	$(ASM) $(SRC_DIR)/bootloader.asm -f bin -o $(BUILD_DIR)/bootloader.bin
