/************************************************************************************************************
* @internal
* @remark     Winbond Electronics Corporation - Confidential
* @copyright  Copyright (c) 2026 by Winbond Electronics Corporation . All rights reserved
* @endinternal
*
* @file       qlib_utils_kd1.c
* @brief      This file contains the KD1 device master key derivation function
*
* ### project calc_kd1
*
************************************************************************************************************/

/*-----------------------------------------------------------------------------------------------------------
                                                   INCLUDES
-----------------------------------------------------------------------------------------------------------*/

#include <string.h>
#include "qlib_utils_kd1.h"
#include "qlib_utils_cmkdf_gen_flash_key.h"

/*-----------------------------------------------------------------------------------------------------------
                                             INTERFACE FUNCTIONS
-----------------------------------------------------------------------------------------------------------*/

DEFS_STATUS QLIB_UTILS_KD1_Derive(const uint8_t cms_au8[QLIB_UTILS_HMAC_SHA256_DIGEST_SIZE_BYTES],
                                  const uint8_t wid_au8[CALC_KD1_WID_SIZE_BYTES],
                                  uint8_t       kd1_au8[CALC_KD1_SIZE_BYTES])
{
    DEFS_STATUS_COND_CHECK(cms_au8 != NULL, DEFS_STATUS_INVALID_PARAMETER);
    DEFS_STATUS_COND_CHECK(wid_au8 != NULL, DEFS_STATUS_INVALID_PARAMETER);
    DEFS_STATUS_COND_CHECK(kd1_au8 != NULL, DEFS_STATUS_INVALID_PARAMETER);
    DEFS_STATUS_COND_CHECK(QLIB_UTILS_CMKDF_KeyDerive(cms_au8,
                                                      wid_au8,
                                                      QLIB_UTILS_CMKDF_LABEL_KD1,
                                                      0u,
                                                      1u,
                                                      kd1_au8,
                                                      CALC_KD1_SIZE_BYTES) == 0,
                           DEFS_STATUS_COMMAND_FAILED);

    return DEFS_STATUS_OK;
}
