#include "z64.h"

// func_800981B8
/* load all object dependencies described by an object context */
/* this is used by ovl_kaleido_scope to reload them when unpausing */
void z_objectctx_load_slots(struct z_objectctx *objectCtx)
{
	int i;

	for (i = 0; i < objectCtx->num; i++)
	{
		struct z_objslot *slot = &objectCtx->slot[i];
#ifdef Z64EXT_OBJECTTABLE
		struct {
			unsigned size;
			unsigned pad;
			unsigned vrom;
		} *udata = z_dma_udata_temp(g_objtable[slot->id]);
		z_dma_req1(slot->segment, udata->vrom, udata->size, 0, 0);
#else
		unsigned size;
		struct z_objecttable *entry = &g_objtable[slot->id];
		size = entry->vrom.w1/*end*/ - entry->vrom.w0/*start*/;
		z_dma_req1(slot->segment, entry->vrom.w0/*start*/, size, 0, 0);
#endif
	}
}

