GPPPARAMS = -m32
ASPARAMS = --32
LDPARAMS = -melf_i386
objects = loader.o kernel.o

%.o: %.cpp
		g++ $(GPPPARAMS) -o $@ -c $<
%.o: %.s
		as $(ASPARAMS) -o $@ $<

kernel.bin: linker.ld $(objects)
		ld $(LDPARAMS) -T $< -o $@ $(objects)

install: kernel.bin
		sudo cp $< /boot/kernel.bin