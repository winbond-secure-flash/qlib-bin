/************************************************************************************************************
* @internal
* @remark     Winbond Electronics Corporation - Confidential
* @copyright  Copyright (c) 2021 by Winbond Electronics Corporation . All rights reserved
* @endinternal
*
* @file       qlib_utils_digest.c
* @brief      This file contains digest utility functions
*
* ### project qlib
*
************************************************************************************************************/

/*-----------------------------------------------------------------------------------------------------------
                                                   INCLUDES
-----------------------------------------------------------------------------------------------------------*/
#include "qlib_utils_digest.h"

/*-----------------------------------------------------------------------------------------------------------
                                             INTERFACE FUNCTIONS
-----------------------------------------------------------------------------------------------------------*/

QLIB_STATUS_T QLIB_UTILS_CalcDigest(uint32_t* buf_pu32, uint32_t size_u32, uint64_t* digest_pu64)
{
    TYPE_256BIT hash_result = {0u};

    /*-----------------------------------------------------------------------------------------------------*/
    /* Error checking                                                                                      */
    /*-----------------------------------------------------------------------------------------------------*/
    if (NULL == digest_pu64)
    {
        return QLIB_STATUS__INVALID_PARAMETER;
    }

    if (NULL == buf_pu32 || 0u == size_u32)
    {
        *digest_pu64 = 0u;
        return QLIB_STATUS__INVALID_PARAMETER;
    }

    /*-----------------------------------------------------------------------------------------------------*/
    /* Calculate digest                                                                                    */
    /*-----------------------------------------------------------------------------------------------------*/
    void* hashCtx_pv;
    QLIB_ASSERT_RET(PLAT_HASH_Init(&hashCtx_pv, QLIB_HASH_OPT_NONE) == 0, QLIB_STATUS__HARDWARE_FAILURE);
    QLIB_ASSERT_RET(PLAT_HASH_Update(hashCtx_pv, buf_pu32, size_u32) == 0, QLIB_STATUS__HARDWARE_FAILURE);
    QLIB_ASSERT_RET(PLAT_HASH_Finish(hashCtx_pv, hash_result) == 0, QLIB_STATUS__HARDWARE_FAILURE);

    /*-----------------------------------------------------------------------------------------------------*/
    /* Set output                                                                                          */
    /*-----------------------------------------------------------------------------------------------------*/
    (void)memcpy((void*)digest_pu64, (void*)&hash_result[6], sizeof(uint64_t));

    return QLIB_STATUS__OK;
}

QLIB_STATUS_T QLIB_UTILS_CalcDigestWithPadding(const void*  buf_pv,
                                               uint32_t     size_u32,
                                               uint8_t      padValue_u8,
                                               uint32_t     padSize_u32,
                                               uint64_t*    digest_pu64,
                                               DIGEST_EXT_T digestExt_pu8)
{
    TYPE_256BIT hash_result = {0u};
    void*       ctx_pv      = NULL;
    uint8_t     padBuf_u8[16];

    /*-----------------------------------------------------------------------------------------------------*/
    /* Error checking                                                                                      */
    /*-----------------------------------------------------------------------------------------------------*/
    if (NULL == digest_pu64)
    {
        return QLIB_STATUS__INVALID_PARAMETER;
    }

    if (NULL == buf_pv || 0u == size_u32)
    {
        *digest_pu64 = 0u;
        return QLIB_STATUS__INVALID_PARAMETER;
    }
    (void)memset((void*)padBuf_u8, (int)padValue_u8, sizeof(padBuf_u8));
    /*-----------------------------------------------------------------------------------------------------*/
    /* Calculate digest                                                                                    */
    /*-----------------------------------------------------------------------------------------------------*/
    QLIB_ASSERT_RET(PLAT_HASH_Init(&ctx_pv, QLIB_HASH_OPT_NONE) == 0, QLIB_STATUS__HARDWARE_FAILURE);
    QLIB_ASSERT_RET(PLAT_HASH_Update(ctx_pv, buf_pv, size_u32) == 0, QLIB_STATUS__HARDWARE_FAILURE);

    while (padSize_u32 > 0u)
    {
        uint32_t padBytes_u32 = MIN(padSize_u32, (uint32_t)sizeof(padBuf_u8));
        QLIB_ASSERT_RET(PLAT_HASH_Update(ctx_pv, padBuf_u8, padBytes_u32) == 0, QLIB_STATUS__HARDWARE_FAILURE);
        padSize_u32 -= padBytes_u32;
    }

    QLIB_ASSERT_RET(PLAT_HASH_Finish(ctx_pv, hash_result) == 0, QLIB_STATUS__HARDWARE_FAILURE);

    /*-----------------------------------------------------------------------------------------------------*/
    /* Set output                                                                                          */
    /*-----------------------------------------------------------------------------------------------------*/
    (void)memcpy((void*)digest_pu64, (void*)&hash_result[6], sizeof(uint64_t));
    if (digestExt_pu8 != NULL)
    {
        (void)memcpy((void*)digestExt_pu8, (void*)hash_result, sizeof(DIGEST_EXT_T));
    }

    return QLIB_STATUS__OK;
}

