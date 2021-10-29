#export PATH="$PATH:/opt/n64/bin"
#run make GAME=oot-debug
# N64 toolchain
CC      = mips64-gcc
LD      = mips64-ld
OBJCOPY = mips64-objcopy
OBJDUMP = mips64-objdump

# default compilation flags
CFLAGS = -DNDEBUG -Wall -Wno-main -mno-gpopt -fno-toplevel-reorder -fomit-frame-pointer -G 0 -Os --std=gnu99 -mtune=vr4300 -mabi=32 -mips3 -mno-check-zero-division -mno-explicit-relocs -mno-memcpy
LDFLAGS = --emit-relocs -T

# every GAME option should have a matching .ld of the same name
LDFILE = ../../ld/$(GAME).ld

# fetch DEFINEs from the linker file
CFLAGS += $(shell cat $(LDFILE) | grep DEFINE | sed -e 's/\s.*$$//')

%.bin : %.c
	@$(CC) -c $< $(CFLAGS)
	@$(LD) -o $(basename $<).elf $(basename $<).o $(LDFLAGS) $(LDFILE)
	@$(OBJCOPY) -R .MIPS.abiflags -O binary $(basename $<).elf $@

all: $(addsuffix .bin, $(basename $(wildcard *.c)))
	mkdir -p bin
	mv *.bin *.elf *.o bin

clean:
	rm -f *.bin *.elf *.o


