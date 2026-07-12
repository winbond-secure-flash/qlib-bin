/************************************************************************************************************
* @internal
* @remark     Winbond Electronics Corporation - Confidential
* @copyright  Copyright (c) 2025 by Winbond Electronics Corporation . All rights reserved
* @endinternal
*
* @file       qlib_sample_safety.c
* @brief      This file contains QLIB safety related functions implementation.
*
* @example    qlib_sample_safety.c
*
* @page       safety sample code
* This sample code shows how to use the safety features
*
* @include    samples/qlib_sample_safety.c
*
************************************************************************************************************/

/*-----------------------------------------------------------------------------------------------------------
                                                   INCLUDES
-----------------------------------------------------------------------------------------------------------*/
#include "qlib_sample_safety.h"
#include "qlib_sample.h"
#include "qlib_utils_crc.h"
#include "qlib_safety.h"

/*-----------------------------------------------------------------------------------------------------------
                                             INTERFACE FUNCTIONS
-----------------------------------------------------------------------------------------------------------*/

QLIB_STATUS_T QLIB_SAMPLE_Safety(void* userData_pv)
{
    QLIB_CONTEXT_T qlibContext;

    /*-------------------------------------------------------------------------------------------------------
     Set the user data into Qlib's context
    -------------------------------------------------------------------------------------------------------*/
    QLIB_SetUserData(&qlibContext, userData_pv);

    /*-------------------------------------------------------------------------------------------------------
     Run sample code
    -------------------------------------------------------------------------------------------------------*/
    QLIB_STATUS_RET_CHECK(QLIB_SAMPLE_Safety_flow(&qlibContext));

    return QLIB_STATUS__OK;
}

QLIB_STATUS_T QLIB_SAMPLE_Safety_flow(QLIB_CONTEXT_T* qlibContext)
{
    KEY_T             masterKey         = QLIB_SAMPLE_KD;
    KEY_T             dataKey           = QLIB_SAMPLE_FULL_ACCESS_K_0;
    QLIB_BUS_FORMAT_T busFormat         = QLIB_BUS_FORMAT(QLIB_BUS_MODE_1_1_1, (bool)false);
    uint32_t          writeBuffer_u32[] = {0x01020304u, 0x05060708u, 0x090a0b0cu, 0x0d0e0f10u};
    uint32_t          readBuffer_u32[sizeof(writeBuffer_u32) / sizeof(uint32_t)] = {0u};
    uint32_t          dataOffset_u32                                             = 0u;
    uint32_t          qlibSafetyVersion_u32 = 0x00010000u; // Example version, should match the actual QLIB version
    QLIB_STATUS_T     status                = QLIB_STATUS__OK;

    /*-------------------------------------------------------------------------------------------------------
     Init QLIB library and W77Q/T device
    -------------------------------------------------------------------------------------------------------*/
    QLIB_STATUS_RET_CHECK(QLIB_SAFETY_Init(qlibContext, busFormat, dataKey, qlibSafetyVersion_u32));

    /*-------------------------------------------------------------------------------------------------------
     Configure the W77Q/T device
    -------------------------------------------------------------------------------------------------------*/
    QLIB_STATUS_RET_CHECK_GOTO(QLIB_SAFETY_ConfigDevice(qlibContext, masterKey, dataKey), status, disconnect);

    /*-------------------------------------------------------------------------------------------------------
     Erase the section area before the first safe write
    -------------------------------------------------------------------------------------------------------*/
    QLIB_STATUS_RET_CHECK_GOTO(QLIB_SAFETY_Erase(qlibContext, dataOffset_u32, QLIB_FLASH_SECTOR_SIZE), status, disconnect);

    /*-------------------------------------------------------------------------------------------------------
     Sample code for safe data write
    -------------------------------------------------------------------------------------------------------*/
    QLIB_STATUS_RET_CHECK_GOTO(QLIB_SAFETY_Write(qlibContext, (uint8_t*)writeBuffer_u32, dataOffset_u32, sizeof(writeBuffer_u32)),
                               status,
                               disconnect);

    /*-------------------------------------------------------------------------------------------------------
     Sample code for safe data read
    -------------------------------------------------------------------------------------------------------*/
    QLIB_STATUS_RET_CHECK_GOTO(QLIB_SAFETY_Read(qlibContext, (uint8_t*)readBuffer_u32, dataOffset_u32, sizeof(readBuffer_u32)),
                               status,
                               disconnect);

    /*-------------------------------------------------------------------------------------------------------
     Verify that the read data is the same as the written data
    -------------------------------------------------------------------------------------------------------*/
    QLIB_ASSERT_WITH_ERROR_GOTO(0u == memcmp(writeBuffer_u32, readBuffer_u32, sizeof(writeBuffer_u32)),
                                QLIB_STATUS__SECURITY_ERR,
                                status,
                                disconnect);

    // Disconnect
    QLIB_STATUS_RET_CHECK(QLIB_Disconnect(qlibContext));

    return QLIB_STATUS__OK;

disconnect:
    (void)QLIB_Disconnect(qlibContext);
    return status;
}
