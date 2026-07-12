/************************************************************************************************************
* @internal
* @remark     Winbond Electronics Corporation - Confidential
* @copyright  Copyright (c) 2024 by Winbond Electronics Corporation . All rights reserved
* @endinternal
*
* @file       qlib_sample_spi_interface.c
* @brief      This file contains QLIB sample code for setting a new SPI interface.
*
* @example    qlib_sample_spi_interface.c
*
* @page       spi_interface SPI interface sample code
* This sample code shows how to set the SPI interface.\n
*
* @include    samples/qlib_sample_spi_interface.c
*
************************************************************************************************************/

/*-----------------------------------------------------------------------------------------------------------
                                                   INCLUDES
-----------------------------------------------------------------------------------------------------------*/
#include <stdio.h>

#include "qlib.h"
#include "qlib_sample_spi_interface.h"
#include "qlib_sample.h"
#include "qlib_sample_secure_storage.h"

#ifndef QLIB_ONLY_SECURE_STORAGE

/*-----------------------------------------------------------------------------------------------------------
                                         LOCAL FUNCTION DECLARATIONS
-----------------------------------------------------------------------------------------------------------*/
#ifndef QLIB_NO_DIRECT_FLASH_ACCESS
static QLIB_STATUS_T QLIB_SAMPLE_DummyCyclesForBusMode_L(QLIB_BUS_FORMAT_T busFormat, uint32_t* dummyCycles_pu32);
#endif // QLIB_NO_DIRECT_FLASH_ACCESS

/*-----------------------------------------------------------------------------------------------------------
                                             INTERFACE FUNCTIONS
-----------------------------------------------------------------------------------------------------------*/
QLIB_STATUS_T QLIB_SAMPLE_SetSpiInterface(void* userData_pv, QLIB_SAMPLE_SetFetchCmd_CB setFetchCmd)
{
    QLIB_CONTEXT_T qlibContext;
    QLIB_STATUS_T  status = QLIB_STATUS__OK;

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
    QLIB_STATUS_RET_CHECK_GOTO(QLIB_InitDevice(&qlibContext, QLIB_BUS_FORMAT(QLIB_BUS_MODE_1_1_1, (bool)false)),
                               status,
                               disconnect);

    /*-------------------------------------------------------------------------------------------------------
     Sample for setting new SPI interface parameters
    -------------------------------------------------------------------------------------------------------*/
    QLIB_STATUS_RET_CHECK_GOTO(QLIB_SAMPLE_SetSpiInterface_flow(&qlibContext, setFetchCmd), status, disconnect);

    /*-------------------------------------------------------------------------------------------------------
     Release the ownership of flash communication channel (it belongs to local MCU or to remote server, exclusively)
    -------------------------------------------------------------------------------------------------------*/
    QLIB_STATUS_RET_CHECK(QLIB_Disconnect(&qlibContext));

    return QLIB_STATUS__OK;

disconnect:
    (void)QLIB_Disconnect(&qlibContext);
    return status;
}

QLIB_STATUS_T QLIB_SAMPLE_SetSpiInterface_flow(QLIB_CONTEXT_T* qlibContext, QLIB_SAMPLE_SetFetchCmd_CB setFetchCmd)
{
    QLIB_STATUS_T        status = QLIB_STATUS__OK;
    QLIB_BUS_FORMAT_T    busFormat;
    uint32_t             dummyCycles_pu32;
    QLIB_STD_ADDR_MODE_T addrMode;
    QLIB_BUS_FORMAT_T    origBusFormat = QLIB_BUS_FORMAT(QLIB_BUS_MODE_1_1_1, (bool)false); // Initial value for current sample
    uint32_t             origDummyCycles_u32 = FLASH_CR_DUMMY_DEFAULT;                      // Initial value for current sample
    QLIB_STD_ADDR_MODE_T origAddrMode        = QLIB_STD_ADDR_MODE__3_BYTE;                  // Initial value for current sample

#ifndef QLIB_NO_DIRECT_FLASH_ACCESS
    QLIB_ASSERT_RET(setFetchCmd != NULL, QLIB_STATUS__INVALID_PARAMETER);
#endif // QLIB_NO_DIRECT_FLASH_ACCESS

#ifdef QLIB_SUPPORT_QPI
    busFormat = QLIB_BUS_FORMAT(QLIB_BUS_MODE_4_4_4, (bool)false);
#else
    busFormat = QLIB_BUS_FORMAT(QLIB_BUS_MODE_1_1_1, (bool)false);
#endif // QLIB_SUPPORT_QPI

    if (QLIB_FeatureSupported(qlibContext, W77Q_FEATURE_FAST_READ_DUMMY_CONFIG) == (bool)false)
    {
        dummyCycles_pu32 = QLIB_QPI_READ_DUMMY_CYCLES;
    }
    else
    {
#ifdef QLIB_SUPPORT_QPI
        dummyCycles_pu32 = 16u; // max needed dummy cycles for QPI SDR in W77Q/T 256Mb to 1Gb
#else
        dummyCycles_pu32 = 8u; // max needed dummy cycles for SPI SDR in W77Q/T 256Mb to 1Gb
#endif // QLIB_SUPPORT_QPI
    }

    addrMode = (QLIB_FeatureSupported(qlibContext, W77Q_FEATURE_4_BYTES_ADDRESS_MODE) == (bool)true) ? QLIB_STD_ADDR_MODE__4_BYTE
                                                                                                     : QLIB_STD_ADDR_MODE__3_BYTE;
#ifndef QLIB_NO_DIRECT_FLASH_ACCESS
    if (QLIB_FeatureSupported(qlibContext, W77Q_FEATURE_FAST_READ_DUMMY_CONFIG) == (bool)false)
    {
        QLIB_STATUS_RET_CHECK(QLIB_SAMPLE_DummyCyclesForBusMode_L(origBusFormat, &origDummyCycles_u32));
    }
#endif // QLIB_NO_DIRECT_FLASH_ACCESS

    /*-------------------------------------------------------------------------------------------------------
     configure chip basic registers to some value (just to make sure GMC, GMT are configured)
    -------------------------------------------------------------------------------------------------------*/
    QLIB_STATUS_RET_CHECK_GOTO(QLIB_SAMPLE_SecureStorage_manuf_flow(qlibContext), status, recover_spi);

    /*-------------------------------------------------------------------------------------------------------
     Set SPI interface
     In case XIP is used, the SPI interface parameters should be set in QLIB, flash and in core simultaneously,
     to allow both proper QLIB operation and command fetch operations from core.
    -------------------------------------------------------------------------------------------------------*/
    QLIB_STATUS_RET_CHECK(QLIB_SAMPLE_SetSPI(qlibContext, busFormat, dummyCycles_pu32, addrMode, setFetchCmd));

    /*-------------------------------------------------------------------------------------------------------
     Perform some commands to test new SPI mode
    -------------------------------------------------------------------------------------------------------*/
    QLIB_STATUS_RET_CHECK_GOTO(QLIB_SAMPLE_SecureStorage_field_flow_plainRW(qlibContext), status, recover_spi);

    /*-------------------------------------------------------------------------------------------------------
     return to original SPI parameters
    -------------------------------------------------------------------------------------------------------*/
    QLIB_STATUS_RET_CHECK(QLIB_SAMPLE_SetSPI(qlibContext, origBusFormat, origDummyCycles_u32, origAddrMode, setFetchCmd));

    return status;

recover_spi:
    (void)QLIB_SAMPLE_SetSPI(qlibContext, origBusFormat, origDummyCycles_u32, origAddrMode, setFetchCmd);
    return status;
}

QLIB_STATUS_T QLIB_SAMPLE_SetSPI(QLIB_CONTEXT_T*            qlibContext,
                                 QLIB_BUS_FORMAT_T          busFormat,
                                 uint32_t                   dummyCycles_pu32,
                                 QLIB_STD_ADDR_MODE_T       addrMode,
                                 QLIB_SAMPLE_SetFetchCmd_CB setFetchCmd)
{
    bool support4ByteAddr_b = (QLIB_FeatureSupported(qlibContext, W77Q_FEATURE_4_BYTES_ADDRESS_MODE) != (bool)false);
#ifndef EXCLUDE_FAST_READ_DUMMY_CONFIG
    bool supportDummyConf_b = (QLIB_FeatureSupported(qlibContext, W77Q_FEATURE_FAST_READ_DUMMY_CONFIG) != (bool)false);
#endif // EXCLUDE_FAST_READ_DUMMY_CONFIG
#ifndef QLIB_NO_DIRECT_FLASH_ACCESS
    if (setFetchCmd != NULL)
    {
        // If direct flash access is needed or QLIB runs from flash, We need to set the SPI fetch command in core.
        QLIB_ASSERT_RET(setFetchCmd(busFormat, dummyCycles_pu32, addrMode == QLIB_STD_ADDR_MODE__3_BYTE ? true : false) == 0u,
                        QLIB_STATUS__HARDWARE_FAILURE);
    }
#else
    (void)setFetchCmd;         // Suppress unused parameter warning when direct flash access is disabled
#endif // QLIB_NO_DIRECT_FLASH_ACCESS

    if (support4ByteAddr_b)
    {
        QLIB_STATUS_RET_CHECK(QLIB_SetAddressMode(qlibContext, addrMode));
    }
#ifndef EXCLUDE_FAST_READ_DUMMY_CONFIG
    if (supportDummyConf_b)
    {
        QLIB_STATUS_RET_CHECK(QLIB_SetFastReadDummyCycles(qlibContext, dummyCycles_pu32));
    }
#else
    (void)dummyCycles_pu32;
#endif // EXCLUDE_FAST_READ_DUMMY_CONFIG
    QLIB_STATUS_RET_CHECK(QLIB_SetInterface(qlibContext, busFormat));
    return QLIB_STATUS__OK;
}

/*-----------------------------------------------------------------------------------------------------------
                                               LOCAL FUNCTIONS
-----------------------------------------------------------------------------------------------------------*/
#ifndef QLIB_NO_DIRECT_FLASH_ACCESS
/************************************************************************************************************
 * @brief       This function calculates the dummy cycles for fast read command in W77Q 16Mb to 128Mb flash
 *
 * @param[in]   busFormat    qlib bus format
 * @param[out]  dummyCycles_pu32  dummy cycles for fast read command
 *
 * @return      QLIB_STATUS__OK if no error occurred, QLIB_STATUS__(ERROR) otherwise
************************************************************************************************************/
static QLIB_STATUS_T QLIB_SAMPLE_DummyCyclesForBusMode_L(QLIB_BUS_FORMAT_T busFormat, uint32_t* dummyCycles_pu32)
{
    QLIB_STATUS_T ret   = QLIB_STATUS__OK;
    bool          dtr_b = QLIB_BUS_FORMAT_GET_DTR(busFormat);

    QLIB_ASSERT_RET(dummyCycles_pu32 != NULL, QLIB_STATUS__INVALID_PARAMETER);

    switch (QLIB_BUS_FORMAT_GET_MODE(busFormat))
    {
        case QLIB_BUS_MODE_1_1_1:
            *dummyCycles_pu32 = ((bool)true == dtr_b) ? SPI_DC_FAST_READ_DTR_111 : SPI_DC_FAST_READ_11X;
            break;
        case QLIB_BUS_MODE_1_1_2:
        case QLIB_BUS_MODE_1_2_2:
            *dummyCycles_pu32 = ((bool)true == dtr_b) ? SPI_DC_FAST_READ_DTR_122 : SPI_DC_FAST_READ_122;
            break;
        case QLIB_BUS_MODE_1_1_4:
        case QLIB_BUS_MODE_1_4_4:
            *dummyCycles_pu32 = ((bool)true == dtr_b) ? SPI_DC_FAST_READ_DTR_144 : SPI_DC_FAST_READ_144;
            break;
        case QLIB_BUS_MODE_4_4_4:
            *dummyCycles_pu32 = ((bool)true == dtr_b) ? SPI_DC_FAST_READ_DTR_444 : QLIB_QPI_READ_DUMMY_CYCLES;
            break;
        default:
            ret = QLIB_STATUS__INVALID_PARAMETER;
            break;
    }

    return ret;
}
#endif // QLIB_NO_DIRECT_FLASH_ACCESS
#endif // QLIB_ONLY_SECURE_STORAGE
