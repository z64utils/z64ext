/* c comments */
; this patches the file named ovl_player_actor in oot-debug

> import "z_dma.txt"

; all offsets used for seeking are relative
; to the start of Link's actor overlay

; this block of assembly changes how the object table
; is indexed whenever Link needs to load a gi model
; (when opening a chest, picking up magic, etc)
; (this is from within function 8083AE40)
> seek 0x8C74
/*00008C74*/	lui         t8, 0x8012          ; object table
/*00008C78*/	addiu       t8, t8, 0x7528
/*00008C7C*/	sll         t7, a3, 0x0001      ; bofs = index * 2
/*00008C80*/	addu        v0, t7, t8          ; v0 = objtable entry
/*00008C84*/	jal         z_dma_udata_temp    ; fetch udata
/*00008C88*/	lh          a0, 0x0000(v0)      ; a0 = *entry
/*00008C8C*/	lui         a0, 0x8085      ; ! ; these two lines are not usable
/*00008C90*/	addiu       a0, a0, 0x5240  ; ! ; because of relocation table
/*00008C94*/	lw          t1, 0x0000(v0)      ; t1 = size
/*00008C98*/	sw          t1, 0x0040(sp)
/*00008C9C*/	sw          v0, 0x0038(sp)      ; store v0 for later
/*00008CA0*/	jal         0x2e10
/*00008CA4*/	addiu       a1, r0, 0x2382
/*00008CA8*/	lui         a0, 0x8085
/*00008CAC*/	addiu       a0, a0, 0x5250
/*00008CB0*/	jal         0x2130
/*00008CB4*/	lw          a1, 0x0040(sp)
/*00008CB8*/	lw          t2, 0x0040(sp)
/*00008CBC*/	lui         a0, 0x8085
/*00008CC0*/	lw          v0, 0x0038(sp)
/*00008CC4*/	sltiu       at, t2, 0x2001
/*00008CC8*/	bne         at, r0, 0x0007
/*00008CCC*/	addiu       a0, a0, 0x525c
/*00008CD0*/	lui         a1, 0x8085
/*00008CD4*/	addiu       a1, a1, 0x5270
/*00008CD8*/	addiu       a2, r0, 0x2383
/*00008CDC*/	jal         0x1ff0
/*00008CE0*/	sw          v0, 0x0038(sp)
/*00008CE4*/	lw          v0, 0x0038(sp)
/*00008CE8*/	lw          a1, 0x01b0(s0)   ; a1 = dest
/*00008CEC*/	lw          t3, 0x003c(sp)
/*00008CF0*/	lui         t4, 0x8085
/*00008CF4*/	addiu       t4, t4, 0x5280
/*00008CF8*/	addiu       t5, r0, 0x238b
/*00008CFC*/	sw          t5, 0x0020(sp)
/*00008D00*/	sw          t4, 0x001c(sp)
/*00008D04*/	sw          r0, 0x0018(sp)
/*00008D08*/	sw          r0, 0x0010(sp)
/*00008D0C*/	addiu       a0, s0, 0x0174    ; a0 = req
/*00008D10*/	lw          a2, 0x0008(v0)    ; a2 = vrom
/*00008D14*/	lw          a3, 0x0040(sp)    ; a3 = size
/*00008D18*/	jal         0x1a5c
/*00008D1C*/	sw          t3, 0x0014(sp)
/*00008D20*/	lw          ra, 0x0034(sp)
/*00008D24*/	lw          s0, 0x0030(sp)
/*00008D28*/	addiu       sp, sp, 0x0048
/*00008D2C*/	jr          ra
/*00008D30*/	nop
        
        
        
        
        
        
        
        
    /*
    > seek 0x8C7C
        sll     t7, a3, 1            ; byte_ofs = index * 2
        addu    v0, t7, t8           ; entry = byte_ofs + table
        nop     sw, 
        jal     z_dma_udata_temp
        lh      a0, 0x0000(v0)       ; dma index
        lui     a0, 0x8085       ; ! ; these two lines are not usable
        addiu   a0, a0, 0x5240   ; ! ; because of relocation table
        lw      a0, 0x0008(v0)       ; file start
        lw      v0, 0x000c(v0)       ; file end
        subu    t1, v0, a0           ; size = end - start
        sw      t1, 0x0040(sp)       ; store size for later
        nop
     */
