all:
	## bootloader ###
	mkdir build
	nasm -f elf32 src/kernel/boot/boot.s -o build/boot.o 

	### Kernel #####
	gcc -m32 -fno-stack-protector -fno-builtin -c src/kernel/kernel.c -o build/kernel.o

	### else #####
	ld -m elf_i386 -T src/kernel/linker.ld -o kernel.elf build/boot.o build/kernel.o 
	mv kernel.elf togekissos/boot/kernel.elf
	grub-mkrescue -o build/togekissos.iso togekissos/

clean:
	rm -rf build
	rm togekissos/boot/kernel.elf

run:
	qemu-system-i386 build/togekissos.iso
