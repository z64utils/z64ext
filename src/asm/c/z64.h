#ifndef Z64_H_INCLUDED
#define Z64_H_INCLUDED

#define Z64EXT_OBJECTTABLE

#define	OBJECT_EXCHANGE_BANK_MAX 19

#define	OS_MESG_NOBLOCK   0
#define	OS_MESG_BLOCK     1

#define	ALIGN16(val) (((val) + 0xF) & ~0xF)

#define	PHYSICAL_TO_VIRTUAL(ADDR) ((unsigned)(ADDR) + 0x80000000)
#define	INCPTR(PTR, BYTES) (PTR = (void*)(((char*)PTR)+(BYTES)))
#define	SEGMENT_NUMBER(ADDR) (((unsigned)(ADDR)<<4)>>28)
#define	SEGMENT_OFFSET(ADDR) ((unsigned)(ADDR)&0xffffff)
#define	SEGMENTED_TO_VIRTUAL(ADDR) (void*)(PHYSICAL_TO_VIRTUAL(g_segtable[SEGMENT_NUMBER(ADDR)]) + SEGMENT_OFFSET(ADDR))
#define	ARRAY_COUNT(arr) (sizeof(arr) / sizeof(arr[0]))
#define	zh_get_player(gl) ((void*)gl->actorctx.actorList[ACTORTYPE_PLAYER].w1/*first*/)

#include "gbi.h"

struct z_actor;
struct z_glctx;

typedef void (*z_actorfunc)(struct z_actor *actor, struct z_glctx *gl);

typedef unsigned   OSEvent;
typedef void      *OSMesg;
typedef struct { char bytes[0x18]; } OSMesgQueue;
typedef struct { char bytes[0x58]; } OSScTask;

#define ACTOR_PSEUDO_TABLE 1

#define enum16(X) unsigned short

#define ACTOR_EN_PART 7 /* TODO what */

#include "regs.h"

typedef struct { char c[0x40]; } MtxF;

// Game Info aka. Static Context (dbg ram start: 80210A10)
// Data normally accessed through REG macros (see regs.h)
struct z_staticctx {
	/* 0x00 */ int    regPage;   // 1 is first page
	/* 0x04 */ int    regGroup;  // "register" group (R, RS, RO, RP etc.)
	/* 0x08 */ int    regCur;    // selected register within page
	/* 0x0C */ int    dpadLast;
	/* 0x10 */ int    repeat;
	/* 0x14 */ short  data[REG_GROUPS * REG_PER_GROUP]; // 0xAE0 bytes
}; // size = 0x15D4

struct tuple32
{
	unsigned  w0;
	unsigned  w1;
};

struct z_objecttable
{
	struct tuple32 vrom;
};

// aka actorinit
struct z_actor_ivar
{
	short         id;
	char          type;
	char          room;
	unsigned      flags;
	short         obj_id; // objectId
	unsigned      size;
	z_actorfunc   init;
	z_actorfunc   dest;
	z_actorfunc   main;
	z_actorfunc   draw;
};

enum alloctype
{
	ALLOCTYPE_NORMAL
	, ALLOCTYPE_ABSOLUTE
	, ALLOCTYPE_PERMANENT
};

// aka actoroverlay
/*struct z_actor_table
{
	struct tuple32        vrom;
	struct tuple32        vram;
	void                 *loaded; // loadedRamAddr
	struct z_actor_ivar  *ivar;   // initInfo
	char                 *name;
	enum16(alloctype)     alloctype;   // allocType
	char                  Nloaded; // nbLoaded
	char                  unused;
};*/
struct z_actor_table
{
	struct tuple32        vrom;
	struct tuple32        vram;
	struct z_actor_table *loaded; // loadedRamAddr
	struct z_actor_ivar  *ivar;   // initInfo
	char                 *name;
	enum16(alloctype)     alloctype;   // allocType
	char                  Nloaded; // nbLoaded
	char                  unused;
};

/* new actor table format */
struct zx_actor_table
{
	short   dmaid;
	short   alloctype;
	void   *loaded;
};

enum z_actortype {
	/* 0x00 */ ACTORTYPE_SWITCH,
	/* 0x01 */ ACTORTYPE_BG,
	/* 0x02 */ ACTORTYPE_PLAYER,
	/* 0x03 */ ACTORTYPE_EXPLOSIVES,
	/* 0x04 */ ACTORTYPE_NPC,
	/* 0x05 */ ACTORTYPE_ENEMY,
	/* 0x06 */ ACTORTYPE_PROP,
	/* 0x07 */ ACTORTYPE_ITEMACTION,
	/* 0x08 */ ACTORTYPE_MISC,
	/* 0x09 */ ACTORTYPE_BOSS,
	/* 0x0A */ ACTORTYPE_DOOR,
	/* 0x0B */ ACTORTYPE_CHEST
};

struct z_actor
{
/*000*/	short id;
/*002*/	unsigned char type;
/*003*/	char room;
/*004*/	unsigned flags;
/*008*/	struct {
			float x;
			float y;
			float z;
		} pos_init;
/*014*/	struct {
			short x;
			short y;
			short z;
		} rot_init;
/*01A*/	unsigned char pad[2];
/*01C*/	short params;
/*01E*/	char obj_slot; // objBankIndex
/*01F*/	char unk_1F;
/*020*/	char pad1[0xC4];
/*0E4*/	float unk_E4[3];
/*0F0*/	char pad2[0x38];
/*128*/	z_actorfunc init;
/*12C*/	z_actorfunc dest;
/*130*/	z_actorfunc main;
/*134*/	z_actorfunc draw;
/*138*/	struct z_actor_table *ovltable;
#ifdef OOT_DEBUG
/*13C*/	char debug_pad[0x10];
#endif
};

struct z_player
{
/*000*/	struct z_actor actor;
/*14c*/	char pad[0x518];
/*664*/	struct z_actor *unk_664;
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

struct z_scene_actor
{
	short id;
	char field[14];
};

struct z_scene_spawn
{
	unsigned char spawn;
	unsigned char room;
};

struct z_savescene
{
/*0000*/	unsigned   chest;
/*0004*/	unsigned   swch;
/*0008*/	unsigned   clear;
/*000C*/	unsigned   collect;
/*0010*/	unsigned   unk;
/*0014*/	unsigned   rooms_1;
/*0018*/	unsigned   rooms_2;
};

struct z_savectx
{
/*0000*/	int                 route;
/*0004*/	int                 link_form;    /* 0 = link, 1 = younglink */
/*0008*/	char                pad0[0xCC];
/*00D4*/	struct z_savescene  scene_flags[124]; /* XXX different in 1.0! */
/*0E8C*/	char                pad_E8C[0x588];
/*1414*/	unsigned char       cutscene_exec;
};

struct z_targetctx_item
{
	/* 0x00 */ float          pos[3];
	/* 0x0C */ float          unk_0C; // radius?
	/* 0x10 */ unsigned char  color[4];
}; // size = 0x14

struct z_targetctx
{
	/* 0x00 */ float                     naviRefPos[3];
	/* 0x0C */ float                     targetCenterPos[3];
	/* 0x18 */ float                     naviInner[4];
	/* 0x28 */ float                     naviOuter[4];
	/* 0x38 */ struct z_actor           *arrowPointedActor;
	/* 0x3C */ struct z_actor           *targetedActor;
	/* 0x40 */ float                     unk_40;
	/* 0x44 */ float                     unk_44;
	/* 0x48 */ short                     unk_48;
	/* 0x4A */ unsigned char             activeType;
	/* 0x4B */ unsigned char             unk_4B;
	/* 0x4C */ char                      unk_4C;
	/* 0x4D */ char                      unk_4D[0x03];
	/* 0x50 */ struct z_targetctx_item   arr_50[3];
	/* 0x8C */ struct z_actor           *unk_8C;
	/* 0x90 */ struct z_actor           *unk_90;
	/* 0x94 */ struct z_actor           *unk_94;
} TargetContext; // size = 0x98

struct z_actorctx {
	/* 0x0000 */ unsigned char       unk_00[0x08];
	/* 0x0008 */ unsigned char       total; // total number of actors loaded
	/* 0x0009 */ char                unk_09[0x03];
	/* 0x000C */ struct tuple32      actorList[12];
	/* 0x006C */ struct z_targetctx  targetCtx;
	struct {
		/* 0x0104 */ unsigned  swch;
		/* 0x0108 */ unsigned  tempSwch;
		/* 0x010C */ unsigned  unk0;
		/* 0x0110 */ unsigned  unk1;
		/* 0x0114 */ unsigned  chest;
		/* 0x0118 */ unsigned  clear;
		/* 0x011C */ unsigned  tempClear;
		/* 0x0120 */ unsigned  collect;
		/* 0x0124 */ unsigned  tempCollect;
	}                   flags;
	/* 0x0128 */ char   titleCtx[0x10];
	/* 0x0138 */ char   unk_138[0x04];
	/* 0x013C */ void  *absoluteSpace; // Space used to allocate actor overlays of alloc type 1
}; // size = 0x140

struct z_objslot
{
	/* 0x00 */ short      id;
	/* 0x04 */ void      *segment;
	/* 0x08 */ struct {unsigned vrom; char a[0x1C];}  dmaRequest;
	/* 0x28 */ struct {char a[0x18];}  loadQueue;
	/* 0x40 */ void       *loadMsg;
}; // size = 0x44

struct z_objectctx
{
	/* 0x0000 */ void  *spaceStart;
	/* 0x0004 */ void  *spaceEnd; // original name: "endSegment"
	/* 0x0008 */ unsigned char     num; // number of objects in bank
	/* 0x0009 */ unsigned char     unk_09;
	/* 0x000A */ unsigned char     mainKeepIndex; // "gameplay_keep" index in bank
	/* 0x000B */ unsigned char     subKeepIndex; // "gameplay_field_keep" or "gameplay_dangeon_keep" index in bank
	/* 0x000C */ struct z_objslot  slot[OBJECT_EXCHANGE_BANK_MAX];
}; // size = 0x518



// Graphics Context
struct z_dispbuf
{
	unsigned          size;                     /* 0x0000 */
	Gfx              *buf;                      /* 0x0004 */
	Gfx              *p;                        /* 0x0008 */
	Gfx              *d;                        /* 0x000C */
		                                        /* 0x0010 */
};

struct z_gfxctx
{
	Gfx              *poly_opa_w;               /* 0x0000 */
	Gfx              *poly_xlu_w;               /* 0x0004 */
	char              unk_00_[0x0008];          /* 0x0008 */
	Gfx              *overlay_w;                /* 0x0010 */
	char              unk_01_[0x0024];          /* 0x0014 */
	OSMesg            task_msg[8];              /* 0x0038 */
	char              unk_02_[0x0004];          /* 0x0058 */
	OSMesgQueue       task_mq;                  /* 0x005C */
	char              pad_00_[0x0004];          /* 0x0074 */
	OSScTask          task;                     /* 0x0078 */
	char              unk_03_[0x00E0];          /* 0x00D0 */
	Gfx              *work_w;                   /* 0x01B0 */
	struct z_dispbuf  work;                     /* 0x01B4 */
	char              unk_04_[0x00E4];          /* 0x01C4 */
	struct z_dispbuf  overlay;                  /* 0x02A8 */
	struct z_dispbuf  poly_opa;                 /* 0x02B8 */
	struct z_dispbuf  poly_xlu;                 /* 0x02C8 */
	unsigned          frame_count_1;            /* 0x02D8 */
	void             *frame_buffer;             /* 0x02DC */
	char              unk_05_[0x0008];          /* 0x02E0 */
	unsigned          frame_count_2;            /* 0x02E8 */
		                                        /* 0x02EC */
};

/* interface context */
struct z_ifctx
{
	char              unk_00_[0x0130];          /* 0x0000 */
	char             *parameter;                /* 0x0130 */
	char             *action_texture;           /* 0x0134 */
	char             *item_texture;             /* 0x0138 */
	char             *minimap_texture;          /* 0x013C */
	char              unk_01_[0x00AC];          /* 0x0140 */
	uint16_t          h1EC;                     /* 0x01EC */
	char              unk_02_[0x0002];          /* 0x01EE */
	uint16_t          a_action;                 /* 0x01F0 */
	char              unk_03_[0x0002];          /* 0x01F2 */
	float             f1F4;                     /* 0x01F4 */
	char              unk_04_[0x0004];          /* 0x01F8 */
	int16_t           b_label;                  /* 0x01FC */
	char              unk_05_[0x0064];          /* 0x01FE */
	struct
	{
		uint8_t         unk_00_;
		uint8_t         b_button;
		uint8_t         unk_01_;
		uint8_t         bottles;
		uint8_t         trade_items;
		uint8_t         hookshot;
		uint8_t         ocarina;
		uint8_t         warp_songs;
		uint8_t         suns_song;
		uint8_t         farores_wind;
		uint8_t         dfnl;
		uint8_t         all;
	}                 restriction_flags;        /* 0x0262 */
	char              unk_06_[0x0002];          /* 0x026E */
		                                     /* 0x0270 */
};

struct z_glctx
{
/*00000*/	struct {
         		struct z_gfxctx    *gfxctx;
         	} common;
/*00004*/	char                   pad[0xA0];
/*000A4*/	short                  scene_number;
/*000A6*/   unsigned char          scene_config;
/*000A7*/   char                   pad1[0x9];
/*000B0*/   void                  *scene_segment;
/*000B4*/   char                   pad2[0x1B70];
/*01C24*/   struct z_actorctx      actorctx;
/*01D64*/   char                   pad_1D64[0xE78C];
/*104F0*/   struct z_ifctx         ifctx;
/*10760*/   char                   pad10760[0x1044];
/*117A4*/	struct z_objectctx     objctx;
/*11CBC*/   struct{char a[0x74];}  roomctx;
/*11D30*/   char                   pad3[0x30];
/*11D60*/   MtxF                   mf_11D60;
/*11DA0*/   MtxF                   mf_11DA0;
/*11DE0*/   void                  *unk_11DE0;
/*11DE4*/   unsigned               gameplayFrames;
/*11DE8*/   unsigned char          link_form_on_spawn;
/*11DE9*/   unsigned char          pad7;
/*11DEA*/   unsigned char          scene_spawn;
/*11DEB*/   unsigned char          Nactors;
/*11DEC*/   unsigned char          Nrooms;
/*11DED*/   char                   pad8[3];
/*11DF0*/   void                  *rooms;
/*11DF4*/   struct z_scene_actor  *link_scene_actor;
/*11DF8*/	char                   pad5[0x8];
/*11E00*/	struct z_scene_spawn  *scene_spawn_points;
/*11E04*/	char                   pad6[0x628];
/*1242C*/	struct z_scene_table  *scene_loaded;
};

/* existing */
extern void *z_file_load_tuple(void *glctx, struct tuple32 *tuple);
extern void  z_scene_init(void *glctx, int spawn);
extern void  z_scene_room(void *glctx, void *roomctx);
extern int   z_dma_req1(void *dest, unsigned vrom, unsigned sz, const char *fn, int line);
extern int   z_dma_req2(void *req, void *dest, unsigned vrom, unsigned sz, unsigned unk5
		                  , void *queue, void *msg, const char *fn, int line);
extern void  z_bcopy(const void *src, void *dst, unsigned bytes);
extern void  z_bzero(void *dst, const int num);
extern int   z_object_spawn(struct z_objectctx *objctx, short objid);
extern int   z_object_id_to_slot(void *objctx, int objid);
extern void  z_actor_type_push(void *actorctx, struct z_actor *actor, int type);
extern void  z_actor_destroy(struct z_actor *actor, struct z_glctx *glctx);
extern void *z_actor_type_pop(void *glctx, void *actorctx, struct z_actor *actor);
extern void  z_actor_free_ovl(struct z_actor_table *ovl);
extern void  z_actor_init(struct z_actor *actor, struct z_glctx *gl);
extern void *z_malloc(unsigned size);
extern void *z_malloc_r(unsigned size);
int          z_actor_load_ovl(unsigned vrom, unsigned vromE, unsigned vram, unsigned vramE, void *dst);
extern int   z_flag_get_roomclear(struct z_glctx *gl, int flag);
extern void  z_actor_table_init(void);
extern void  z_actor_spawn_entry(struct z_actorctx *actorctx, struct z_scene_actor *actor, struct z_glctx *gl);
extern void  z_actor_titlecard_clear(struct z_glctx *gl, void *titlectx);
extern void  z_actor_targetctx_init(void *targetctx, unsigned focus, struct z_glctx *gl);
extern void  z_actor_magic_wind_test(struct z_glctx *gl);
extern void  z_free(void *ptr);
extern void  z_player_clear_something(struct z_player *player);

extern void  func_800F89E8(float unk[3]);
extern void  func_8005A444(void *camera, short b);
extern void *z_gameplay_get_camera(struct z_glctx *glctx, short cam_id);
extern short z_gameplay_get_active_camera_id(struct z_glctx *glctx);
extern void  z_matrixf_copy(MtxF *dst, MtxF *src);
extern void  z_cutscene_script_init(struct z_glctx *gl, void *segptr);
extern void  z_if_viewport_init(void *if_ctxt);
extern void  z_matrix_translate_3f(float x, float y, float z, int apply);
extern void  z_matrix_scale_3f(float x, float y, float z, int apply);
#ifdef OOTDEBUG
extern void *z_matrix_alloc(void *gfxctx, void *str, int line);
#	define z_matrix_alloc(x) z_matrix_alloc(x, 0, 0)
#else
extern void *z_matrix_alloc(void *gfxctx);
#endif

extern void osCreateMesgQueue(void *mq, void *msg, int count);
extern int  osRecvMesg(void *mq, void *msg, int flag);

/* new */
extern void *z_dma_udata(int index, void *dst);
extern void *z_dma_udata_temp(int index);
extern void *zx_dma_udata_tempP(short *index);

/* global variables */
extern struct z_savectx g_savectx;
//extern struct z_actor_table g_actortable[471];
extern short     g_playerobjdep[4];
extern unsigned  g_segtable[16];
extern struct z_staticctx *g_staticctx;
extern MtxF gMtxFClear;

#ifdef Z64EXT_OBJECTTABLE
extern short g_objtable[402*4];
#else
extern struct z_objecttable g_objtable[402];
#endif

extern unsigned g_routetable[1556];

#include "z_dma.h"

#endif /* Z64_H_INCLUDED */

