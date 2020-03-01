GPPPARAMS = -m32 -fno-use-cxa-atexit -nostdlib -fno-builtin -fno-rtti -fno-exceptions -fno-leading-underscore
# as the compiler assumes that the OS will take care of dynamic memory allocation,exception handling
# we hace to explicitly tell state that no such services,libraries are provided yet
ASPARAMS = --32
LDPARAMS = -melf_i386

objects = loader.o gdt.o interrupts.o port.o interruptstubs.o kernel.o

%.o: %.cpp	# instructions to convert .cpp files to .o file
	g++ $(GPPPARAMS) -o  $@ -c $<

%.o: %.s	# instructions to convert .s files to .o file
	as $(ASPARAMS) -o $@ $<

mykernel.bin: linker.ld $(objects)	# to generate .bin file from linker.ld file
	ld $(LDPARAMS) -T $< -o $@ $(objects)

install: mykernel.bin
	sudo cp $< /boot/mykernel.bin

mykernel.iso: mykernel.bin		#creating .iso file
	mkdir iso
	mkdir iso/boot
	mkdir iso/boot/grub
	cp mykernel.bin iso/boot/mykernel.bin
	echo 'set timeout=0'                      > iso/boot/grub/grub.cfg
	echo 'set default=0'                     >> iso/boot/grub/grub.cfg
	echo ''                                  >> iso/boot/grub/grub.cfg
	echo 'menuentry "My Operating System" {' >> iso/boot/grub/grub.cfg
	echo '  multiboot /boot/mykernel.bin'    >> iso/boot/grub/grub.cfg
	echo '  boot'                            >> iso/boot/grub/grub.cfg
	echo '}'                                 >> iso/boot/grub/grub.cfg
	grub-mkrescue --output=mykernel.iso iso
	rm -rf iso


run: mykernel.iso
	(killall VirtualBox && sleep 1) || true
	VirtualBox --startvm 'My Operating System' &

.PHONY: clean	# removing all objects,.bin,.iso file
clean:
	rm -rf obj mykernel.bin mykernel.iso