#include "z64.h"

/* oot debug 80098508 */
void z_scene_spawn_player(struct z_glctx *gl, unsigned cmd[2])
{
	extern struct z_actor_ivar g_playerivar;
	
	short playerobj;
	int form = g_savectx.link_form;

	gl->link_scene_actor = SEGMENTED_TO_VIRTUAL(cmd[1]);
	gl->link_scene_actor += gl->scene_spawn_points[gl->scene_spawn].spawn;
	gl->link_form_on_spawn = form;

	playerobj = g_playerobjdep[form];

//	g_actortable[gl->link_scene_actor->id].ivar->objdep = playerobj;
	g_playerivar.obj_id = playerobj;
	z_object_spawn(&gl->objctx, playerobj);
}
