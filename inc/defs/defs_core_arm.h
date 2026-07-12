/*---------------------------------------------------------------------------------------------------------*/
/*  Winbond Electronics Corporation                                                                        */
/*                                                                                                         */
/*  Copyright (c) 2019 by Winbond Electronics Corporation                                                  */
/*  All rights reserved                                                                                    */
/*                                                                                                         */
/*<<<------------------------------------------------------------------------------------------------------*/
/* File Contents:                                                                                          */
/*   defs_core_arm.h                                                                                       */
/*            This file contains definitions for ARM core and its compilers                                */
/*  Project:                                                                                               */
/*            SWC DEFS                                                                                     */
/*---------------------------------------------------------------------------------------------------------*/
#ifndef DEFS_ARM_H_
#define DEFS_ARM_H_

/*---------------------------------------------------------------------------------------------------------*/
/* Core dependent PTR definitions                                                                          */
/*---------------------------------------------------------------------------------------------------------*/
#define PTR8  (volatile uint8_t*)
#define PTR16 (volatile uint16_t*)
#define PTR32 (volatile uint32_t*)

/*---------------------------------------------------------------------------------------------------------*/
/* Variables alignment                                                                                     */
/*---------------------------------------------------------------------------------------------------------*/
#ifndef ALIGN_
#define ALIGN_(x, decl) decl __attribute__((aligned(x)))
#endif

/*---------------------------------------------------------------------------------------------------------*/
/* Variables packing                                                                                       */
/*---------------------------------------------------------------------------------------------------------*/
#ifndef PACK_
#define PACK_(decl) decl __attribute__((__packed__))
#endif

/*---------------------------------------------------------------------------------------------------------*/
/* Section naming                                                                                          */
/*---------------------------------------------------------------------------------------------------------*/
#define _SECTION_(name)   __attribute__((section(name)))
#define SECTION_ZI_(name) __attribute__((section(name), zero_init))

/*---------------------------------------------------------------------------------------------------------*/
/* Jump to for THUMB                                                                                       */
/*---------------------------------------------------------------------------------------------------------*/
#if defined(__thumb__)
#define JUMP_TO(addr) ((void (*)(void))((addr) | 1))()
#endif

/*---------------------------------------------------------------------------------------------------------*/
/* NOP                                                                                                     */
/*---------------------------------------------------------------------------------------------------------*/
#if defined(__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050)
/* ARM Compiler 6 (armclang)                                                                               */
#define _NOP_() __builtin_arm_nop()
#else
#define _NOP_() __nop()
#endif

/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
/*                                    ARMCC windows compiler(s) support                                    */
/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
#if defined(__ARMCC_VERSION)

/*---------------------------------------------------------------------------------------------------------*/
/* Check if ARMCLANG compiler is used                                                                      */
/*---------------------------------------------------------------------------------------------------------*/
#ifdef __VERSION__
#define __ARMCLANG
#include <arm_compat.h>
#endif

/*---------------------------------------------------------------------------------------------------------*/
/* ARMCLANG function abbreviations                                                                         */
/*---------------------------------------------------------------------------------------------------------*/
#ifndef IRQ
#ifdef __ARMCLANG
#define IRQ __attribute__((interrupt("IRQ")))
#else
#define IRQ __irq
#endif
#endif

/*---------------------------------------------------------------------------------------------------------*/
/* Compiler warnings                                                                                       */
/*---------------------------------------------------------------------------------------------------------*/
#ifdef __ARMCLANG
#pragma clang diagnostic ignored "-Wunused-function"
#pragma clang diagnostic ignored "-Wunused-parameter"
#pragma clang diagnostic ignored "-Wconversion"
#pragma clang diagnostic ignored "-Wpadded"
#pragma clang diagnostic ignored "-Wswitch-enum"
#pragma clang diagnostic ignored "-Wcast-qual"
#pragma clang diagnostic ignored "-Wundef"
#pragma clang diagnostic ignored "-Wmissing-field-initializers"
#endif

/*---------------------------------------------------------------------------------------------------------*/
/* Inline functions                                                                                        */
/*---------------------------------------------------------------------------------------------------------*/
#ifndef INLINE_
#define INLINE_ __attribute__((always_inline))
#endif

/*---------------------------------------------------------------------------------------------------------*/
/* Compiler intrinsics                                                                                     */
/*---------------------------------------------------------------------------------------------------------*/
#ifdef __ARMCLANG
/*---------------------------------------------------------------------------------------------------------*/
/* ARMCLANG                                                                                                */
/*---------------------------------------------------------------------------------------------------------*/

static uint32_t __rev32(uint32_t num) __attribute__((always_inline));
static uint32_t __rev32(uint32_t num)
{
    uint32_t ret;
    __asm("REV  %[result], %[input]" : [result] "=r"(ret) : [input] "r"(num));
    return ret;
}

static void __waitForInterrupt(void) __attribute__((always_inline));
static void __waitForInterrupt(void)
{
    __asm("WFI");
}

#ifdef __ARM_FEATURE_CLZ
#ifndef CLZ32_
#define CLZ32_(x) __clz(x)
#endif // _CLZ32_
#endif

#define LE32(x)                       __rev32(x)
#define LE16(x)                       MS16(__rev32((uint32_t)(x)))
#define CURRENT_INSTRUCTION_ADDRESS() __current_pc()
#define CURRENT_STACK_POINTER()       __current_sp()
#define _WFI                          __waitForInterrupt

#else
/*---------------------------------------------------------------------------------------------------------*/
/* ARMCC                                                                                                   */
/*---------------------------------------------------------------------------------------------------------*/
#ifndef CLZ32_
#define CLZ32_(x) __clz(x)
#endif // _CLZ32_
#define LE32(x)                       __rev(x)
#define LE16(x)                       MS16(__rev((uint32_t)(x)))
#define CURRENT_INSTRUCTION_ADDRESS() __current_pc()
#define CURRENT_STACK_POINTER()       __current_sp()
#define _WFI                          __wfi
#endif

/*---------------------------------------------------------------------------------------------------------*/
/* Interrupt macros                                                                                        */
/*---------------------------------------------------------------------------------------------------------*/
#ifndef ENABLE_INTERRUPTS
#define ENABLE_INTERRUPTS() __enable_irq()
#endif
#ifndef DISABLE_INTERRUPTS
#define DISABLE_INTERRUPTS() __disable_irq()
#endif
#ifndef INTERRUPTS_VAR_DECLARE
#define INTERRUPTS_VAR_DECLARE(var) uint32_t var = 0u
#endif
#ifndef INTERRUPTS_SAVE_DISABLE
#define INTERRUPTS_SAVE_DISABLE(var) \
    {                                \
        var = __disable_irq();       \
    }
#endif
#ifndef INTERRUPTS_RESTORE
#define INTERRUPTS_RESTORE(var) \
    {                           \
        if (!var)               \
        {                       \
            __enable_irq();     \
        }                       \
    }
#endif
#define IS_INTERRUPTS_ENABLED()  (__get_PRIMASK() == 0)
#define IS_INSIDE_INTERRUPT()    ((SCB->ICSR & SCB_ICSR_VECTACTIVE_Msk) != 0)
#define IS_INTERRUPT_AVAILABLE() ((IS_INTERRUPTS_ENABLED()) && (!(IS_INSIDE_INTERRUPT())))

/*---------------------------------------------------------------------------------------------------------*/
/* Assertion macros                                                                                        */
/*---------------------------------------------------------------------------------------------------------*/
#if !defined(__TARGET_ARCH_4) && !defined(__TARGET_ARCH_4T)
#define CORE_ASSERT()    \
    {                    \
        __breakpoint(0); \
    }
#endif

#if defined(__thumb__)
/*---------------------------------------------------------------------------------------------------------*/
/*                                      THUMB INSTRUCTION SET                                              */
/*                          Inline assembly instructions are NOT allowed                                   */
/*---------------------------------------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------------------------------------*/
/* CPU power management                                                                                    */
/*---------------------------------------------------------------------------------------------------------*/
#if (__TARGET_ARCH_THUMB > 2)
#define CPU_IDLE() _WFI()
#endif

/*---------------------------------------------------------------------------------------------------------*/
/* Cache macros                                                                                            */
/*---------------------------------------------------------------------------------------------------------*/
#if (__TARGET_ARCH_THUMB > 3)
static __inline __asm uint32_t __defs_getCP15(void)
{
    MRC p15, 0, r0, c1, c0;
    BX  lr;
}

static __inline __asm void __defs_setCP15(uint32_t val)
{
    MCR p15, 0, r0, c1, c0;
    BX  lr;
}

#define ICACHE_SAVE_DISABLE(var)       \
    {                                  \
        var = __defs_getCP15();        \
        __defs_setCP15(var&(~0x1000)); \
    }
#define ICACHE_SAVE_ENABLE(var)       \
    {                                 \
        var = __defs_getCP15();       \
        __defs_setCP15(var | 0x1000); \
    }
#define ICACHE_RESTORE(var) __defs_setCP15(var)
#endif

#else
/*---------------------------------------------------------------------------------------------------------*/
/*                                       ARM INSTRUCTION SET                                               */
/*                             Inline assembly instructions are allowed                                    */
/*---------------------------------------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------------------------------------*/
/* CPU power management                                                                                    */
/*---------------------------------------------------------------------------------------------------------*/
#if defined(__TARGET_ARCH_4) || (__TARGET_ARCH_4T) || (__TARGET_ARCH_5T) || (__TARGET_ARCH_5TE) || (__TARGET_ARCH_5TEJ) || \
    (__TARGET_ARCH_6) || (__TARGET_ARCH_6_M) || (__TARGET_ARCH_6S_M) || (__TARGET_ARCH_6K) || (__TARGET_ARCH_6T2) ||       \
    (__TARGET_ARCH_6Z)

#define CPU_IDLE()                                         \
    {                                                      \
        uint32_t __armcc_temp_var = 0;                     \
        __asm {                                           \
                                                        MCR p15, 0, __armcc_temp_var, c7, c0, 4; } \
    }
#else
#define CPU_IDLE() _WFI()
#endif

/*---------------------------------------------------------------------------------------------------------*/
/* Cache macros                                                                                            */
/*---------------------------------------------------------------------------------------------------------*/
#define ICACHE_SAVE_DISABLE(var)                                                                                    \
    {                                                                                                               \
        uint32_t __armcc_temp_var;                                                                                  \
        __asm { MRC  p15, 0, var, c1, c0;                \
                                                   BIC  __armcc_temp_var, var, 0x1000;      \
                                                   MCR  p15, 0, __armcc_temp_var, c1, c0; } \
    }

#define ICACHE_SAVE_ENABLE(var)                                                                                     \
    {                                                                                                               \
        uint32_t __armcc_temp_var;                                                                                  \
        __asm { MRC  p15, 0, var, c1, c0;                \
                                                   ORR  __armcc_temp_var, var, 0x1000;      \
                                                   MCR  p15, 0, __armcc_temp_var, c1, c0; } \
    }

#define ICACHE_RESTORE(var) __asm { MCR  p15, 0, var, c1, c0}

#endif

/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
/*                                     GNU GCC based compilers support                                     */
/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
#elif defined(__GNUC__)

/*---------------------------------------------------------------------------------------------------------*/
/* Compiler intrinsics                                                                                     */
/*---------------------------------------------------------------------------------------------------------*/
#define CURRENT_STACK_POINTER() gcc_current_sp()

/*---------------------------------------------------------------------------------------------------------*/
/* Assertion macros                                                                                        */
/*---------------------------------------------------------------------------------------------------------*/
#if !defined(CORE_ASSERT) && !defined(__ARM_ARCH_2__) && !defined(__ARM_ARCH_3__) && !defined(__ARM_ARCH_3M__) && \
    !defined(__ARM_ARCH_4__) && !defined(__ARM_ARCH_4T__)
#define CORE_ASSERT()                  \
    {                                  \
        __asm __volatile__("BKPT #0"); \
    }
#endif

#ifndef INLINE_
/*---------------------------------------------------------------------------------------------------------*/
/* Inline functions                                                                                        */
/*---------------------------------------------------------------------------------------------------------*/
#define INLINE_ __attribute__((always_inline)) inline
#endif

#if defined(__thumb__)
/*---------------------------------------------------------------------------------------------------------*/
/*                                      THUMB INSTRUCTION SET                                              */
/*                          Some assembly instructions are not available                                   */
/*---------------------------------------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------------------------------------*/
/* CPU power management                                                                                    */
/*---------------------------------------------------------------------------------------------------------*/
#if defined(__ARM_ARCH_2__) || (__ARM_ARCH_3__) || (__ARM_ARCH_3M__) || (__ARM_ARCH_4__) || (__ARM_ARCH_4T__) || \
    (__ARM_ARCH_5T__) || (__ARM_ARCH_5TE__) || (__ARM_ARCH_5TEJ__) || (__ARM_ARCH_6J__)
/*---------------------------------------------------------------------------------------------------------*/
/* Not supported by architecture with Thumb instructions                                                   */
/*---------------------------------------------------------------------------------------------------------*/
#define CPU_IDLE()

#elif defined(__ARM_ARCH_6_M__) || (__ARM_ARCH_6__) || (__ARM_ARCH_6S_M__) || (__ARM_ARCH_6K__) || (__ARM_ARCH_6T2__)

/*---------------------------------------------------------------------------------------------------------*/
/* Supported via CP15                                                                                      */
/*---------------------------------------------------------------------------------------------------------*/
#define CPU_IDLE()                                    \
    __asm __volatile__("MOV r0, #0 \n\t"              \
                       "MCR p15, 0, r0, c7, c0, 4" :: \
                           : "r0", "cc")
#else
/*---------------------------------------------------------------------------------------------------------*/
/* Supported with WFI instruction                                                                          */
/*---------------------------------------------------------------------------------------------------------*/
#define CPU_IDLE() __asm__ __volatile__("WFI")
#endif

/*---------------------------------------------------------------------------------------------------------*/
/* Interrupt macros                                                                                        */
/*---------------------------------------------------------------------------------------------------------*/
#if defined(__ARM_ARCH_4__) || (__ARM_ARCH_4T__) || (__ARM_ARCH_5T__) || (__ARM_ARCH_5TE__) || (__ARM_ARCH_5TEJ__)

/*---------------------------------------------------------------------------------------------------------*/
/* Not supported with Thumb instructions                                                                   */
/*---------------------------------------------------------------------------------------------------------*/

#elif defined(__ARM_ARCH_6J__) || (__ARM_ARCH_6ZK__)
/*---------------------------------------------------------------------------------------------------------*/
/* Support for CPSIE instruction                                                                           */
/*---------------------------------------------------------------------------------------------------------*/
#ifndef INTERRUPTS_VAR_DECLARE
#define INTERRUPTS_VAR_DECLARE(var)
#endif
#ifndef ENABLE_INTERRUPTS
#define ENABLE_INTERRUPTS() __asm__ __volatile__("CPSIE i")
#endif
#ifndef DISABLE_INTERRUPTS
#define DISABLE_INTERRUPTS() __asm__ __volatile__("CPSID i")
#endif
#ifndef INTERRUPTS_SAVE_DISABLE
#define INTERRUPTS_SAVE_DISABLE(var) DISABLE_INTERRUPTS()
#endif
#ifndef INTERRUPTS_RESTORE
#define INTERRUPTS_RESTORE(var) ENABLE_INTERRUPTS()
#endif
#else
/*---------------------------------------------------------------------------------------------------------*/
/* Full assembly support with Thumb2                                                                       */
/*---------------------------------------------------------------------------------------------------------*/
#ifndef INTERRUPTS_VAR_DECLARE
#define INTERRUPTS_VAR_DECLARE(var) uint32_t var = 0u
#endif
#ifndef ENABLE_INTERRUPTS
#define ENABLE_INTERRUPTS()                          \
    __asm__ __volatile__("MRS    r6, APSR     \n\t"  \
                         "MOV    r5, #0x80    \n\t"  \
                         "BIC    r6,r6,r5     \n\t"  \
                         "MSR    APSR_nzcvq,r6  " :: \
                             : "r6", "r5", "cc")
#endif
#ifndef DISABLE_INTERRUPTS
#define DISABLE_INTERRUPTS()                         \
    __asm__ __volatile__("MRS    r6, APSR     \n\t"  \
                         "MOV    r5, #0x80    \n\t"  \
                         "ORR    r6, r6,r5    \n\t"  \
                         "MSR    APSR_nzcvq, r6 " :: \
                             : "r6", "r5", "cc")
#endif
#ifndef INTERRUPTS_SAVE_DISABLE
#define INTERRUPTS_SAVE_DISABLE(var)                    \
    __asm__ __volatile__("MRS    %[val], APSR     \n\t" \
                         "MOV    r6, #0x80        \n\t" \
                         "ORR    r6, %[val],r6    \n\t" \
                         "MSR    APSR_nzcvq,r6  "       \
                         : [val] "+r"(var)::"r6", "cc")
#endif

#ifndef INTERRUPTS_RESTORE
#define INTERRUPTS_RESTORE(var) __asm__ __volatile__("MSR    APSR_nzcvq,%[val]  " : [val] "+r"(var))
#endif
#endif

/*---------------------------------------------------------------------------------------------------------*/
/* Cache macros                                                                                            */
/*---------------------------------------------------------------------------------------------------------*/
#if defined(__ARM_ARCH_2__) || (__ARM_ARCH_3__) || (__ARM_ARCH_3M__) || (__ARM_ARCH_4__) || (__ARM_ARCH_4T__) || \
    (__ARM_ARCH_5TE__) || (__ARM_ARCH_5T__) || (__ARM_ARCH_5TEJ__) || (__ARM_ARCH_6J__) || (__ARM_ARCH_6ZK__) || \
    (__ARM_ARCH_6K__) || (__ARM_ARCH_6M__)
/*---------------------------------------------------------------------------------------------------------*/
/* Not supported by architecture                                                                           */
/*---------------------------------------------------------------------------------------------------------*/
#else
#define ICACHE_SAVE_DISABLE(var)                             \
    __asm__ __volatile__("MRC  p15, 0, %[val], c1, c0 \n\t"  \
                         "BIC  r6, %[val], #0x1000     \n\t" \
                         "MCR  p15, 0, r6, c1, c0 "          \
                         : [val] "+r"(var)::"r6", "cc")

#define ICACHE_SAVE_ENABLE(var)                              \
    __asm__ __volatile__("MRC  p15, 0, %[val], c1, c0 \n\t"  \
                         "ORR  r6, %[val], #0x1000     \n\t" \
                         "MCR  p15, 0, r6, c1, c0 "          \
                         : [val] "+r"(var)::"r6", "cc")

#define ICACHE_RESTORE(var) __asm__ __volatile__("MCR  p15, 0, %[val], c1, c0" : [val] "+r"(var))
#endif
#else

/*---------------------------------------------------------------------------------------------------------*/
/*                                       ARM INSTRUCTION SET                                               */
/*---------------------------------------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------------------------------------*/
/* CPU power management                                                                                    */
/*---------------------------------------------------------------------------------------------------------*/
#if defined(__ARM_ARCH_2__) || (__ARM_ARCH_3__) || (__ARM_ARCH_3M__) || (__ARM_ARCH_4__) || (__ARM_ARCH_4T__) || \
    (__ARM_ARCH_5T__) || (__ARM_ARCH_5TE__) || (__ARM_ARCH_5TEJ__) || (__ARM_ARCH_6_M__) || (__ARM_ARCH_6__) ||  \
    (__ARM_ARCH_6S_M__) || (__ARM_ARCH_6J__) || (__ARM_ARCH_6T2__)
#define CPU_IDLE()                                    \
    __asm __volatile__("MOV r0, #0 \n\t"              \
                       "MCR p15, 0, r0, c7, c0, 4" :: \
                           : "r0", "cc")
#else
#define CPU_IDLE() __asm__ __volatile__("WFI")
#endif

/*---------------------------------------------------------------------------------------------------------*/
/* Interrupt macros                                                                                        */
/*---------------------------------------------------------------------------------------------------------*/
#ifndef ENABLE_INTERRUPTS
#define ENABLE_INTERRUPTS()                          \
    __asm__ __volatile__("MRS    r6, APSR     \n\t"  \
                         "BIC    r6,r6,#0x80  \n\t"  \
                         "MSR    APSR_nzcvq,r6  " :: \
                             : "r6", "cc")
#endif

#ifndef DISABLE_INTERRUPTS
#define DISABLE_INTERRUPTS()                         \
    __asm__ __volatile__("MRS    r6, APSR     \n\t"  \
                         "ORR    r6, r6,#0x80 \n\t"  \
                         "MSR    APSR_nzcvq, r6 " :: \
                             : "r6", "cc")
#endif

#ifndef INTERRUPTS_VAR_DECLARE
#define INTERRUPTS_VAR_DECLARE(var) uint32_t var = 0u
#endif

#ifndef INTERRUPTS_SAVE_DISABLE
#define INTERRUPTS_SAVE_DISABLE(var)                    \
    __asm__ __volatile__("MRS    %[val], APSR     \n\t" \
                         "ORR    r6, %[val],#0x80 \n\t" \
                         "MSR    APSR_nzcvq,r6  "       \
                         : [val] "+r"(var)::"r6", "cc")
#endif

#ifndef INTERRUPTS_RESTORE
#define INTERRUPTS_RESTORE(var) __asm__ __volatile__("MSR    APSR_nzcvq,%[val]  " : [val] "+r"(var))
#endif

/*---------------------------------------------------------------------------------------------------------*/
/* Cache macros                                                                                            */
/*---------------------------------------------------------------------------------------------------------*/
#define ICACHE_SAVE_DISABLE(var)                             \
    __asm__ __volatile__("MRC  p15, 0, %[val], c1, c0 \n\t"  \
                         "BIC  r6, %[val], #0x1000     \n\t" \
                         "MCR  p15, 0, r6, c1, c0 "          \
                         : [val] "+r"(var)::"r6", "cc")

#define ICACHE_SAVE_ENABLE(var)                              \
    __asm__ __volatile__("MRC  p15, 0, %[val], c1, c0 \n\t"  \
                         "ORR  r6, %[val], #0x1000     \n\t" \
                         "MCR  p15, 0, r6, c1, c0 "          \
                         : [val] "+r"(var)::"r6", "cc")

#define ICACHE_RESTORE(var) __asm__ __volatile__("MCR  p15, 0, %[val], c1, c0" : [val] "+r"(var))
#endif

#else
#warning Warning: INTERRUPTS_xxx and ICHACHE_xxx macros are not defined for current compiler
#endif

#endif // DEFS_ARM_H_
