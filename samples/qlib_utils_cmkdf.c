/************************************************************************************************************
* @internal
* @remark     Winbond Electronics Corporation - Confidential
* @copyright  Copyright (c) 2026 by Winbond Electronics Corporation . All rights reserved
* @endinternal
*
* @file       qlib_utils_cmkdf.c
* @brief      This file contains a generic NIST SP 800-108 HMAC-SHA256 counter-mode KDF utility function
*
* ### project qlib
*
************************************************************************************************************/

/*-----------------------------------------------------------------------------------------------------------
                                                   INCLUDES
-----------------------------------------------------------------------------------------------------------*/
#include "qlib_utils_cmkdf.h"

#include <string.h>

// It is user responsibility to provide an implementation of HMAC-SHA256 as defined in common_platform_hmac_sha256.h
#include "common_platform_hmac_sha256.h"

/*-----------------------------------------------------------------------------------------------------------
                                                   DEFINITIONS
-----------------------------------------------------------------------------------------------------------*/
// Buffer overhead of constant size: counter (4B) + delimiter (1B) + L field (4B)
#define QLIB_UTILS_CMKDF_BUFFER_OVERHEAD_SIZE_BYTES ((uint32_t)sizeof(uint32_t) + 1u + (uint32_t)sizeof(uint32_t))

#define QLIB_UTILS_CMKDF_INPUT_BUFFER_SIZE_BYTES                                           \
    (QLIB_UTILS_CMKDF_MAX_LABEL_SIZE_BYTES + QLIB_UTILS_CMKDF_BUFFER_OVERHEAD_SIZE_BYTES + \
     QLIB_UTILS_CMKDF_MAX_CONTEXT_SIZE_BYTES)
#define QLIB_UTILS_CMKDF_DELIMITER_BYTE 0x00u

#ifndef SET_U32_IN_NETWORK_BUF
#define SET_U32_IN_NETWORK_BUF(val, buf)              \
    {                                                 \
        (buf)[0] = (uint8_t)(((val) >> 24u) & 0xFFu); \
        (buf)[1] = (uint8_t)(((val) >> 16u) & 0xFFu); \
        (buf)[2] = (uint8_t)(((val) >> 8u) & 0xFFu);  \
        (buf)[3] = (uint8_t)((val)&0xFFu);            \
    }
#endif // SET_U32_IN_NETWORK_BUF

/*-----------------------------------------------------------------------------------------------------------
                                          LOCAL FUNCTION PROTOTYPES
-----------------------------------------------------------------------------------------------------------*/
static uint32_t QLIB_UTILS_BuildCmKdfInput_L(uint8_t*    outBuf_pu8,
                                             uint32_t    bufSize_u32,
                                             uint32_t    counter_u32,
                                             const char* label,
                                             uint32_t    labelSize_u32,
                                             const char* context,
                                             uint32_t    contextSize_u32,
                                             uint32_t    lengthBits_u32);

/*-----------------------------------------------------------------------------------------------------------
                                             INTERFACE FUNCTIONS
-----------------------------------------------------------------------------------------------------------*/
int QLIB_UTILS_HmacSha256CmKdf(const QLIB_UTILS_CMKDF_INPUT_T* input_ps, void* key_pv, uint32_t keySize_u32)
{
    uint8_t  digest_au8[QLIB_UTILS_HMAC_SHA256_DIGEST_SIZE_BYTES];
    uint8_t  kdfBuf_au8[QLIB_UTILS_CMKDF_INPUT_BUFFER_SIZE_BYTES];
    uint32_t written_u32;

    /*-----------------------------------------------------------------------------------------------------*/
    /* Error checking                                                                                      */
    /*-----------------------------------------------------------------------------------------------------*/
    if ((NULL == input_ps) || (NULL == input_ps->kdk_pv) || (NULL == input_ps->label) || (NULL == input_ps->context) ||
        (NULL == key_pv) || (0u == keySize_u32) || (keySize_u32 > (uint32_t)sizeof(digest_au8)))
    {
        return -1;
    }

    /*-----------------------------------------------------------------------------------------------------*/
    /* Build KDF input and calculate HMAC-SHA256                                                          */
    /*-----------------------------------------------------------------------------------------------------*/
    written_u32 = QLIB_UTILS_BuildCmKdfInput_L(kdfBuf_au8,
                                               (uint32_t)sizeof(kdfBuf_au8),
                                               input_ps->counter_u32,
                                               input_ps->label,
                                               input_ps->labelSize_u32,
                                               input_ps->context,
                                               input_ps->contextSize_u32,
                                               input_ps->lengthBits_u32);
    if (0u == written_u32)
    {
        return -1;
    }

    PLAT_HMAC_SHA256(digest_au8, (const uint8_t*)input_ps->kdk_pv, input_ps->kdkSize_u32, kdfBuf_au8, written_u32);

    /*-----------------------------------------------------------------------------------------------------*/
    /* Set output                                                                                          */
    /*-----------------------------------------------------------------------------------------------------*/
    (void)memcpy((void*)key_pv, (const void*)digest_au8, keySize_u32);
    (void)memset((void*)digest_au8, 0, sizeof(digest_au8));

    return 0;
}

/*-----------------------------------------------------------------------------------------------------------
                                             LOCAL FUNCTIONS
-----------------------------------------------------------------------------------------------------------*/

/************************************************************************************************************
 * @internal
 * @brief       This function builds a NIST SP 800-108 counter-mode KDF input buffer:
 *              Buffer = counter || Label || 0x00 || Context || L
 *
 * @param[out]  outBuf_pu8      Output buffer
 * @param[in]   bufSize_u32     Total size of @p outBuf_pu8 in bytes
 * @param[in]   counter_u32     32-bit iteration counter
 * @param[in]   label           Label buffer
 * @param[in]   labelSize_u32   Label size in bytes
 * @param[in]   context         Context buffer
 * @param[in]   contextSize_u32 Context size in bytes
 * @param[in]   lengthBits_u32  KDF "L" field: total requested KDF output length in bits
 *
 * @return
 * Total bytes written, or 0 if @p bufSize_u32 is too small
 * @endinternal
************************************************************************************************************/
static uint32_t QLIB_UTILS_BuildCmKdfInput_L(uint8_t*    outBuf_pu8,
                                             uint32_t    bufSize_u32,
                                             uint32_t    counter_u32,
                                             const char* label,
                                             uint32_t    labelSize_u32,
                                             const char* context,
                                             uint32_t    contextSize_u32,
                                             uint32_t    lengthBits_u32)
{
    uint32_t totalSize_u32;
    uint8_t* ptr_pu8 = outBuf_pu8;

    // Bound label/context to their public contract sizes first, so the addition below cannot overflow.
    if ((labelSize_u32 > QLIB_UTILS_CMKDF_MAX_LABEL_SIZE_BYTES) || (contextSize_u32 > QLIB_UTILS_CMKDF_MAX_CONTEXT_SIZE_BYTES))
    {
        return 0u;
    }

    totalSize_u32 = QLIB_UTILS_CMKDF_BUFFER_OVERHEAD_SIZE_BYTES + labelSize_u32 + contextSize_u32;
    if (totalSize_u32 > bufSize_u32)
    {
        return 0u;
    }

    /*-----------------------------------------------------------------------------------------------------*/
    /* Pack counter in big-endian format                                                                  */
    /*-----------------------------------------------------------------------------------------------------*/
    SET_U32_IN_NETWORK_BUF(counter_u32, ptr_pu8);
    ptr_pu8 += sizeof(uint32_t);

    /*-----------------------------------------------------------------------------------------------------*/
    /* Copy label                                                                                          */
    /*-----------------------------------------------------------------------------------------------------*/
    if (labelSize_u32 > 0u)
    {
        (void)memcpy((void*)ptr_pu8, (const void*)label, labelSize_u32);
        ptr_pu8 += labelSize_u32;
    }

    /*-----------------------------------------------------------------------------------------------------*/
    /* Insert delimiter byte                                                                               */
    /*-----------------------------------------------------------------------------------------------------*/
    *ptr_pu8++ = (uint8_t)QLIB_UTILS_CMKDF_DELIMITER_BYTE;

    /*-----------------------------------------------------------------------------------------------------*/
    /* Copy context                                                                                        */
    /*-----------------------------------------------------------------------------------------------------*/
    if (contextSize_u32 > 0u)
    {
        (void)memcpy((void*)ptr_pu8, (const void*)context, contextSize_u32);
        ptr_pu8 += contextSize_u32;
    }

    /*-----------------------------------------------------------------------------------------------------*/
    /* Pack derived key length in big-endian format                                                       */
    /*-----------------------------------------------------------------------------------------------------*/
    SET_U32_IN_NETWORK_BUF(lengthBits_u32, ptr_pu8);
    ptr_pu8 += sizeof(uint32_t);

    return totalSize_u32;
}
