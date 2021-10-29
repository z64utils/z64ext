```
z64ext <z64.me>

SYNOPSIS
   
   z64ext is a file table expansion patch for The Legend of
   Zelda: Ocarina of Time. It is meant to be used in tandem
   with zzrtl.
   
   All code is tested and confirmed working on N64 hardware as
   well as Wii VC, though it's all held together by metaphorical
   duct tape. Scene/actor/object/dma tables all use new formats
   that allow more entries to fit into each, and have been tested
   quite thoroughly.
   
   Assembly files get assembled using minasm, for which binaries
   are included in bin/util. minasm has since been open-sourced
   under the name minimips64, which you can find on my website.
   This code base, however, has not been updated to account for
   minimips64's arguments, which may be different from minasm's.
   
   Despite getting the code to a working state, I never bothered
   cleaning it up, so please excuse the spaghetti (both in the
   code, and the way everything is organized). Should anyone ever
   update it, I highly recommend incorporating the symbols and
   types from the z64hdr project.

BUILDING

   1. Place your copy of oot-debug.z64 (not provided) in the
      same folder as this README.
      
   2. Navigate into private/zzrtl and run the oot_dump.rtl
      build script using zzrtl. The game's filesystem should
      be dumped into the private/zzrtl folder.
      
   3. The Makefile currently expects a zzrtl executable to be
      in private/zzrtl. Place the latest version there, and
      tweak the Makefile if necessary.
      
      NOTE: The dump script is the vanilla zzrtl OoT dump script,
            but the build script has extra features specific to
            the z64ext file table patches.
      
   2. In the same directory as this README, run:
      
      make clean && make z64ext GAME=oot-debug CODEC=yaz
      
      NOTE: oot-debug.z64 is modified during this step (not all
            changes are isolated to individual patches yet; boot
            may be the only one not yet handled this way)
      
      NOTE: Please keep an oot-debug-clean.z64 backed up in case
            a breaking change is introduced as you work. There is
            one disabled line in the Makefile that overwrites
            oot-debug.z64 with oot-debug-clean.z64 each time.
            Re-enable it if you find that convenient.
      
   3. If all goes well, build.z64 will appear in private/zzrtl.

MOTIVE

   In Zelda 64 games on the Nintendo 64, modders are limited to
   replacing content that already exists. This is because files
   are organized into tables corresponding with their types. The
   scene table stores scene files, the actor overlay table stores
   actor overlay files, etc.
   
   The goal of this project was to increase the amount of entries
   that can fit into each table, without resizing or relocating
   them. By reducing the number of bytes required to store one
   table entry, the entire table shrinks. Thus, more entries can
   be fit into the same space originally allocated for that table.
   This was accomplished by changing how each table is formatted,
   which means meticulously updating the functions handling each.
   
FORMATTING
   
   All formats are described in hexadecimal notation unless
   otherwise specified. 'Old Format' refers to the format
   originally used by the game, whereas 'New Format' refers
   to the way each table is formatted with the z64ext patch.
   
   Scene Table
   
      Old Format
      
         ssssssss eeeeeeee xxxxxxxx zzzzzzzz aarrbb00
         
         s = vrom start
         e = vrom end
         x = title card start
         z = title card end
         a = unknown parameter
         r = render init function id
         b = unknown parameter
      
      New Format
         
         dddd
         
         d = dma index
         
         DMA prefix
         
         xxxxxxxx zzzzzzzz aarrbb00 00000000
         
         x = title card start
         z = title card end
         a = unknown parameter
         r = render init function id
         b = unknown parameter
   
   Object Table
   
      Old Format
      
         ssssssss eeeeeeee
         
         s = vrom start
         e = vrom end
      
      New Format
         
         dddd
         
         d = dma index
         
         No DMA prefix
   
   Actor Table
   
      Old Format
      
         ssssssss eeeeeeee xxxxxxxx zzzzzzzz
         rrrrrrrr iiiiiiii nnnnnnnn aaaa0000
         
         s = vrom start
         e = vrom end
         x = vram start
         z = vram end
         r = ram address (when loaded, 0 if not loaded)
         i = virtual initialization variable address
         n = name address (or 0 if no filename present)
         a = allocation type
      
      New Format
         
         dddd aaaa rrrrrrrr
         
         d = dma index
         a = allocation type
         r = ram address (when loaded, 0 if not loaded)
         
         DMA prefix
         
         xxxxxxxx zzzzzzzz aaaa0000 iiiiiiii
         
         x = vram start
         z = vram end
         a = allocation type
         i = virtual initialization variable address
   
   DMA Table
   
      Old Format
      
         ssssssss eeeeeeee xxxxxxxx zzzzzzzz
         
         s = vrom start (virtual start / end)
         e = vrom end
         x = rom start (physical start / end)
         z = rom end (only for compressed files)
      
      New Format
         
         Entries overlap when doing so saves space.
         
         ssssssss bbbbbbbb eeeeeeee
                           ssssssss pppppppp ...
         
         s = virtual start
         b = bitfield
         e = virtual end
         
         b is a bitfield:
            
            cppp pppp pppp ppho
            
            c = if non-zero, file is compressed
            p = physical file start (b & 0x7FFFFFFC)
            h = if non-zero, file has dma prefix (header)
            o = if non-zero, this entry overlaps the next

HOW DMA PREFIXES WORK
   
   One trick employed to save space in each table, is moving
   superfluous data into what I refer to as a DMA prefix. This
   is a 16-byte block of data that optionally prefixes each file
   that is packed into the game. The data is only needed at the
   time that the file is loaded, so it's actually beneficial to
   store it in rom instead of allowing it to waste ram.
   
   In addition to the z_file_load() function being updated to
   be compatible with the new DMA table format, two functions
   have been introduced for retrieving a file's DMA prefix:
   
      /* get userdata (prefix) of a dmadata file, by index;
       * the prefix is written into the data pointed to by
       * dst, which must be at least 16 bytes long
       */
      void *z_dma_udata(int index, void *dst);
      
      /* get userdata (prefix) of a dmadata file, by index;
       * the prefix is written into a static buffer reserved
       * by z_file_load, so each time this function is called,
       * the data previously returned from it expires; for this
       * reason, use or copy the data immediately after invoking
       * this function; do not store the pointer it returns!
       */
      void *z_dma_udata_temp(int index);
   
   The user data is prefixed with the following structure:
   
   	bbbbbbbb 00000000 ssssssss eeeeeeee
   	
   	b = size in bytes
   	s = virtual start
   	e = virtual end
   
TODO
   
   Ports
      ✓  OoT debug
         OoT NTSC 1.0
         Majora's Mask... ?
   
   Tables
      ✓  Scene
      ✓  Object
      ✓  Actor
         Particle
         Text
      ✓  DMA

CHANGELOG
   
   0.0.0
   
      Changes the formats of following tables:
         Scene (fits 10x as many entries)
         Object (fits 4x as many entries)
         Actor (fits 4x as many entries)
         DMA (fits 1.333x to 2x as many entries, depending
              on how many entries are able to be overlapped)
```
