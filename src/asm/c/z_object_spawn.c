#include "z64.h"

// ObjectSpawn
int z_object_spawn(struct z_objectctx *objectCtx, short objectId)
{
	struct z_objslot *slot = &objectCtx->slot[objectCtx->num];
	
	slot->id = objectId;
	
#ifdef Z64EXT_OBJECTTABLE
	struct {
		unsigned size;
		unsigned pad;
		unsigned vrom;
	} *udata = z_dma_udata_temp(g_objtable[objectId]);
	
	z_dma_req1(
		slot->segment
		, udata->vrom
		, udata->size
		, 0
		, 0
	);
	#define size udata->size
#else
	unsigned size;
	size = g_objtable[objectId].vrom.w1/*end*/ - g_objtable[objectId].vrom.w0/*start*/;
	
	z_dma_req1(
		slot->segment
		, g_objtable[objectId].vrom.w0/*start*/
		, size
		, 0
		, 0
	);
#endif
	
	/* update start of next pointer in list */
	if (objectCtx->num < OBJECT_EXCHANGE_BANK_MAX - 1)
	{
		/* XXX careful, does not account for separate slot bank */
		struct z_objslot *Nslot = &objectCtx->slot[objectCtx->num + 1];
		Nslot->segment = (void*)ALIGN16((int)slot->segment + size);
	}
	
	objectCtx->num++;
	objectCtx->unk_09 = objectCtx->num;
	
	return objectCtx->num - 1;
}
