/************************************************************************************************************
* @internal
* @remark     Winbond Electronics Corporation - Confidential
* @copyright  Copyright (c) 2023 by Winbond Electronics Corporation . All rights reserved
* @endinternal
*
* @file       qlib_utils_lms.h
* @brief      This file contains LMS utility functions
*
* ### project qlib
*
************************************************************************************************************/
#ifndef QLIB_UTILS_LMS_H__
#define QLIB_UTILS_LMS_H__

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

/*-----------------------------------------------------------------------------------------------------------
                                                   INCLUDES
-----------------------------------------------------------------------------------------------------------*/
#include "qlib.h"
#include "qlib_utils_digest.h"

#ifndef EXCLUDE_LMS_ATTESTATION
/*-----------------------------------------------------------------------------------------------------------
                                             INTERFACE FUNCTIONS
-----------------------------------------------------------------------------------------------------------*/

/************************************************************************************************************
 * @brief This function computes hash-chain
 *
 * @param[in]      input        Input buffer for 1st iteration
 * @param[in]      keyId        key ID
 * @param[in]      q_u32        Leaf index
 * @param[in]      chainIndex_u16   Chain index
 * @param[in]      iterStart_u8    Iteration start index
 * @param[in]      iter_u8         Num of iterations
 * @param[out]     output       Hash chain output
 *
 * @return
 * QLIB_STATUS__OK = 0                 - no error occurred\n
 * QLIB_STATUS__(ERROR)                - Other error
************************************************************************************************************/
QLIB_STATUS_T QLIB_LMS_Attest_HashChain(const LMS_OTS_ATTEST_CHUNK_T input,
                                        const QLIB_LMS_KEY_ID_T      keyId,
                                        uint32_t                     q_u32,
                                        uint16_t                     chainIndex_u16,
                                        uint8_t                      iterStart_u8,
                                        uint8_t                      iter_u8,
                                        LMS_OTS_ATTEST_CHUNK_T       output);

#endif // EXCLUDE_LMS_ATTESTATION

#ifdef __cplusplus
}
#endif // __cplusplus
#endif // QLIB_UTILS_LMS_H__
