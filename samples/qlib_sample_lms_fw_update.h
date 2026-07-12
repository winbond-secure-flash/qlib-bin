/************************************************************************************************************
* @internal
* @remark     Winbond Electronics Corporation - Confidential
* @copyright  Copyright (c) 2023 by Winbond Electronics Corporation . All rights reserved
* @endinternal
*
* @file       qlib_sample_lms_fw_update.h
* @brief      This file contains QLIB LMS FW update sample definitions
*
* ### project qlib_sample
*
************************************************************************************************************/

#ifndef QLIB_SAMPLE_LMS_FW_UPDATE_H__
#define QLIB_SAMPLE_LMS_FW_UPDATE_H__

#include "qlib.h"

#if !defined(EXCLUDE_LMS) && (defined(LMS_SAMPLE_PREPARE_IN_ADVANCE) || !defined(VERIFY_ONLY))

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

/************************************************************************************************************
 * @brief       This function demonstrates LMS FW update usage with initialization sequence.
 *              It is dependent on the results of the lms_tool.py Python script, as the buffer generate by
 *              this script is send to the flash in this sample.(when LMS_SAMPLE_PREPARE_IN_ADVANCE defined)
 *
 * @param[in]   userData_pv      Pointer to data associated with this qlib instance.
 *
 * @return      QLIB_STATUS__OK if no error occurred, QLIB_STATUS__(ERROR) otherwise
************************************************************************************************************/
QLIB_STATUS_T QLIB_SAMPLE_LmsFwUpdate(void* userData_pv);

/************************************************************************************************************
 * @brief       This function configures the FW update section and LMS public key for sample
 *              This function assumes the QLIB library and flash device are already initialized and connected
 *
 * @param[in]   qlibContext      [QLIB internal state](md_definitions.html#DEF_CONTEXT)
 *
 * @return      QLIB_STATUS__OK if no error occurred, QLIB_STATUS__(ERROR) otherwise
************************************************************************************************************/
QLIB_STATUS_T QLIB_SAMPLE_LmsFwUpdate_manuf_flow(QLIB_CONTEXT_T* qlibContext);

/************************************************************************************************************
 * @brief       This function demonstrates LMS FW update usage in Qlib
 *              It is dependent on the results of the lms_tool.py Python script, as the buffer generate by
 *              this script is send to the flash in this sample.(when LMS_SAMPLE_PREPARE_IN_ADVANCE defined)
 *              This function assumes the QLIB library and flash device are already initialized and connected
 *
 * @param[in]   qlibContext      [QLIB internal state](md_definitions.html#DEF_CONTEXT)
 *
 * @return      QLIB_STATUS__OK if no error occurred, QLIB_STATUS__(ERROR) otherwise
************************************************************************************************************/
QLIB_STATUS_T QLIB_SAMPLE_LmsFwUpdate_field_flow(QLIB_CONTEXT_T* qlibContext);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // !defined(EXCLUDE_LMS) && (defined(LMS_SAMPLE_PREPARE_IN_ADVANCE) || !defined(VERIFY_ONLY))
#endif // QLIB_SAMPLE_LMS_FW_UPDATE_H__
