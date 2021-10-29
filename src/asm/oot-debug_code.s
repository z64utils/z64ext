/* c comments */
; this patches the file named code in oot-debug
; code starts at 8001ce60 in ram

> import "z_dma.txt"

/* this is inside func_8003BB18 */
/* func_8003B3C8 is executed, and its return value says whether *
 * or not to test collision */
/*; just a fun test
> seek 0x1EF74
nop                  ; always collision test
;beq r0, r0, 0x0006  ; always skip collision testing
;beq v0, r0, 0x0006  ; conditionally skip collision test (original)*/

> seek 0xA31A8
> incbin "c/bin/z_scene_load.bin"

> define zx_dma_udata_tempP 0x8012A280
> define zx_scene_command_9 0x80118d80 /* this is end of actor table, minus 4kb (0x1000 bytes) */

; write pointer to custom function zx_scene_command_9
> seek 0x10A664
> write32 zx_scene_command_9

> seek 0xFBF20 /* actortable end - 0x1000 */
> incbin "c/bin/zx_scene_command_9.bin"

/*;> seek 0xFFE0 ; 8002ce40
;> incbin "c/bin/z_actor_titlecard_placename.bin"*/

; disable these two logging functions
> seek 0x1B560 jr ra nop ; 800383c0 aka ActorOverlayTable_LogPrint
> seek 0x1B644 jr ra nop ; 800384a4 aka ActorOverlayTable_FaultPrint

> seek 0x7B6A8
> incbin "c/bin/z_scene_spawn_player.bin"

> seek 0x150F0
> incbin "c/bin/z_actor_spawn.bin"

> seek 0x1367C
> incbin "c/bin/z_actor_init_context.bin"

/* function 800901738 */
/* this code is used for loading Link and gameplay_keep on the pause screen */
/* it has been refactored to use z_dma_udata_temp */
> seek 0x748D8
/*000748E0*/	addiu   sp, sp, 0xffb8
/*000748F8*/	sw      a0, 0x0048(sp)
/*000748EC*/	sw      a1, 0x004c(sp)
/*00074910*/	sw      ra, 0x002c(sp)
/*00074914*/	sw      a2, 0x0050(sp)
/*00074920*/	sw      s0, 0x0028(sp)
/*000748E4*/	lui     v1, 0x8012       ; objtable
               jal     z_dma_udata_temp
/*000748E8*/	lh      a0, 0x752A(v1)   ; ...
/*00074904*/	lw      a1, 0x0008(v0)   ; gk start = udata[2]
nop;///*00074908*/	lw      t8, 0x000c(v0)   ; gk end
/*0007490C*/	lw      a0, 0x004c(sp)
/*00074918*/	lui     a3, 0x8014
/*0007491C*/	addiu   t9, r0, 0x0ba6
/*00074924*/	sw      t9, 0x0010(sp)
/*00074928*/	addiu   a3, a3, 0xe8b0
/*00074930*/	lw      a2, 0x0000(v0)   ; gk size = udata[0]
/*00074934*/	jal     0x1aa0           ; load gameplay_keep
/*00074938*/	addiu   a0, a0, 0x3800
/*000748D8*/	lui     v0, 0x8016       ; link form (age)
/*000748DC*/	lw      v0, 0xe664(v0)   ; ...
/*000748F0*/	lui     t7, 0x8012
/*000748F4*/	sll     t6, v0, 0x0001   ; index into form[x]
/*000748FC*/	addu    t7, t7, t6
/*00074900*/	lh      t7, 0x7520(t7)   ; t7 = link objid
;///*0007492C*/	sh      t7, 0x0046(sp)   ; store for later
;///*0007493C*/	lh      t2, 0x0046(sp)   ; reload it
/*00074940*/	lui     t4, 0x8012
/*00074944*/	addiu   t4, t4, 0x7528
/*00074948*/	sll     t3, t7, 0x0001 ;!; * 2 instead of * 8
/*0007494C*/	addu    v0, t3, t4
               jal     z_dma_udata_temp
               lh      a0, 0x0000(v0)   ; link object index
/*00074950*/	lw      a1, 0x0008(v0)   ; link zobj start = udata[2]
;///*00074954*/	lw      t5, 0x0004(v0)
/*00074958*/	lw      a0, 0x004c(sp)
/*0007495C*/	ori     at, r0, 0x8800
/*00074960*/	lui     a3, 0x8014
/*00074964*/	addiu   t6, r0, 0x0bac
;///*00074968*/	subu    s0, t5, a1
/*0007496C*/	addu    a0, a0, at
/*00074970*/	sw      a0, 0x0034(sp)
/*00074974*/	lw      a2, 0x0000(v0)   ; link zobj size
               lw      s0, 0x0000(v0)
/*00074978*/	sw      t6, 0x0010(sp)
/*0007497C*/	jal     0x1aa0           ; load link object
/*00074980*/	addiu   a3, a3, 0xe8c4
/*00074984*/	lw      t7, 0x0034(sp)
/*00074988*/	addiu   at, r0, 0xfff0
/*0007498C*/	lw      t1, 0x004c(sp)
/*00074990*/	addu    v1, t7, s0
/*00074994*/	addiu   v1, v1, 0x000f
/*00074998*/	and     v1, v1, at
/*0007499C*/	lui     at, 0x8000
/*000749A0*/	ori     at, at, 0x3800
/*000749A4*/	addu    t8, t1, at
/*000749A8*/	lui     t0, 0x8016
/*000749AC*/	lui     at, 0x8000
/*000749B0*/	addiu   t0, t0, 0x6fa8
/*000749B4*/	ori     at, at, 0x8800
/*000749B8*/	addu    t9, t1, at
/*000749BC*/	sw      t8, 0x0010(t0)
/*000749C0*/	sw      t9, 0x0018(t0)
/*000749C4*/	lui     v0, 0x8016
/*000749C8*/	lw      v0, 0xe664(v0)
/*000749CC*/	lui     a2, 0x8012
/*000749D0*/	lui     a3, 0x0400
/*000749D4*/	sll     t2, v0, 0x0002
/*000749D8*/	addu    a2, a2, t2
/*000749DC*/	addiu   t3, r0, 0x0009
/*000749E0*/	addiu   t4, r0, 0x0016
/*000749E4*/	sw      t4, 0x001c(sp)
/*000749E8*/	sw      t3, 0x0010(sp)
/*000749EC*/	lw      a2, 0x5b70(a2)
/*000749F0*/	addiu   a3, a3, 0x3238
/*000749F4*/	sw      v1, 0x0014(sp)
/*000749F8*/	sw      v1, 0x0018(sp)
/*000749FC*/	lw      a0, 0x0048(sp)
/*00074A00*/	jal     0xa39ac
/*00074A04*/	lw      a1, 0x0050(sp)
/*00074A08*/	lw      ra, 0x002c(sp)
/*00074A0C*/	ori     at, r0, 0x8890
/*00074A10*/	addu    v0, s0, at
/*00074A14*/	lw      s0, 0x0028(sp)
/*00074A18*/	jr      ra
/*00074A1C*/	addiu   sp, sp, 0x0048



> seek 0x7ADA0
> incbin "c/bin/z_object_spawn.bin"

> seek 0x7B13C
> incbin "c/bin/z_object_update_slot.bin"

> seek 0x7B358
> incbin "c/bin/z_objectctx_load_slots.bin"

> seek 0x7B49C
> incbin "c/bin/z_objectctx_prepare_slot.bin";

/* testing; these are not necessary */
/* 800326ac : ActorDelete */
/*> seek 0x1584C
	;jr ra nop
	> incbin "c/bin/z_actor_delete.bin"

> seek 0x10928
	;jr ra nop ; 8002d788 : Actor_Destroy
	> incbin "c/bin/z_actor_destroy.bin"

;> seek 0x13CCC jr ra nop ; 80030b2c : Actor_FaultPrint ; just a debugging function

> seek 0x14FCC
	;jr ra nop ; 80031e2c : Actor_FreeOverlay
	> incbin "c/bin/z_actor_free_ovl.bin"*/

