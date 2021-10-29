#define PHYSICAL_TO_VIRTUAL(ADDR) ((unsigned)(ADDR) + 0x80000000)
#define INCPTR(PTR, BYTES) (PTR = (void*)(((char*)PTR)+BYTES))

struct tuple32
{
	unsigned  h;
	unsigned  l;
};

struct z_scene_table
{
	struct tuple32  vrom;
	struct tuple32  card;
	unsigned char   unk_x10;
	unsigned char   config;
	unsigned char   unk_x12;
	unsigned char   unk_x13;
};

struct glctx
{
/*00000*/	char                   pad[0xA4];
/*000A4*/	short                  scene_number;
/*000A6*/   unsigned char          scene_config;
/*000A7*/   char                   pad1[0x9];
/*000B0*/   void                  *scene_segment;
/*000B4*/   char                   pad2[0x11C08];
/*11CBC*/   unsigned               room_ctx;
/*11CC0*/   char                   pad3[0x76C];
/*1242C*/	struct z_scene_table  *scene_loaded;
};

extern void *z_file_load_g(void *glctx, struct tuple32 *tuple);
asm("z_file_load_g = 0x800BFE5C;");

extern void z_scene_init(void *glctx, int spawn);
asm("z_scene_init = 0x800BFF0C;");

extern void func_80096FE8(void *glctx, void *roomctx);
asm("func_80096FE8 = 0x80096FE8;");

extern void *z_dma_head(int index);
asm("z_dma_head = 0x800014E0;");

void z_scene_load(struct glctx *gl, int scene, int spawn)
{
#if 0 /* original format */
//	z_dma_head(0xc5);
	struct z_scene_table *entry = (void*)(0x80129A10 + scene * 20);
	unsigned *segment = (void*)(0x80166FA8);
	entry->unk_x13 = 0;
	gl->scene_loaded = entry;
	gl->scene_number = scene;
	gl->scene_config = entry->config;
	
	gl->scene_segment = z_file_load_g(gl, &entry->vrom);
	entry->unk_x13 = 0;
	segment[2] = PHYSICAL_TO_VIRTUAL(gl->scene_segment);
	
	z_scene_init(gl, spawn);
	
	func_80096FE8(gl, &gl->room_ctx);
#else /* new format */
	short index = *(short*)(0x80129A10 + scene * 2);
	unsigned *segment = (void*)(0x80166FA8);
	struct z_scene_table *entry = z_dma_head(index);
	INCPTR(entry, 8);
	entry->unk_x13 = 0;
	gl->scene_loaded = entry;
	gl->scene_number = scene;
	gl->scene_config = entry->config;
	
	gl->scene_segment = z_file_load_g(gl, &entry->vrom);
	entry->unk_x13 = 0;
	segment[2] = PHYSICAL_TO_VIRTUAL(gl->scene_segment);
	
	z_scene_init(gl, spawn);
	
	func_80096FE8(gl, &gl->room_ctx);
#endif
}

