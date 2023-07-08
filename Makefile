ASM=nasm
CC=gcc

SRC_DIR=src
BUILD_DIR=build

$(BUILD_DIR)/main_floppy.img: $(BUILD_DIR)/boot.bin $(BUILD_DIR)/gdt.o $(BUILD_DIR)/idt.o $(BUILD_DIR)/tss.o
	cat $(BUILD_DIR)/boot.bin $(BUILD_DIR)/gdt.o $(BUILD_DIR)/idt.o $(BUILD_DIR)/tss.o > $(BUILD_DIR)/main_floppy.img
	truncate -s 1440k $(BUILD_DIR)/main_floppy.img

$(BUILD_DIR)/boot.bin: $(SRC_DIR)/boot.asm
	$(ASM) $(SRC_DIR)/boot.asm -f bin -o $(BUILD_DIR)/boot.bin

$(BUILD_DIR)/gdt.o: $(SRC_DIR)/kernel/gdt.c
	$(CC) -c $(SRC_DIR)/kernel/gdt.c -o $(BUILD_DIR)/gdt.o

$(BUILD_DIR)/idt.o: $(SRC_DIR)/kernel/idt.c
	$(CC) -c $(SRC_DIR)/kernel/idt.c -o $(BUILD_DIR)/idt.o

$(BUILD_DIR)/tss.o: $(SRC_DIR)/kernel/tss.c
	$(CC) -c $(SRC_DIR)/kernel/tss.c -o $(BUILD_DIR)/tss.o
