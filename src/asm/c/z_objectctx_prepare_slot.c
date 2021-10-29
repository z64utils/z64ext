#include "z64.h"

// func_800982FC
void *z_objectctx_prepare_slot(struct z_objectctx *objectCtx, int slotID, short objectId)
{
	struct z_objslot *slot = &objectCtx->slot[slotID];
	unsigned size;
	void *nextPtr;
	
	slot->id = -objectId;
	slot->dmaRequest.vrom = 0;
	
#ifdef Z64EXT_OBJECTTABLE
	size = *(unsigned*)z_dma_udata_temp(g_objtable[objectId]);
#else
	struct z_objecttable *obj = &g_objtable[objectId];
	size = obj->vrom.w1/*end*/ - obj->vrom.w0/*start*/;
#endif
	
	nextPtr = (void*)ALIGN16((int)slot->segment + size);
	
	return nextPtr;
}

