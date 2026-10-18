/************************************************************************************************************
* @internal
* @remark     Winbond Electronics Corporation
* @copyright  Copyright (c) 2026 by Winbond Electronics Corporation . All rights reserved
* @endinternal
*
* @file       qlib_platform_stubs.c
* @brief      Stub implementations of all platform hooks.
*
*             These stubs allow the qlib library + samples to link and compile on a
*             host PC without real hardware.  They intentionally do nothing / return
*             success just to allow build-system and size-measurement tool work.
*
* ### project qlib
*
************************************************************************************************************/

/*-----------------------------------------------------------------------------------------------------------
                                                   INCLUDES
-----------------------------------------------------------------------------------------------------------*/
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "qlib_platform.h"
#include "common_platform_hmac_sha256.h"

/*-----------------------------------------------------------------------------------------------------------
                                                 DEFINITIONS
-----------------------------------------------------------------------------------------------------------*/
#define PLAT_HMAC_SHA256_DIGEST_SIZE_BYTES 32u


/*-----------------------------------------------------------------------------------------------------------
                                        CORE / SYSTEM
-----------------------------------------------------------------------------------------------------------*/

void CORE_RESET(void)
{
#pragma message("WARNING: CORE_RESET stub - provide valid implementation for your platform ")
}

/*-----------------------------------------------------------------------------------------------------------
                                        SPI TRANSPORT
-----------------------------------------------------------------------------------------------------------*/

int PLAT_SPI_WriteReadTransaction(const void*     userData_pv,
                                  QLIB_BUS_MODE_T format,
                                  uint32_t        flags_u32,
                                  const uint8_t*  dataOutStream_pu8,
                                  uint32_t        cmdSize_u32,
                                  uint32_t        addressSize_u32,
                                  uint32_t        dataOutSize_u32,
                                  uint32_t        dummyCycles_u32,
                                  uint8_t*        dataIn_pu8,
                                  uint32_t        dataInSize_u32)
{
#pragma message("WARNING: PLAT_SPI_WriteReadTransaction stub - provide valid implementation for your platform ")

    (void)userData_pv;
    (void)format;
    (void)flags_u32;
    (void)dataOutStream_pu8;
    (void)cmdSize_u32;
    (void)addressSize_u32;
    (void)dataOutSize_u32;
    (void)dummyCycles_u32;
    (void)dataIn_pu8;
    (void)dataInSize_u32;

    return -1;
}

/*-----------------------------------------------------------------------------------------------------------
                                        HASH / CRYPTO
-----------------------------------------------------------------------------------------------------------*/

int PLAT_HASH_Init(void** ctx_pv, QLIB_HASH_OPT_T opt)
{
#pragma message("WARNING: PLAT_HASH_Init stub - provide valid implementation for your platform ")

    (void)opt;
    (void)ctx_pv;

    return -1;
}

int PLAT_HASH_Update(void* ctx_pv, const void* data_pv, uint32_t dataSize_u32)
{
#pragma message("WARNING: PLAT_HASH_Update stub - provide valid implementation for your platform ")

    (void)ctx_pv;
    (void)data_pv;
    (void)dataSize_u32;
    return -1;
}

int PLAT_HASH_Finish(void* ctx_pv, uint32_t* output_pu32)
{
#pragma message("WARNING: PLAT_HASH_Finish stub - provide valid implementation for your platform ")

    (void)ctx_pv;
    (void)output_pu32;

    return -1;
}


/*-----------------------------------------------------------------------------------------------------------
                                        NONCE
-----------------------------------------------------------------------------------------------------------*/

uint64_t PLAT_GetNONCE(void)
{
#pragma message("WARNING: PLAT_GetNONCE stub - provide valid implementation for your platform ")

    return 0u;
}

#ifdef USE_CONSTANT_NONCE
int PLAT_SetNONCE(uint64_t constNonce_u64)
{
#pragma message("WARNING: PLAT_SetNONCE stub - provide valid implementation for your platform ")

    (void)constNonce_u64;

    return -1;
}
#endif // USE_CONSTANT_NONCE

/*-----------------------------------------------------------------------------------------------------------
                                        SPI OPTIMIZATION
-----------------------------------------------------------------------------------------------------------*/

#ifdef QLIB_SPI_OPTIMIZATION_ENABLED
void PLAT_SPI_MultiTransactionStart(void)
{
#pragma message("WARNING: PLAT_SPI_MultiTransactionStart stub - provide valid implementation for your platform ")
}

void PLAT_SPI_MultiTransactionStop(void)
{
#pragma message("WARNING: PLAT_SPI_MultiTransactionStop stub - provide valid implementation for your platform ")
}
#endif // QLIB_SPI_OPTIMIZATION_ENABLED

/*-----------------------------------------------------------------------------------------------------------
                  HMAC, not required by QLIB, only for KDF key generation supplied in QLIB utils
-----------------------------------------------------------------------------------------------------------*/

void PLAT_HMAC_SHA256(uint8_t*       output_pu8,
                      const uint8_t* key_pu8,
                      uint32_t       keySize_u32,
                      const void*    data_pv,
                      uint32_t       dataSize_u32)
{
#pragma message("WARNING: PLAT_HMAC_SHA256 stub - provide valid implementation for your platform ")

    (void)key_pu8;
    (void)keySize_u32;
    (void)data_pv;
    (void)dataSize_u32;

    (void)memset(output_pu8, 0, PLAT_HMAC_SHA256_DIGEST_SIZE_BYTES);
}

