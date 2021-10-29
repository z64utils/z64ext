/* c comments */
; this patches the file named boot in oot-debug
; code starts at 80000460 in ram

; 800005a0 aka Main_ThreadEntry
> seek 0x140
> incbin "c/bin/Main_ThreadEntry.bin"

