/************************************************************************************************************
* @internal
* @remark     Winbond Electronics Corporation - Confidential
* @copyright  Copyright (c) 2026 by Winbond Electronics Corporation . All rights reserved
* @endinternal
*
* @file       qlib_utils_cmkdf_gen_flash_key.c
* @brief      This file contains functions for generating flash keys using the NIST SP 800-108 counter-mode KDF
*
* ### project qlib
*
************************************************************************************************************/

/*-----------------------------------------------------------------------------------------------------------
                                                   INCLUDES
-----------------------------------------------------------------------------------------------------------*/

#include <string.h>
#include "qlib_utils_cmkdf_gen_flash_key.h"
#include "qlib_utils_cmkdf.h"

/*-----------------------------------------------------------------------------------------------------------
                                                   DEFINITIONS
-----------------------------------------------------------------------------------------------------------*/
// KDF key derivation constants. These are common to all customers.
#ifdef QLIB_DEPRECATED_KDF_GEN
#define QLIB_UTILS_CMKDF_CTX_PREFIX "Winbond Secure Flash "
#else
#define QLIB_UTILS_CMKDF_CTX_PREFIX "Winbond Secure Flash"
#endif // QLIB_DEPRECATED_KDF_GEN
#define QLIB_UTILS_CMKDF_CTX_PREFIX_LEN ((uint32_t)(sizeof(QLIB_UTILS_CMKDF_CTX_PREFIX) - 1u))
#ifdef QLIB_DEPRECATED_KDF_GEN
#define QLIB_UTILS_CMKDF_COUNTER_BASE 0u
#define QLIB_UTILS_CMKDF_LENGTH_BYTES 16u
#else
#define QLIB_UTILS_CMKDF_COUNTER_BASE 1u
#define QLIB_UTILS_CMKDF_LENGTH_BYTES QLIB_UTILS_HMAC_SHA256_DIGEST_SIZE_BYTES
#endif // QLIB_DEPRECATED_KDF_GEN

/*-----------------------------------------------------------------------------------------------------------
                                             INTERFACE FUNCTIONS
-----------------------------------------------------------------------------------------------------------*/
int QLIB_UTILS_CMKDF_KeyDerive(const uint8_t secret_au8[QLIB_UTILS_HMAC_SHA256_DIGEST_SIZE_BYTES],
                               const uint8_t wid_au8[CALC_KEY_WID_SIZE_BYTES],
                               const char*   label_pc,
                               uint32_t      keyIdx_u32,
                               uint32_t      totalKeys_u32,
                               uint8_t*      key_pu8,
                               uint32_t      keySize_u32)
{
    static const char        contextPrefix_ac[] = QLIB_UTILS_CMKDF_CTX_PREFIX;
    uint8_t                  fullContext_au8[QLIB_UTILS_CMKDF_CTX_PREFIX_LEN + CALC_KEY_WID_SIZE_BYTES];
    uint32_t                 fullContextSize_u32;
    QLIB_UTILS_CMKDF_INPUT_T cmkdfInput_s;
    size_t                   labelLen_sz;

    /*-----------------------------------------------------------------------------------------------------*/
    /* Error checking                                                                                      */
    /*-----------------------------------------------------------------------------------------------------*/
    if ((NULL == secret_au8) || (NULL == wid_au8) || (NULL == label_pc) || (NULL == key_pu8) || (0u == keySize_u32) ||
        (0u == totalKeys_u32))
    {
        return -1;
    }
    // Bound labelLen_sz to the KDF's real capacity (label + fixed-size context) right after strlen
    labelLen_sz = strlen(label_pc);
    if ((labelLen_sz == 0u) || (labelLen_sz > (size_t)(QLIB_UTILS_CMKDF_MAX_LABEL_SIZE_BYTES)))
    {
        return -1;
    }
    if (keyIdx_u32 > (UINT32_MAX - QLIB_UTILS_CMKDF_COUNTER_BASE))
    {
        return -1;
    }
    if (totalKeys_u32 > (UINT32_MAX / (8u * QLIB_UTILS_CMKDF_LENGTH_BYTES)))
    {
        return -1;
    }

    /*-----------------------------------------------------------------------------------------------------*/
    /* Build context = context prefix || WID                                                              */
    /*-----------------------------------------------------------------------------------------------------*/
    (void)memcpy((void*)fullContext_au8, (const void*)contextPrefix_ac, QLIB_UTILS_CMKDF_CTX_PREFIX_LEN);
    (void)memcpy((void*)&fullContext_au8[QLIB_UTILS_CMKDF_CTX_PREFIX_LEN], (const void*)wid_au8, CALC_KEY_WID_SIZE_BYTES);
    fullContextSize_u32 = QLIB_UTILS_CMKDF_CTX_PREFIX_LEN + CALC_KEY_WID_SIZE_BYTES;

    /*-----------------------------------------------------------------------------------------------------*/
    /* Derive KD1                                                                                          */
    /*-----------------------------------------------------------------------------------------------------*/
    cmkdfInput_s.kdk_pv          = secret_au8;
    cmkdfInput_s.kdkSize_u32     = QLIB_UTILS_HMAC_SHA256_DIGEST_SIZE_BYTES;
    cmkdfInput_s.counter_u32     = keyIdx_u32 + QLIB_UTILS_CMKDF_COUNTER_BASE;
    cmkdfInput_s.label           = label_pc;
    cmkdfInput_s.labelSize_u32   = (uint32_t)labelLen_sz;
    cmkdfInput_s.context         = (const char*)fullContext_au8;
    cmkdfInput_s.contextSize_u32 = fullContextSize_u32;
    cmkdfInput_s.lengthBits_u32 =
        totalKeys_u32 * 8u * (QLIB_UTILS_CMKDF_LENGTH_BYTES); // Total requested KDF output length in bits

    if (QLIB_UTILS_HmacSha256CmKdf(&cmkdfInput_s, key_pu8, keySize_u32) != 0)
    {
        return -1;
    }

    return 0;
}
