////////////////////////////////////////////////////////////////////////////////
// core.h
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <proto/exec.h>
#include "support/gcc8_c_support.h"

////////////////////////////////////////////////////////////////////////////////
// Basic types.
////////////////////////////////////////////////////////////////////////////////
typedef char s8;
typedef short s16;
typedef int s32;
typedef unsigned int uint;
typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;

////////////////////////////////////////////////////////////////////////////////
// Type sizes.
////////////////////////////////////////////////////////////////////////////////
template<typename T> inline T private_minof();
template<typename T> inline T private_maxof();
template<> inline char private_minof<char>() { return -128; }
template<> inline char private_maxof<char>() { return 127; }
template<> inline short private_minof<short>() { return -32768; }
template<> inline short private_maxof<short>() { return 32767; }
template<> inline int private_minof<int>() { return -2147483648; }
template<> inline int private_maxof<int>() { return 2147483647; }
template<> inline long private_minof<long>() { return -2147483648; }
template<> inline long private_maxof<long>() { return 2147483647; }
template<> inline unsigned char private_minof<unsigned char>() { return 0; }
template<> inline unsigned char private_maxof<unsigned char>() { return 255; }
template<> inline unsigned short private_minof<unsigned short>() { return 0; }
template<> inline unsigned short private_maxof<unsigned short>() { return 65535; }
template<> inline unsigned int private_minof<unsigned int>() { return 0; }
template<> inline unsigned int private_maxof<unsigned int>() { return 4294967295; }
template<> inline unsigned long private_minof<unsigned long>() { return 0; }
template<> inline unsigned long private_maxof<unsigned long>() { return 4294967295; }
#define minof(a) private_minof<a>()
#define maxof(a) private_maxof<a>()

////////////////////////////////////////////////////////////////////////////////
// Asserts.
////////////////////////////////////////////////////////////////////////////////
#if defined(DEBUG)
inline void assert(bool b) { if (!b) asm volatile("illegal"); }
inline void assert_pointer(const void* const p) { assert(p != nullptr); }
#else
#define assert(b)
#define assert_pointer(p)
#endif
#define static_assert(b) private_static_assert<(b) ? true : false>::static_assert_failed();
template<bool b> struct private_static_assert {};
template<> struct private_static_assert<true> { static void static_assert_failed() {}; };

////////////////////////////////////////////////////////////////////////////////
// Compiler hints.
////////////////////////////////////////////////////////////////////////////////
#define unused(a) ((void) a)
#define restrict __restrict__

////////////////////////////////////////////////////////////////////////////////
// Alignment.
////////////////////////////////////////////////////////////////////////////////
#define alignof(t) __alignof__(t)
#define aligned(a, b) (((a) & ((b) - 1)) == 0)
#define alignup(a, b) (((a) + (b) - 1) & ~((b) - 1))
#define aligndown(a, b) ((a) & ~((b) - 1))
inline constexpr int alignnext(int a) { a--; a |= a >> 1; a |= a >> 2; a |= a >> 4; a |= a >> 8; a |= a >> 16; a++; return a; }

////////////////////////////////////////////////////////////////////////////////
// Arrays.
////////////////////////////////////////////////////////////////////////////////
#define countof(a) ((int) (sizeof(a) / sizeof((a)[0])))

////////////////////////////////////////////////////////////////////////////////
// Math.
////////////////////////////////////////////////////////////////////////////////
#define sqr(a) ((a) * (a))
#define is_log2(a) (((a) != 0) && (((a) & ((a) - 1)) == 0))
template<typename T> inline constexpr T abs(const T& a) { return ((a < 0) ? -a : a); }
template<typename T> inline constexpr T sign(const T& a) { return ((a < 0) ? ((T) -1) : ((T) 1)); }
template<typename T> inline constexpr T min(const T& a, const T& b) { return ((a < b) ? a : b); }
template<typename T> inline constexpr T max(const T& a, const T& b) { return ((a > b) ? a : b); }
template<typename T> inline constexpr T clamp(const T& a, const T& amin, const T& amax) { T t = ((a > amin) ? a : amin); t = ((t < amax) ? t : amax); return t; }
template<typename T> inline void swap(T& a, T& b) { T t = a; a = b; b = t; }
inline u16 rand(int& value) { value = value * 1103515245 + 12345; return ((value >> 16) & 0x7fff); }

////////////////////////////////////////////////////////////////////////////////
// Unrolling.
////////////////////////////////////////////////////////////////////////////////
#define unroll(n, a) private_unroll##n(a)
#define private_unroll1(a)  a
#define private_unroll2(a)  private_unroll1(a)  a
#define private_unroll3(a)  private_unroll2(a)  a
#define private_unroll4(a)  private_unroll3(a)  a
#define private_unroll5(a)  private_unroll4(a)  a
#define private_unroll6(a)  private_unroll5(a)  a
#define private_unroll7(a)  private_unroll6(a)  a
#define private_unroll8(a)  private_unroll7(a)  a
#define private_unroll9(a)  private_unroll8(a)  a
#define private_unroll10(a) private_unroll9(a)  a
#define private_unroll11(a) private_unroll10(a) a
#define private_unroll12(a) private_unroll11(a) a
#define private_unroll13(a) private_unroll12(a) a
#define private_unroll14(a) private_unroll13(a) a
#define private_unroll15(a) private_unroll14(a) a
#define private_unroll16(a) private_unroll15(a) a
#define private_unroll17(a) private_unroll16(a) a
#define private_unroll18(a) private_unroll17(a) a
#define private_unroll19(a) private_unroll18(a) a
#define private_unroll20(a) private_unroll19(a) a
#define private_unroll21(a) private_unroll20(a) a
#define private_unroll22(a) private_unroll21(a) a
#define private_unroll23(a) private_unroll22(a) a
#define private_unroll24(a) private_unroll23(a) a
#define private_unroll25(a) private_unroll24(a) a
#define private_unroll26(a) private_unroll25(a) a
#define private_unroll27(a) private_unroll26(a) a
#define private_unroll28(a) private_unroll27(a) a
#define private_unroll29(a) private_unroll28(a) a
#define private_unroll30(a) private_unroll29(a) a
#define private_unroll31(a) private_unroll30(a) a
#define private_unroll32(a) private_unroll31(a) a
#define private_unroll33(a) private_unroll32(a) a
#define private_unroll34(a) private_unroll33(a) a
#define private_unroll35(a) private_unroll34(a) a
#define private_unroll36(a) private_unroll35(a) a
#define private_unroll37(a) private_unroll36(a) a
#define private_unroll38(a) private_unroll37(a) a
#define private_unroll39(a) private_unroll38(a) a
#define private_unroll40(a) private_unroll39(a) a
#define private_unroll41(a) private_unroll40(a) a
#define private_unroll42(a) private_unroll41(a) a
#define private_unroll43(a) private_unroll42(a) a
#define private_unroll44(a) private_unroll43(a) a
#define private_unroll45(a) private_unroll44(a) a
#define private_unroll46(a) private_unroll45(a) a
#define private_unroll47(a) private_unroll46(a) a
#define private_unroll48(a) private_unroll47(a) a
#define private_unroll49(a) private_unroll48(a) a
#define private_unroll50(a) private_unroll49(a) a
#define private_unroll51(a) private_unroll50(a) a
#define private_unroll52(a) private_unroll51(a) a
#define private_unroll53(a) private_unroll52(a) a
#define private_unroll54(a) private_unroll53(a) a
#define private_unroll55(a) private_unroll54(a) a
#define private_unroll56(a) private_unroll55(a) a
#define private_unroll57(a) private_unroll56(a) a
#define private_unroll58(a) private_unroll57(a) a
#define private_unroll59(a) private_unroll58(a) a
#define private_unroll60(a) private_unroll59(a) a
#define private_unroll61(a) private_unroll60(a) a
#define private_unroll62(a) private_unroll61(a) a
#define private_unroll63(a) private_unroll62(a) a
#define private_unroll64(a) private_unroll63(a) a
