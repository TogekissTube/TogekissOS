ASM=nasm

SRC_DIR=src
BUILD_DIR=build

$(BUILD_DIR)/main_floppy.img: $(BUILD_DIR)/boot.bin 
	cat $(BUILD_DIR)/boot.bin  > $(BUILD_DIR)/main_floppy.img
	truncate -s 1440k $(BUILD_DIR)/main_floppy.img

$(BUILD_DIR)/boot.bin: $(SRC_DIR)/boot.asm
	$(ASM) $(SRC_DIR)/boot.asm -f bin -o $(BUILD_DIR)/boot.bin


