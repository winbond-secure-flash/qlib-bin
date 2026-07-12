/************************************************************************************************************
* @internal
* @remark     Winbond Electronics Corporation - Confidential
* @copyright  Copyright (c) 2026 by Winbond Electronics Corporation . All rights reserved
* @endinternal
*
* @file       qlib_sample_secure_indications.h
* @brief      This file contains QLIB secure indications sample definitions
*
* ### project qlib_sample
*
************************************************************************************************************/

#ifndef QLIB_SAMPLE_SECURE_INDICATIONS_H__
#define QLIB_SAMPLE_SECURE_INDICATIONS_H__

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

/************************************************************************************************************
 * @brief       This function demonstrates how to get secure indications
 *
 * @param[in]   userData_pv      Pointer to data associated with this qlib instance.
 *
 * @return      QLIB_STATUS__OK if no error occurred, QLIB_STATUS__(ERROR) otherwise
************************************************************************************************************/
QLIB_STATUS_T QLIB_SAMPLE_SecureIndications(void* userData_pv);

/************************************************************************************************************
 * @brief       This function demonstrates how to get secure indications, using an already initialized QLIB context
 *
 * @param[in]   ctx      [QLIB internal state](md_definitions.html#DEF_CONTEXT)
 *
 * @return      QLIB_STATUS__OK if no error occurred, QLIB_STATUS__(ERROR) otherwise
************************************************************************************************************/
QLIB_STATUS_T QLIB_SAMPLE_SecureIndications_flow(QLIB_CONTEXT_T* ctx);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // QLIB_SAMPLE_SECURE_INDICATIONS_H__
