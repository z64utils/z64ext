#include "z64.h"

// Object_UpdateBank
void z_object_update_slot(struct z_objectctx *objectCtx)
{
	int i;
	struct z_objslot *slot;

	slot = &objectCtx->slot[0];
	for (i = 0; i < objectCtx->num; i++)
	{
		if (slot->id < 0)
		{
			if (slot->dmaRequest.vrom == 0)
			{
				osCreateMesgQueue(&slot->loadQueue, &slot->loadMsg, 1);
#ifdef Z64EXT_OBJECTTABLE
				struct {
					unsigned size;
					unsigned pad;
					unsigned vrom;
				} *udata = z_dma_udata_temp(g_objtable[-slot->id]);
				z_dma_req2(&slot->dmaRequest, slot->segment, udata->vrom, udata->size, 0,
									&slot->loadQueue, 0, 0, 0);
#else
				unsigned size;
				struct z_objecttable *obj;
				obj = &g_objtable[-slot->id];
				size = obj->vrom.w1/*end*/ - obj->vrom.w0/*start*/;
				z_dma_req2(&slot->dmaRequest, slot->segment, obj->vrom.w0/*start*/, size, 0,
									&slot->loadQueue, 0, 0, 0);
#endif
			}
			else if (!osRecvMesg(&slot->loadQueue, 0, OS_MESG_NOBLOCK))
			{
				slot->id = -slot->id;
			}
		}
		slot++;
	}
}


