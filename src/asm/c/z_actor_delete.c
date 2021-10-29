#include "z64.h"

struct z_actor *z_actor_delete(
	struct z_actorctx *actorCtx
	, struct z_actor *actor
	, struct z_glctx *globalCtx
)
{
	struct z_player *player;
	struct z_actor  *newFirstActor;
	struct z_actor_table *overlayEntry;

	player = zh_get_player(globalCtx);

	overlayEntry = actor->ovltable;

	/*if (HREG(20) != 0) {
		// Translates to: "ACTOR CLASS DELETED [%s]"
		osSyncPrintf("アクタークラス削除 [%s]\n", name);
	}*/

	if (player && actor == player->unk_664) {
		z_player_clear_something(player); /* clears unk_664 */
		func_8005A444(
			z_gameplay_get_camera(
				globalCtx
				, z_gameplay_get_active_camera_id(globalCtx)
			)
			, 0
		);
	}

	if (actor == actorCtx->targetCtx.arrowPointedActor) {
		actorCtx->targetCtx.arrowPointedActor = 0;
	}

	if (actor == actorCtx->targetCtx.unk_8C) {
		actorCtx->targetCtx.unk_8C = 0;
	}

	if (actor == actorCtx->targetCtx.unk_90) {
		actorCtx->targetCtx.unk_90 = 0;
	}

	func_800F89E8(actor->unk_E4);
	z_actor_destroy(actor, globalCtx);

	newFirstActor = z_actor_type_pop(globalCtx, actorCtx, actor);

	z_free(actor);

	if (overlayEntry->vram.w0/*start*/ == 0) {
		/*if (HREG(20) != 0) {
			// Translates to: "NOT AN OVERLAY"
			osSyncPrintf("オーバーレイではありません\n");
		}*/
	} else {
		/*if (overlayEntry->loadedRamAddr == NULL) {
			__assert("actor_dlftbl->allocp != NULL", "../z_actor.c", 7251);
		}

		if (overlayEntry->nbLoaded <= 0) {
			__assert("actor_dlftbl->clients > 0", "../z_actor.c", 7252);
		}*/

		overlayEntry->Nloaded--;
		z_actor_free_ovl(overlayEntry);
	}

	return newFirstActor;
}
