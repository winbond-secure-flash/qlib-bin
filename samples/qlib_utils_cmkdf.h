/************************************************************************************************************
* @internal
* @remark     Winbond Electronics Corporation - Confidential
* @copyright  Copyright (c) 2026 by Winbond Electronics Corporation . All rights reserved
* @endinternal
*
* @file       qlib_utils_cmkdf.h
* @brief      This file contains a generic NIST SP 800-108 HMAC-SHA256 counter-mode KDF utility function
*
* ### project qlib
*
************************************************************************************************************/
#ifndef QLIB_UTILS_CMKDF_H__
#define QLIB_UTILS_CMKDF_H__

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

/*-----------------------------------------------------------------------------------------------------------
                                                   INCLUDES
-----------------------------------------------------------------------------------------------------------*/
#include <stdint.h>

/*-----------------------------------------------------------------------------------------------------------
                                                   DEFINITIONS
-----------------------------------------------------------------------------------------------------------*/
#define QLIB_UTILS_HMAC_SHA256_DIGEST_SIZE_BYTES 32u
#define QLIB_UTILS_CMKDF_MAX_LABEL_SIZE_BYTES    50u
#define QLIB_UTILS_CMKDF_MAX_CONTEXT_SIZE_BYTES  50u

/*-----------------------------------------------------------------------------------------------------------
                                                   TYPES
-----------------------------------------------------------------------------------------------------------*/

// Input parameters for QLIB_UTILS_HmacSha256CmKdf. Grouped in a struct so new fields can be added later
// without breaking existing call sites. Output (derived key) is passed separately, see below.
typedef struct QLIB_UTILS_CMKDF_INPUT_T
{
    const void* kdk_pv;          ///< Key Derivation Key
    uint32_t    kdkSize_u32;     ///< Key Derivation Key size in bytes
    uint32_t    counter_u32;     ///< 32-bit KDF iteration counter
    const char* label;           ///< Label buffer (max size QLIB_UTILS_CMKDF_MAX_LABEL_SIZE_BYTES)
    uint32_t    labelSize_u32;   ///< Label size in bytes
    const char* context;         ///< Context buffer (max size QLIB_UTILS_CMKDF_MAX_CONTEXT_SIZE_BYTES)
    uint32_t    contextSize_u32; ///< Context size in bytes
    uint32_t    lengthBits_u32;  ///< KDF "L" field: total requested KDF output length in bits
} QLIB_UTILS_CMKDF_INPUT_T;

/*-----------------------------------------------------------------------------------------------------------
                                             INTERFACE FUNCTIONS
-----------------------------------------------------------------------------------------------------------*/

/************************************************************************************************************
 * @brief       This function derives a key using the NIST SP 800-108 HMAC-SHA256 counter-mode KDF, single
 *              block expansion:  Buffer = counter || Label || 0x00 || Context || L ; key = HMAC(kdk, Buffer)
 *
 * @param[in]   input_ps        KDF input parameters, see @ref QLIB_UTILS_CMKDF_INPUT_T
 * @param[out]  key_pv          Derived key output
 * @param[in]   keySize_u32     Derived key size in bytes, must be in range (0, QLIB_UTILS_HMAC_SHA256_DIGEST_SIZE_BYTES]
 *
 * @return 0 on success, non-zero on failure
************************************************************************************************************/
int QLIB_UTILS_HmacSha256CmKdf(const QLIB_UTILS_CMKDF_INPUT_T* input_ps, void* key_pv, uint32_t keySize_u32);


#ifdef __cplusplus
}
#endif // __cplusplus

#endif // QLIB_UTILS_CMKDF_H__
