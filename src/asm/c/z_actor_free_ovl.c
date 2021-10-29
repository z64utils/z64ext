#include "z64.h"

void z_actor_free_ovl(struct z_actor_table *ovlentry)
{
	/* only free an overlay if no clients are using it */
	if (ovlentry->Nloaded == 0 && ovlentry->loaded)
	{
		/* dealloc the overlay for this type */
		if (ovlentry->alloctype == ALLOCTYPE_NORMAL)
			z_free(ovlentry->loaded);
		
		/* zero the pointer to the alloc'd data if not permanent */
		if (ovlentry->alloctype != ALLOCTYPE_PERMANENT)
			ovlentry->loaded = 0;
	}
}

