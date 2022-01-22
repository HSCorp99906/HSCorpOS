all:
	@ ##
	@ # Buildscript for HSCorp99906
	@ #
	@ # @Makefile
	@ # @version 1.0	
	@ #
	@ #
	@ # Build kernel.
	nasm -fbin src/x86_64/boot/bootloader.S -o bin/bootloader.bin
	nasm -felf src/x86_64/kernel/kasm.S -o obj/kasm.o
	nasm -felf src/x86_64/kernel/interrupts/syscalls/syscall_dispatcher.S -o obj/syscalldispatcher.o
	nasm -felf src/x86_64/applications/root/startupShell/shellutils.S -o obj/shellutils.o
	gcc -c -m32 src/x86_64/kernel/kmain.c -mgeneral-regs-only -ffreestanding -fno-pie -fstack-protector -o obj/kmain.o
	gcc -c -m32 src/x86_64/kernel/util/impl/strings.c -ffreestanding -fno-pie -fstack-protector -o obj/strings.o
	gcc -c -m32 src/x86_64/kernel/drivers/impl/VGA.c -ffreestanding -fno-pie -fstack-protector -o obj/vga.o
	gcc -c -m32 src/x86_64/kernel/drivers/impl/IO.c -ffreestanding -fno-pie -fstack-protector -o obj/IO.o
	gcc -c -m32 src/x86_64/applications/root/startupShell/main.c -ffreestanding -fno-pie -fstack-protector -o obj/startupShell.o
	gcc -c -m32 src/x86_64/kernel/util/impl/startup.c -ffreestanding -fno-pie -fstack-protector -o obj/startup.o
	i686-elf-ld -Tlink.ld obj/kasm.o obj/kmain.o obj/strings.o \
		obj/vga.o obj/IO.o obj/syscalldispatcher.o obj/shellutils.o obj/startupShell.o \
		obj/startup.o --oformat binary -o bin/kernel.bin
	cat bin/bootloader.bin bin/kernel.bin > bin/HSCorpOS.bin
	@ # Prepare the image.
	sudo dd if=/dev/zero of=HSCorpOS.img bs=1024 count=1440
	@ # Put the OS stuff in the image.
	sudo dd if=bin/HSCorpOS.bin of=HSCorpOS.img
	sudo dd if=HSCorpOS.img of=/dev/sdb


run:
	sudo qemu-system-x86_64 -hda /dev/sdb -monitor stdio
