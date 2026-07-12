/************************************************************************************************************
* @internal
* @remark     Winbond Electronics Corporation - Confidential
* @copyright  Copyright (c) 2025 by Winbond Electronics Corporation . All rights reserved
* @endinternal
*
* @file       qlib_sample_recovery.h
* @brief      This file contains QLIB recovery configuration sample related headers
*
* ### project qlib_samples
*
************************************************************************************************************/

#ifndef QLIB_SAMPLE_RECOVERY_H__
#define QLIB_SAMPLE_RECOVERY_H__

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#include "qlib_sample.h"

/************************************************************************************************************
 * @brief       This routine shows QCONF recovery flow with initialization sequence
 *
 * @param[in]   userData_pv      Pointer to data associated with this qlib instance (NULL if no data required).
 *
 * @return      0 if no error occurred, QLIB_STATUS__(ERROR) otherwise
************************************************************************************************************/
QLIB_STATUS_T QLIB_SAMPLE_Recovery(void* userData_pv);

/************************************************************************************************************
* @brief       This routine shows QCONF recovery flow, assuming initialization and connect already executed
*
* @param[out]  qlibContext     [QLIB internal state](md_definitions.html#DEF_CONTEXT)
*
* @return      0 if no error occurred, QLIB_STATUS__(ERROR) otherwise
************************************************************************************************************/
QLIB_STATUS_T QLIB_SAMPLE_Recovery_flow(QLIB_CONTEXT_T* qlibContext);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // QLIB_SAMPLE_RECOVERY_H__
