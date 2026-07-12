/************************************************************************************************************
* @internal
* @remark     Winbond Electronics Corporation - Confidential
* @copyright  Copyright (c) 2021 by Winbond Electronics Corporation . All rights reserved
* @endinternal
*
* @file       qlib_utils_crc.h
* @brief      This file contains CRC utility functions
*
* ### project qlib
*
************************************************************************************************************/
#ifndef QLIB_UTILS_CRC_H__
#define QLIB_UTILS_CRC_H__

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

/*-----------------------------------------------------------------------------------------------------------
                                                   INCLUDES
-----------------------------------------------------------------------------------------------------------*/
#include "qlib.h"

/*-----------------------------------------------------------------------------------------------------------
                                             INTERFACE FUNCTIONS
-----------------------------------------------------------------------------------------------------------*/

/************************************************************************************************************
 * @brief       This function calculates the CRC of a given data
 *
 * @param[in]   buf_pu32       data buffer
 * @param[in]   size_u32       data buffer size
 * @param[in]   padValue_u8    bytes padding after data value
 * @param[in]   padSize_u32    padding after data size
 * @param[out]  crc_pu32       CRC value
 *
 * @return
 * QLIB_STATUS__OK = 0              - no error occurred\n
 * QLIB_STATUS__INVALID_PARAMETER   - @p buf or @p crc is NULL\n
 * QLIB_STATUS__INVALID_PARAMETER   - @p size or @p padSize are not multiply of 4\n
 * QLIB_STATUS__(ERROR)             - Other error
************************************************************************************************************/
QLIB_STATUS_T QLIB_UTILS_CalcCRCWithPadding(const uint32_t* buf_pu32,
                                            uint32_t        size_u32,
                                            uint8_t         padValue_u8,
                                            uint32_t        padSize_u32,
                                            uint32_t*       crc_pu32);

/************************************************************************************************************
 * @brief       This function calculates the checksum of a given section.
 *              The function assumes there is an open session to the section with full or restricted access.
 *
 * @param       qlibContext  qlib context object
 * @param[in]   sectionId_u32    Section Id to read from
 * @param[in]   offset_u32       Start offset inside the section
 * @param[in]   dataSize_u32     Data size
 * @param[out]  crc_pu32         Result checksum value
 *
 * @return      QLIB_STATUS__OK on success or QLIB_STATUS__[ERROR] otherwise
************************************************************************************************************/
QLIB_STATUS_T QLIB_UTILS_CalcCRCForSection(QLIB_CONTEXT_T* qlibContext,
                                           uint32_t        sectionId_u32,
                                           uint32_t        offset_u32,
                                           uint32_t        dataSize_u32,
                                           uint32_t*       crc_pu32);

/************************************************************************************************************
 * @brief       This function allows to calculate the CRC of a given data in chunks
 *              The function assumes that CRC on some data was already calculated and it adds new data to the CRC calculation.
 *              On first call, function should be called with previous crc as 0
 * @param[in]      buf_pu32       data buffer to be added to CRC calculation
 * @param[in]      size_u32       data buffer size
 * @param[in, out] crc_pu32       previous / init CRC value on input and crc result on output
 *
 * @return
 * QLIB_STATUS__OK = 0              - no error occurred\n
 * QLIB_STATUS__INVALID_PARAMETER   - @p buf or @p crc is NULL\n
 * QLIB_STATUS__INVALID_PARAMETER   - @p size or @p padSize are not multiply of 4\n
 * QLIB_STATUS__(ERROR)             - Other error
 *
************************************************************************************************************/
QLIB_STATUS_T QLIB_UTILS_CalcCRCProgressive(const uint32_t* buf_pu32, uint32_t size_u32, uint32_t* crc_pu32);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // QLIB_UTILS_CRC_H__
