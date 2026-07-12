/*---------------------------------------------------------------------------------------------------------*/
/*  Winbond Electronics Corporation                                                                        */
/*                                                                                                         */
/*  Copyright (c) 2019 by Winbond Electronics Corporation                                                  */
/*  All rights reserved                                                                                    */
/*                                                                                                         */
/*<<<------------------------------------------------------------------------------------------------------*/
/* File Contents:                                                                                          */
/*   defs_secure.h                                                                                         */
/*            This file contains DEFS security utils                                                       */
/*  Project:                                                                                               */
/*            SWC_DEFS                                                                                     */
/*---------------------------------------------------------------------------------------------------------*/
#ifndef DEFS_SECURE_H__
#define DEFS_SECURE_H__

/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
/*                                             Secure Compare                                              */
/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
#define DEFS_STATUS_ASSERT_EQUAL(a, b)                                         \
    {                                                                          \
        DEFS_STATUS_COND_CHECK((a) == (b), DEFS_STATUS_SECURITY_ERROR);        \
        DEFS_STATUS_COND_CHECK(((a) ^ (b)) == 0u, DEFS_STATUS_SECURITY_ERROR); \
        DEFS_STATUS_COND_CHECK(((a) - (b)) == 0u, DEFS_STATUS_SECURITY_ERROR); \
    }

/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
/*                                               Secure Loop                                               */
/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
#define SEC_FOR_INC(i, start, threshold, step, body)                                              \
    {                                                                                             \
        uint32_t i;                                                                               \
        for ((i) = ((uint32_t)(start)); (i) < ((uint32_t)(threshold)); (i) += ((uint32_t)(step))) \
        {                                                                                         \
            body;                                                                                 \
        }                                                                                         \
                                                                                                  \
        DEFS_STATUS_ASSERT_EQUAL(i, threshold);                                                   \
    }

#define SEC_FOR_DEC(i, start, threshold, step, body)                                            \
    {                                                                                           \
        int32_t i;                                                                              \
        for ((i) = ((int32_t)(start)); (i) >= ((int32_t)(threshold)); (i) -= ((int32_t)(step))) \
        {                                                                                       \
            body;                                                                               \
        }                                                                                       \
                                                                                                \
        DEFS_STATUS_ASSERT_EQUAL(i, ((threshold)-1));                                           \
    }

/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
/*                                               Secure ENUM                                               */
/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/

#define SEC_ENUM_IN_RANGE(e, prefix) ((ENUM_IN_RANGE_FIRST(e, prefix)) && ENUM_IN_RANGE_LAST(e, prefix))
#define SEC_ENUM_TO_VALUE(e, prefix) (((size_t)(e) - (size_t)(prefix##_FIRST)) - 1u)
#define SEC_VALUE_TO_ENUM(e, prefix)                                                       \
    (SEC_ENUM_IN_RANGE(((((size_t)(e)) + (size_t)(CONCAT2(prefix, _FIRST))) + 1u), prefix) \
         ? ((prefix##_T)((((size_t)(e)) + (size_t)(CONCAT2(prefix, _FIRST))) + 1u))        \
         : CONCAT2(prefix, _LAST))
#define SEC_ENUM_ITERATE(i, prefix)                                                              \
    for ((i) = (CONCAT2(prefix, _T))(CONCAT2(prefix, _FIRST) + 1); (i) < CONCAT2(prefix, _LAST); \
         (i) = (CONCAT2(prefix, _T))((i) + 1))

#define ENUM_IN_RANGE_FIRST(e, prefix) (((size_t)(e)) > (size_t)(CONCAT2(prefix, _FIRST)))
#define ENUM_IN_RANGE_LAST(e, prefix)  (((size_t)(e)) < (size_t)(CONCAT2(prefix, _LAST)))

/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
/*                                          Hamming Distance ENUM                                          */
/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
/*                                                                                                         */
/* Sample usage:                                                                                           */
/* #define MY_HD_ENUM_DEFS      0x3A, 8, 8, 8      ... mask, mask_size, index_size, num_of_elements        */
/*                                                                                                         */
/* typedef enum                                                                                            */
/* {                                                                                                       */
/*      MY_ENUM1    = HD_ENUM_ELEMENT(MY_HD_ENUM_DEFS, 0),                                                 */
/*      MY_ENUM2    = HD_ENUM_ELEMENT(MY_HD_ENUM_DEFS, 1),                                                 */
/*      MY_ENUM3    = HD_ENUM_ELEMENT(MY_HD_ENUM_DEFS, 2),                                                 */
/*      ...                                                                                                */
/* } MY_HD_ENUM_T;                                                                                         */
/*                                                                                                         */
/*                                                                                                         */
/* DEFS_STATUS func(MY_HD_ENUM_T x)                                                                        */
/* {                                                                                                       */
/*      uint32_t x_val;                                                                                    */
/*                                                                                                         */
/*      ... Check parameter                                                                                */
/*      DEFS_STATUS_COND_CHECK(HD_ENUM_VALID(MY_HD_ENUM_DEFS, x),   DEFS_STATUS_PARAMETER_OUT_OF_RANGE);   */
/*                                                                                                         */
/*      ... Convert enum to value                                                                          */
/*      x_val = HD_ENUM_VAL(MY_HD_ENUM_DEFS, x);                                                           */
/*                                                                                                         */
/*      ...                                                                                                */
/* }                                                                                                       */
/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
#define HD_ENUM_ELEMENT(defs, index)                                       \
    (uint32_t) HD_ENUM_ELEMENT_INT((uint64_t)HD_ENUM_GET_MASK(defs),       \
                                   (uint64_t)HD_ENUM_GET_MASK_SIZE(defs),  \
                                   (uint64_t)HD_ENUM_GET_INDEX_SIZE(defs), \
                                   index)

#define HD_ENUM_VAL(defs, val) HD_ENUM_VAL_INT(HD_ENUM_GET_INDEX_SIZE(defs), val)

#define HD_ENUM_VALID(defs, val)                              \
    HD_ENUM_VALID_INT((uint64_t)HD_ENUM_GET_MASK(defs),       \
                      (uint64_t)HD_ENUM_GET_MASK_SIZE(defs),  \
                      (uint64_t)HD_ENUM_GET_INDEX_SIZE(defs), \
                      val)

#define HD_ENUM_ITERATE(defs, type, i)                                                                                          \
    for ((i) = (type)HD_ENUM_ELEMENT_INT(HD_ENUM_GET_MASK(defs), HD_ENUM_GET_MASK_SIZE(defs), HD_ENUM_GET_INDEX_SIZE(defs), 0); \
         HD_ENUM_VAL_INT(HD_ENUM_GET_INDEX_SIZE(defs), (i)) < (HD_ENUM_MIRROR_VAL(HD_ENUM_GET_NUM_OF_ELEMENTS(defs)));          \
         (i) = (type)HD_ENUM_ELEMENT_INT(HD_ENUM_GET_MASK(defs),                                                                \
                                         HD_ENUM_GET_MASK_SIZE(defs),                                                           \
                                         HD_ENUM_GET_INDEX_SIZE(defs),                                                          \
                                         (HD_ENUM_VAL_INT(HD_ENUM_GET_INDEX_SIZE(defs), (i)) + 1)))

#define HD_ENUM_ELEMENT_INT(mask, bitsize, index_size, index)                                                            \
    (((((((((~(mask)) * ((index) % 2)) | (((mask)) * ((uint64_t)1 - ((index) % 2)))) & (((uint64_t)1 << (bitsize)) - 1)) \
         << ((bitsize) * (((index) / 2)))))                                                                              \
       << (index_size)) &                                                                                                \
      0x7FFFFFFF) |                                                                                                      \
     ((index) & (((uint64_t)1 << (index_size)) - 1)))
#define HD_ENUM_GET_MASK(mask, mask_size, index_size, num)            (mask)
#define HD_ENUM_GET_MASK_SIZE(mask, mask_size, index_size, num)       (mask_size)
#define HD_ENUM_GET_INDEX_SIZE(mask, mask_size, index_size, num)      (index_size)
#define HD_ENUM_GET_NUM_OF_ELEMENTS(mask, mask_size, index_size, num) (num)
#define HD_ENUM_MIRROR_VAL(x)                                         (x)

#define HD_ENUM_VAL_INT(index_size, val) ((val) & ((1 << (index_size)) - 1))
#define HD_ENUM_VALID_INT(mask, mask_size, index_size, val) \
    ((uint32_t)(val) == (uint32_t)HD_ENUM_ELEMENT_INT(mask, mask_size, index_size, HD_ENUM_VAL_INT(index_size, val)))

/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
/*                                             Secure Boolean                                              */
/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
#define SEC_BOOLEAN_IS_TRUE(b)  ((b) == TRUE)
#define SEC_BOOLEAN_IS_FALSE(b) ((b) == FALSE)
#define SEC_BOOLEAN_IN_RANGE(b) ((SEC_BOOLEAN_IS_TRUE(b) || SEC_BOOLEAN_IS_FALSE(b)))
#define SEC_BOOLEAN_TO_INT(b)   ((SEC_BOOLEAN_IS_TRUE(b) ? 1u : (SEC_BOOLEAN_IS_FALSE(b) ? 0u : BAD_VAL)))
#define SEC_BOOLEAN_FROM_INT(i) (((i)) != 0u ? TRUE : FALSE)

#define SEC_BOOLEAN_IF(b, trueCode, falseCode)  \
    {                                           \
        if (SEC_BOOLEAN_IS_TRUE(b))             \
        {                                       \
            trueCode;                           \
        }                                       \
        else if (SEC_BOOLEAN_IS_FALSE(b))       \
        {                                       \
            falseCode;                          \
        }                                       \
        else                                    \
        {                                       \
            return DEFS_STATUS_CORRUPTED_VALUE; \
        }                                       \
    }

/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
/*                                             Overflow checks                                             */
/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
#define SAFE_ADDR_SIZE(addr, size, max) ((addr) <= (((max) - (size)) + 1u))

/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
/*                                      Control Flow Integrity (CFI)                                       */
/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/

#ifndef CFI_DISABLED

/*---------------------------------------------------------------------------------------------------------*/
/* Default CFI Fail action                                                                                 */
/*---------------------------------------------------------------------------------------------------------*/
#ifndef CFI_FAIL_ACTION
#define CFI_FAIL_ACTION() return DEFS_STATUS_SECURITY_ERROR
#endif

/*---------------------------------------------------------------------------------------------------------*/
/* Macro:           CFI_DECLARE                                                                            */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  cfi     - CFI variable name                                                            */
/*                  val     - CFI initialization value                                                     */
/*                                                                                                         */
/* Returns:                                                                                                */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This macro declares new CFI variable with given initial value                          */
/*---------------------------------------------------------------------------------------------------------*/
#define CFI_DECLARE(cfi, val) volatile DEFS_STATUS cfi = (DEFS_STATUS)(DEFS_STATUS_OK_MASK + (val))

/*---------------------------------------------------------------------------------------------------------*/
/* Macro:           CFI_INIT                                                                               */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  cfi     - CFI variable name                                                            */
/*                  val     - CFI initialization value                                                     */
/*                                                                                                         */
/* Returns:                                                                                                */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This macro initialize the CFI variable with given value                                */
/*---------------------------------------------------------------------------------------------------------*/
#define CFI_INIT(cfi, val) ((cfi) = (DEFS_STATUS)(DEFS_STATUS_OK_MASK + (val)))

/*---------------------------------------------------------------------------------------------------------*/
/* Macro:           CFI_CHECK                                                                              */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  cfi         - CFI variable name                                                        */
/*                  check_val   - Expected CFI variable value                                              */
/*                                                                                                         */
/* Returns:                                                                                                */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This macro checks the value in the CFI variable                                        */
/*---------------------------------------------------------------------------------------------------------*/
#define CFI_CHECK(cfi, check_val)                                      \
    {                                                                  \
        if ((cfi) != (DEFS_STATUS)(DEFS_STATUS_OK_MASK + (check_val))) \
        {                                                              \
            CFI_FAIL_ACTION();                                         \
        }                                                              \
    }

/*---------------------------------------------------------------------------------------------------------*/
/* Macro:           CFI_DEC                                                                                */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  cfi         - CFI variable name                                                        */
/*                                                                                                         */
/* Returns:                                                                                                */
/* Side effects:    none                                                                                   */
/* Description:                                                                                            */
/*                  This routine decrements the CFI value                                                  */
/*---------------------------------------------------------------------------------------------------------*/
#define CFI_DEC_VAL(cfi, val) ((cfi) = ((DEFS_STATUS)(((uint32_t)(cfi)) - (val))))

/*---------------------------------------------------------------------------------------------------------*/
/* Macro:           CFI_DEC                                                                                */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  cfi         - CFI variable name                                                        */
/*                                                                                                         */
/* Returns:                                                                                                */
/* Side effects:    none                                                                                   */
/* Description:                                                                                            */
/*                  This routine decrements the CFI value                                                  */
/*---------------------------------------------------------------------------------------------------------*/
#define CFI_DEC(cfi) CFI_DEC_VAL(cfi, 1u)

/*---------------------------------------------------------------------------------------------------------*/
/* Macro:           CFI_CHECK_DEC                                                                          */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  cfi         - CFI variable name                                                        */
/*                  check_val   - Expected CFI variable value before decrement                             */
/*                                                                                                         */
/* Returns:                                                                                                */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This macro checks the value in the CFI variable and decrements it by 1                 */
/*---------------------------------------------------------------------------------------------------------*/
#define CFI_CHECK_DEC(cfi, check_val) \
    {                                 \
        CFI_CHECK(cfi, check_val);    \
        CFI_DEC(cfi);                 \
    }

/*---------------------------------------------------------------------------------------------------------*/
/* Macro:           CFI_GET_VALUE                                                                          */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  cfi         - CFI variable name                                                        */
/*                                                                                                         */
/* Returns:                                                                                                */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This macro returns the CFI value                                                       */
/*---------------------------------------------------------------------------------------------------------*/
#define CFI_GET_VALUE(cfi) (cfi)

#else
/*---------------------------------------------------------------------------------------------------------*/
/*                                                CFI stubs                                                */
/*---------------------------------------------------------------------------------------------------------*/
#define CFI_DECLARE(cfi, val)
#define CFI_INIT(cfi, val)
#define CFI_CHECK(cfi, check_val)
#define CFI_DEC(cfi)
#define CFI_DEC_VAL(cfi, val)
#define CFI_CHECK_DEC(cfi, check_val)
#define CFI_GET_VALUE(cfi) (DEFS_STATUS_OK)
#endif

/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
/*                                         Secure Register Access                                          */
/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
#define SEC_REG_READ(reg)                                                           \
    (REG_READ_AUX_(GET_REG_ADDR(reg), GET_REG_ACCESSTYPE(reg), GET_REG_SIZE(reg)) | \
     REG_READ_AUX_(GET_REG_ADDR(reg), GET_REG_ACCESSTYPE(reg), GET_REG_SIZE(reg)))

#define SEC_REG_WRITE(reg, val)                                                                                       \
    {                                                                                                                 \
        REG_WRITE_AUX_((val), GET_REG_ADDR(reg), GET_REG_ACCESSTYPE(reg), GET_REG_SIZE(reg));                         \
        DEFS_STATUS_COND_CHECK(REG_READ_AUX_(GET_REG_ADDR(reg), GET_REG_ACCESSTYPE(reg), GET_REG_SIZE(reg)) == (val), \
                               DEFS_STATUS_SECURITY_ERROR);                                                           \
    }

#endif // DEFS_SECURE_H__
