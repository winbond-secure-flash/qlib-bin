/************************************************************************************************************
* @internal
* @remark     Winbond Electronics Corporation - Confidential
* @copyright  Copyright (c) 2025 by Winbond Electronics Corporation . All rights reserved
* @endinternal
*
* @file       test.c
* @brief      Entry point (main) for the qlib_samples binary.
*
*             Replace the stub functions below with your platform's SPI access method.
*
*
* ### project qlib_samples
*
************************************************************************************************************/

/*-----------------------------------------------------------------------------------------------------------
                                                   INCLUDES
-----------------------------------------------------------------------------------------------------------*/
#include <stdio.h>
#include "qlib_sample_flash_handle_stubs.h"
#include "qlib_sample.h"
#include "qlib_sample_run.h"

/*-----------------------------------------------------------------------------------------------------------
                                                    MAIN
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

