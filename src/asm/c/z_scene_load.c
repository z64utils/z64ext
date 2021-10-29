#include "z64.h"

void z_scene_load(struct z_glctx *gl, int scene, int spawn)
{
#if 0 /* original format */
//	z_dma_head(0xc5);
	struct z_scene_table *entry = (void*)(0x80129A10 + scene * 20);
	unsigned *segment = (void*)(0x80166FA8);
	entry->unk_x13 = 0;
	gl->scene_loaded = entry;
	gl->scene_number = scene;
	gl->scene_config = entry->config;
	
	gl->scene_segment = z_file_load_tuple(gl, &entry->vrom);
	entry->unk_x13 = 0;
	segment[2] = PHYSICAL_TO_VIRTUAL(gl->scene_segment);
	
	z_scene_init(gl, spawn);
	
	func_80096FE8(gl, &gl->room_ctx);
#else /* new format */
	extern short g_scenetable[1024];
	extern struct z_scene_table g_sceneentry;
	struct z_scene_table *entry = z_dma_udata(g_scenetable[scene], &g_sceneentry);
	INCPTR(entry, 8);
	gl->scene_loaded = entry;
	gl->scene_number = scene;
	gl->scene_config = entry->config;
	
	gl->scene_segment = z_file_load_tuple(gl, &entry->vrom);
	g_segtable[2] = PHYSICAL_TO_VIRTUAL(gl->scene_segment);
	
	z_scene_init(gl, spawn);
	
	z_scene_room(gl, &gl->roomctx);
#endif
}

