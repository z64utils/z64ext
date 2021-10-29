#include "z64.h"

#define ACTOR_NUMBER_MAX 200
#define AM_FIELD_SIZE 0x27A0

extern struct z_actor_table g_actortable[471];

/* oot debug 80031f50 */
void *z_actor_spawn(
	struct z_actorctx *actorctx
	, struct z_glctx *gl
	, short actorId
	, float posX
	, float posY
	, float posZ
	, short rotX
	, short rotY
	, short rotZ
	, short params
)
{
	struct z_actor *actor;
	struct z_actor_ivar *ivar;
	int obj_slot;
	struct z_actor_table *ovl;
	unsigned temp;
	unsigned overlaySize;
	char room_id = *((char*)&gl->roomctx); /* literally first byte */
	
	/* TODO allocate an overlay */
	//ovl = z_malloc_r(sizeof(*ovl));

	ovl = &g_actortable[actorId];
	ivar = ovl->ivar;

	/*if (actorId >= sizeof(g_actortable) / sizeof(*g_actortable)) {
		__assert("profile < ACTOR_DLF_MAX", name, 0);
	}*/
	
	overlaySize = ovl->vram.w1/*end*/ - ovl->vram.w0/*start*/;

	/*if (HREG(20) != 0) {
		// Translates to: "ACTOR CLASS ADDITION [%d:%s]"
		osSyncPrintf("アクタークラス追加 [%d:%s]\n", actorId, name);
	}*/

	if (actorctx->total > ACTOR_NUMBER_MAX) {
		// Translates to: "Ａｃｔｏｒ SET NUMBER EXCEEDED"
		//osSyncPrintf(VT_COL(YELLOW, BLACK) "Ａｃｔｏｒセット数オーバー\n" VT_RST);
		return 0;
	}

	/* does this ever happen? how about Link? */
	if (ovl->vram.w0 == 0) {
		if (HREG(20) != 0) {
			// Translates to: "NOT AN OVERLAY"
			//osSyncPrintf("オーバーレイではありません\n");
		}

	} else {
		if (ovl->loaded != 0) {
			if (HREG(20) != 0) {
				// Translates to: "ALREADY LOADED"
				//osSyncPrintf("既にロードされています\n");
			}
		} else {
			if (ovl->alloctype & ALLOCTYPE_ABSOLUTE) {
				/*if (overlaySize > AM_FIELD_SIZE) {
					__assert("actor_segsize <= AM_FIELD_SIZE", name, 0);
				}*/

				if (actorctx->absoluteSpace == 0) {
					// Translates to: "AMF: ABSOLUTE MAGIC FIELD"
					actorctx->absoluteSpace = z_malloc_r(AM_FIELD_SIZE);
					if (HREG(20) != 0) {
						// Translates to: "ABSOLUTE MAGIC FIELD RESERVATION - %d BYTES RESERVED"
						//osSyncPrintf("絶対魔法領域確保 %d バイト確保\n", AM_FIELD_SIZE);
					}
				}

				ovl->loaded = actorctx->absoluteSpace;
			} else if (ovl->alloctype & ALLOCTYPE_PERMANENT) {
				ovl->loaded = z_malloc_r(overlaySize);
			} else {
				ovl->loaded = z_malloc(overlaySize);
			}

			if (ovl->loaded == 0) {
				// Translates to: "CANNOT RESERVE ACTOR PROGRAM MEMORY"
				//osSyncPrintf(VT_COL(RED, WHITE) "Ａｃｔｏｒプログラムメモリが確保できません\n" VT_RST);
				return 0;
			}
			
			/* load overlay */
			z_actor_load_ovl(
				ovl->vrom.w0, ovl->vrom.w1
				, ovl->vram.w0, ovl->vram.w1
				, ovl->loaded
			);

			/*osSyncPrintf(VT_FGCOL(GREEN));
			osSyncPrintf("OVL(a):Seg:%08x-%08x Ram:%08x-%08x Off:%08x %s\n"
				, ovl->vram.w0, ovl->vram.w1
				, ovl->loaded, (u32)ovl->loaded + (u32)ovl->vram.w1 - (u32)ovl->vram.w0
				, (u32)ovl->vram.w0 - (u32)ovl->loaded, name
			);
			osSyncPrintf(VT_RST);*/

			ovl->Nloaded = 0;
		}

		if (ivar)
			ivar = (void*)(((unsigned)ivar) - (int)(ovl->vram.w0 - ((unsigned)ovl->loaded)));
	}

	obj_slot = z_object_id_to_slot(&gl->objctx, ivar->obj_id);
	
	if (
		/* object dependency not met */
		(obj_slot < 0)
		
		/* or overlay is to be unloaded when the room is cleared */
		|| ((ivar->type == ACTORTYPE_ENEMY) && (z_flag_get_roomclear(gl, room_id)))
	)
	{
		// Translates to: "NO DATA BANK!! <DATA BANK＝%d> (profilep->bank=%d)"
		//osSyncPrintf(VT_COL(RED, WHITE) "データバンク無し！！<データバンク＝%d>(profilep->bank=%d)\n" VT_RST,
		//			 obj_slot, ivar->obj_id);
		z_actor_free_ovl(ovl);
		return 0;
	}

	actor = z_malloc(ivar->size);

	if (actor == 0) {
		// Translates to: "ACTOR CLASS CANNOT BE RESERVED! %s <SIZE＝%d BYTES>"
		//osSyncPrintf(VT_COL(RED, WHITE) "Ａｃｔｏｒクラス確保できません！ %s <サイズ＝%dバイト>\n", VT_RST, name,
		//			 ivar->size);
		z_actor_free_ovl(ovl);
		return 0;
	}

	/*if (ovl->Nloaded >= 255) {
		__assert("actor_dlftbl->clients < 255", name, 0);
	}*/

	ovl->Nloaded++;

	if (HREG(20) != 0) {
		// Translates to: "ACTOR CLIENT No. %d"
		//osSyncPrintf("アクタークライアントは %d 個目です\n", ovl->Nloaded);
	}
	
	z_bzero(actor, ivar->size);
//	Lib_MemSet(actor, ivar->size, 0);
	actor->ovltable = ovl; /* XXX pointing to stale data! */
	actor->id = ivar->id;
	actor->flags = ivar->flags;

	if (ivar->id == ACTOR_EN_PART) {
		actor->obj_slot = rotZ;
		rotZ = 0;
	} else {
		actor->obj_slot = obj_slot;
	}

	actor->init = ivar->init;
	actor->dest = ivar->dest;
	actor->main = ivar->main;
	actor->draw = ivar->draw;
	actor->room = room_id;
	actor->pos_init.x = posX;
	actor->pos_init.y = posY;
	actor->pos_init.z = posZ;
	actor->rot_init.x = rotX;
	actor->rot_init.y = rotY;
	actor->rot_init.z = rotZ;
	actor->params = params;

	z_actor_type_push(actorctx, actor, ivar->type);

	temp = g_segtable[6];
	z_actor_init(actor, gl);
	g_segtable[6] = temp;

	return actor;
}

