/************************************************************************************************************
* @internal
* @remark     Winbond Electronics Corporation - Confidential
* @copyright  Copyright (c) 2019 by Winbond Electronics Corporation . All rights reserved
* @endinternal
*
* @file       qlib_sample_integrity.h
* @brief      This file contains QLIB integrity check sample definitions
*
* ### project qlib_sample
*
************************************************************************************************************/

#ifndef QLIB_SAMPLE_INTEGRITY_H__
#define QLIB_SAMPLE_INTEGRITY_H__

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

/************************************************************************************************************
 * @brief       This function demonstrates CRC integrity check with initialization sequence.
 *              It configures sections and verifies CRC integrity detection.
 *
 * @param[in]   userData_pv     Pointer to data associated with this qlib instance.
 *
 * @return      QLIB_STATUS__OK if no error occurred, QLIB_STATUS__(ERROR) otherwise
************************************************************************************************************/
QLIB_STATUS_T QLIB_SAMPLE_Integrity(void* userData_pv);

/************************************************************************************************************
* @brief      This function configures sections for integrity check demonstration.
*             Section 0 is configured with CRC integrity protection, section 1 with plain read/write access.
*             This function assumes the QLIB library and flash device are already initialized and user is connected.
*
* @param[out]  qlibContext      [QLIB internal state](md_definitions.html#DEF_CONTEXT)
*
* @return      QLIB_STATUS__OK if no error occurred, QLIB_STATUS__(ERROR) otherwise
************************************************************************************************************/
QLIB_STATUS_T QLIB_SAMPLE_Integrity_manuf_flow(QLIB_CONTEXT_T* qlibContext);

/************************************************************************************************************
 * @brief       This function verifies CRC integrity detection. It checks that section 0 CRC passes,
 *              corrupts data through section 1, then verifies that section 0 CRC now fails.
 *
 * @param[out]  qlibContext      [QLIB internal state](md_definitions.html#DEF_CONTEXT)
 *
 * @return      QLIB_STATUS__OK if no error occurred, QLIB_STATUS__(ERROR) otherwise
************************************************************************************************************/
QLIB_STATUS_T QLIB_SAMPLE_Integrity_field_flow(QLIB_CONTEXT_T* qlibContext);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // QLIB_SAMPLE_INTEGRITY_H__
