/************************************************************************************************************
* @internal
* @remark     Winbond Electronics Corporation - Confidential
* @copyright  Copyright (c) 2021 by Winbond Electronics Corporation . All rights reserved
* @endinternal
*
* @file       qlib_utils_digest.h
* @brief      This file contains digest utility functions functions
*
* ### project qlib
*
************************************************************************************************************/
#ifndef QLIB_UTILS_DIGEST_H__
#define QLIB_UTILS_DIGEST_H__

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

/*-----------------------------------------------------------------------------------------------------------
                                                   INCLUDES
-----------------------------------------------------------------------------------------------------------*/
#include "qlib.h"

/*-----------------------------------------------------------------------------------------------------------
                                                   DEFINITIONS
-----------------------------------------------------------------------------------------------------------*/
#define DIGEST_EXT_SIZE 24u

/*-----------------------------------------------------------------------------------------------------------
                                                   TYPES
-----------------------------------------------------------------------------------------------------------*/

typedef uint8_t DIGEST_EXT_T[DIGEST_EXT_SIZE];

/*-----------------------------------------------------------------------------------------------------------
                                             INTERFACE FUNCTIONS
-----------------------------------------------------------------------------------------------------------*/

/************************************************************************************************************
 * @brief       This function calculates the digest of a given data
 *
 * @param[in]   buf_pu32      Section data buffer
 * @param[in]   size_u32      Section data buffer size
 * @param[out]  digest_pu64   digest value
 *
 * @return
 * QLIB_STATUS__OK = 0              - no error occurred\n
 * QLIB_STATUS__INVALID_PARAMETER   - @p buf_pv or @p digest is NULL\n
 * QLIB_STATUS__INVALID_PARAMETER   - @p is 0\n
 * QLIB_STATUS__(ERROR)             - Other error
************************************************************************************************************/
QLIB_STATUS_T QLIB_UTILS_CalcDigest(uint32_t* buf_pu32, uint32_t size_u32, uint64_t* digest_pu64);

/************************************************************************************************************
 * @brief       This function calculates the digest of a given data
 *
 * @param[in]   buf_pv        data buffer
 * @param[in]   size_u32      data buffer size
 * @param[in]   padValue_u8   padding after data value
 * @param[in]   padSize_u32   padding after data size
 * @param[out]  digest_pu64   digest value
 * @param[out]  digestExt_pu8 if not NULL, the function also returns the 192 LS-bits of the digest
 *
 * @return
 * QLIB_STATUS__OK = 0              - no error occurred\n
 * QLIB_STATUS__INVALID_PARAMETER   - @p buf_pv or @p digest is NULL\n
 * QLIB_STATUS__INVALID_PARAMETER   - @p is 0\n
 * QLIB_STATUS__(ERROR)             - Other error
************************************************************************************************************/
QLIB_STATUS_T QLIB_UTILS_CalcDigestWithPadding(const void*  buf_pv,
                                               uint32_t     size_u32,
                                               uint8_t      padValue_u8,
                                               uint32_t     padSize_u32,
                                               uint64_t*    digest_pu64,
                                               DIGEST_EXT_T digestExt_pu8);
#ifdef __cplusplus
}
#endif // __cplusplus

#endif // QLIB_UTILS_DIGEST_H__
