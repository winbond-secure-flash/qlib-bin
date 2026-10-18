/************************************************************************************************************
* @internal
* @remark     Winbond Electronics Corporation - Confidential
* @copyright  Copyright (c) 2022 by Winbond Electronics Corporation . All rights reserved
* @endinternal
*
* @file       qlib_sample_sign_verify.c
* @brief      This file contains QLIB data signing and signature verification sample code.
*
* @example    qlib_sample_sign_verify.c
*
* @page       sign_verify sign and verify sample code
* This sample code shows how to use qlib for data signing and signature verification .\n
*
* @include    samples/qlib_sample_sign_verify.c
*
************************************************************************************************************/

/*-----------------------------------------------------------------------------------------------------------
                                                   INCLUDES
-----------------------------------------------------------------------------------------------------------*/
#include <stdio.h>

#include "qlib.h"
#include "qlib_sample.h"
#include "qlib_sample_sign_verify.h"

#if defined(QLIB_SIGN_DATA_BY_FLASH) && !defined(QLIB_ONLY_SECURE_STORAGE)
/*-----------------------------------------------------------------------------------------------------------
                                                 DEFINITIONS
-----------------------------------------------------------------------------------------------------------*/
#define SIGN_SECTION      1u
#define SIGN_SECTION_SIZE SIZE_256KB_

/*-----------------------------------------------------------------------------------------------------------
                                             INTERFACE FUNCTIONS
-----------------------------------------------------------------------------------------------------------*/
QLIB_STATUS_T QLIB_SAMPLE_SignVerify_manuf_flow(QLIB_CONTEXT_T* qlibContext)
{
    QLIB_HW_VER_T                 hwVer;
    QLIB_ID_T                     id;
    uint32_t                      sect_u32;
    uint8_t                       die_u8   = 0u;
    QLIB_FLASH_CONFIG_T           flashCfg = {0u};
    QLIB_DIE_CONFIG_T             dieCfg   = {0u};
    QLIB_SECTIONS_CONF_TABLE_T    cfgLine[QLIB_MAX_DIES_SUPPORTED];
    QLIB_EXTENDED_SECTION_CONF_T* sectCfg;
    uint8_t                       diesNum_u8 = QLIB_GET_NUM_OF_DIES(qlibContext);

    // See what chip is connected and its unique ids. We might need this info to obtain keys for this
    // chips according to formula or via DB see QLIB_SAMPLE_GetDeviceKey() function
    QLIB_STATUS_RET_CHECK(QLIB_GetHWVersion(qlibContext, &hwVer));
    QLIB_STATUS_RET_CHECK(QLIB_GetId(qlibContext, &id));

#if QLIB_MAX_DIES_SUPPORTED > 1u
    QLIB_STATUS_RET_CHECK(QLIB_GetActiveDie(qlibContext, &die_u8));
#endif // QLIB_MAX_DIES_SUPPORTED > 1u

    // We do read / modify /write configuration since we want to change only what is needed
    // read configuration
    QLIB_STATUS_RET_CHECK(QLIB_GetDeviceConfigMultiDie(qlibContext, &flashCfg, &dieCfg, cfgLine, diesNum_u8));

    // modify configuration

    // set pre provision master key if needed
    QLIB_STATUS_RET_CHECK(
        QLIB_SAMPLE_GetDeviceKey(qlibContext, id.sec.wid, QLIB_KID__DEVICE_KEY_PRE_PROVISIONING, dieCfg.preProvisionedMasterKey));

    // set new master key
    QLIB_STATUS_RET_CHECK(QLIB_SAMPLE_GetDeviceKey(qlibContext, id.sec.wid, QLIB_KID__DEVICE_MASTER, dieCfg.deviceMasterKey));

    // configure section 1 as fully secure section
    sect_u32 = SIGN_SECTION;

    sectCfg = cfgLine[die_u8].sectionConfigTable;

    sectCfg[sect_u32].baseAddr = QLIB_SAMPLE_RESERVED_BOOT_SECTION_SIZE;
    sectCfg[sect_u32].size     = SIGN_SECTION_SIZE;

    sectCfg[sect_u32].sectionConfig.policy.plainAccessReadEnable   = 0u;
    sectCfg[sect_u32].sectionConfig.policy.plainAccessWriteEnable  = 0u;
    sectCfg[sect_u32].sectionConfig.policy.authPlainAccess         = 0u;
    sectCfg[sect_u32].sectionConfig.policy.checksumIntegrity       = 0u;
    sectCfg[sect_u32].sectionConfig.policy.digestIntegrity         = 0u;
    sectCfg[sect_u32].sectionConfig.policy.digestIntegrityOnAccess = 0u;
    sectCfg[sect_u32].sectionConfig.policy.rollbackProt            = 0u;
    sectCfg[sect_u32].sectionConfig.policy.slog                    = 0u;
    sectCfg[sect_u32].sectionConfig.policy.writeProt               = 0u;

    // Full access key configuration.This is the key to use later for the section configuration
    // changes or write access
    QLIB_STATUS_RET_CHECK(QLIB_SAMPLE_GetSectionAccessKey(qlibContext,
                                                          id.sec.wid,
                                                          QLIB_KID__FULL_ACCESS_SECTION,
                                                          sect_u32,
                                                          die_u8,
                                                          sectCfg[sect_u32].fullAccessKey));
    // Restricted key is enough for the sign/verify operations
    QLIB_STATUS_RET_CHECK(QLIB_SAMPLE_GetSectionAccessKey(qlibContext,
                                                          id.sec.wid,
                                                          QLIB_KID__RESTRICTED_ACCESS_SECTION,
                                                          sect_u32,
                                                          die_u8,
                                                          sectCfg[sect_u32].restrictedKey));

    // write configuration
    QLIB_STATUS_RET_CHECK(QLIB_ConfigDeviceMultiDie(qlibContext, &flashCfg, &dieCfg, cfgLine, diesNum_u8));

#if QLIB_MAX_DIES_SUPPORTED > 1u
    // write configuration sets die to 0. return to orig die
    QLIB_STATUS_RET_CHECK(QLIB_SetActiveDie(qlibContext, die_u8));
#endif // QLIB_MAX_DIES_SUPPORTED > 1u

    return QLIB_STATUS__OK;
}

QLIB_STATUS_T QLIB_SAMPLE_SignVerify_field_flow(QLIB_CONTEXT_T* qlibContext)
{
    QLIB_STATUS_T status = QLIB_STATUS__OK;
    TYPE_256BIT   signature;
    bool          isFullAccess_b = (bool)true;
    uint32_t      section_u32    = SIGN_SECTION; // fully secure section
    KEY_T         fullAccessKey;
    KEY_T         restrictedAccessKey;
    uint8_t       data_u8[QLIB_FLASH_PAGE_SIZE];
    QLIB_ID_T     id;
    uint8_t       die_u8 = 0u;

#if QLIB_MAX_DIES_SUPPORTED > 1u
    QLIB_STATUS_RET_CHECK(QLIB_GetActiveDie(qlibContext, &die_u8));
#endif // QLIB_MAX_DIES_SUPPORTED > 1u

    /*-------------------------------------------------------------------------------------------------------
     Erase flash data, since the sign and verify operations expect an empty section
    -------------------------------------------------------------------------------------------------------*/
    QLIB_STATUS_RET_CHECK(QLIB_GetId(qlibContext, &id));
    QLIB_STATUS_RET_CHECK(QLIB_SAMPLE_GetSectionAccessKey(qlibContext,
                                                          id.sec.wid,
                                                          QLIB_KID__FULL_ACCESS_SECTION,
                                                          section_u32,
                                                          die_u8,
                                                          fullAccessKey));
    QLIB_STATUS_RET_CHECK(QLIB_LoadKey(qlibContext, section_u32, fullAccessKey, (bool)true));
    QLIB_STATUS_RET_CHECK_GOTO(QLIB_OpenSession(qlibContext, section_u32, QLIB_SESSION_ACCESS_FULL), status, remove_key);
    QLIB_STATUS_RET_CHECK_GOTO(QLIB_Erase(qlibContext, section_u32, 0u, SIGN_SECTION_SIZE, (bool)true), status, close_session);
    QLIB_STATUS_RET_CHECK_GOTO(QLIB_CloseSession(qlibContext, section_u32), status, remove_key);
    QLIB_STATUS_RET_CHECK(QLIB_RemoveKey(qlibContext, section_u32, (bool)true));

    /*-------------------------------------------------------------------------------------------------------
     Load the restricted key for the sign/verify operations
    -------------------------------------------------------------------------------------------------------*/
    QLIB_STATUS_RET_CHECK(QLIB_SAMPLE_GetSectionAccessKey(qlibContext,
                                                          id.sec.wid,
                                                          QLIB_KID__RESTRICTED_ACCESS_SECTION,
                                                          section_u32,
                                                          die_u8,
                                                          restrictedAccessKey));
    QLIB_STATUS_RET_CHECK(QLIB_LoadKey(qlibContext, section_u32, restrictedAccessKey, (bool)false));
    isFullAccess_b = (bool)false;

    /*-------------------------------------------------------------------------------------------------------
     Open secure session for the secure commands
    -------------------------------------------------------------------------------------------------------*/
    QLIB_STATUS_RET_CHECK_GOTO(QLIB_OpenSession(qlibContext, section_u32, QLIB_SESSION_ACCESS_RESTRICTED), status, remove_key);

    /*-------------------------------------------------------------------------------------------------------
     Calculate signature for given data using the open section key
    -------------------------------------------------------------------------------------------------------*/
    (void)memset(data_u8, 0xa5, sizeof(data_u8));
    QLIB_STATUS_RET_CHECK_GOTO(QLIB_Sign(qlibContext, section_u32, data_u8, (uint32_t)sizeof(data_u8), signature),
                               status,
                               close_session);

    /*-------------------------------------------------------------------------------------------------------
     verify the signature
    -------------------------------------------------------------------------------------------------------*/
    QLIB_STATUS_RET_CHECK_GOTO(QLIB_Verify(qlibContext, section_u32, data_u8, (uint32_t)sizeof(data_u8), signature),
                               status,
                               close_session);

    /*-------------------------------------------------------------------------------------------------------
     Demonstrate failure scenario where data_u8 and signature don't match
    -------------------------------------------------------------------------------------------------------*/
    QLIB_STATUS_RET_CHECK_GOTO(QLIB_Sign(qlibContext, section_u32, data_u8, (uint32_t)sizeof(data_u8), signature),
                               status,
                               close_session);

    /*-------------------------------------------------------------------------------------------------------
     verification of signature should fail for wrong data
    -------------------------------------------------------------------------------------------------------*/
    data_u8[0]++;
    QLIB_SAMPLE_ALLOW_TO_FAIL__START();
    status = QLIB_Verify(qlibContext, section_u32, data_u8, (uint32_t)sizeof(data_u8), signature);
    QLIB_SAMPLE_ALLOW_TO_FAIL__END();
    QLIB_ASSERT_WITH_ERROR_GOTO(status == QLIB_STATUS__DEVICE_AUTHENTICATION_ERR, QLIB_STATUS__TEST_FAIL, status, close_session);
    status = QLIB_STATUS__OK;

close_session:
    (void)QLIB_CloseSession(qlibContext, section_u32);

remove_key:
    (void)QLIB_RemoveKey(qlibContext, section_u32, isFullAccess_b);

    return status;
}

QLIB_STATUS_T QLIB_SAMPLE_SignVerify(void* userData_pv)
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
     configuration for sample
    -------------------------------------------------------------------------------------------------------*/
    QLIB_STATUS_RET_CHECK_GOTO(QLIB_SAMPLE_SignVerify_manuf_flow(&qlibContext), status, disconnect);

    /*-------------------------------------------------------------------------------------------------------
     sample flow
    -------------------------------------------------------------------------------------------------------*/
    QLIB_STATUS_RET_CHECK_GOTO(QLIB_SAMPLE_SignVerify_field_flow(&qlibContext), status, disconnect);

    /*-------------------------------------------------------------------------------------------------------
     Release the ownership of flash communication channel (it belongs to local MCU or to remote server, exclusively)
    -------------------------------------------------------------------------------------------------------*/
    QLIB_STATUS_RET_CHECK(QLIB_Disconnect(&qlibContext));

    return QLIB_STATUS__OK;

disconnect:
    (void)QLIB_Disconnect(&qlibContext);
    return status;
}
#endif // defined(QLIB_SIGN_DATA_BY_FLASH) && !defined(QLIB_ONLY_SECURE_STORAGE)
