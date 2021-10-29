#export PATH="$PATH:/opt/n64/bin"
#export PATH="$PATH:$HOME/n64/bin"
FILE   = src/$(CODEC)
OBJ    = src/$(CODEC).o
BIN    = bin/$(CODEC)
PUT    = bin/util/put        # this utility generates the cloudpatch
MINASM = bin/util/minasm

# N64 toolchain
CC      = mips64-gcc
LD      = mips64-ld
OBJCOPY = mips64-objcopy
OBJDUMP = mips64-objdump

# default compilation flags
CFLAGS = -DNDEBUG -Wall -Wno-main -mno-gpopt -fno-toplevel-reorder -fomit-frame-pointer -G 0 -Os --std=gnu99 -mtune=vr4300 -mabi=32 -mips3 -mno-check-zero-division -mno-explicit-relocs -mno-memcpy
LDFLAGS = --emit-relocs -T

# enable this if you want to build for condensed dmadata
CFLAGS += -DDMA_CONDENSED
CFLAGS += -DDMA_PACKED

# display compilation options
default:
	@echo "type: make z64ext GAME=game CODEC=CODEC"
	@echo "  GAME options"
	@echo "    oot-debug      build for OoT debug"
	@echo "    oot-ntsc-10    build for OoT NTSC 1.0"
	@echo "    mm-u           build for MM U"
	@echo "  CODEC options"
	@echo "    yaz"
	@echo "    ucl"

# every GAME option should have a matching .ld of the same name
LDFILE = src/ld/$(GAME).ld

# ROMOFS is the rom offset where we inject the code
ROMOFS = $(shell cat $(LDFILE) | grep ROMOFS | head -c 8)

# ROMGETFILE is the rom offset where we inject z_file_load.bin
ROMGETFILE = $(shell cat $(LDFILE) | grep ROMGETFILE | head -c 8)

# ROMJAL is the rom offset of the jal that invokes the original
#        function; this gets overwritten with one that will call
#        the new function
ROMJAL = $(shell cat $(LDFILE) | grep ROMJAL | head -c 8)

# MMJUMP is the rom offset of the original decompression function;
#        it happens to be invoked by `code` when processing yaz
#        archives; because these patches are meant to support both
#        compressed and decompressed versions of MM, modifying `code`
#        directly is not viable, especially considering its contents
#        can be rearranged and the file itself could be relocated; the
#        cleanest solution is to simply place a jump at the start of
#        the old function, redirecting it to the new function
MMJUMP = $(shell cat $(LDFILE) | grep MMJUMP | head -c 8)

# MAXBYTES is the number of bytes that have been deemed safe to
#          overwrite with custom code
MAXBYTES = $(shell cat $(LDFILE) | grep MAXBYTES | head -n1 | sed -e 's/\s.*$$//' | head -c -1)

# MAXGETFILE is the number of bytes that have been deemed safe to
#            overwrite with custom code (for z_file_load)
MAXGETFILE = $(shell cat $(LDFILE) | grep MAXGETFILE | head -n1 | sed -e 's/\s.*$$//' | head -c -1)

# TARGET is the cloudpatch that will be created, of the form
#        "patch/codec/game_z64enc_codec.txt"
#        this may seem verbose, but consider how cryptic mm-u.txt
#        would be in a messy download folder
# NOTE   you can change this to a rom to target a rom directly; if
#        you do that, you will want to add something to this makefile
#        to produce a clean rom each run
TARGET = $(shell printf 'patch/' && printf $(CODEC) && printf '/' && printf $(GAME) && printf '_z64enc_' && printf $(CODEC) && printf '.txt')

TARGET = oot-debug.z64

# add the linker file for the chosen ga me to the compilation flags
#LDFLAGS += $(LDFILE)

# fetch DEFINEs from the linker file
CFLAGS += $(shell cat $(LDFILE) | grep DEFINE | sed -e 's/\s.*$$//')

z64ext: all

all: clean build rompatch

build: $(OBJ)
	@mkdir -p patch/$(CODEC)
	@mkdir -p bin/util
	@$(LD) -o $(FILE).elf --defsym ADDRESS_START=$(shell cat src/ld/addr/$(GAME)/z_decompress) $(OBJ) $(LDFLAGS) $(LDFILE)
	@$(OBJCOPY) -R .MIPS.abiflags -O binary $(FILE).elf $(FILE).bin
# stats
	@printf "$(FILE).bin: "
	@stat --printf="%s" $(FILE).bin
	@printf " out of "
	@printf $(MAXBYTES)
	@printf " bytes used\n"
	@mv src/*.bin src/*.elf src/*.o bin
# build utils
	@gcc -o bin/util/put src/util/put.c
	@gcc -o bin/util/n64crc src/util/n64crc.c
# z_file_load.ld
	@echo -n "ADDRESS_START = 0x" > z_file_load.ld
	@cat $(LDFILE) | grep ADDRGETFILE | head -c 8 >> z_file_load.ld
	@echo ";" >> z_file_load.ld
	@cat $(LDFILE) | grep DMARomToRam >> z_file_load.ld
	@cat $(LDFILE) | grep z_malloc >> z_file_load.ld
	@cat $(LDFILE) | grep osSetThreadPri >> z_file_load.ld
	@cat $(LDFILE) | grep g_dmadata >> z_file_load.ld
	@echo -n "z_decompress = 0x" >> z_file_load.ld
	@$(OBJDUMP) -t $(BIN).elf | grep .text.startup | head -c 8 >> z_file_load.ld
	@echo ";" >> z_file_load.ld
	@echo "main = z_file_load;" >> z_file_load.ld
	@echo "INCLUDE src/ld/n64.ld" >> z_file_load.ld
# z_file_load.bin
	@$(CC) -c src/z_file_load.c $(CFLAGS)
	@$(LD) -o z_file_load.elf z_file_load.o $(LDFLAGS) z_file_load.ld
	@$(OBJCOPY) -R .MIPS.abiflags -O binary z_file_load.elf z_file_load.bin
# z_file_load stats
	@printf "z_file_load.bin: "
	@stat --printf="%s" z_file_load.bin
	@printf " out of "
	@printf $(MAXGETFILE)
	@printf " bytes used\n"
	@printf "functions:"
	@printf "\n  z_file_load "
	@$(OBJDUMP) -t z_file_load.elf | grep ENTRY_POINT | head -c 8
	@printf "\n  z_file_end  "
	@$(OBJDUMP) -t z_file_load.elf | grep z_file_end | head -c 8
	@printf "\n  z_dma_find  "
	@$(OBJDUMP) -t z_file_load.elf | grep z_dma_find | head -c 8
	@printf "\nz_dma_udata  0x" > src/asm/z_dma.txt
	@$(OBJDUMP) -t z_file_load.elf | grep -w z_dma_udata | head -c 8 >> src/asm/z_dma.txt
	@printf "\nasm(\"z_dma_udata = 0x" > src/asm/c/z_dma.h
	@$(OBJDUMP) -t z_file_load.elf | grep -w z_dma_udata | head -c 8 >> src/asm/c/z_dma.h
	@printf "\");" >> src/asm/c/z_dma.h
	@printf "\nz_dma_udata_temp  0x" >> src/asm/z_dma.txt
	@$(OBJDUMP) -t z_file_load.elf | grep z_dma_udata_temp | head -c 8 >> src/asm/z_dma.txt
	@printf "\nasm(\"z_dma_udata_temp = 0x" >> src/asm/c/z_dma.h
	@$(OBJDUMP) -t z_file_load.elf | grep z_dma_udata_temp | head -c 8 >> src/asm/c/z_dma.h
	@printf "\");" >> src/asm/c/z_dma.h
	
	@printf "\n"
# z_scene_load.bin
#	@$(CC) -c src/z_scene_load.c $(CFLAGS)
#	@touch z_scene_load.ld
#	@$(LD) -o z_scene_load.elf z_scene_load.o $(LDFLAGS) z_scene_load.ld
#	@$(OBJCOPY) -R .MIPS.abiflags -O binary z_scene_load.elf z_scene_load.bin
# compile individual function sources
	$(MAKE) -C src/asm/c
# cleanup
	@mv *.bin *.elf *.o *.ld bin
	@mv bin/n64.ld n64.ld

rompatch:
#	@$(PUT) $(TARGET) --file 0x0 10u-yaz-unpatched.z64
	@$(PUT) $(TARGET) --file 0x0 oot-debug.z64
	@$(PUT) $(TARGET) --file $(ROMOFS) $(BIN).bin
	@$(PUT) $(TARGET) --jal $(ROMJAL) $(shell $(OBJDUMP) -t $(BIN).elf | grep .text.startup | head -c 8)
	@$(PUT) $(TARGET) --file $(ROMGETFILE) bin/z_file_load.bin
#	@$(PUT) $(TARGET) --file 0xB371A8 bin/z_scene_load.bin
#	@$(MINASM) --Ooot-debug.z64 --B0xA94000 --N src/asm/$(GAME)_code.s
	@$(MINASM) --Ocode.txt   src/asm/$(GAME)_code.s
	@$(MINASM) --Oplayer.txt src/asm/$(GAME)_player.s
	@mv player.txt private/zzrtl/system/overlay/ovl_player_actor/z64ext_player.txt
	@mv code.txt   private/zzrtl/z64ext_code.txt
#	@$(MINASM) --Ooot-debug.z64 --B0x1060 --N src/asm/$(GAME)_boot.s
# disable sanity checks and debug info
#	@$(PUT) $(TARGET) --bytes 0x24B0 1000003E
# test: ovl_player_actor
#	@$(PUT) $(TARGET) --bytes 0xC09D30 01F8102110000003AFA200383C048085248452400C0005348C44000000444823AFA9004000000000
# NOTE the following is MM only
	@if [ $(shell echo $(GAME) | head -c 3) = "mm-" ]; then\
		$(PUT) $(TARGET) --jump $(MMJUMP) $(shell $(OBJDUMP) -t $(BIN).elf | grep .text.startup | head -c 8); \
	fi
#	xfer/minasm --Ooot-debug.z64 --B0xA94000 --N xfer/code-debug-linkdma-objects1.s
#	xfer/minasm --Ooot-debug.z64 --B0xA94000 --N xfer/scene.s
	private/zzrtl/zzrtl private/zzrtl/oot_build.rtl
#	bin/util/n64crc Ooot-debug.z64

clean:
	@echo "do nothing"

