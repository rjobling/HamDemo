////////////////////////////////////////////////////////////////////////////////
// customhelpers.h
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "core.h"

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
struct CopCommand
{
	u16 inst;
	u16 data;
};

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
#define CopMove(reg, value) {(u16) offsetof(Custom, reg), (u16) (value)}
#define CopMoveH(reg, value) {(u16) offsetof(Custom, reg), (u16) (((u32) (value)) >> 16)}
#define CopMoveL(reg, value) {(u16) offsetof(Custom, reg) + 2, (u16) (((u32) (value)) & 0xffff)}
inline constexpr CopCommand CopWait(int hp, int vp, int he = 0x7f, int ve = 0x7f, bool bfd = true) { return {(u16) ((vp << 8) | (hp << 1) | 0x1), (u16) ((bfd ? 0x8000 : 0) | (ve << 8) | (he << 1))}; }
inline constexpr CopCommand CopSkip(int hp, int vp, int he = 0x7f, int ve = 0x7f, bool bfd = true) { return {(u16) ((vp << 8) | (hp << 1) | 0x1), (u16) ((bfd ? 0x8000 : 0) | (ve << 8) | (he << 1) | 1)}; }
inline constexpr CopCommand CopEnd() { return {0xffff, 0xfffe}; }

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
inline constexpr u16 PackBplcon0(int bpls, bool dpf = false, bool ham = false, bool hires = false) { return ((hires ? 0x8000 : 0) | (bpls << 12) | (ham ? 0x800 : 0) | (dpf ? 0x400 : 0) | 0x200); }
inline constexpr u16 PackBplcon1(int x1, int x2, int x1aga = 0, int x2aga = 0) { return ((x2aga << 12) | (x1aga << 8) | (x2 << 4) | x1); }
inline constexpr u16 PackBplcon2(bool pf2pri, int sprpri) { return ((pf2pri ? 0x40 : 0) | sprpri); }
inline constexpr u16 PackDiwstrt(int sx, int sy) { return (((sy + 0x2c) << 8) | (sx + 0x81)); }
inline constexpr u16 PackDiwstop(int sx, int sy) { return ((((sy - 256) + 0x2c) << 8) | ((sx - 256) + 0x81)); }
inline constexpr u16 PackDdfstrt(int sx, bool hires = false) { return (((sx + 0x81 - (hires ? 9 : 17)) / 2) & 0xfc); }
inline constexpr u16 PackDdfstop(int sx, bool hires = false, u16 fmode = 0x0000) { return (PackDdfstrt(0, hires) + (hires ? (4 * (sx / 16 - 2 - (fmode & 3) * 2)) : (8 * (sx / 16 - 1 - (fmode & 3))))); }
