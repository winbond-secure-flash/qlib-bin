/************************************************************************************************************
* @internal
* @remark     Winbond Electronics Corporation - Confidential
* @copyright  Copyright (c) 2025 by Winbond Electronics Corporation . All rights reserved
* @endinternal
*
* @file       qlib_sample_lms_key_backup.h
* @brief      This file contains QLIB sample definitions for replacing LMS root key
*
* ### project qlib_sample
*
************************************************************************************************************/

#ifndef QLIB_SAMPLE_LMS_KEY_BACKUP_H__
#define QLIB_SAMPLE_LMS_KEY_BACKUP_H__

#include "qlib.h"

#if !defined(EXCLUDE_LMS) && (defined(LMS_SAMPLE_PREPARE_IN_ADVANCE) || !defined(VERIFY_ONLY))

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

/************************************************************************************************************
 * @brief       This function demonstrates how to set and replace LMS public key with initialization sequence.
 *
 * @param[in]   userData_pv      Pointer to data associated with this qlib instance.
 *
 * @return      QLIB_STATUS__OK if no error occurred, QLIB_STATUS__(ERROR) otherwise
************************************************************************************************************/
QLIB_STATUS_T QLIB_SAMPLE_LmsKeyBackup(void* userData_pv);

/************************************************************************************************************
 * @brief       This function configures the a section with an initial LMS public key for sample. It also prepares
 *              a backup key to enable LMS key replacement.
 *              This function assumes the QLIB library and flash device are already initialized and connected
 *
 * @param[in]   qlibContext      [QLIB internal state](md_definitions.html#DEF_CONTEXT)
 *
 * @return      QLIB_STATUS__OK if no error occurred, QLIB_STATUS__(ERROR) otherwise
************************************************************************************************************/
QLIB_STATUS_T QLIB_SAMPLE_LmsKeyBackup_manuf_flow(QLIB_CONTEXT_T* qlibContext);

/************************************************************************************************************
 * @brief       This function demonstrates how QLIB is used to send LMS message of type SET_KEY_LMS to replace
 *              existing LMS root key of a section.
 *              This function assumes the QLIB library and flash device are already initialized and connected
 *
 * @param[in]   qlibContext      [QLIB internal state](md_definitions.html#DEF_CONTEXT)
 *
 * @return      QLIB_STATUS__OK if no error occurred, QLIB_STATUS__(ERROR) otherwise
************************************************************************************************************/
QLIB_STATUS_T QLIB_SAMPLE_LmsKeyBackup_field_flow(QLIB_CONTEXT_T* qlibContext);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // !defined(EXCLUDE_LMS) && (defined(LMS_SAMPLE_PREPARE_IN_ADVANCE) || !defined(VERIFY_ONLY))
#endif // QLIB_SAMPLE_LMS_KEY_BACKUP_H__
