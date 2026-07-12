/************************************************************************************************************
* @internal
* @remark     Winbond Electronics Corporation - Confidential
* @copyright  Copyright (c) 2023 by Winbond Electronics Corporation . All rights reserved
* @endinternal
*
* @file       qlib_utils_lms.c
* @brief      This file contains LMS utility functions
*
* ### project qlib
*
************************************************************************************************************/

/*-----------------------------------------------------------------------------------------------------------
                                                   INCLUDES
-----------------------------------------------------------------------------------------------------------*/
#include "qlib_utils_lms.h"
#include "qlib_platform.h"

/*-----------------------------------------------------------------------------------------------------------
                                                TYPES
-----------------------------------------------------------------------------------------------------------*/

#ifndef EXCLUDE_LMS_ATTESTATION
/************************************************************************************************************
 * @brief Buffer used for chain hashing
************************************************************************************************************/
PACKED_START
typedef struct
{
    QLIB_LMS_KEY_ID_T      keyId;
    uint32_t               q_u32;
    uint16_t               index_u16;
    uint8_t                iter_u8;
    LMS_OTS_ATTEST_CHUNK_T prevChunk;
} PACKED QLIB_LMS_Attest_HASH_CHAIN_BUF_T;
PACKED_END
#endif // EXCLUDE_LMS_ATTESTATION


/*-----------------------------------------------------------------------------------------------------------
                                               LOCAL FUNCTIONS PROTOTYPES
-----------------------------------------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------------------------------------
                                             INTERFACE FUNCTIONS
-----------------------------------------------------------------------------------------------------------*/
#ifndef EXCLUDE_LMS_ATTESTATION
QLIB_STATUS_T QLIB_LMS_Attest_HashChain(const LMS_OTS_ATTEST_CHUNK_T input,
                                        const QLIB_LMS_KEY_ID_T      keyId,
                                        uint32_t                     q_u32,
                                        uint16_t                     chainIndex_u16,
                                        uint8_t                      iterStart_u8,
                                        uint8_t                      iter_u8,
                                        LMS_OTS_ATTEST_CHUNK_T       output)
{
    QLIB_LMS_Attest_HASH_CHAIN_BUF_T buf;
    TYPE_256BIT                      hash = {0u};
    uint8_t                          j_u8;

    // Set buffer static parameters
    // I
    (void)memcpy((uint8_t*)buf.keyId, (const uint8_t*)keyId, sizeof(QLIB_LMS_KEY_ID_T));
    // ots index
    buf.q_u32 = REVERSE_BYTES_32_BIT(q_u32);
    // chain index
    buf.index_u16 = REVERSE_BYTES_16_BIT(chainIndex_u16);

    // Set OTS private key (Xi) as initial prev chunk
    (void)memcpy(buf.prevChunk, input, sizeof(LMS_OTS_ATTEST_CHUNK_T));

    // Perform iterations
    for (j_u8 = 0u; j_u8 < iter_u8; j_u8++)
    {
        uint32_t size_u32 = (uint32_t)sizeof(QLIB_LMS_Attest_HASH_CHAIN_BUF_T);
        void*    hashCtx_pv;

        buf.iter_u8 = j_u8 + iterStart_u8;
        QLIB_ASSERT_RET(PLAT_HASH_Init(&hashCtx_pv, QLIB_HASH_OPT_NONE) == 0, QLIB_STATUS__HARDWARE_FAILURE);
        QLIB_ASSERT_RET(PLAT_HASH_Update(hashCtx_pv, &buf, size_u32) == 0, QLIB_STATUS__HARDWARE_FAILURE);
        QLIB_ASSERT_RET(PLAT_HASH_Finish(hashCtx_pv, hash) == 0, QLIB_STATUS__HARDWARE_FAILURE);

        (void)memcpy((uint8_t*)buf.prevChunk, (uint8_t*)hash, sizeof(LMS_OTS_ATTEST_CHUNK_T));
    }

    // signed digit result
    (void)memcpy(output, buf.prevChunk, sizeof(LMS_OTS_ATTEST_CHUNK_T));

    return QLIB_STATUS__OK;
}
#endif // EXCLUDE_LMS_ATTESTATION


/*-----------------------------------------------------------------------------------------------------------
                                               LOCAL FUNCTIONS
-----------------------------------------------------------------------------------------------------------*/
