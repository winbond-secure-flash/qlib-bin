/*---------------------------------------------------------------------------------------------------------*/
/*  Winbond Electronics Corporation                                                                        */
/*                                                                                                         */
/*  Copyright (c) 2013 by Winbond Technology Ltd.                                                          */
/*  All rights reserved                                                                                    */
/*                                                                                                         */
/*<<<------------------------------------------------------------------------------------------------------*/
/* File Contents:                                                                                          */
/*    defs_internal.h                                                                                      */
/*            This file contains internal auxiliary macros                                                 */
/* Project:                                                                                                */
/*            SW DEFS                                                                                      */
/*---------------------------------------------------------------------------------------------------------*/

#ifndef DEFS_INTERNAL_H__
#define DEFS_INTERNAL_H__

#if defined(__KERNEL__) && !defined(CFG_OPTEE_REVISION_MAJOR)
#include <linux/types.h>
#else
#include <stdint.h>
#endif
/*---------------------------------------------------------------------------------------------------------*/
/* Low-level getters                                                                                       */
/*---------------------------------------------------------------------------------------------------------*/
#define REG_READ_AUX_(addr, type, size)       REG_ACCESS_FORM1(type, R, size, addr)
#define REG_WRITE_AUX_(val, addr, type, size) REG_ACCESS_FORM2(type, W, size, addr, val)

#define REG_ACCESS_FORM1(type, rw, size, arg)        CONCAT3(type, rw, size)((arg))
#define REG_ACCESS_FORM2(type, rw, size, arg1, arg2) CONCAT3(type, rw, size)((arg1), (arg2))
#define CASTING_FROM(size)                           CASTING_FROM2(UINT, size)
#define CASTING_FROM2(type, size)                    type##size

#define GET_REG(addr, accesstype, size)            addr, accesstype, size
#define GET_REG_ADDR(addr, accesstype, size)       addr
#define GET_REG_ACCESSTYPE(addr, accesstype, size) accesstype
#define GET_REG_SIZE(addr, accesstype, size)       size

#define GET_FIELD_INT(pos, size)  pos, size
#define GET_FIELD_POS(pos, size)  (pos)
#define GET_FIELD_SIZE(pos, size) (size)

/*---------------------------------------------------------------------------------------------------------*/
/* Low-level get field value                                                                               */
/*---------------------------------------------------------------------------------------------------------*/
#define GET_FIELD_VAL(val, field_size, field_pos) \
    (uintmax_t)((((uintmax_t)(val)) >> (field_pos)) & ((((uintmax_t)1U << (field_size)) - (uintmax_t)1U)))

/* Low-level set field value                                                                               */
/*---------------------------------------------------------------------------------------------------------*/
// clang-format off
#define SET_FIELD_VAL(val, field_size, field_pos, value)                                         \
   (((field_size) == 1u) ?                                                                        \
        (((value) == 1u) ?                                                                        \
            SET_BIT_VAL((val),  (field_pos))                                                     \
        :                                                                                         \
            CLEAR_BIT_VAL((val), (field_pos)))                                                   \
    :                                                                                             \
        (   SET_MASK(  BUILD_FIELD_VAL_SIZE((value), (field_size), (field_pos)),                \
                        CLEAR_FIELD_VAL((val), (field_size), (field_pos))))                      \
    )
// clang-format on

/*---------------------------------------------------------------------------------------------------------*/
/* Low-level get bit value                                                                                 */
/*---------------------------------------------------------------------------------------------------------*/
#define GET_BIT_VAL(val, bit_num) READ_MASK(((uintmax_t)(val)) >> (bit_num), 1U)

/*---------------------------------------------------------------------------------------------------------*/
/* Low-level set bit value                                                                                 */
/*---------------------------------------------------------------------------------------------------------*/
#define SET_BIT_VAL(val, bit_num) SET_MASK((val), ((uintmax_t)1u) << (bit_num))

/*---------------------------------------------------------------------------------------------------------*/
/* Low-level clear bit value                                                                               */
/*---------------------------------------------------------------------------------------------------------*/
#define CLEAR_BIT_VAL(val, bit_num) CLEAR_MASK((val), ((uintmax_t)1u) << (bit_num))

/*---------------------------------------------------------------------------------------------------------*/
/* Low-level field mask                                                                                    */
/*---------------------------------------------------------------------------------------------------------*/
#define BUILD_FIELD_MASK(field_size, field_pos) ((((((uintmax_t)1u) << (field_size)) - ((uintmax_t)1u))) << (field_pos))

/*---------------------------------------------------------------------------------------------------------*/
/* Low-level clear mask field value                                                                        */
/*---------------------------------------------------------------------------------------------------------*/
#define CLEAR_FIELD_VAL(val, field_size, field_pos) CLEAR_MASK(val, BUILD_FIELD_MASK(field_size, field_pos))

/*---------------------------------------------------------------------------------------------------------*/
/* Low-level build field value with size                                                                   */
/*---------------------------------------------------------------------------------------------------------*/
#define BUILD_FIELD_VAL_SIZE(val, field_size, field_pos) \
    ((((((uintmax_t)1u) << (field_size)) - ((uintmax_t)1u)) & (val)) << (field_pos))

/*---------------------------------------------------------------------------------------------------------*/
/* Low-level set mask                                                                                      */
/*---------------------------------------------------------------------------------------------------------*/
#define SET_MASK(val, bit_mask) ((val) | (bit_mask))

/*---------------------------------------------------------------------------------------------------------*/
/* Low-level read mask                                                                                     */
/*---------------------------------------------------------------------------------------------------------*/
#define READ_MASK(val, bit_mask) ((val) & (bit_mask))

/*---------------------------------------------------------------------------------------------------------*/
/* Low-level clear mask                                                                                    */
/*---------------------------------------------------------------------------------------------------------*/
#define CLEAR_MASK(val, bit_mask) ((val) & (~(bit_mask)))

/*---------------------------------------------------------------------------------------------------------*/
/* Low-level mask field                                                                                    */
/*---------------------------------------------------------------------------------------------------------*/

// clang-format off
#define BUILD_FIELD_VAL_INT(field_size, field_pos, val)                                                        \
    (BUILD_FIELD_VAL_SIZE(val, field_size, field_pos))
// clang-format on

#define LOG_1_(n)  ((((n) >> 1u) != 0u) ? 1u : 0u)
#define LOG_2_(n)  ((((n) >> 2u) != 0u) ? (2u + LOG_1_(((n) >> 2u))) : LOG_1_(n))
#define LOG_4_(n)  ((((n) >> 4u) != 0u) ? (4u + LOG_2_(((n) >> 4u))) : LOG_2_(n))
#define LOG_8_(n)  ((((n) >> 8u) != 0u) ? (8u + LOG_4_(((n) >> 8u))) : LOG_4_(n))
#define LOG_16_(n) ((((n) >> 16u) != 0u) ? (16u + LOG_8_(((n) >> 16u))) : LOG_8_(n))

/*---------------------------------------------------------------------------------------------------------*/
/*                                          MACRO EXPANSION                                                */
/*---------------------------------------------------------------------------------------------------------*/
#define EVAL(x) x

/*---------------------------------------------------------------------------------------------------------*/
/*                                            LOOP UNROLL                                                  */
/*---------------------------------------------------------------------------------------------------------*/
#define REPEAT_1(f, ...) EVAL(f(0, __VA_ARGS__))
#define REPEAT_2(f, ...)      \
    REPEAT_1(f, __VA_ARGS__); \
    EVAL(f((1), __VA_ARGS__))
#define REPEAT_3(f, ...)      \
    REPEAT_2(f, __VA_ARGS__); \
    EVAL(f((2), __VA_ARGS__))
#define REPEAT_4(f, ...)      \
    REPEAT_3(f, __VA_ARGS__); \
    EVAL(f((3), __VA_ARGS__))
#define REPEAT_5(f, ...)      \
    REPEAT_4(f, __VA_ARGS__); \
    EVAL(f((4), __VA_ARGS__))
#define REPEAT_6(f, ...)      \
    REPEAT_5(f, __VA_ARGS__); \
    EVAL(f((5), __VA_ARGS__))
#define REPEAT_7(f, ...)      \
    REPEAT_6(f, __VA_ARGS__); \
    EVAL(f((6), __VA_ARGS__))
#define REPEAT_8(f, ...)      \
    REPEAT_7(f, __VA_ARGS__); \
    EVAL(f((7), __VA_ARGS__))
#define REPEAT_9(f, ...)      \
    REPEAT_8(f, __VA_ARGS__); \
    EVAL(f((8), __VA_ARGS__))
#define REPEAT_10(f, ...)     \
    REPEAT_9(f, __VA_ARGS__); \
    EVAL(f((9), __VA_ARGS__))
#define REPEAT_11(f, ...)      \
    REPEAT_10(f, __VA_ARGS__); \
    EVAL(f((10), __VA_ARGS__))
#define REPEAT_12(f, ...)      \
    REPEAT_11(f, __VA_ARGS__); \
    EVAL(f((11), __VA_ARGS__))
#define REPEAT_13(f, ...)      \
    REPEAT_12(f, __VA_ARGS__); \
    EVAL(f((12), __VA_ARGS__))
#define REPEAT_14(f, ...)      \
    REPEAT_13(f, __VA_ARGS__); \
    EVAL(f((13), __VA_ARGS__))
#define REPEAT_15(f, ...)      \
    REPEAT_14(f, __VA_ARGS__); \
    EVAL(f((14), __VA_ARGS__))
#define REPEAT_16(f, ...)      \
    REPEAT_15(f, __VA_ARGS__); \
    EVAL(f((15), __VA_ARGS__))

/*---------------------------------------------------------------------------------------------------------*/
/*                                           Assertion macros                                              */
/*---------------------------------------------------------------------------------------------------------*/
#ifndef CORE_ASSERT
#define CORE_ASSERT() \
    {                 \
        for (;;)      \
            ;         \
    }
#endif

#ifndef ASSERT
#define ASSERT(cond)
#endif

#endif
