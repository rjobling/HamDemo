////////////////////////////////////////////////////////////////////////////////
// ham.cpp
////////////////////////////////////////////////////////////////////////////////

#include "ham.h"
#include <hardware/custom.h>
#include <hardware/dmabits.h>
#include "core.h"
#include "customhelpers.h"
#include "system.h"

#define HAM6
//#define HAM5
//#define EHB

#if !defined(HAM6) && !defined(HAM5) && !defined(EHB)
#error "Define one of the modes!"
#endif

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
static const int kScreenWidth	   = 320;
static const int kScreenHeight	   = 256;
#if defined(HAM5)
static const int kScreenPlanes	   = 5;
#else
static const int kScreenPlanes	   = 6;
#endif
static const int kScreenPlaneSize  = kScreenWidth / 8 * kScreenHeight;
static const int kScreenBufferSize = kScreenPlanes * kScreenPlaneSize;

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
static const u16 kPalette[] = {
	0x000, 0x111, 0x222, 0x333, 0x444, 0x555, 0x666, 0x777,	0x888, 0x999, 0xaaa, 0xbbb, 0xccc, 0xddd, 0xeee, 0xfff,
	0xf00, 0xf50, 0xfa0, 0xff0, 0x8f0, 0x0f0, 0x0ff, 0x0cf, 0x08f, 0x04f, 0x00f, 0x40f, 0x80f, 0xc0f, 0xf0f, 0xf08,
};

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
struct CopList
{
	CopCommand bpl0pth;
	CopCommand bpl0ptl;
	CopCommand bpl1pth;
	CopCommand bpl1ptl;
	CopCommand bpl2pth;
	CopCommand bpl2ptl;
	CopCommand bpl3pth;
	CopCommand bpl3ptl;
	CopCommand bpl4pth;
	CopCommand bpl4ptl;
	#if !defined(HAM5)
	CopCommand bpl5pth;
	CopCommand bpl5ptl;
	#endif

	#if !defined(EHB)
	CopCommand color[16];
	#else
	CopCommand color[32];
	#endif

	CopCommand end;
};

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
static u16 sScreenBpl[kScreenBufferSize / sizeof(u16)] __attribute__((section (".MEMF_CHIP"))) = {};

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
static CopList sCopList __attribute__((section (".MEMF_CHIP")));

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
bool Ham_Init()
{
	warpmode(true);

	for (int j = 0; j < kScreenHeight; j++)
	{
		for (int i = 0; i < (kScreenWidth / 16); i++)
		{
			int col = i + (j >> 2);

			sScreenBpl[kScreenPlaneSize / 2 * 0 + j * kScreenWidth / 16 + i] = (col &  1) ? 0xffff : 0x0000;
			sScreenBpl[kScreenPlaneSize / 2 * 1 + j * kScreenWidth / 16 + i] = (col &  2) ? 0xffff : 0x0000;
			sScreenBpl[kScreenPlaneSize / 2 * 2 + j * kScreenWidth / 16 + i] = (col &  4) ? 0xffff : 0x0000;
			sScreenBpl[kScreenPlaneSize / 2 * 3 + j * kScreenWidth / 16 + i] = (col &  8) ? 0xffff : 0x0000;
			sScreenBpl[kScreenPlaneSize / 2 * 4 + j * kScreenWidth / 16 + i] = (col & 16) ? 0xffff : 0x0000;
			#if !defined(HAM5)
			sScreenBpl[kScreenPlaneSize / 2 * 5 + j * kScreenWidth / 16 + i] = (col & 32) ? 0xffff : 0x0000;
			#endif
		}
	}

	sCopList.bpl0pth = CopMoveH(bplpt[0], sScreenBpl + kScreenPlaneSize / 2 * 0);
	sCopList.bpl0ptl = CopMoveL(bplpt[0], sScreenBpl + kScreenPlaneSize / 2 * 0);
	sCopList.bpl1pth = CopMoveH(bplpt[1], sScreenBpl + kScreenPlaneSize / 2 * 1);
	sCopList.bpl1ptl = CopMoveL(bplpt[1], sScreenBpl + kScreenPlaneSize / 2 * 1);
	sCopList.bpl2pth = CopMoveH(bplpt[2], sScreenBpl + kScreenPlaneSize / 2 * 2);
	sCopList.bpl2ptl = CopMoveL(bplpt[2], sScreenBpl + kScreenPlaneSize / 2 * 2);
	sCopList.bpl3pth = CopMoveH(bplpt[3], sScreenBpl + kScreenPlaneSize / 2 * 3);
	sCopList.bpl3ptl = CopMoveL(bplpt[3], sScreenBpl + kScreenPlaneSize / 2 * 3);
	sCopList.bpl4pth = CopMoveH(bplpt[4], sScreenBpl + kScreenPlaneSize / 2 * 4);
	sCopList.bpl4ptl = CopMoveL(bplpt[4], sScreenBpl + kScreenPlaneSize / 2 * 4);
	#if !defined(HAM5)
	sCopList.bpl5pth = CopMoveH(bplpt[5], sScreenBpl + kScreenPlaneSize / 2 * 5);
	sCopList.bpl5ptl = CopMoveL(bplpt[5], sScreenBpl + kScreenPlaneSize / 2 * 5);
	#endif

	sCopList.color[ 0] = CopMove(color[ 0], kPalette[ 0]);
	sCopList.color[ 1] = CopMove(color[ 1], kPalette[ 1]);
	sCopList.color[ 2] = CopMove(color[ 2], kPalette[ 2]);
	sCopList.color[ 3] = CopMove(color[ 3], kPalette[ 3]);
	sCopList.color[ 4] = CopMove(color[ 4], kPalette[ 4]);
	sCopList.color[ 5] = CopMove(color[ 5], kPalette[ 5]);
	sCopList.color[ 6] = CopMove(color[ 6], kPalette[ 6]);
	sCopList.color[ 7] = CopMove(color[ 7], kPalette[ 7]);
	sCopList.color[ 8] = CopMove(color[ 8], kPalette[ 8]);
	sCopList.color[ 9] = CopMove(color[ 9], kPalette[ 9]);
	sCopList.color[10] = CopMove(color[10], kPalette[10]);
	sCopList.color[11] = CopMove(color[11], kPalette[11]);
	sCopList.color[12] = CopMove(color[12], kPalette[12]);
	sCopList.color[13] = CopMove(color[13], kPalette[13]);
	sCopList.color[14] = CopMove(color[14], kPalette[14]);
	sCopList.color[15] = CopMove(color[15], kPalette[15]);
	#if defined(EHB)
	sCopList.color[16] = CopMove(color[16], kPalette[16]);
	sCopList.color[17] = CopMove(color[17], kPalette[17]);
	sCopList.color[18] = CopMove(color[18], kPalette[18]);
	sCopList.color[19] = CopMove(color[19], kPalette[19]);
	sCopList.color[20] = CopMove(color[20], kPalette[20]);
	sCopList.color[21] = CopMove(color[21], kPalette[21]);
	sCopList.color[22] = CopMove(color[22], kPalette[22]);
	sCopList.color[23] = CopMove(color[23], kPalette[23]);
	sCopList.color[24] = CopMove(color[24], kPalette[24]);
	sCopList.color[25] = CopMove(color[25], kPalette[25]);
	sCopList.color[26] = CopMove(color[26], kPalette[26]);
	sCopList.color[27] = CopMove(color[27], kPalette[27]);
	sCopList.color[28] = CopMove(color[28], kPalette[28]);
	sCopList.color[29] = CopMove(color[29], kPalette[29]);
	sCopList.color[30] = CopMove(color[30], kPalette[30]);
	sCopList.color[31] = CopMove(color[31], kPalette[31]);
	#endif

	sCopList.end = CopEnd();

	debug_register_bitmap(sScreenBpl , "Bpl", kScreenWidth, kScreenHeight, kScreenPlanes, 0);
	debug_register_palette(kPalette, "Palette", countof(kPalette), 0);

	warpmode(false);

	#if defined(HAM5)
	custom.bplcon0 = PackBplcon0(5, false, true);
	#elif defined(HAM6)
	custom.bplcon0 = PackBplcon0(6, false, true);
	#elif defined(EHB)
	custom.bplcon0 = PackBplcon0(kScreenPlanes, false, false);
	#endif
	custom.bplcon1 = PackBplcon1(0, 0);
	custom.bplcon2 = PackBplcon2(false, 0);
	custom.bpl1mod = 0;
	custom.bpl2mod = 0;
	custom.diwstrt = PackDiwstrt(0, 0);
	custom.diwstop = PackDiwstop(kScreenWidth, kScreenHeight);
	custom.ddfstrt = PackDdfstrt(0);
	custom.ddfstop = PackDdfstop(kScreenWidth);
	custom.fmode   = 0x0000;
	custom.copcon  = 2;
	custom.cop1lc  = (u32) &sCopList;

	System_WaitVbl();

	custom.dmacon = DMAF_SETCLR | DMAF_COPPER | DMAF_RASTER | DMAF_MASTER;

	return true;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
void Ham_Deinit()
{
	debug_unregister(kPalette);
	debug_unregister(sScreenBpl);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
void Ham_Update()
{
	System_WaitVbl();
}
