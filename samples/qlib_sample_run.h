/************************************************************************************************************
* @internal
* @remark     Winbond Electronics Corporation - Confidential
* @copyright  Copyright (c) 2026 by Winbond Electronics Corporation . All rights reserved
* @endinternal
*
* @file       qlib_sample_run.h
* @brief      This file contains the main sample execution function
*
* ### project qlib
*
************************************************************************************************************/

#ifndef QLIB_SAMPLE_RUN_H__
#define QLIB_SAMPLE_RUN_H__

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

/*-----------------------------------------------------------------------------------------------------------
                                                   INCLUDES
-----------------------------------------------------------------------------------------------------------*/
#include "defs.h"
#include "qlib_types.h"
#include "qlib.h"

/*-----------------------------------------------------------------------------------------------------------
                                                  FUNCTIONS
-----------------------------------------------------------------------------------------------------------*/

/************************************************************************************************************
 * @brief       This sample includes the main function performing sample dispatch based on command line arguments.
 *
 * @param[in]   sampleName    Sample name string.
 * @param[in]   userData_pv   Pointer to data associated with this qlib instance.
 *
 * @return      QLIB_STATUS__OK if no error occurred, QLIB_STATUS__(ERROR) otherwise
************************************************************************************************************/
QLIB_STATUS_T QLIB_SAMPLE_Run(char* sampleName, void* userData_pv);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // QLIB_SAMPLE_RUN_H__
