/************************************************************************************************************
* @internal
* @remark     Winbond Electronics Corporation - Confidential
* @copyright  Copyright (c) 2020 by Winbond Electronics Corporation . All rights reserved
* @endinternal
*
* @file       qlib_sample_main.c
* @brief      This file contains QLIB sample main code
*
* @example    qlib_sample_main.c
*
* @page       qlib_sample_main
*             This sample includes the main function performing sample dispatch based on command line argument. \n
*             It also includes platform-specific implementations of QLIB_SAMPLE_GetFlashHandle and QLIB_SAMPLE_ReleaseFlashHandle \n
*             to provide user data (e.g. SPI handle) to all samples. \n
 * @include    samples/qlib_sample_main.c
*
************************************************************************************************************/

/*-----------------------------------------------------------------------------------------------------------
                                                   INCLUDES
-----------------------------------------------------------------------------------------------------------*/
#include "qlib_sample_run.h"

/*---------------------------------------------------------------------------------------------------------*/
/* Platform hooks - provide userData (e.g. SPI handle) to all samples.                                     */
/* Default no-op implementations are used unless QLIB_SAMPLE_PLATFORM_DEFINED is set by the build system,  */
/* in which case a platform-specific file supplies them.                                                   */
/*---------------------------------------------------------------------------------------------------------*/
#ifndef QLIB_SAMPLE_PLATFORM_DEFINED
void* QLIB_SAMPLE_GetFlashHandle(void)
{
    return NULL;
}
void QLIB_SAMPLE_ReleaseFlashHandle(void* userData_pv)
{
    (void)userData_pv;
}
#else
extern void* QLIB_SAMPLE_GetFlashHandle(void);
extern void  QLIB_SAMPLE_ReleaseFlashHandle(void* userData_pv);
#endif // QLIB_SAMPLE_PLATFORM_DEFINED

/*-----------------------------------------------------------------------------------------------------------
                                                  FUNCTIONS
-----------------------------------------------------------------------------------------------------------*/
int main(int argc, char** argv)
{
    QLIB_STATUS_T ret;
    void*         userData_pv = QLIB_SAMPLE_GetFlashHandle();

    if (argc != 2)
    {
        return -1;
    }

    ret = QLIB_SAMPLE_Run(argv[1], userData_pv);

    QLIB_SAMPLE_ReleaseFlashHandle(userData_pv);

    if (ret != QLIB_STATUS__OK)
    {
        return 1;
    }

    return 0;
}

