/* <z64.me> extern.h - contains prototypes for in-game functions */

#ifndef Z64ENC_EXTERN_H_INCLUDED

#define Z64ENC_EXTERN_H_INCLUDED

#define z_bcopy Bcopy

extern void Bcopy(void *src, void *dst, unsigned int n);

extern void DMARomToRam(unsigned rom_src, void *ram_dst, unsigned sz);

extern void osSetThreadPri(void *, int);

extern void z_decompress(unsigned pstart, void *dst, unsigned sz);

extern void *z_malloc(
	unsigned sz
#ifdef OOTDEBUG
	, const char *string
	, int line
#	define z_malloc(SZ) z_malloc(SZ, "test", __LINE__);
//#	define z_malloc(SZ) z_malloc(SZ, 0, 0);
#else
#	define z_malloc(SZ) z_malloc(SZ);
#endif
);

#endif /* Z64ENC_EXTERN_H_INCLUDED */
