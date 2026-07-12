/************************************************************************************************************
* @internal
* @remark     Winbond Electronics Corporation - Confidential
* @copyright  Copyright (c) 2025 by Winbond Electronics Corporation . All rights reserved
* @endinternal
*
* @file       qlib_sample_safety.h
* @brief      This file contains QLIB safety related headers
*
* ### project qlib_sample
*
************************************************************************************************************/

#ifndef QLIB_SAMPLE_SAFETY_H__
#define QLIB_SAMPLE_SAFETY_H__

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus
/*-----------------------------------------------------------------------------------------------------------
                                                   INCLUDES
-----------------------------------------------------------------------------------------------------------*/
#include "qlib.h"

/************************************************************************************************************
 * @brief       This function demonstrates safe read & write features with initialization sequence.
 *
 * @param[in]   userData_pv     Pointer to data associated with this qlib instance.
 *
 * @return      QLIB_STATUS__OK if no error occurred, QLIB_STATUS__(ERROR) otherwise
************************************************************************************************************/
QLIB_STATUS_T QLIB_SAMPLE_Safety(void* userData_pv);

/************************************************************************************************************
 * @brief       This function configures flash section for safe read & write features and
 *              demonstrates safe read & write features
 *
 * @param[in]   qlibContext   [QLIB internal state](md_definitions.html#DEF_CONTEXT)
 *
 * @return      QLIB_STATUS__OK if no error occurred, QLIB_STATUS__(ERROR) otherwise
************************************************************************************************************/
QLIB_STATUS_T QLIB_SAMPLE_Safety_flow(QLIB_CONTEXT_T* qlibContext);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // QLIB_SAMPLE_SAFETY_H__
