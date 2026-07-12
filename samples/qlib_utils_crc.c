/************************************************************************************************************
* @internal
* @remark     Winbond Electronics Corporation - Confidential
* @copyright  Copyright (c) 2021 by Winbond Electronics Corporation . All rights reserved
* @endinternal
*
* @file       qlib_utils_crc.c
* @brief      This file contains CRC utility functions
*
* ### project qlib
*
************************************************************************************************************/

/*-----------------------------------------------------------------------------------------------------------
                                                   INCLUDES
-----------------------------------------------------------------------------------------------------------*/
#include "qlib_utils_crc.h"

/*-----------------------------------------------------------------------------------------------------------
                                                 DEFINITIONS
-----------------------------------------------------------------------------------------------------------*/

#define QLIB_UTILS_CRC_READ_BUFFER_SIZE SIZE_256B_
/*-----------------------------------------------------------------------------------------------------------
                                                  CONSTANTS
-----------------------------------------------------------------------------------------------------------*/
static const uint32_t table_u32[] = {
    0xb8bc6765u, 0xaa09c88bu, 0x8f629757u, 0xc5b428efu, 0x5019579fu, 0xa032af3eu, 0x9b14583du, 0xed59b63bu,
    0x01c26a37u, 0x0384d46eu, 0x0709a8dcu, 0x0e1351b8u, 0x1c26a370u, 0x384d46e0u, 0x709a8dc0u, 0xe1351b80u,
    0x191b3141u, 0x32366282u, 0x646cc504u, 0xc8d98a08u, 0x4ac21251u, 0x958424a2u, 0xf0794f05u, 0x3b83984bu,
    0x77073096u, 0xee0e612cu, 0x076dc419u, 0x0edb8832u, 0x1db71064u, 0x3b6e20c8u, 0x76dc4190u, 0xedb88320u,
};

/*-----------------------------------------------------------------------------------------------------------
                                             FORWARD DECLARATION
-----------------------------------------------------------------------------------------------------------*/
static uint32_t QLIB_UTILS_CRC_churn32_L(uint32_t x_u32);

/*-----------------------------------------------------------------------------------------------------------
                                             INTERFACE FUNCTIONS
-----------------------------------------------------------------------------------------------------------*/
QLIB_STATUS_T QLIB_UTILS_CalcCRCWithPadding(const uint32_t* buf_pu32,
                                            uint32_t        size_u32,
                                            uint8_t         padValue_u8,
                                            uint32_t        padSize_u32,
                                            uint32_t*       crc_pu32)
{
    uint32_t res_u32 = 0xFFFFFFFFu;
    uint32_t padding_u32 =
        ((uint32_t)padValue_u8 << 24u) | ((uint32_t)padValue_u8 << 16u) | ((uint32_t)padValue_u8 << 8u) | (uint32_t)padValue_u8;
    uint32_t i_u32;

    /*-----------------------------------------------------------------------------------------------------*/
    /* Error checking                                                                                      */
    /*-----------------------------------------------------------------------------------------------------*/
    if (NULL == crc_pu32)
    {
        return QLIB_STATUS__INVALID_PARAMETER;
    }

    if (NULL == buf_pu32)
    {
        *crc_pu32 = 0u;
        return QLIB_STATUS__INVALID_PARAMETER;
    }

    //size is multiple of 4 bytes
    QLIB_ASSERT_RET(0u == (size_u32 % (sizeof(uint32_t))), QLIB_STATUS__INVALID_PARAMETER);
    QLIB_ASSERT_RET(0u == (padSize_u32 % (sizeof(uint32_t))), QLIB_STATUS__INVALID_PARAMETER);

    for (i_u32 = 0u; i_u32 < size_u32 / sizeof(uint32_t); ++i_u32)
    {
        res_u32 = QLIB_UTILS_CRC_churn32_L(buf_pu32[i_u32] ^ res_u32);
    }

    for (i_u32 = 0u; i_u32 < padSize_u32 / sizeof(uint32_t); ++i_u32)
    {
        res_u32 = QLIB_UTILS_CRC_churn32_L(padding_u32 ^ res_u32);
    }

    *crc_pu32 = (res_u32 ^ 0xFFFFFFFFu);

    return QLIB_STATUS__OK;
}

QLIB_STATUS_T QLIB_UTILS_CalcCRCForSection(QLIB_CONTEXT_T* qlibContext,
                                           uint32_t        sectionId_u32,
                                           uint32_t        offset_u32,
                                           uint32_t        dataSize_u32,
                                           uint32_t*       crc_pu32)
{
    uint32_t res_u32 = 0xFFFFFFFFu;
    uint32_t i_u32   = 0u;
    uint32_t readBuf_u32[QLIB_UTILS_CRC_READ_BUFFER_SIZE / sizeof(uint32_t)];
    uint32_t readSize_u32;

    while (dataSize_u32 > 0u)
    {
        readSize_u32 = MIN(dataSize_u32, (uint32_t)sizeof(readBuf_u32));
        QLIB_STATUS_RET_CHECK(
            QLIB_Read(qlibContext, (uint8_t*)readBuf_u32, sectionId_u32, offset_u32, readSize_u32, (bool)true, (bool)false));
        for (i_u32 = 0u; i_u32 < (readSize_u32 / sizeof(uint32_t)); i_u32++)
        {
            res_u32 = QLIB_UTILS_CRC_churn32_L(readBuf_u32[i_u32] ^ res_u32);
        }
        dataSize_u32 -= readSize_u32;
        offset_u32 += readSize_u32;
    }
    *crc_pu32 = (res_u32 ^ 0xFFFFFFFFu);

    return QLIB_STATUS__OK;
}

QLIB_STATUS_T QLIB_UTILS_CalcCRCProgressive(const uint32_t* buf_pu32, uint32_t size_u32, uint32_t* crc_pu32)
{
    uint32_t res_u32;
    uint32_t i_u32;

    //size is multiple of 4 bytes
    QLIB_ASSERT_RET(buf_pu32 != NULL, QLIB_STATUS__INVALID_PARAMETER);
    QLIB_ASSERT_RET(crc_pu32 != NULL, QLIB_STATUS__INVALID_PARAMETER);
    //size is multiple of 4 bytes
    QLIB_ASSERT_RET(0u == (size_u32 % (sizeof(uint32_t))), QLIB_STATUS__INVALID_PARAMETER);

    res_u32 = (*crc_pu32) ^ 0xFFFFFFFFu;
    for (i_u32 = 0u; i_u32 < size_u32 / sizeof(uint32_t); i_u32++)
    {
        res_u32 = QLIB_UTILS_CRC_churn32_L(buf_pu32[i_u32] ^ res_u32);
    }

    *crc_pu32 = (res_u32 ^ 0xFFFFFFFFu);
    return QLIB_STATUS__OK;
}

/*-----------------------------------------------------------------------------------------------------------
                                               LOCAL FUNCTIONS
-----------------------------------------------------------------------------------------------------------*/
static uint32_t QLIB_UTILS_CRC_churn32_L(uint32_t x_u32)
{
    uint32_t res_u32 = 0u;
    uint32_t i_u32;
    for (i_u32 = 0u; i_u32 < 32u; i_u32++)
    {
        if ((x_u32 & 1u) == 1u)
        {
            res_u32 ^= table_u32[i_u32];
        }
        x_u32 >>= 1u;
    }
    return res_u32;
}
