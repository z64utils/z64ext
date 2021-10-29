/* <z64.me> file loading with extended functionality */

#include "extern.h"

#define INCPTR(PTR, BYTES) (PTR = (void*)(((char*)PTR)+BYTES))
#define PMASK (~(COMPRESSED | OVERLAP | HEADER))
#define RESERVE 32 /* first 32 bytes of dmadata reserved */

enum {
	  COMPRESSED = 1 << 31
	, HEADER     = 1 <<  1
	, OVERLAP    = 1 <<  0
};

struct z64_file_request
{
	unsigned    vrom;  /* virtual rom address of block to transfer  */
	void       *dram;  /* destination buffer (0 will alloc for you) */
	unsigned    sz;    /* size of block to transfer (0 if unknown)  */
};

/* dma entry */
struct z64_dma
{
	unsigned  Vstart;  /* virtual start */
	unsigned  Pstart;  /* physical start */
	unsigned  Vend;    /* virtual end */
};

/* function prototypes */
static inline
struct z64_dma *z_dma_find(unsigned vrom);

/* dmadata table */
extern struct z64_dma g_dmadata[4096];

/* function 0x800013FC in OoT debug */
void z_file_load(struct z64_file_request *req)
{
	struct z64_dma *dma = z_dma_find(req->vrom);
	
	/* this prevents a teq from being generated (pj64) */
	if (!dma)
		return;
	
	/* compressed file */
	if (dma->Pstart & COMPRESSED)
	{
		unsigned Pstart = dma->Pstart & PMASK;
		unsigned Psz;
		if (dma->Pstart & HEADER)
			Pstart += 16;
		
		/* compressed size is first word in file */
		DMARomToRam(Pstart, &Psz, 4);
		
		/* decompress file */
		osSetThreadPri(0, 0x0A);
		z_decompress(Pstart, req->dram, Psz);
		osSetThreadPri(0, 0x10);
	}
	
	/* uncompressed file: direct transfer */
	else
		DMARomToRam(
			(dma->Pstart & PMASK) + (req->vrom - dma->Vstart)
			, req->dram
			, req->sz
		);
}

#if 0
/* given a start vrom address, return end vrom address
 * (custom function for adapting tables)
 */
unsigned z_file_end(unsigned vrom)
{
	return z_dma_find(vrom)->Vend;
	
// commented out to save space in generated binary;
// this error checking most likely wouldn't prevent
// a crash anyway
//	struct z64_dma *dma;
//	/* match found */
//	if ((dma = z_dma_find(vrom)))
//		return dma->Vend;
//	
//	/* no match found */
//	return vrom;
}
#endif

#if 0
/* a version that uses a2 for vrom */
unsigned z_file_end_a2(int a0, int a1, unsigned vrom)
{
	return z_dma_find(vrom)->Vend;
}
#endif

static inline
struct z64_dma *z_dma_find(unsigned vrom)
{
	int sz = 0;                        /* size of entry  */
	struct z64_dma *dma;
	
	/* walk dmadata table (skipping first entry) */
	dma = g_dmadata;
	INCPTR(dma, RESERVE);
	for (; dma->Vend; INCPTR(dma, sz))
	{
		sz = 12;
		
		/* overlaps next entry */
		if (dma->Pstart & OVERLAP)
			sz = 8;
		
		/* match found */
		if (vrom >= dma->Vstart && vrom < dma->Vend)
			return dma;
	}
	
	/* no match found */
	return 0;
}

static
inline
struct z64_dma *z_dma_index(int index)
{
	int sz = 0;                        /* size of entry  */
	struct z64_dma *dma;
	
	/* walk dmadata table (skipping first entries) */
	dma = g_dmadata;
	INCPTR(dma, RESERVE);
	for (; index; INCPTR(dma, sz))
	{
		sz = 12;
		
		/* overlaps next entry */
		if (dma->Pstart & OVERLAP)
			sz = 8;
		
		--index;
	}
	
	/* no match found */
	return dma;
}

#if 0
/* get userdata of a dmadata file, by index */
void *z_dma_udata(int index)
{
	struct z64_dma *dma;
	unsigned *head;// = z_malloc(32);
	//head = z_malloc(32);//z_game_alloc((void*)0x80212020, 32, "x", __LINE__);
	head = g_dmadata;
	
	/* find entry */
	dma = z_dma_index(index);
	
	/* compose header */
	head[2] = dma->Vstart;
	
	/* account for 16-byte header */
	if (dma->Pstart & HEADER)
		head[2] += 16;
	
	head[3] = dma->Vend;
	head[0] = dma->Vend - head[2];
	DMARomToRam(dma->Pstart & PMASK, head + 4, 4 * sizeof(*head));
	
	return head;
}
#else

/* get userdata of a dmadata file, by index */
void *z_dma_udata(int index, void *dst)
{
	struct z64_dma *dma;
	unsigned *head = dst;
	
	/* find entry */
	dma = z_dma_index(index);
	
	/* compose header */
	head[2] = dma->Vstart;
	
	/* account for 16-byte header */
	if (dma->Pstart & HEADER)
		head[2] += 16;
	
	head[3] = dma->Vend;
	head[0] = dma->Vend - head[2];
	DMARomToRam(dma->Pstart & PMASK, head + 4, 4 * sizeof(*head));
	
	return head;
}

/* get userdata of a dmadata file, by index */
/* WARNING: do not store the pointer this function *
 *          returns anywhere, as it will expire;   *
 *          use the data immediately or copy it    */
void *z_dma_udata_temp(int index)
{
	return z_dma_udata(index, g_dmadata);
}
#endif

#if 0
void *z_dma_head(short index)
{
	struct z64_dma *dma;
	unsigned *head = g_dmadata;
	
	/* find entry */
	dma = z_dma_index(index);
	
	/* compose header */
	head += 4;
	head[0] = dma->Vstart;
	head[1] = dma->Vend;
	head[2] = dma->Vend - dma->Vstart;
	
	return head;
}
#endif

