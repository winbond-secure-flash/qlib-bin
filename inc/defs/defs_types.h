/*---------------------------------------------------------------------------------------------------------*/
/*  Winbond Electronics Corporation                                                                        */
/*                                                                                                         */
/*  Copyright (c) 2019 by Winbond Electronics Corporation                                                  */
/*  All rights reserved                                                                                    */
/*                                                                                                         */
/*<<<------------------------------------------------------------------------------------------------------*/
/* File Contents:                                                                                          */
/*    defs_types.h                                                                                         */
/*            This file contains generic types, including compiler and core dependent                      */
/* Project:                                                                                                */
/*            SWC DEFS                                                                                     */
/*---------------------------------------------------------------------------------------------------------*/

#ifndef DEFS_TYPES_H__
#define DEFS_TYPES_H__

#if defined(__KERNEL__) && !defined(CFG_OPTEE_REVISION_MAJOR)
#include <linux/types.h>
#else
#include <stdint.h>
#include <stdbool.h>
#endif

/*---------------------------------------------------------------------------------------------------------*/
/*                                                CONSTANTS                                                */
/*---------------------------------------------------------------------------------------------------------*/
#ifndef FALSE_VAL
#define FALSE_VAL 0U
#endif

#ifndef TRUE_VAL
#define TRUE_VAL 1U
#endif

#ifndef BAD_VAL
#define BAD_VAL 0xAAu
#endif

#ifndef NULL
#define NULL (0U)
#endif

#ifndef ENABLE
#define ENABLE ((BOOLEAN)TRUE)
#endif

#ifndef DISABLE
#define DISABLE ((BOOLEAN)FALSE)
#endif

#ifndef ON
#define ON ((BOOLEAN)TRUE)
#endif

#ifndef OFF
#define OFF ((BOOLEAN)FALSE)
#endif

#if defined(WDFAPI) || defined(_WIN32)
/*---------------------------------------------------------------------------------------------------------*/
/* BOOLEAN  defined for Windows OS, redefine TRUE,FALSE                                                    */
/*---------------------------------------------------------------------------------------------------------*/
#ifndef FALSE
#define FALSE ((BOOLEAN)FALSE_VAL)
#endif

#ifndef TRUE
#define TRUE ((BOOLEAN)TRUE_VAL)
#endif

#ifndef BAD
#define BAD ((BOOLEAN)BAD_VAL)
#endif

#else
/*---------------------------------------------------------------------------------------------------------*/
/* BOOLEAN definition                                                                                      */
/*---------------------------------------------------------------------------------------------------------*/

#if !defined(FALSE) && !defined(TRUE) && !defined(BAD)
typedef enum
{
    FALSE = FALSE_VAL,
    TRUE  = TRUE_VAL,
    BAD   = BAD_VAL,
} BOOLEAN;
#endif
#endif //defined(WDFAPI) || defined(_WIN32)

/*---------------------------------------------------------------------------------------------------------*/
/* Pointer to integer conversions                                                                          */
/*---------------------------------------------------------------------------------------------------------*/
#if defined(__LP64__) || defined(__x86_64__) || defined(_M_X64) || defined(__aarch64__) || defined(_WIN64)
// pointer type is 64 bits
#define PTR_TO_UINT(ptr)       ((uint64_t)((uint8_t*)(ptr)))
#define UINT_TO_U8_PTR(val)    ((uint8_t*)((uint64_t)((val)&0xffffffffffffffffULL)))
#define UINT_TO_U16_PTR(val)   ((uint16_t*)((uint64_t)((val)&0xffffffffffffffffULL)))
#define UINT_TO_U32_PTR(val)   ((uint32_t*)((uint64_t)((val)&0xffffffffffffffffULL)))
#define UINT_TO_PTR(val, type) ((type)((uint64_t)((val)&0xffffffffffffffffULL)))
#else
#define PTR_TO_UINT(ptr)       ((uint32_t)((uint8_t*)(ptr)))
#define UINT_TO_U8_PTR(val)    ((uint8_t*)((uint32_t)((val)&0xffffffffu)))
#define UINT_TO_U16_PTR(val)   ((uint16_t*)((uint32_t)((val)&0xffffffffu)))
#define UINT_TO_U32_PTR(val)   ((uint32_t*)((uint32_t)((val)&0xffffffffu)))
#define UINT_TO_PTR(val, type) ((type)((uint32_t)((val)&0xffffffffu)))
#endif

/*---------------------------------------------------------------------------------------------------------*/
/*                          OS and Core dependent definitions                                              */
/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
/* Windows OS Applications                                                                                 */
/*---------------------------------------------------------------------------------------------------------*/
#if defined(_WIN32) || defined(_WIN64)
#include "defs_os_windows.h"

/*---------------------------------------------------------------------------------------------------------*/
/* Linux Kernel                                                                                            */
/*---------------------------------------------------------------------------------------------------------*/
#elif defined(KBUILD_STR) && defined(__KERNEL__)
#include "defs_os_linux.h"

/*---------------------------------------------------------------------------------------------------------*/
/* Linux Application                                                                                       */
/*---------------------------------------------------------------------------------------------------------*/
#elif defined(__LINUX_APP__)
#include "defs_os_linux_app.h"

/*---------------------------------------------------------------------------------------------------------*/
/* Core: ARM                                                                                               */
/* Supported Tools: GCC 3.1 (based) and above, RVDS 3.0 for windows and above                              */
/*---------------------------------------------------------------------------------------------------------*/
#elif defined(_ARM_) || defined(__arm__) || defined(__arm) || defined(__thumb__)
#include "defs_core_arm.h"
#endif

/*---------------------------------------------------------------------------------------------------------*/
/* Default definitions                                                                                     */
/*---------------------------------------------------------------------------------------------------------*/
#include "defs_os_general.h"

/*---------------------------------------------------------------------------------------------------------*/
/*                                             Very Long Types                                             */
/*---------------------------------------------------------------------------------------------------------*/
typedef uint32_t TYPE_64BIT[2];
typedef uint32_t TYPE_96BIT[3];
typedef uint32_t TYPE_128BIT[4];
typedef uint32_t TYPE_192BIT[6];
typedef uint32_t TYPE_256BIT[8];
typedef uint32_t TYPE_320BIT[10];
typedef uint32_t TYPE_512BIT[16];
typedef uint32_t TYPE_1024BIT[32];
typedef uint32_t TYPE_2048BIT[64];
typedef uint32_t TYPE_4096BIT[128];

/*---------------------------------------------------------------------------------------------------------*/
/* Variable packing                                                                                        */
/*---------------------------------------------------------------------------------------------------------*/
#ifndef PACKED
#ifdef _MSC_VER
#define PACKED_START __pragma(pack(push, 1u))
#define PACKED
#define PACKED_END __pragma(pack(pop))
#else
#define PACKED_START
#define PACKED __attribute__((packed))
#define PACKED_END
#endif
#endif //PACKED

/*---------------------------------------------------------------------------------------------------------*/
/* Inline functions                                                                                        */
/*---------------------------------------------------------------------------------------------------------*/
#ifndef INLINE_
#ifdef _MSC_VER
#define INLINE_ __forceinline
#elif defined(__GNUC__)
#define INLINE_ inline __attribute__((__always_inline__))
#elif defined(__CLANG__)
#if __has_attribute(__always_inline__)
#define INLINE_ inline __attribute__((__always_inline__))
#else
#define INLINE_ inline
#endif
#elif defined __ARMCC_VERSION
#define INLINE_ __inline
#else
#define INLINE_ inline
#endif
#endif //_INLINE_

#endif // DEFS_TYPES_H__
