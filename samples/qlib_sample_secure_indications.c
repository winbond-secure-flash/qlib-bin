/************************************************************************************************************
* @internal
* @remark     Winbond Electronics Corporation - Confidential
* @copyright  Copyright (c) 2026 by Winbond Electronics Corporation . All rights reserved
* @endinternal
*
* @file       qlib_sample_secure_indications.c
* @brief      This file contains QLIB secure indications sample code. Showing how to get ESSR fields and section runtime status
*
* @example    qlib_sample_secure_indications.c
*
* @page       secure_indications Secure Indications sample code
* This sample code shows how to get ESSR fields and section runtime status (rollback active partition).\n
* Note that the ESSR fields are only supported on W77Q devices, and not on W77A devices. On W77A devices,
* the SSR fields can are reflected inside ESSR, but it is not recommended to use them as they are updated in real time
* and will show busy = 1. In order to get the SSR fields on any of the devices, use the QLIB_GetStatus() function.

* @include    samples/qlib_sample_secure_indications.c
*
************************************************************************************************************/

/*-----------------------------------------------------------------------------------------------------------
                                                   INCLUDES
-----------------------------------------------------------------------------------------------------------*/
#include <stdio.h>

#include "qlib.h"
#include "qlib_sample.h"
#include "qlib_sample_secure_indications.h"

#ifndef QLIB_ONLY_SECURE_STORAGE
#include "defs_utils.h"

/*-----------------------------------------------------------------------------------------------------------
                                                DEFINITIONS
-----------------------------------------------------------------------------------------------------------*/
#ifndef PRINTF
#define PRINTF(...) (void)0
#endif // PRINTF
/*-----------------------------------------------------------------------------------------------------------
                                             INTERFACE FUNCTIONS
-----------------------------------------------------------------------------------------------------------*/
QLIB_STATUS_T QLIB_SAMPLE_SecureIndications(void* userData_pv)
{
    QLIB_CONTEXT_T    qlibContext;
    QLIB_STATUS_T     status    = QLIB_STATUS__OK;
    QLIB_BUS_FORMAT_T busFormat = QLIB_BUS_FORMAT(QLIB_BUS_MODE_1_1_1, (bool)false);

    /*-------------------------------------------------------------------------------------------------------
     Init QLIB  - needed when running QLIB either on a device or on a remote server.
    -------------------------------------------------------------------------------------------------------*/
    QLIB_STATUS_RET_CHECK(QLIB_InitLib(&qlibContext));

    /*-------------------------------------------------------------------------------------------------------
     Set the user data into Qlib's context
    -------------------------------------------------------------------------------------------------------*/
    QLIB_SetUserData(&qlibContext, userData_pv);

    /*-------------------------------------------------------------------------------------------------------
     Take the ownership of flash communication channel (it belongs to local MCU or to remote server, exclusively)
    -------------------------------------------------------------------------------------------------------*/
    QLIB_STATUS_RET_CHECK(QLIB_Connect(&qlibContext));

    /*-------------------------------------------------------------------------------------------------------
     Init Flash Device - not needed when using QLIB on a remote server
    -------------------------------------------------------------------------------------------------------*/
    QLIB_STATUS_RET_CHECK_GOTO(QLIB_InitDevice(&qlibContext, busFormat), status, disconnect);

    /*-------------------------------------------------------------------------------------------------------
     Run sample code
    -------------------------------------------------------------------------------------------------------*/
    /*-------------------------------------------------------------------------------------------------------
     Get indications for sample
    -------------------------------------------------------------------------------------------------------*/
    QLIB_STATUS_RET_CHECK_GOTO(QLIB_SAMPLE_SecureIndications_flow(&qlibContext), status, disconnect);

disconnect:
    (void)QLIB_Disconnect(&qlibContext);
    return status;
}

QLIB_STATUS_T QLIB_SAMPLE_SecureIndications_flow(QLIB_CONTEXT_T* qlibContext)
{
    if (QLIB_FeatureSupported(qlibContext, W77Q_FEATURE_ESSR) == 1u)
    {
        QLIB_REG_ESSR_T essr;
        QLIB_STATUS_RET_CHECK(QLIB_GetExtendedStatus(qlibContext, &essr));
        if (QLIB_FeatureSupported(qlibContext, W77Q_FEATURE_RNG) == 1u)
        {
            PRINTF("RNG_RDY == %u\n", essr.asStruct.RNG_RDY_u32);
        }
        PRINTF("CMVP_PASS == %u\n", essr.asStruct.CMVP_PASS_u32);
        PRINTF("CMVP_DONE == %u\n", essr.asStruct.CMVP_DONE_u32);
        PRINTF("AWDT_75 == %u\n", essr.asStruct.AWDT_75_u32);
        PRINTF("DIE_ID == %u\n", essr.asStruct.DIE_ID_u32);
        PRINTF("KID_MSB == %u\n", essr.asStruct.KID_MSB_u32);
    }
    QLIB_REG_SSR_T ssr;
    QLIB_STATUS_RET_CHECK(QLIB_GetStatus(qlibContext, &ssr));
    PRINTF("BUSY == %u\n", ssr.asStruct.BUSY);
    PRINTF("SUSPEND_E == %u\n", ssr.asStruct.SUSPEND_E);
    PRINTF("SUSPEND_W == %u\n", ssr.asStruct.SUSPEND_W);
    PRINTF("SES_READY == %u\n", ssr.asStruct.SES_READY);
    PRINTF("RESP_READY == %u\n", ssr.asStruct.RESP_READY);

    /*-------------------------------------------------------------------------------------------------------
     Get section runtime status (rollback active partition indication)
    -------------------------------------------------------------------------------------------------------*/
    QLIB_SECTION_RUNTIME_STATUS_T sectionStatus;
    QLIB_STATUS_RET_CHECK(QLIB_GetSectionRuntimeStatus(qlibContext, 0u, &sectionStatus));
    PRINTF("Section 0 runtime status:\n");
    PRINTF("  rollbackEnabled == %u\n", sectionStatus.rollbackEnabled);
    PRINTF("  swapped == %u\n", sectionStatus.swapped);
    PRINTF("  plainReadEnabled == %u\n", sectionStatus.plainReadEnabled);
    PRINTF("  plainWriteEnabled == %u\n", sectionStatus.plainWriteEnabled);
    PRINTF("  integrityCheckPass == %u\n", sectionStatus.integrityCheckPass);

    return QLIB_STATUS__OK;
}

#endif // QLIB_ONLY_SECURE_STORAGE
