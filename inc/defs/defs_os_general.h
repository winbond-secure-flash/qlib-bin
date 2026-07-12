/*---------------------------------------------------------------------------------------------------------*/
/*  Winbond Electronics Corporation                                                                        */
/*                                                                                                         */
/*  Copyright (c) 2019 by Winbond Electronics Corporation                                                  */
/*  All rights reserved                                                                                    */
/*                                                                                                         */
/*<<<------------------------------------------------------------------------------------------------------*/
/* File Contents:                                                                                          */
/*   defs_os_general.h                                                                                     */
/*            This file contains default OS Applications definitions                                       */
/*  Project:                                                                                               */
/*            SWC DEFS                                                                                     */
/*---------------------------------------------------------------------------------------------------------*/
#ifndef DEFS_OS_GENERAL_H__
#define DEFS_OS_GENERAL_H__

#include "defs_types.h"

#if defined(__KERNEL__) && !defined(CFG_OPTEE_REVISION_MAJOR)
#include <linux/string.h>
#else
#include <string.h>
#endif
/*---------------------------------------------------------------------------------------------------------*/
/* Core dependent MEM definitions                                                                          */
/* If use Linux and its didnt work try fix it to ioread8(a), ioread16(a), ioread32(a)...                   */
/* If use Windows Driver Foundation API - need to fix it READ_REGISTER_UCHAR(a), READ_REGISTER_USHORT(a),  */
/* READ_REGISTER_ULONG(a), WRITE_REGISTER_UCHAR(a), WRITE_REGISTER_USHORT(a), WRITE_REGISTER_ULONG(a)      */
/*---------------------------------------------------------------------------------------------------------*/
#define MEMMAP(offset, len)

#ifndef MEMR8
#define MEMR8(a) (*UINT_TO_U8_PTR(a))
#endif
#ifndef MEMR16
#define MEMR16(a) (*UINT_TO_U16_PTR(a))
#endif
#ifndef MEMR32
#define MEMR32(a) (*UINT_TO_U32_PTR(a))
#endif
#ifndef MEMW8
#define MEMW8(a, v) ((*(UINT_TO_U8_PTR(a))) = ((uint8_t)(v)))
#endif
#ifndef MEMW16
#define MEMW16(a, v) ((*(UINT_TO_U16_PTR(a))) = ((uint16_t)(v)))
#endif
#ifndef MEMW32
#define MEMW32(a, v) ((*(UINT_TO_U32_PTR(a))) = ((uint32_t)(v)))
#endif
/*---------------------------------------------------------------------------------------------------------*/
/* Core dependent IO definitions                                                                           */
/* If use X86 and its didnt work try fix it to inp(a)), inpw(a), inpd(a)...                                */
/* If use Linux and its didnt work try fix it to inb(a), inw(a), inl(a)...                                 */
/* If use Windows Driver Foundation API - need to fix READ_PORT_UCHAR((PUCHAR)(a)), READ_PORT_USHORT(a),   */
/* READ_PORT_ULONG(a), WRITE_PORT_UCHAR((PUCHAR)(a), (v)), WRITE_PORT_USHORT((a), (v)),                    */
/* WRITE_PORT_ULONG((a), (v))                                                                              */
/*---------------------------------------------------------------------------------------------------------*/
#ifndef IOR8
#define IOR8(a) MEMR8(a)
#endif
#ifndef IOR16
#define IOR16(a) MEMR16(a)
#endif
#ifndef IOR32
#define IOR32(a) MEMR32(a)
#endif

#ifndef IOW8
#define IOW8(a, v) MEMW8(a, v)
#endif
#ifndef IOW16
#define IOW16(a, v) MEMW16(a, v)
#endif
#ifndef IOW32
#define IOW32(a, v) MEMW32(a, v)
#endif

/*---------------------------------------------------------------------------------------------------------*/
/* Variables alignment                                                                                     */
/*---------------------------------------------------------------------------------------------------------*/
#ifndef ALIGN_
// no alignment
#define ALIGN_(x, decl) decl
#endif

/*---------------------------------------------------------------------------------------------------------*/
/* Allocation                                                                                              */
/*---------------------------------------------------------------------------------------------------------*/
#define ALLOCA(size) alloca(size)

#ifndef MEMCPY
#define MEMCPY(dst, src, size) memcpy(dst, src, size)
#endif
#ifndef MEMSET
#define MEMSET(buf, val, size) memset(buf, val, size)
#endif
#ifndef MEMCMP
#define MEMCMP(buf1, buf2, size) memcmp(buf1, buf2, size)
#endif

/*---------------------------------------------------------------------------------------------------------*/
/* Variables packing                                                                                       */
/*---------------------------------------------------------------------------------------------------------*/
#ifndef PACK_
#define PACK_(decl) decl
#endif

#ifndef SECTION_ZI_
#define SECTION_ZI_(name)
#endif

#ifdef GLOBALS_IN_SEPARATE_SECTIONS
#define GLOBALS_SECTION(name) SECTION_ZI_(name)
#else
#define GLOBALS_SECTION(name)
#endif

#ifndef IRQ
#define IRQ
#endif

#ifndef ENABLE_INTERRUPTS
#define ENABLE_INTERRUPTS()
#endif

#ifndef DISABLE_INTERRUPTS
#define DISABLE_INTERRUPTS()
#endif

#ifndef INTERRUPTS_VAR_DECLARE
#define INTERRUPTS_VAR_DECLARE(var)
#endif

#ifndef INTERRUPTS_SAVE_DISABLE
#define INTERRUPTS_SAVE_DISABLE(var) \
    {                                \
    }
#endif

#ifndef INTERRUPTS_RESTORE
#define INTERRUPTS_RESTORE(var) \
    {                           \
    }
#endif

/*---------------------------------------------------------------------------------------------------------*/
/* Rotate Left 32bit                                                                                       */
/*---------------------------------------------------------------------------------------------------------*/
#ifndef ROL32_
#define ROL32_(x, y) \
    ((((uint32_t)(x) << (uint32_t)((y)&31u)) | (((uint32_t)(x)&0xFFFFFFFFUL) >> (uint32_t)(32u - ((y)&31u)))) & 0xFFFFFFFFUL)
#endif

/*---------------------------------------------------------------------------------------------------------*/
/* Rotate Right 32bit                                                                                      */
/*---------------------------------------------------------------------------------------------------------*/
#ifndef ROR32_
#define ROR32_(x, y) \
    (((((uint32_t)(x)&0xFFFFFFFFUL) >> (uint32_t)((y)&31u)) | ((uint32_t)(x) << (uint32_t)(32u - ((y)&31u)))) & 0xFFFFFFFFUL)
#endif

/*---------------------------------------------------------------------------------------------------------*/
/* Population count 32bit                                                                                  */
/*---------------------------------------------------------------------------------------------------------*/
#ifndef POP32_
#define POP1_(x)  (x)
#define POP2_(x)  (POP1_((x)&0x1) + POP1_((x) >> 1))
#define POP4_(x)  (POP2_((x)&0x3) + POP2_((x) >> 2))
#define POP8_(x)  ((x == 0u) ? 0u : (POP4_(LS4(x)) + POP4_(MS4(x))))
#define POP16_(x) ((x == 0u) ? 0u : (POP8_(LS8(x)) + POP8_(MS8(x))))
#define POP32_(x) ((x == 0u) ? 0u : (POP16_(LS16(x)) + POP16_(MS16(x))))
#endif

/*---------------------------------------------------------------------------------------------------------*/
/* Count Leading Zeros 32bit                                                                               */
/*---------------------------------------------------------------------------------------------------------*/
#ifndef CLZ32_
#define CLZ1_(x)  ((x) == 0u ? 1u : 0u)
#define CLZ2_(x)  ((((x)&0x2u) == 0u) ? (1u + (CLZ1_(x))) : (CLZ1_((x) >> 1u)))
#define CLZ4_(x)  ((((x)&0xCu) == 0u) ? (2u + (CLZ2_(x))) : (CLZ2_((x) >> 2u)))
#define CLZ8_(x)  ((((x)&0xF0u) == 0u) ? (4u + (CLZ4_(x))) : (CLZ4_((x) >> 4u)))
#define CLZ16_(x) ((((x)&0xFF00u) == 0u) ? (8u + (CLZ8_(x))) : (CLZ8_((x) >> 8u)))
#define CLZ32_(x) ((x) == 0u ? 32u : ((((x)&0xFFFF0000u) == 0u) ? (16u + (CLZ16_(x))) : (CLZ16_((x) >> 16u))))
#endif

/*---------------------------------------------------------------------------------------------------------*/
/* Count Trailing Zeros 32bit                                                                              */
/*---------------------------------------------------------------------------------------------------------*/
#ifndef CTZ32_
#define CTZ32_(x) (32u - CLZ32_((~(x)) & ((x)-1u)))
#endif

/*---------------------------------------------------------------------------------------------------------*/
/* CPU power management                                                                                    */
/*---------------------------------------------------------------------------------------------------------*/
#ifndef CPU_IDLE
#define CPU_IDLE()
#endif

/*---------------------------------------------------------------------------------------------------------*/
/* Compiler-specific macros for section placement                                                          */
/*---------------------------------------------------------------------------------------------------------*/
// CRITICAL: Exclude Clang from MSVC branch because Clang on Windows defines _MSC_VER for compatibility
#if defined(_MSC_VER) && !defined(__clang__)
// True MSVC compiler (not Clang pretending to be MSVC)
// Both macros are empty for MSVC: all code is in RAM on Windows, section placement is meaningless.
#define COMPILER_NOINLINE
#define COMPILER_SECTION(section_name)
#else
// Default: GCC-style attributes (works for GCC, Clang, armcc, gcc-arm, riscv, keil, etc.)
#define COMPILER_NOINLINE              __attribute__((noinline))
#define COMPILER_SECTION(section_name) __attribute__((section(section_name)))
#endif //defined(_MSC_VER) && !defined(__clang__)

#endif // DEFS_OS_GENERAL_H__
