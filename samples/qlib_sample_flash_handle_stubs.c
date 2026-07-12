/************************************************************************************************************
* @internal
* @remark     Winbond Electronics Corporation - Confidential
* @copyright  Copyright (c) 2026 by Winbond Electronics Corporation . All rights reserved
* @endinternal
*
* @file       qlib_sample_flash_handle_stubs.c
* @brief      Stub implementations of QLIB_SAMPLE_GetFlashHandle / QLIB_SAMPLE_ReleaseFlashHandle sample entry-point hooks.
*             Replace these stubs with your platform's SPI access method.
*
* ### project qlib_samples
*
************************************************************************************************************/

/*-----------------------------------------------------------------------------------------------------------
                                                   INCLUDES
-----------------------------------------------------------------------------------------------------------*/
#include "qlib_sample.h"
#include "qlib_sample_flash_handle_stubs.h"

/*-----------------------------------------------------------------------------------------------------------
                                            INTERFACE FUNCTIONS
-----------------------------------------------------------------------------------------------------------*/

void* QLIB_SAMPLE_GetFlashHandle(void)
{
#pragma message("WARNING: QLIB_SAMPLE_GetFlashHandle is a stub. Implement for your platform.")
    return NULL;
}

void QLIB_SAMPLE_ReleaseFlashHandle(void* userData_pv)
{
#pragma message("WARNING: QLIB_SAMPLE_ReleaseFlashHandle is a stub. Implement for your platform.")
    (void)userData_pv;
}
