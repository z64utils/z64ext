/* FIXME lots of hard-coded debug rom addresses */
#define CODE_VRAM      0x8001ce60
#define CODE_START     0xA94000
#define CODE_SIZE      0x13AF30
#define CODE_BSS       0x80157d90
#define CODE_BSS_SIZE  0x1DC30
//#define CODE1_REQ      0x80012364 /* ask me about hacky code1 spec */

typedef unsigned long long OSTime;
	
extern unsigned osGetMemSize(void);
asm("osGetMemSize = 0x80005100;");

#define MB_8  0x00800000
#define HAS_EXPANSIONPAK  (osGetMemSize() >= MB_8)

extern OSTime osGetTime(void);
asm("osGetTime = 0x800069e0;");

extern int z_dma_req1(void *dest, unsigned vrom, unsigned sz, const char *fn, int line);
asm("z_dma_req1 = 0x80001aa0;");

extern void DmaMgr_Start();
asm("DmaMgr_Start = 0x8000183c;");

extern void z_bzero(void *dst, const int num);
asm("z_bzero = 0x80004450;");

extern void Main(void*);
asm("Main = 0x800c6e20;");

extern void z_file_load(void *req);
asm("z_file_load = 0x800013FC;");

/* function 800005a0, goes at 0x140 in boot */
void Main_ThreadEntry(void* arg0)
{
	OSTime var1;

//	osSyncPrintf("mainx 実行開始\n");
	DmaMgr_Start();
//	osSyncPrintf("codeセグメントロード中...");
	var1 = osGetTime();
	z_dma_req1(
		(void*)CODE_VRAM
		, CODE_START
		, CODE_SIZE
		, 0
		, 0
	);
	var1 -= osGetTime();
#if CODE1_REQ
	if (HAS_EXPANSIONPAK)
		z_file_load((void*)CODE1_REQ);
#endif
//	osSyncPrintf("\rcodeセグメントロード中...完了\n");
//	osSyncPrintf("転送時間 %6.3f\n");
	z_bzero((void*)CODE_BSS, CODE_BSS_SIZE);
//	osSyncPrintf("codeセグメントBSSクリア完了\n");
	Main(arg0);
//	osSyncPrintf("mainx 実行終了\n");
}
