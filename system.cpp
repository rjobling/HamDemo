////////////////////////////////////////////////////////////////////////////////
// system.cpp
////////////////////////////////////////////////////////////////////////////////

#include "system.h"
#include <exec/execbase.h>
#include <graphics/gfxbase.h>
#include <hardware/cia.h>
#include <hardware/custom.h>
#include <hardware/dmabits.h>
#include <proto/dos.h>
#include <proto/exec.h>
#include <proto/graphics.h>
#include <proto/intuition.h>
#include <proto/mathffp.h>
#include "core.h"

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
struct ExecBase* SysBase;
struct IntuitionBase* IntuitionBase;
struct GfxBase* GfxBase;
struct DosLibrary* DOSBase;
struct Library* MathBase;

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
volatile Custom& custom = *((Custom*) 0xdff000);
volatile CIA& ciaa = *((CIA*) 0xbfe001);
volatile CIA& ciab = *((CIA*) 0xbfd000);

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
static const char* sError;

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
static volatile void* sVBR;

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
static bool sSavedWorkbench;
static View* sSavedActiView;
static u16 sSavedADKCON;
static u16 sSavedDMACON;
static u16 sSavedINTENA;
static System_IrqFunc* sSavedIrqHandler;

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
bool System_Init()
{
	sError = nullptr;

	SysBase = *((struct ExecBase**) 4);

	IntuitionBase = (struct IntuitionBase*) OpenLibrary((CONST_STRPTR) "intuition.library", 0);
	if (IntuitionBase == nullptr)
	{
		return false;
	}

	GfxBase = (struct GfxBase*) OpenLibrary((CONST_STRPTR) "graphics.library", 0);
	if (GfxBase == nullptr)
	{
		CloseLibrary((Library*) IntuitionBase);

		return false;
	}

	DOSBase = (struct DosLibrary*) OpenLibrary((CONST_STRPTR) "dos.library", 0);
	if (DOSBase == nullptr)
	{
		CloseLibrary((Library*) GfxBase);
		CloseLibrary((Library*) IntuitionBase);

		return false;
	}

	MathBase = OpenLibrary("mathffp.library", 0);
	if (MathBase == nullptr)
	{
		CloseLibrary((Library*) DOSBase);
		CloseLibrary((Library*) GfxBase);
		CloseLibrary((Library*) IntuitionBase);

		return false;
	}

	sSavedWorkbench = CloseWorkBench();

	sSavedActiView = GfxBase->ActiView;

	OwnBlitter();
	WaitBlit();
	Disable();

	// Save current interrupt and DMA settings.
	sSavedADKCON = custom.adkconr;
	sSavedDMACON = custom.dmaconr;
	sSavedINTENA = custom.intenar;

	// Disable all interrupts.
	custom.intena = 0x7fff;

	// Clear all pending interrupts.
	custom.intreq = 0x7fff;

	System_WaitVbl();
	System_WaitVbl();

	// Clear all DMA channels.
	custom.dmacon = 0x7fff;

	// Set all colors to black.
	for (int i = 0; i < countof(custom.color); i++)
	{
		custom.color[i] = 0;
	}

	LoadView(nullptr);

	WaitTOF();
	WaitTOF();

	System_WaitVbl();
	System_WaitVbl();

	// Make sure toggle is set to long frame.
	custom.vposw = 0x8000;

	sVBR = 0;
	if (SysBase->AttnFlags & AFF_68010)
	{
		u16 getvbr[] = {0x4e7a, 0x0801, 0x4e73}; // movec.l vbr,d0 rte
		sVBR = (volatile void*) Supervisor((ULONG (*)()) getvbr);
	}

	// Save current interrupt handler.
	sSavedIrqHandler = System_GetIrqHandler();

	System_WaitVbl();

	return true;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
void System_Deinit()
{
	System_WaitVbl();
	System_WaitBlt();

	// Disable all interrupts.
	custom.intena = 0x7fff;

	// Clear all pending interrupts.
	custom.intreq = 0x7fff;

	// Clear all DMA channels.
	custom.dmacon = 0x7fff;

	// Restore interrupts.
	System_SetIrqHandler(sSavedIrqHandler);

	// Restore copper lists.
	custom.cop1lc = (u32) GfxBase->copinit;
	custom.cop2lc = (u32) GfxBase->LOFlist;
	custom.copjmp1 = 0x7fff;

	// Restore interrupts and DMA settings.
	custom.adkcon = sSavedADKCON | 0x8000;
	custom.dmacon = sSavedDMACON | 0x8000;
	custom.intena = sSavedINTENA | 0x8000;

	LoadView(sSavedActiView);

	WaitTOF();
	WaitTOF();
	WaitBlit();
	DisownBlitter();
	Enable();

	if (sSavedWorkbench)
	{
		OpenWorkBench();
	}

	if (sError != nullptr)
	{
		Write(Output(), (APTR) sError, strlen(sError) + 1);
	}

	CloseLibrary((Library*) MathBase);
	CloseLibrary((Library*) DOSBase);
	CloseLibrary((Library*) GfxBase);
	CloseLibrary((Library*) IntuitionBase);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
void System_SetError(const char* error)
{
	sError = error;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
bool System_IsAGA()
{
	return (custom.vposr & 0x0100);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
System_IrqFunc* System_GetIrqHandler()
{
	return *((System_IrqFunc**) (((u8*) sVBR) + 0x6c));
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
void System_SetIrqHandler(System_IrqFunc* func)
{
	*((System_IrqFunc**) (((u8*) sVBR) + 0x6c)) = func;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
void System_WaitVbl()
{
	const u32 vend = 311 << 8;
	const u32 mask = 0x1ff00;

	volatile u32* vpos = (u32*) &custom.vposr;

	while ((*vpos & mask) == vend) {}
	while ((*vpos & mask) != vend) {}
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
void System_WaitBlt()
{
	volatile u16* dmaconr = (u16*) &custom.dmaconr;

	u16 test = *dmaconr; // For compatiblity A1000.
	unused(test);

	while (*dmaconr & DMAF_BLTDONE) {}
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
bool System_TestLMB()
{
	return !(ciaa.ciapra & CIAF_GAMEPORT0);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
bool System_TestRMB()
{
	return !(custom.potinp & (1 << 10));
}
