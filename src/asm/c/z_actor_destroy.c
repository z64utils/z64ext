#include "z64.h"

void z_actor_destroy(struct z_actor *actor, struct z_glctx *glctx)
{
	if (actor->dest)
	{
		actor->dest(actor, glctx);
		actor->dest = 0;
	}
}
