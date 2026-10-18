/************************************************************************************************************
* @internal
* @remark     Winbond Electronics Corporation - Confidential
* @copyright  Copyright (c) 2026 by Winbond Electronics Corporation . All rights reserved
* @endinternal
*
* @file       qlib_utils_kd1.h
* @brief      This file contains the KD1 device master key derivation function definitions
*
* ### project calc_kd1
*
************************************************************************************************************/
#ifndef QLIB_UTILS_KD1_H__
#define QLIB_UTILS_KD1_H__

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

/*-----------------------------------------------------------------------------------------------------------
                                                   INCLUDES
-----------------------------------------------------------------------------------------------------------*/
#include "defs.h"
#include "qlib_utils_cmkdf.h"

/*-----------------------------------------------------------------------------------------------------------
                                                   DEFINITIONS
-----------------------------------------------------------------------------------------------------------*/

#define CALC_KD1_WID_SIZE_BYTES 8u
#define CALC_KD1_SIZE_BYTES     16u ///< KD1 is truncated to the first 16 bytes (128 bits) of the HMAC-SHA256 KDF output

/*-----------------------------------------------------------------------------------------------------------
                                             INTERFACE FUNCTIONS
-----------------------------------------------------------------------------------------------------------*/

/************************************************************************************************************
 * @brief       This function derives the KD1 device master key for a given device Wafer ID (WID), using the
 *              customer secret and the NIST SP 800-108 HMAC-SHA256 counter-mode KDF
 *
 * @param[in]   cms_au8    Customer Master Secret, the KDF key
 * @param[in]   wid_au8    Device Wafer ID, CALC_KD1_WID_SIZE_BYTES bytes long
 * @param[out]  kd1_au8    Resulting KD1 device master key, the first CALC_KD1_SIZE_BYTES bytes of the
 *                         HMAC-SHA256 KDF output
 *
 * @return
 * DEFS_STATUS_OK = 0              - no error occurred\n
 * DEFS_STATUS_INVALID_PARAMETER   - @p cms_au8, @p wid_au8 or @p kd1_au8 is NULL\n
 * DEFS_STATUS_COMMAND_FAILED        - KDF calculation failed
************************************************************************************************************/
DEFS_STATUS QLIB_UTILS_KD1_Derive(const uint8_t cms_au8[QLIB_UTILS_HMAC_SHA256_DIGEST_SIZE_BYTES],
                                  const uint8_t wid_au8[CALC_KD1_WID_SIZE_BYTES],
                                  uint8_t       kd1_au8[CALC_KD1_SIZE_BYTES]);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // QLIB_UTILS_KD1_H__
