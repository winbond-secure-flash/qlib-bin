/*---------------------------------------------------------------------------------------------------------*/
/*  Winbond Electronics Corporation                                                                        */
/*                                                                                                         */
/*  Copyright (c) 2019 by Winbond Electronics Corporation                                                  */
/*  All rights reserved                                                                                    */
/*                                                                                                         */
/*<<<------------------------------------------------------------------------------------------------------*/
/* File Contents:                                                                                          */
/*   defs_os_windows.h                                                                                     */
/*            This file contains definitions for Windows OS Applications                                   */
/*  Project:                                                                                               */
/*            SWC DEFS                                                                                     */
/*---------------------------------------------------------------------------------------------------------*/

#ifndef DEFS_WINDOWS_H_
#define DEFS_WINDOWS_H_

#include <windows.h>
#include <intrin.h>
#include "malloc.h"

/*---------------------------------------------------------------------------------------------------------*/
/* OS dependent PTR definitions                                                                            */
/*---------------------------------------------------------------------------------------------------------*/
#define PTR8  (volatile uint8_t*)
#define PTR16 (volatile uint16_t*)
#define PTR32 (volatile uint32_t*)

/*---------------------------------------------------------------------------------------------------------*/
/* Variables alignment                                                                                     */
/*---------------------------------------------------------------------------------------------------------*/
#ifndef ALIGN_
#define ALIGN_(x, decl) __declspec(align(x)) decl
#endif

/*---------------------------------------------------------------------------------------------------------*/
/* Intrinsics                                                                                              */
/*---------------------------------------------------------------------------------------------------------*/
#ifdef __has_builtin
#if __has_builtin(__builtin_ctz)
#define CTZ32_(x) __builtin_ctz(x)
#endif
#endif
#ifndef CTZ32_
static __inline uint32_t __builtin_ctz(uint32_t value)
{
    DWORD trailing_zero = 0;

    if (TRUE == _BitScanForward(&trailing_zero, value))
    {
        return trailing_zero;
    }
    else
    {
        return 32u;
    }
}
#define CTZ32_(x) __builtin_ctz(x)
#endif // _CTZ32_

#ifdef __has_builtin
#if __has_builtin(__builtin_clz)
#define CLZ32_(x) __builtin_clz(x)
#endif
#endif
#ifndef CLZ32_
static __inline uint32_t __builtin_clz(uint32_t value)
{
    DWORD leading_zero = 0;

    if (TRUE == _BitScanReverse(&leading_zero, value))
    {
        return 31u - leading_zero;
    }
    else
    {
        return 32u;
    }
}
#define CLZ32_(x) __builtin_clz(x)
#endif // _CLZ32_

#define ROL32_(x, y) _lrotl(x, y)
//#define _ROR32_(x, y) _lrotr(x, y)
#define ROL64_(x, y) _rotl64(x, y)
#define ROR64_(x, y) _rotr64(x, y)
#ifndef POP32_
#define POP32_(x) __popcnt(x)
#endif // _POP32_
#define INLINE_ __inline

/*---------------------------------------------------------------------------------------------------------*/
/* Variable packing                                                                                        */
/*---------------------------------------------------------------------------------------------------------*/
#ifndef PACK_
#define PACK_(decl) __pragma(pack(push, 1)) decl __pragma(pack(pop))
#endif

/*---------------------------------------------------------------------------------------------------------*/
/* Assertion macros                                                                                        */
/*---------------------------------------------------------------------------------------------------------*/
#define CORE_ASSERT() DebugBreak()

#endif // DEFS_WINDOWS_H_
