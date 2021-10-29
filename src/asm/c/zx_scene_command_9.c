#include "z64.h"

/* XXX please use https://github.com/z64me/z64scene instead */

/* extended functionality for oot scenes and rooms */

/* specification
	the 09 command in a scene/room header now works like this:
		09000000 xxxxxxxx
	where xxxxxxxx points to a new command list
	this is compatible with both scene and room files
	the data pointed to follows this format
	xxxxxxxx zzzzzzzz for each command
		x = command id
	 	z = various
	a command id of 0 signifies the end of the list;
	
	valid command ids:
		0 = end of list
		1 = render init callback; z is a segment pointer to a
		    custom render init function embedded in the scene
		2 = mini route table; z points to data of this form:
		       wwwwwwww oooooooo
		          w = num bytes that comprise mini route table
		          o = points to mini route table bytes
		5 = title card list; z points to an array of title cards;
		    no count is defined; you just put 00000000 at the end
		    of the array; see struct title for the form of each item
*/

enum cmdt
{
	CMD_LAST         = 0
	, CMD_RENDERINIT = 1
	, CMD_ROUTETABLE = 2
	, CMD_CUTSCENE   = 3
	, CMD_MINIMAP    = 4
	, CMD_TITLE      = 5
};

struct cmd
{
	enum cmdt   cmd;
	void       *data;
};

/*struct renderinit
{
	void *(exec)(struct z_glctx *gl);
};*/

struct routetable
{
	unsigned   numbytes;  /* number of bytes for route data */
	void      *segptr;    /* segment pointer to route data  */
};

struct cutscene
{
	/* TODO flag */
	void      *segptr;    /* segment pointer to route data  */
};

struct minimap_item
{
	void  *segtex;  /* segptr to texture */
	void  *segpal;  /* segptr to palette */
	
};

struct minimap
{
	unsigned              numitem;
	struct minimap_item   item[256];
};

struct title
{
	char        *segtext;    /* segptr to text to display */
	float        fontsize;   /* font size multiplier      */
	struct {
	   int       x;          /* screen coordinates describing */
	   int       y;          /* the rectangle to draw         */
	   int       w;          /* (if 0, fall back to MM-style) */
	   int       h;
	} rect;
	struct {
	   unsigned  ul;         /* color of each corner of rect */
	   unsigned  ll;         /* (rgba8888 aka rgba32 format) */
	   unsigned  ur;
	   unsigned  lr;
	   unsigned  text;       /* text color        */
	   unsigned  shadow;     /* text shadow color */
	} color;
	struct {
	   char      u;          /* number of times to repeat  */
	   char      v;          /* texture along a given axis */
	} repeat;
	struct {
	   char      u;          /* wrap mode along each axis      */
	   char      v;          /* [0,1,2](repeat, mirror, clamp) */
	} wrap;
	struct {
	   short     u;          /* just in case we ever need them */
	   short     v;
	} shift;
	struct {
	   short     u;          /* just in case we ever need them */
	   short     v;
	} mask;
	char         vbuf[64];   /* byte buffer used for vertex cache */
};

/* execute inline renderinit function */
static inline void renderinit(
	struct z_glctx *gl
	, void exec(struct z_glctx *gl)
)
{
	exec(gl);
}

/* set mini route table */
static inline void routetable(struct z_glctx *gl, struct routetable *cmd)
{
	void *virt = SEGMENTED_TO_VIRTUAL(cmd->segptr);
	z_bcopy(virt, g_routetable, cmd->numbytes);
}

/* cutscene, triggered by a flag */
static inline void cutscene(struct z_glctx *gl, struct cutscene *cmd)
{
	if (/*TODO cmd->flag conditions met*/0)
	{
		z_cutscene_script_init(gl, cmd->segptr);
		g_savectx.cutscene_exec = 1;
	}
}

/* custom minimap */
static inline void minimap(struct z_glctx *gl, struct minimap *cmd)
{
}

/* draw title card */
static inline void title_draw(struct z_glctx *gl, struct title *title)
{
	struct z_gfxctx *gfx = gl->common.gfxctx;
	struct z_dispbuf *ovl = &gfx->overlay;
	struct z_ifctx *ifctx = &gl->ifctx;
	unsigned param_static = (unsigned)ifctx->parameter;
	void *m;
	
	//kill_interface(en, gl);
	z_if_viewport_init(ifctx); /* Disappears without this function call. */
	
	gDPPipeSync(ovl->p++);
	
	gSPDisplayList(ovl->p++, 0x80126A60);
	
	gSPClearGeometryMode(ovl->p++, G_CULL_BOTH);
	
	gDPSetCombineLERP(
		ovl->p++
		, TEXEL0, 0, PRIMITIVE, 0
		, TEXEL0, 0, PRIMITIVE, 0
		, TEXEL0, 0, PRIMITIVE, 0
		, TEXEL0, 0, PRIMITIVE, 0
	);
	
	gDPSetPrimColor(ovl->p++, 0, 0, 0xFF, 0x37, 0xCD, 0xFF);
	
	z_matrix_translate_3f(0.0f, 0.0f, 1.0f, 0);
	
	z_matrix_scale_3f(1.0f, 1.0f, 1.0f, 1);
	
	m = z_matrix_alloc(gfx);
	
	gSPMatrix(ovl->p++, m, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
	
	gSPVertex(ovl->p++, title->vbuf, 4, 0);
	
	gDPLoadTextureBlock(
		ovl->p++
		, (param_static + 0xA00)
		, G_IM_FMT_IA
		, G_IM_SIZ_8b
		, 32
		, 32
		, 0
		, G_TX_NOMIRROR | G_TX_WRAP
		, G_TX_NOMIRROR | G_TX_WRAP
		, G_TX_NOMASK
		, G_TX_NOMASK
		, G_TX_NOLOD
		, G_TX_NOLOD
	);
	gSP1Quadrangle(ovl->p++, 0, 2, 3, 1, 0);
}

/* custom title card */
static inline void title(struct z_glctx *gl, struct title *title)
{
	/* go through each in list */
	while (title->segtext)
	{
		if (1)//if (zx_flag(&title->flag))
		{
			title_draw(gl, title);
			break;
		}
		++title;
	}
}

void zx_scene_command_9(struct z_glctx *gl, unsigned *ptr)
{
	unsigned *ok = (void*)0x80700000;
	ok[0] = 0xdead;
	ok[1] = 0xbeef;
	//ok[2] = (unsigned)cmd;
	//ok[3] = 0xdeadbeef;
	return;
	struct cmd *cmd = SEGMENTED_TO_VIRTUAL(ptr[1]);
	while (cmd->cmd != CMD_LAST)
	{
		void *data = SEGMENTED_TO_VIRTUAL(cmd->data);
		#if 0
		switch (cmd->cmd)
		{
			case CMD_LAST:
				break;
			
			case CMD_RENDERINIT:
				renderinit(gl, data);
				break;
			
			case CMD_ROUTETABLE:
				routetable(gl, data);
				break;
			
			case CMD_CUTSCENE:
				cutscene(gl, data);
				break;
			
			case CMD_MINIMAP:
				minimap(gl, data);
				break;
			
			case CMD_TITLE:
				title(gl, data);
				break;
		}
		#endif
		/* advance to next */
		++cmd;
	}
}

