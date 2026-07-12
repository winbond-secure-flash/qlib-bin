/************************************************************************************************************
* @internal
* @remark     Winbond Electronics Corporation - Confidential
* @copyright  Copyright (c) 2020 by Winbond Electronics Corporation . All rights reserved
* @endinternal
*
* @file       qlib_sample_multidie_secure_storage.c
* @brief      This file contains QLIB multidie secure storage sample code
*
* @example    qlib_sample_multidie_secure_storage.c
*
* @page       multidie_secure_storage multidie secure storage sample code
* This sample code shows how to perform read from multiDie flash.\n
* The first function shows erase/write/read from multiDie flash.\n
* The second function shows erase/write/read from multiDie flash using full key.\n
* The third function shows erase/write/read from multiDie flash using restricted key.\n

* @include    samples/qlib_sample_multidie_secure_storage.c
*
************************************************************************************************************/

/*-----------------------------------------------------------------------------------------------------------
                                                   INCLUDES
-----------------------------------------------------------------------------------------------------------*/
#include <stdio.h>

#include "qlib.h"
#include "qlib_sample.h"
#include "qlib_sample_multidie_secure_storage.h"
#include "qlib_sample_secure_storage.h"

#ifndef QLIB_ONLY_SECURE_STORAGE

/*-----------------------------------------------------------------------------------------------------------
                                             INTERFACE FUNCTIONS
-----------------------------------------------------------------------------------------------------------*/
QLIB_STATUS_T QLIB_SAMPLE_MultiDieSecureStorage(void* userData_pv, QLIB_SAMPLE_SetFetchCmd_CB setFetchCmd)
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
     Run sample code - configuration for sample
    -------------------------------------------------------------------------------------------------------*/
    QLIB_STATUS_RET_CHECK_GOTO(QLIB_SAMPLE_MultiDieSecureStorage_manuf_flow(&qlibContext), status, disconnect);

    /*-------------------------------------------------------------------------------------------------------
    Run sample flow
    -------------------------------------------------------------------------------------------------------*/
    QLIB_STATUS_RET_CHECK_GOTO(QLIB_SAMPLE_MultiDieSecureStorage_field_flow(&qlibContext, setFetchCmd), status, disconnect);

    /*-------------------------------------------------------------------------------------------------------
     Release the ownership of flash communication channel (it belongs to local MCU or to remote server, exclusively)
    -------------------------------------------------------------------------------------------------------*/
    QLIB_STATUS_RET_CHECK(QLIB_Disconnect(&qlibContext));

    return QLIB_STATUS__OK;

disconnect:
    (void)QLIB_Disconnect(&qlibContext);
    return status;
}

QLIB_STATUS_T QLIB_SAMPLE_MultiDieSecureStorage_manuf_flow(QLIB_CONTEXT_T* qlibContext)
{
    uint32_t dieId_u32;
    for (dieId_u32 = 0u; dieId_u32 < QLIB_GET_NUM_OF_DIES(qlibContext); dieId_u32++)
    {
#if QLIB_MAX_DIES_SUPPORTED > 1u
        QLIB_STATUS_RET_CHECK(QLIB_SetActiveDie(qlibContext, dieId_u32));
#endif // QLIB_MAX_DIES_SUPPORTED > 1u

        QLIB_STATUS_RET_CHECK(QLIB_SAMPLE_SecureStorage_manuf_flow(qlibContext));
    }
    return QLIB_STATUS__OK;
}

QLIB_STATUS_T QLIB_SAMPLE_MultiDieSecureStorage_field_flow(QLIB_CONTEXT_T* qlibContext, QLIB_SAMPLE_SetFetchCmd_CB setFetchCmd)
{
    uint32_t             dieId_u32;
    QLIB_STATUS_T        status          = QLIB_STATUS__OK;
    QLIB_STD_ADDR_MODE_T addrMode        = QLIB_STD_ADDR_MODE__3_BYTE;                        // Initial value for current sample
    QLIB_BUS_FORMAT_T    busFormat       = QLIB_BUS_FORMAT(QLIB_BUS_MODE_1_1_1, (bool)false); // Initial value for current sample
    uint32_t             dummyCycles_u32 = FLASH_CR_DUMMY_DEFAULT;                            // Initial value for current sample

    QLIB_ASSERT_RET((dummyCycles_u32 == FLASH_CR_DUMMY_DEFAULT) ||
                        (QLIB_FeatureSupported(qlibContext, W77Q_FEATURE_FAST_READ_DUMMY_CONFIG) != 0u),
                    QLIB_STATUS__INVALID_PARAMETER);

    /*-------------------------------------------------------------------------------------------------------
     Set SPI interface
     In case XIP is used, the SPI interface parameters should be set in QLIB, flash and in core simultaneously,
     to allow both proper QLIB operation and command fetch operations from core.
     Bus mode and dummy cycles are unchanged
    -------------------------------------------------------------------------------------------------------*/
    if (addrMode != QLIB_STD_ADDR_MODE__4_BYTE)
    {
        QLIB_ASSERT_RET((QLIB_FeatureSupported(qlibContext, W77Q_FEATURE_4_BYTES_ADDRESS_MODE) != 0u),
                        QLIB_STATUS__INVALID_PARAMETER);
        QLIB_STATUS_RET_CHECK(
            QLIB_SAMPLE_SetSPI(qlibContext, busFormat, dummyCycles_u32, QLIB_STD_ADDR_MODE__4_BYTE, setFetchCmd));
    }

    for (dieId_u32 = 0u; dieId_u32 < QLIB_GET_NUM_OF_DIES(qlibContext); dieId_u32++)
    {
#if QLIB_MAX_DIES_SUPPORTED > 1u
        QLIB_STATUS_RET_CHECK_GOTO(QLIB_SetActiveDie(qlibContext, dieId_u32), status, recover_spi);
#endif // QLIB_MAX_DIES_SUPPORTED > 1u

        QLIB_STATUS_RET_CHECK_GOTO(QLIB_SAMPLE_SecureStorage_field_flow(qlibContext), status, recover_spi);
    }

    /*-------------------------------------------------------------------------------------------------------
     Restore original SPI parameters (3-byte address mode) to allow proper XIP operation for subsequent code
    -------------------------------------------------------------------------------------------------------*/
    QLIB_STATUS_RET_CHECK(QLIB_SAMPLE_SetSPI(qlibContext, busFormat, dummyCycles_u32, addrMode, setFetchCmd));

    return QLIB_STATUS__OK;

recover_spi:
    (void)QLIB_SAMPLE_SetSPI(qlibContext, busFormat, dummyCycles_u32, addrMode, setFetchCmd);
    return status;
}

#endif // QLIB_ONLY_SECURE_STORAGE
