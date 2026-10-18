/************************************************************************************************************
* @internal
* @remark     Winbond Electronics Corporation - Confidential
* @copyright  Copyright (c) 2026 by Winbond Electronics Corporation . All rights reserved
* @endinternal
*
* @file       qlib_utils_cmkdf_gen_flash_key.h
* @brief      This file contains function definitions for generating flash keys using the NIST SP 800-108 counter-mode KDF
*
* ### project qlib
*
************************************************************************************************************/
#ifndef QLIB_UTILS_CMKDF_GEN_FLASH_KEY_H__
#define QLIB_UTILS_CMKDF_GEN_FLASH_KEY_H__

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

/*-----------------------------------------------------------------------------------------------------------
                                                   INCLUDES
-----------------------------------------------------------------------------------------------------------*/
#include "qlib_utils_cmkdf.h"

/*-----------------------------------------------------------------------------------------------------------
                                                   DEFINITIONS
-----------------------------------------------------------------------------------------------------------*/

#define CALC_KEY_WID_SIZE_BYTES 8u
#ifdef QLIB_DEPRECATED_KDF_GEN
#define QLIB_UTILS_CMKDF_LABEL_KD1 "Preprovision Device Master Key"
#else
#define QLIB_UTILS_CMKDF_LABEL_KD1 "KD1-Preprovision Device Master Key"
#endif // QLIB_DEPRECATED_KDF_GEN
#define QLIB_UTILS_CMKDF_LABEL_KD "KD-Device Master Key"
#define QLIB_UTILS_CMKDF_LABEL_KS "KS-Device Secret Key"
#define QLIB_UTILS_CMKDF_LABEL_FK "FK-Full Access Section Key"
#define QLIB_UTILS_CMKDF_LABEL_RK "RK-Restricted Access Section Key"

/*-----------------------------------------------------------------------------------------------------------
                                             INTERFACE FUNCTIONS
-----------------------------------------------------------------------------------------------------------*/

/************************************************************************************************************
 * @brief       This function derives the key for a given device Wafer ID (WID), using the
 *              customer secret and the NIST SP 800-108 HMAC-SHA256 counter-mode KDF
 *
 * @param[in]   secret_au8    Customer Secret, the KDF key
 * @param[in]   wid_au8       Device Wafer ID, CALC_KEY_WID_SIZE_BYTES bytes long
 * @param[in]   label_pc      Label for the key derivation
 * @param[in]   keyIdx_u32    Index of the key to derive
 * @param[in]   totalKeys_u32 Total number of keys to derive
 * @param[out]  key_pu8       Resulting key
 * @param[in]   keySize_u32   Size of the output key buffer in bytes
 *
 * @return 0 on success, other value for failure
************************************************************************************************************/
int QLIB_UTILS_CMKDF_KeyDerive(const uint8_t secret_au8[QLIB_UTILS_HMAC_SHA256_DIGEST_SIZE_BYTES],
                               const uint8_t wid_au8[CALC_KEY_WID_SIZE_BYTES],
                               const char*   label_pc,
                               uint32_t      keyIdx_u32,
                               uint32_t      totalKeys_u32,
                               uint8_t*      key_pu8,
                               uint32_t      keySize_u32);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // QLIB_UTILS_CMKDF_GEN_FLASH_KEY_H__
