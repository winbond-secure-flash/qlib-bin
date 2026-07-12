/************************************************************************************************************
* @internal
* @remark     Winbond Electronics Corporation - Confidential
* @copyright  Copyright (c) 2023 by Winbond Electronics Corporation . All rights reserved
* @endinternal
*
* @file       qlib_sample_lms_attestation.h
* @brief      This file contains lms attestation example
*
* ### project qlib_sample
*
************************************************************************************************************/

#ifndef QLIB_SAMPLE_LMS_ATTESTATION_H__
#define QLIB_SAMPLE_LMS_ATTESTATION_H__

#if !defined(EXCLUDE_LMS_ATTESTATION)

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

/************************************************************************************************************
 * @brief       This routine demonstrates LMS OTS sign & verify functionality with initialization sequence
 *
 * @param[in]   userData_pv      Pointer to data associated with this qlib instance.
 *
 * @return      QLIB_STATUS__OK if no error occurred, QLIB_STATUS__(ERROR) otherwise
************************************************************************************************************/
QLIB_STATUS_T QLIB_SAMPLE_LmsAttest(void* userData_pv);

/************************************************************************************************************
* @brief      This function demonstrate the creation of LMS key pair for attestation flow.
*             This function assumes the QLIB library and flash device are already initialized and connected
*
* @param[out]  ctx      [QLIB internal state](md_definitions.html#DEF_CONTEXT)
*
* @return      QLIB_STATUS__OK if no error occurred, QLIB_STATUS__(ERROR) otherwise
************************************************************************************************************/
QLIB_STATUS_T QLIB_SAMPLE_LmsAttest_manuf_flow(QLIB_CONTEXT_T* qlibContext);

/************************************************************************************************************
 * @brief       This routine demonstrates LMS OTS sign & verify functionality
 *              This function assumes the QLIB library and flash device are already initialized and connected
 *
 * @param[out]  qlibContext      [QLIB internal state](md_definitions.html#DEF_CONTEXT)
 *
 * @return      QLIB_STATUS__OK if no error occurred, QLIB_STATUS__(ERROR) otherwise
************************************************************************************************************/
QLIB_STATUS_T QLIB_SAMPLE_LmsAttest_field_flow(QLIB_CONTEXT_T* qlibContext);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // EXCLUDE_LMS_ATTESTATION
#endif // QLIB_SAMPLE_LMS_ATTESTATION_H__
