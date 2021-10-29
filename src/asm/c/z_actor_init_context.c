#if 1

#include "z64.h"

#ifdef ACTOR_PSEUDO_TABLE
	extern struct zx_actor_table g_actortable[471*4];
	#define TABLETYPE zx_actor_table
#else
	extern struct z_actor_table g_actortable[471];
	#define TABLETYPE z_actor_table
#endif

/* oot debug 800304DC */
void z_actor_init_context(
	struct z_glctx *globalCtx
	, struct z_actorctx *actorCtx
	, struct z_scene_actor *actorEntry
)
{
	struct TABLETYPE *overlayEntry;
	struct z_savescene *saveSceneFlags;
	int i;

	saveSceneFlags = &g_savectx.scene_flags[globalCtx->scene_number];

	z_bzero(actorCtx, sizeof(*actorCtx));

	z_actor_table_init();
	z_matrixf_copy(&globalCtx->mf_11DA0, &gMtxFClear);
	z_matrixf_copy(&globalCtx->mf_11D60, &gMtxFClear);

	overlayEntry = &g_actortable[0];
	for (i = 0; i < ARRAY_COUNT(g_actortable); i++) {
		overlayEntry->loaded = 0;
	#ifndef ACTOR_PSEUDO_TABLE
		overlayEntry->Nloaded = 0;
	#endif
		overlayEntry++;
	}

	actorCtx->flags.chest = saveSceneFlags->chest;
	actorCtx->flags.swch = saveSceneFlags->swch;
	actorCtx->flags.clear = saveSceneFlags->clear;
	actorCtx->flags.collect = saveSceneFlags->collect;

	z_actor_titlecard_clear(globalCtx, &actorCtx->titleCtx);

	actorCtx->absoluteSpace = 0;

	z_actor_spawn_entry(actorCtx, actorEntry, globalCtx);
	z_actor_targetctx_init(&actorCtx->targetCtx
		, actorCtx->actorList[ACTORTYPE_PLAYER].w1/*first*/
		, globalCtx
	);
	z_actor_magic_wind_test(globalCtx);
}
#endif
