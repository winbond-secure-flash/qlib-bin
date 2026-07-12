/*---------------------------------------------------------------------------------------------------------*/
/*  Winbond Electronics Corporation                                                                        */
/*                                                                                                         */
/*  Copyright (c) 2019 by Winbond Electronics Corporation                                                  */
/*  All rights reserved                                                                                    */
/*                                                                                                         */
/*<<<------------------------------------------------------------------------------------------------------*/
/* File Contents:                                                                                          */
/*    defs_access.h                                                                                        */
/*            This file contains hardware registers access and field manipulation macros                   */
/* Project:                                                                                                */
/*            SWC DEFS                                                                                     */
/*---------------------------------------------------------------------------------------------------------*/

#ifndef DEFS_ACCESS_H__
#define DEFS_ACCESS_H__

#include "defs_internal.h"

/*---------------------------------------------------------------------------------------------------------*/
/*                      REGISTERS AND VARIABLES ACCESS, BIT AND FIELD OPERATION MACROS                     */
/*---------------------------------------------------------------------------------------------------------*/
/* Generic registers access macros assuming registers database defined as follows: (see Coding Standards   */
/* document Chapter 12 for detailed description)                                                           */
/*                                                                                                         */
/* Module definition:                                                                                      */
/* ------------------                                                                                      */
/*                                                                                                         */
/* #define <MODULE>_BASE_ADDR          (<module_base_addr> )             these definition should be kept   */
/*                                                                       in  a <chip>.h file, since it is  */
/*                                                                       chip specific                     */
/*                                                                                                         */
/* #define <MODULE>_ACCESS             (<module_access_type> )           same name as access macros        */
/*                                                                       name,i.e MEM, IO, INDEXDATA, etc. */
/*                                                                       The rest of the definitions       */
/*                                                                       should be kept in a <hw_module>.h */
/*                                                                       file since they are module        */
/*                                                                       specific                          */
/*                                                                                                         */
/* Register definition:                                                                                    */
/* --------------------                                                                                    */
/*                                                                                                         */
/* #define <MY_REG_1>              (<MODULE>_BASE_ADDR + <offset>), <ACCESS TYPE>, <8/16/32>               */
/*                                                                                                         */
/* Fields definition:                                                                                      */
/* -------------------                                                                                     */
/*                                                                                                         */
/* #define <MY_REG_1_FIELD_1>   <field 1 position>, <field 1 size>                                         */
/* #define <MY_REG_1_FIELD_2>   <field 2 position>, <field 2 size>                                         */
/* ...                                                                                                     */
/*---------------------------------------------------------------------------------------------------------*/
/*                                                                                                         */
/* When there is a definition of registers as described above, manipulations on registers and fields can   */
/* be done in the following ways:                                                                          */
/*                                                                                                         */
/* 1. The registers can be accessed directly using the following macros:                                   */
/*                                                                                                         */
/*          REG_READ(reg)                                                                                  */
/*          REG_WRITE(reg,val)                                                                             */
/*                                                                                                         */
/* 2. Registers field can be accessed using the following macros, whose internal implementation access the */
/*    register using REG_READ and REG_WRITE macros (implicit read-modify-write)                            */
/*                                                                                                         */
/*                                                                                                         */
/*          READ_REG_FIELD(reg, field)                                                                     */
/*          SET_REG_FIELD(reg, field, value)                                                               */
/*                                                                                                         */
/*                                                                                                         */
/* 3. For registers field manipulation via explicit read-modify-write procedure on a variable, use the     */
/*    following macros:                                                                                    */
/*                                                                                                         */
/*          READ_VAR_FIELD(var, field)                                                                     */
/*          SET_VAR_FIELD(var, field, value)                                                               */
/*                                                                                                         */
/*                                                                                                         */
/* 4. For the cases when the fields are single bits and are stored in a variable rather than in a constant,*/
/*    the following sets of macros can be used:                                                            */
/*                                                                                                         */
/*          SET_REG_BIT(reg, bit_no)                                                                       */
/*          READ_REG_BIT(reg, bit_no)                                                                      */
/*          CLEAR_REG_BIT(reg, bit_no)                                                                     */
/*                                                                                                         */
/*          SET_VAR_BIT(var, bit_no)                                                                       */
/*          READ_VAR_BIT(var, bit_no)                                                                      */
/*          CLEAR_VAR_BIT(var, bit_no                                                                      */
/*                                                                                                         */
/* 5. For the cases where masks that should be written to the registers are variables, one may use the     */
/*    the following macros. These macros should be used rarely, only in the cases where the usual method   */
/*    of fields definitions can not be used:                                                               */
/*                                                                                                         */
/*          SET_VAR_MASK(var, bit_mask)                                                                    */
/*          READ_VAR_MASK(var, bit_mask)                                                                   */
/*          CLEAR_VAR_MASK(var, bit_mask)                                                                  */
/*                                                                                                         */
/*          SET_REG_MASK(reg, bit_mask)                                                                    */
/*          READ_REG_MASK(reg, bit_mask)                                                                   */
/*          CLEAR_REG_MASK(reg, bit_mask)                                                                  */
/*                                                                                                         */
/*                                                                                                         */
/* 6. Sometimes it is required to just get a mask (filled with 1b) that fits a certain field without       */
/*    writing a value to it. For this purpose use the macro                                                */
/*                                                                                                         */
/*          MASK_FIELD(field)                                                                              */
/*                                                                                                         */
/* 7. For building a value of a mask without writing to actual register or value, use:                     */
/*                                                                                                         */
/*          BUILD_FIELD_VAL(field, value)                                                                  */
/*                                                                                                         */
/*    This macro is useful when several fields of register/variable are required to be set at a time.      */
/*    In this case, an ORed chain of BUILD_FIELD_VAL invocations will be written to the                    */
/*    register/variable                                                                                    */
/*                                                                                                         */
/*---------------------------------------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------------------------------------*/
/* Direct register access                                                                                  */
/*---------------------------------------------------------------------------------------------------------*/
#define REG_READ(reg) REG_READ_AUX_(GET_REG_ADDR(reg), GET_REG_ACCESSTYPE(reg), GET_REG_SIZE(reg))

#define REG_WRITE(reg, val) REG_WRITE_AUX_((val), GET_REG_ADDR(reg), GET_REG_ACCESSTYPE(reg), GET_REG_SIZE(reg))

/*---------------------------------------------------------------------------------------------------------*/
/* Get field of a register / variable                                                                      */
/*---------------------------------------------------------------------------------------------------------*/
#define READ_REG_FIELD(reg, field) GET_FIELD_VAL(REG_READ(GET_REG(reg)), GET_FIELD_SIZE(field), GET_FIELD_POS(field))

// clang-format off
#define READ_VAR_FIELD(var, field)                                                                          \
   GET_FIELD_VAL(var, GET_FIELD_SIZE(field), GET_FIELD_POS(field))
// clang-format on

/*---------------------------------------------------------------------------------------------------------*/
/* Set field of a register / variable                                                                      */
/*---------------------------------------------------------------------------------------------------------*/

#define SET_REG_FIELD(reg, field, value, regTypeCast) \
    REG_WRITE(GET_REG(reg),                           \
              (regTypeCast)SET_FIELD_VAL(REG_READ(GET_REG(reg)), GET_FIELD_SIZE(field), GET_FIELD_POS(field), (value)))

// clang-format off
#define SET_VAR_FIELD(var, field, value, varTypeCast)                                                                    \
    ((var) = (varTypeCast)SET_FIELD_VAL(var, GET_FIELD_SIZE(field), GET_FIELD_POS(field), (value)))
// clang-format on

/*---------------------------------------------------------------------------------------------------------*/
/* Variable bit operation macros                                                                           */
/*---------------------------------------------------------------------------------------------------------*/
#define READ_VAR_BIT(var, nb) GET_BIT_VAL((var), (nb))

#define SET_VAR_BIT(var, nb, varTypeCast) ((var) = (varTypeCast)SET_BIT_VAL((var), (nb)))

// clang-format off
#define CLEAR_VAR_BIT(var, nb, varTypeCast) ((var) =(varTypeCast)CLEAR_BIT_VAL((var), (nb)))
// clang-format on

// clang-format off
#define WRITE_VAR_BIT(var, nb, val, varTypeCast) \
    {                                            \
        if (val)                                 \
        {                                        \
            SET_VAR_BIT(var, nb, varTypeCast);   \
        }                                        \
        else                                     \
        {                                        \
            CLEAR_VAR_BIT(var, nb, varTypeCast); \
        }                                        \
    }
// clang-format on

/*---------------------------------------------------------------------------------------------------------*/
/* Array Bit operation macros                                                                              */
/*---------------------------------------------------------------------------------------------------------*/
#define READ_ARRAY_BIT(arr, n) READ_VAR_BIT(((uint8_t*)(arr))[(n) / 8], (n) % 8)
#define READ_ARRAY_BIT_RANGE(arr, n, size, result)                  \
    {                                                               \
        uint32_t __i = 0;                                           \
        *(result)    = 0;                                           \
        for (__i = 0; __i < (size); __i++)                          \
        {                                                           \
            *(result) |= ((READ_ARRAY_BIT(arr, (n) + __i) << __i)); \
        }                                                           \
    }

#define WRITE_ARRAY_BIT(arr, n, data, varTypeCast) WRITE_VAR_BIT(((uint8_t*)(arr))[(n) / 8], (n) % 8, data, varTypeCast)

#define WRITE_ARRAY_BIT_RANGE(src, dst, n_dst, size, varTypeCast)                                             \
    {                                                                                                         \
        uint32_t __i = 0;                                                                                     \
        for (__i = 0; __i < (size); __i++)                                                                    \
        {                                                                                                     \
            WRITE_ARRAY_BIT((dst), ((n_dst) + __i), (((uint32_t)(src) & (0x01 << __i)) >> __i), varTypeCast); \
        }                                                                                                     \
    }

#define READ_ARRAY_BIT_BE(arr, n)        READ_VAR_BIT(((uint8_t*)(arr))[(n) / 8], (7 - ((n) % 8)))
#define WRITE_ARRAY_BIT_BE(arr, n, data) WRITE_VAR_BIT(((uint8_t*)(arr))[(n) / 8], (7 - ((n) % 8)), data, uint8_t)

/*---------------------------------------------------------------------------------------------------------*/
/* Register bit operation macros                                                                           */
/*---------------------------------------------------------------------------------------------------------*/
#define SET_REG_BIT(reg, nb) REG_WRITE(GET_REG(reg), SET_BIT_VAL(REG_READ(GET_REG(reg)), nb))

#define CLEAR_REG_BIT(reg, nb) REG_WRITE(GET_REG(reg), CLEAR_BIT_VAL(REG_READ(GET_REG(reg)), nb))

#define READ_REG_BIT(reg, nb) GET_BIT_VAL(REG_READ(GET_REG(reg)), nb)

#define WRITE_REG_BIT(reg, nb, val)          \
    {                                        \
        if ((val) == 1u)                     \
        {                                    \
            SET_REG_BIT(GET_REG(reg), nb);   \
        }                                    \
        else                                 \
        {                                    \
            CLEAR_REG_BIT(GET_REG(reg), nb); \
        }                                    \
    }

/*---------------------------------------------------------------------------------------------------------*/
/* Variable mask operation macros                                                                          */
/*---------------------------------------------------------------------------------------------------------*/
#define READ_VAR_MASK(var, bit_mask) READ_MASK(var, bit_mask)

#define SET_VAR_MASK(var, bit_mask) ((var) = SET_MASK(var, bit_mask))

#define CLEAR_VAR_MASK(var, bit_mask) ((var) = CLEAR_MASK(var, bit_mask))

/*---------------------------------------------------------------------------------------------------------*/
/* Register mask operation macros                                                                          */
/*---------------------------------------------------------------------------------------------------------*/
#define READ_REG_MASK(reg, bit_mask) READ_MASK(REG_READ(GET_REG(reg)), bit_mask)

#define SET_REG_MASK(reg, bit_mask) REG_WRITE(GET_REG(reg), SET_MASK(REG_READ(GET_REG(reg)), bit_mask))

#define CLEAR_REG_MASK(reg, bit_mask) REG_WRITE(GET_REG(reg), CLEAR_MASK(REG_READ(GET_REG(reg)), bit_mask))

/*---------------------------------------------------------------------------------------------------------*/
/* Build a mask of a register / variable field                                                             */
/*---------------------------------------------------------------------------------------------------------*/
#define MASK_FIELD(field) BUILD_FIELD_MASK(GET_FIELD_SIZE(field), GET_FIELD_POS(field))

#define MASK_BIT(bit) BUILD_FIELD_MASK(1u, bit)

/*---------------------------------------------------------------------------------------------------------*/
/* Expand the value of the given field into its correct position                                           */
/*---------------------------------------------------------------------------------------------------------*/
#define BUILD_FIELD_VAL(field, value) BUILD_FIELD_VAL_INT(GET_FIELD_SIZE(field), GET_FIELD_POS(field), value)

/*---------------------------------------------------------------------------------------------------------*/
/* Getting info about registers and fields                                                                 */
/*---------------------------------------------------------------------------------------------------------*/
#define REG_ADDR(reg)        GET_REG_ADDR(reg)
#define GET_FIELD(pos, size) pos, size

#endif // DEFS_ACCESS_H__
