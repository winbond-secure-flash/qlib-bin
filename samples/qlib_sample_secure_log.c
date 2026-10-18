/************************************************************************************************************
* @internal
* @remark     Winbond Electronics Corporation - Confidential
* @copyright  Copyright (c) 2023 by Winbond Electronics Corporation . All rights reserved
* @endinternal
*
* @file       qlib_sample_secure_log.c
* @brief      This file contains QLIB vault sample code
*
* @example    qlib_sample_secure_log.c
*
* @page       Secure log sample code
* This sample code shows how to use a section configured as secure log.\n
*
* @include    samples/qlib_sample_secure_log.h
*
************************************************************************************************************/

/*-----------------------------------------------------------------------------------------------------------
                                                   INCLUDES
-----------------------------------------------------------------------------------------------------------*/
#include <stdio.h>

#include "qlib.h"
#include "qlib_sample.h"
#include "qlib_sample_secure_log.h"

#ifndef QLIB_ONLY_SECURE_STORAGE

/*-----------------------------------------------------------------------------------------------------------
                                         LOCAL FUNCTION DECLARATIONS
-----------------------------------------------------------------------------------------------------------*/
QLIB_STATUS_T QLIB_SAMPLE_SecureLog_eraseLog_L(QLIB_CONTEXT_T* ctx,
                                               QLIB_ID_T*      id,
                                               uint32_t        die_u32,
                                               uint32_t        sect_u32,
                                               bool            secure_b);
QLIB_STATUS_T QLIB_SAMPLE_SecureLog_checkLogByAccess(QLIB_CONTEXT_T* ctx,
                                                     QLIB_ID_T*      id,
                                                     uint32_t        die_u32,
                                                     uint32_t        sect_u32,
                                                     bool            secure_b,
                                                     bool            fullAccess_b,
                                                     bool*           can_erase_pb,
                                                     bool*           can_read_pb,
                                                     bool*           can_write_pb);
/*-----------------------------------------------------------------------------------------------------------
                                             INTERFACE FUNCTIONS
-----------------------------------------------------------------------------------------------------------*/
QLIB_STATUS_T QLIB_SAMPLE_SecureLog(void* userData_pv)
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
     configuration for sample
    -------------------------------------------------------------------------------------------------------*/
    QLIB_STATUS_RET_CHECK_GOTO(QLIB_SAMPLE_SecureLog_manuf_flow(&qlibContext), status, disconnect);

    /*-------------------------------------------------------------------------------------------------------
     sample flow
    -------------------------------------------------------------------------------------------------------*/
    QLIB_STATUS_RET_CHECK_GOTO(QLIB_SAMPLE_SecureLog_field_flow(&qlibContext), status, disconnect);

disconnect:
    (void)QLIB_Disconnect(&qlibContext);
    return status;
}

QLIB_STATUS_T QLIB_SAMPLE_SecureLog_manuf_flow(QLIB_CONTEXT_T* ctx)
{
    QLIB_HW_VER_T              hwVer;
    QLIB_ID_T                  id;
    uint32_t                   sect_u32;
    uint8_t                    die_u8 = 0u;
    QLIB_FLASH_CONFIG_T        flashCfg;
    QLIB_DIE_CONFIG_T          dieCfg;
    QLIB_SECTIONS_CONF_TABLE_T cfgArr[QLIB_MAX_DIES_SUPPORTED];
    uint8_t                    numOfDies_u8 = QLIB_GET_NUM_OF_DIES(ctx);

    // See what chip is connected and its unique ids. We might need this info to obtain keys for this
    // chips according to formula or via DB see QLIB_SAMPLE_GetDeviceKey() function
    QLIB_STATUS_RET_CHECK(QLIB_GetHWVersion(ctx, &hwVer));
    QLIB_STATUS_RET_CHECK(QLIB_GetId(ctx, &id));

    // We do read / modify /write configuration since we want to change only what is needed
    // read configuration
    QLIB_STATUS_RET_CHECK(QLIB_GetDeviceConfigMultiDie(ctx, &flashCfg, &dieCfg, cfgArr, numOfDies_u8));

    // modify configuration
    // set pre provision master key if needed
    QLIB_STATUS_RET_CHECK(
        QLIB_SAMPLE_GetDeviceKey(ctx, id.sec.wid, QLIB_KID__DEVICE_KEY_PRE_PROVISIONING, dieCfg.preProvisionedMasterKey));

    // set new master key
    QLIB_STATUS_RET_CHECK(QLIB_SAMPLE_GetDeviceKey(ctx, id.sec.wid, QLIB_KID__DEVICE_MASTER, dieCfg.deviceMasterKey));

    // set new keys ( we do only full access ) and layout
#if QLIB_MAX_DIES_SUPPORTED > 1u
    QLIB_STATUS_RET_CHECK(QLIB_GetActiveDie(ctx, &die_u8));
#endif // QLIB_MAX_DIES_SUPPORTED > 1u
    QLIB_EXTENDED_SECTION_CONF_T* sectCfg = cfgArr[die_u8].sectionConfigTable;

    // setting layout and policy of the section 1
    // this section plain read enabled and plain write enabled
    sect_u32 = 1u;

    sectCfg[sect_u32].baseAddr = QLIB_SAMPLE_RESERVED_BOOT_SECTION_SIZE;
    sectCfg[sect_u32].size     = SIZE_256KB_;

    sectCfg[sect_u32].sectionConfig.policy.plainAccessReadEnable   = 1u;
    sectCfg[sect_u32].sectionConfig.policy.plainAccessWriteEnable  = 1u;
    sectCfg[sect_u32].sectionConfig.policy.authPlainAccess         = 0u;
    sectCfg[sect_u32].sectionConfig.policy.checksumIntegrity       = 0u;
    sectCfg[sect_u32].sectionConfig.policy.digestIntegrity         = 0u;
    sectCfg[sect_u32].sectionConfig.policy.digestIntegrityOnAccess = 0u;
    sectCfg[sect_u32].sectionConfig.policy.rollbackProt            = 0u;
    sectCfg[sect_u32].sectionConfig.policy.slog                    = QLIB_FeatureSupported(ctx, W77Q_FEATURE_SECURE_LOG);
    sectCfg[sect_u32].sectionConfig.policy.writeProt               = 0u;

    // Full access key configuration.This is the key to use later for the section configuration
    // changes or write access
    QLIB_STATUS_RET_CHECK(QLIB_SAMPLE_GetSectionAccessKey(ctx,
                                                          id.sec.wid,
                                                          QLIB_KID__FULL_ACCESS_SECTION,
                                                          sect_u32,
                                                          die_u8,
                                                          sectCfg[sect_u32].fullAccessKey));

    // setting layout of the section 2
    // this section plain read enabled and plain write disabled
    sect_u32 = 2u;

    sectCfg[sect_u32].baseAddr = sectCfg[sect_u32 - 1].baseAddr + sectCfg[sect_u32 - 1].size;
    sectCfg[sect_u32].size     = SIZE_256KB_;

    sectCfg[sect_u32].sectionConfig.policy.plainAccessReadEnable   = 1u;
    sectCfg[sect_u32].sectionConfig.policy.plainAccessWriteEnable  = 0u;
    sectCfg[sect_u32].sectionConfig.policy.authPlainAccess         = 0u;
    sectCfg[sect_u32].sectionConfig.policy.checksumIntegrity       = 0u;
    sectCfg[sect_u32].sectionConfig.policy.digestIntegrity         = 0u;
    sectCfg[sect_u32].sectionConfig.policy.digestIntegrityOnAccess = 0u;
    sectCfg[sect_u32].sectionConfig.policy.rollbackProt            = 0u;
    sectCfg[sect_u32].sectionConfig.policy.slog                    = QLIB_FeatureSupported(ctx, W77Q_FEATURE_SECURE_LOG);
    sectCfg[sect_u32].sectionConfig.policy.writeProt               = 0u;

    // get full access key to configure into chip.
    QLIB_STATUS_RET_CHECK(QLIB_SAMPLE_GetSectionAccessKey(ctx,
                                                          id.sec.wid,
                                                          QLIB_KID__FULL_ACCESS_SECTION,
                                                          sect_u32,
                                                          die_u8,
                                                          sectCfg[sect_u32].fullAccessKey));

    QLIB_STATUS_RET_CHECK(QLIB_SAMPLE_GetSectionAccessKey(ctx,
                                                          id.sec.wid,
                                                          QLIB_KID__RESTRICTED_ACCESS_SECTION,
                                                          sect_u32,
                                                          die_u8,
                                                          sectCfg[sect_u32].restrictedKey));

    // setting layout of the section 3
    // this section plain read disabled and plain write enabled
    // this kind of log is supported only if the HW LOG is supported
    sect_u32 = 3u;

    sectCfg[sect_u32].baseAddr = sectCfg[sect_u32 - 1].baseAddr + sectCfg[sect_u32 - 1].size;
    sectCfg[sect_u32].size     = SIZE_256KB_;

    sectCfg[sect_u32].sectionConfig.policy.plainAccessReadEnable   = 0u;
    sectCfg[sect_u32].sectionConfig.policy.plainAccessWriteEnable  = 1u;
    sectCfg[sect_u32].sectionConfig.policy.authPlainAccess         = 0u;
    sectCfg[sect_u32].sectionConfig.policy.checksumIntegrity       = 0u;
    sectCfg[sect_u32].sectionConfig.policy.digestIntegrity         = 0u;
    sectCfg[sect_u32].sectionConfig.policy.digestIntegrityOnAccess = 0u;
    sectCfg[sect_u32].sectionConfig.policy.rollbackProt            = 0u;
    sectCfg[sect_u32].sectionConfig.policy.slog                    = QLIB_FeatureSupported(ctx, W77Q_FEATURE_SECURE_LOG);
    sectCfg[sect_u32].sectionConfig.policy.writeProt               = 0u;

    // get full access key to configure into chip.
    QLIB_STATUS_RET_CHECK(QLIB_SAMPLE_GetSectionAccessKey(ctx,
                                                          id.sec.wid,
                                                          QLIB_KID__FULL_ACCESS_SECTION,
                                                          sect_u32,
                                                          die_u8,
                                                          sectCfg[sect_u32].fullAccessKey));

    QLIB_STATUS_RET_CHECK(QLIB_SAMPLE_GetSectionAccessKey(ctx,
                                                          id.sec.wid,
                                                          QLIB_KID__RESTRICTED_ACCESS_SECTION,
                                                          sect_u32,
                                                          die_u8,
                                                          sectCfg[sect_u32].restrictedKey));

    // setting layout of the section 4
    // this section plain read disabled and plain write disabled
    sect_u32 = 4u;

    sectCfg[sect_u32].baseAddr = sectCfg[sect_u32 - 1].baseAddr + sectCfg[sect_u32 - 1].size;
    sectCfg[sect_u32].size     = SIZE_256KB_;

    sectCfg[sect_u32].sectionConfig.policy.plainAccessReadEnable   = 0u;
    sectCfg[sect_u32].sectionConfig.policy.plainAccessWriteEnable  = 0u;
    sectCfg[sect_u32].sectionConfig.policy.authPlainAccess         = 0u;
    sectCfg[sect_u32].sectionConfig.policy.checksumIntegrity       = 0u;
    sectCfg[sect_u32].sectionConfig.policy.digestIntegrity         = 0u;
    sectCfg[sect_u32].sectionConfig.policy.digestIntegrityOnAccess = 0u;
    sectCfg[sect_u32].sectionConfig.policy.rollbackProt            = 0u;
    sectCfg[sect_u32].sectionConfig.policy.slog                    = QLIB_FeatureSupported(ctx, W77Q_FEATURE_SECURE_LOG);
    sectCfg[sect_u32].sectionConfig.policy.writeProt               = 0u;

    // get full access key to configure into chip.
    QLIB_STATUS_RET_CHECK(QLIB_SAMPLE_GetSectionAccessKey(ctx,
                                                          id.sec.wid,
                                                          QLIB_KID__FULL_ACCESS_SECTION,
                                                          sect_u32,
                                                          die_u8,
                                                          sectCfg[sect_u32].fullAccessKey));

    // get restricted access key to configure into chip.
    QLIB_STATUS_RET_CHECK(QLIB_SAMPLE_GetSectionAccessKey(ctx,
                                                          id.sec.wid,
                                                          QLIB_KID__RESTRICTED_ACCESS_SECTION,
                                                          sect_u32,
                                                          die_u8,
                                                          sectCfg[sect_u32].restrictedKey));

    // write configuration
    QLIB_STATUS_RET_CHECK(QLIB_ConfigDeviceMultiDie(ctx, &flashCfg, &dieCfg, cfgArr, numOfDies_u8));

#if QLIB_MAX_DIES_SUPPORTED > 1u
    // write configuration sets die_u32 to 0. return to orig die_u32
    QLIB_STATUS_RET_CHECK(QLIB_SetActiveDie(ctx, die_u8));
#endif // QLIB_MAX_DIES_SUPPORTED > 1u

    //erase log entries
    QLIB_SAMPLE_SecureLog_eraseLog_L(ctx, &id, die_u8, 1u, false);
    QLIB_SAMPLE_SecureLog_eraseLog_L(ctx, &id, die_u8, 2u, true);
    QLIB_SAMPLE_SecureLog_eraseLog_L(ctx, &id, die_u8, 3u, false);
    QLIB_SAMPLE_SecureLog_eraseLog_L(ctx, &id, die_u8, 4u, true);

    return QLIB_STATUS__OK;
}

QLIB_STATUS_T QLIB_SAMPLE_SecureLog_field_flow(QLIB_CONTEXT_T* ctx)
{
    QLIB_HW_VER_T hwVer;
    QLIB_ID_T     id;
    uint32_t      sect_u32;
    uint8_t       die_u8       = 0u;
    bool          secure_b     = (bool)false; // TRUE if access is secure_b, FALSE otherwise
    bool          fullAccess_b = (bool)false; // TRUE if secure_b access if full ( write ), FALSE if restricted ( read )
    bool          can_erase_b  = (bool)false;
    bool          can_read_b   = (bool)false;
    bool          can_write_b  = (bool)false;

    // See what chip is connected and its unique ids. We might need this info to obtain keys for this
    // chips according to formula or via DB
    QLIB_STATUS_RET_CHECK(QLIB_GetHWVersion(ctx, &hwVer));
    QLIB_STATUS_RET_CHECK(QLIB_GetId(ctx, &id));

#if QLIB_MAX_DIES_SUPPORTED > 1u
    QLIB_STATUS_RET_CHECK(QLIB_GetActiveDie(ctx, &die_u8));
#endif // QLIB_MAX_DIES_SUPPORTED > 1u
    // Log section 1 is pa_r = 1, pa_w = 1
    sect_u32 = 1u;

    // any user
    secure_b = (bool)false;
    QLIB_STATUS_RET_CHECK(QLIB_SAMPLE_SecureLog_checkLogByAccess(ctx,
                                                                 &id,
                                                                 die_u8,
                                                                 sect_u32,
                                                                 secure_b,
                                                                 fullAccess_b,
                                                                 &can_erase_b,
                                                                 &can_read_b,
                                                                 &can_write_b));

    QLIB_ASSERT_RET(can_erase_b == (bool)true, QLIB_STATUS__TEST_FAIL);
    QLIB_ASSERT_RET(can_read_b == (bool)true, QLIB_STATUS__TEST_FAIL);
    QLIB_ASSERT_RET(can_write_b == (bool)true, QLIB_STATUS__TEST_FAIL);

    // Log section 2 is pa_r = 1, pa_w = 0
    sect_u32 = 2u;

    // any user
    secure_b = (bool)false;
    QLIB_STATUS_RET_CHECK(QLIB_SAMPLE_SecureLog_checkLogByAccess(ctx,
                                                                 &id,
                                                                 die_u8,
                                                                 sect_u32,
                                                                 secure_b,
                                                                 fullAccess_b,
                                                                 &can_erase_b,
                                                                 &can_read_b,
                                                                 &can_write_b));
    QLIB_ASSERT_RET(can_erase_b == (bool)false, QLIB_STATUS__TEST_FAIL);
    QLIB_ASSERT_RET(can_read_b == (bool)true, QLIB_STATUS__TEST_FAIL);
    QLIB_ASSERT_RET(can_write_b == (bool)false, QLIB_STATUS__TEST_FAIL);

    // user with full access key
    secure_b     = (bool)true;
    fullAccess_b = (bool)true;
    QLIB_STATUS_RET_CHECK(QLIB_SAMPLE_SecureLog_checkLogByAccess(ctx,
                                                                 &id,
                                                                 die_u8,
                                                                 sect_u32,
                                                                 secure_b,
                                                                 fullAccess_b,
                                                                 &can_erase_b,
                                                                 &can_read_b,
                                                                 &can_write_b));

    QLIB_ASSERT_RET(can_erase_b == (bool)true, QLIB_STATUS__TEST_FAIL);
    QLIB_ASSERT_RET(can_read_b == (bool)true, QLIB_STATUS__TEST_FAIL);
    QLIB_ASSERT_RET(can_write_b == (bool)true, QLIB_STATUS__TEST_FAIL);

    // user with restricted key
    secure_b     = (bool)true;
    fullAccess_b = (bool)false;
    QLIB_STATUS_RET_CHECK(QLIB_SAMPLE_SecureLog_checkLogByAccess(ctx,
                                                                 &id,
                                                                 die_u8,
                                                                 sect_u32,
                                                                 secure_b,
                                                                 fullAccess_b,
                                                                 &can_erase_b,
                                                                 &can_read_b,
                                                                 &can_write_b));

    QLIB_ASSERT_RET(can_erase_b == (bool)false, QLIB_STATUS__TEST_FAIL);
    QLIB_ASSERT_RET(can_read_b == (bool)true, QLIB_STATUS__TEST_FAIL);
    if (QLIB_FeatureSupported(ctx, W77Q_FEATURE_SECURE_LOG) == 1u)
    {
        QLIB_ASSERT_RET(can_write_b == (bool)true, QLIB_STATUS__TEST_FAIL);
    }
    else
    {
        QLIB_ASSERT_RET(can_write_b == (bool)false, QLIB_STATUS__TEST_FAIL);
    }

    // Log section 3 is pa_r = 0, pa_w = 1
    // this log access is supported only if the HW LOG is supported

    if (QLIB_FeatureSupported(ctx, W77Q_FEATURE_SECURE_LOG) == 1u)
    {
        sect_u32 = 3u;

        // any user
        secure_b = (bool)false;
        QLIB_STATUS_RET_CHECK(QLIB_SAMPLE_SecureLog_checkLogByAccess(ctx,
                                                                     &id,
                                                                     die_u8,
                                                                     sect_u32,
                                                                     secure_b,
                                                                     fullAccess_b,
                                                                     &can_erase_b,
                                                                     &can_read_b,
                                                                     &can_write_b));
        QLIB_ASSERT_RET(can_erase_b == (bool)true, QLIB_STATUS__TEST_FAIL);
        QLIB_ASSERT_RET(can_read_b == (bool)false, QLIB_STATUS__TEST_FAIL);
        QLIB_ASSERT_RET(can_write_b == (bool)true, QLIB_STATUS__TEST_FAIL);

        // user with full access key
        secure_b     = (bool)true;
        fullAccess_b = (bool)true;
        QLIB_STATUS_RET_CHECK(QLIB_SAMPLE_SecureLog_checkLogByAccess(ctx,
                                                                     &id,
                                                                     die_u8,
                                                                     sect_u32,
                                                                     secure_b,
                                                                     fullAccess_b,
                                                                     &can_erase_b,
                                                                     &can_read_b,
                                                                     &can_write_b));

        QLIB_ASSERT_RET(can_erase_b == (bool)true, QLIB_STATUS__TEST_FAIL);
        QLIB_ASSERT_RET(can_read_b == (bool)true, QLIB_STATUS__TEST_FAIL);
        QLIB_ASSERT_RET(can_write_b == (bool)true, QLIB_STATUS__TEST_FAIL);

        // user with restricted key
        secure_b     = (bool)true;
        fullAccess_b = (bool)false;
        QLIB_STATUS_RET_CHECK(QLIB_SAMPLE_SecureLog_checkLogByAccess(ctx,
                                                                     &id,
                                                                     die_u8,
                                                                     sect_u32,
                                                                     secure_b,
                                                                     fullAccess_b,
                                                                     &can_erase_b,
                                                                     &can_read_b,
                                                                     &can_write_b));

        QLIB_ASSERT_RET(can_erase_b == (bool)false, QLIB_STATUS__TEST_FAIL);
        QLIB_ASSERT_RET(can_read_b == (bool)true, QLIB_STATUS__TEST_FAIL);
        QLIB_ASSERT_RET(can_write_b == (bool)true, QLIB_STATUS__TEST_FAIL);
    }
    // Log section 4 is pa_r = 0, pa_w = 0
    sect_u32 = 4u;

    secure_b = (bool)false;
    QLIB_STATUS_RET_CHECK(QLIB_SAMPLE_SecureLog_checkLogByAccess(ctx,
                                                                 &id,
                                                                 die_u8,
                                                                 sect_u32,
                                                                 secure_b,
                                                                 fullAccess_b,
                                                                 &can_erase_b,
                                                                 &can_read_b,
                                                                 &can_write_b));

    QLIB_ASSERT_RET(can_erase_b == (bool)false, QLIB_STATUS__TEST_FAIL);
    QLIB_ASSERT_RET(can_read_b == (bool)false, QLIB_STATUS__TEST_FAIL);
    QLIB_ASSERT_RET(can_write_b == (bool)false, QLIB_STATUS__TEST_FAIL);

    secure_b     = (bool)true;
    fullAccess_b = (bool)true;
    QLIB_STATUS_RET_CHECK(QLIB_SAMPLE_SecureLog_checkLogByAccess(ctx,
                                                                 &id,
                                                                 die_u8,
                                                                 sect_u32,
                                                                 secure_b,
                                                                 fullAccess_b,
                                                                 &can_erase_b,
                                                                 &can_read_b,
                                                                 &can_write_b));

    QLIB_ASSERT_RET(can_erase_b == (bool)true, QLIB_STATUS__TEST_FAIL);
    QLIB_ASSERT_RET(can_read_b == (bool)true, QLIB_STATUS__TEST_FAIL);
    QLIB_ASSERT_RET(can_write_b == (bool)true, QLIB_STATUS__TEST_FAIL);

    secure_b     = (bool)true;
    fullAccess_b = (bool)false;
    QLIB_STATUS_RET_CHECK(QLIB_SAMPLE_SecureLog_checkLogByAccess(ctx,
                                                                 &id,
                                                                 die_u8,
                                                                 sect_u32,
                                                                 secure_b,
                                                                 fullAccess_b,
                                                                 &can_erase_b,
                                                                 &can_read_b,
                                                                 &can_write_b));

    QLIB_ASSERT_RET(can_erase_b == (bool)false, QLIB_STATUS__TEST_FAIL);
    QLIB_ASSERT_RET(can_read_b == (bool)true, QLIB_STATUS__TEST_FAIL);
    if (QLIB_FeatureSupported(ctx, W77Q_FEATURE_SECURE_LOG) == 1u)
    {
        QLIB_ASSERT_RET(can_write_b == (bool)true, QLIB_STATUS__TEST_FAIL);
    }
    else
    {
        QLIB_ASSERT_RET(can_write_b == (bool)false, QLIB_STATUS__TEST_FAIL);
    }
    return QLIB_STATUS__OK;
}

/*-----------------------------------------------------------------------------------------------------------
                                               LOCAL FUNCTIONS
-----------------------------------------------------------------------------------------------------------*/

QLIB_STATUS_T QLIB_SAMPLE_SecureLog_eraseLog_L(QLIB_CONTEXT_T* ctx,
                                               QLIB_ID_T*      id,
                                               uint32_t        die_u32,
                                               uint32_t        sect_u32,
                                               bool            secure_b)
{
    KEY_T         key;
    QLIB_STATUS_T status = QLIB_STATUS__OK;

    if (secure_b == (bool)true)
    {
        QLIB_STATUS_RET_CHECK(
            QLIB_SAMPLE_GetSectionAccessKey(ctx, id->sec.wid, QLIB_KID__FULL_ACCESS_SECTION, sect_u32, die_u32, key));
        QLIB_STATUS_RET_CHECK(QLIB_LoadKey(ctx, sect_u32, key, (bool)true));
        QLIB_STATUS_RET_CHECK_GOTO(QLIB_OpenSession(ctx, sect_u32, QLIB_SESSION_ACCESS_FULL), status, remove_key);
    }

    QLIB_STATUS_RET_CHECK_GOTO(QLIB_EraseSection(ctx, sect_u32, secure_b), status, close_session);

    if (secure_b == (bool)true)
    {
        QLIB_STATUS_RET_CHECK_GOTO(QLIB_CloseSession(ctx, sect_u32), status, remove_key);
        QLIB_STATUS_RET_CHECK(QLIB_RemoveKey(ctx, sect_u32, (bool)true));
    }

    return QLIB_STATUS__OK;

close_session:
    (void)QLIB_CloseSession(ctx, sect_u32);

remove_key:
    (void)QLIB_RemoveKey(ctx, sect_u32, (bool)true);

    return status;
}

QLIB_STATUS_T QLIB_SAMPLE_SecureLog_checkLogByAccess(QLIB_CONTEXT_T* ctx,
                                                     QLIB_ID_T*      id,
                                                     uint32_t        die_u32,
                                                     uint32_t        sect_u32,
                                                     bool            secure_b,
                                                     bool            fullAccess_b,
                                                     bool*           can_erase_pb,
                                                     bool*           can_read_pb,
                                                     bool*           can_write_pb)
{
    uint32_t              maxLen_u32;
    uint32_t              currLen_u32;
    uint32_t              initLen_u32;
    QLIB_STATUS_T         status;
    LOG_ENTRY_T           entry;
    LOG_ENTRY_T           entryExpected;
    KEY_T                 key;
    QLIB_SESSION_ACCESS_T session;
    QLIB_KID_TYPE_T       keyType;
    uint32_t              numOfEntries_u32 = 10u;
    uint32_t              entryNum_u32;

    // how many entries are possible for this section
    QLIB_STATUS_RET_CHECK(QLIB_LogMaxSize(ctx, sect_u32, &maxLen_u32));

    if (secure_b == (bool)true)
    {
        session = (fullAccess_b == (bool)true) ? QLIB_SESSION_ACCESS_FULL : QLIB_SESSION_ACCESS_RESTRICTED;
        keyType = (fullAccess_b == (bool)true) ? QLIB_KID__FULL_ACCESS_SECTION : QLIB_KID__RESTRICTED_ACCESS_SECTION;
        QLIB_STATUS_RET_CHECK(QLIB_SAMPLE_GetSectionAccessKey(ctx, id->sec.wid, keyType, sect_u32, die_u32, key));
        QLIB_STATUS_RET_CHECK(QLIB_LoadKey(ctx, sect_u32, key, fullAccess_b));
        QLIB_STATUS_RET_CHECK_GOTO(QLIB_OpenSession(ctx, sect_u32, session), status, remove_key);
    }

    /*-------------------------------------------------------------------------------------------------------
     erase log section check
    -------------------------------------------------------------------------------------------------------*/
    QLIB_SAMPLE_ALLOW_TO_FAIL__START();
    status = QLIB_EraseSection(ctx, sect_u32, secure_b);
    QLIB_SAMPLE_ALLOW_TO_FAIL__END();
    *can_erase_pb = (QLIB_STATUS__OK == status) ? (bool)true : (bool)false;

    /*-------------------------------------------------------------------------------------------------------
     get number of log entries
    -------------------------------------------------------------------------------------------------------*/
    QLIB_SAMPLE_ALLOW_TO_FAIL__START();
    status = QLIB_LogUsedEntries(ctx, sect_u32, &initLen_u32);
    QLIB_SAMPLE_ALLOW_TO_FAIL__END();
    *can_read_pb = (QLIB_STATUS__OK == status) ? (bool)true : (bool)false;
    QLIB_ASSERT_WITH_ERROR_GOTO((*can_read_pb == (bool)false) || (*can_erase_pb == (bool)false) || (initLen_u32 == 0u),
                                QLIB_STATUS__TEST_FAIL,
                                status,
                                close_session);

    /*-------------------------------------------------------------------------------------------------------
     Write several log entries
    -------------------------------------------------------------------------------------------------------*/
    *can_write_pb = (bool)true;
    (void)memset(entry, 0u, sizeof(entry));
    for (entryNum_u32 = initLen_u32; entryNum_u32 < (initLen_u32 + numOfEntries_u32); ++entryNum_u32)
    {
        entry[0] = (uint8_t)(entryNum_u32 & 0xff);
        QLIB_SAMPLE_ALLOW_TO_FAIL__START();
        status = QLIB_LogAppend(ctx, entry, sect_u32);
        QLIB_SAMPLE_ALLOW_TO_FAIL__END();
        if (status != QLIB_STATUS__OK)
        {
            *can_write_pb = (bool)false;
            break;
        }
    }

    /*-------------------------------------------------------------------------------------------------------
     Read the current size of the log - after new entries written
    -------------------------------------------------------------------------------------------------------*/
    if ((*can_read_pb == (bool)true) && (*can_write_pb == (bool)true))
    {
        QLIB_STATUS_RET_CHECK_GOTO(QLIB_LogUsedEntries(ctx, sect_u32, &currLen_u32), status, close_session);

        QLIB_ASSERT_WITH_ERROR_GOTO(currLen_u32 == (numOfEntries_u32 + initLen_u32),
                                    QLIB_STATUS__TEST_FAIL,
                                    status,
                                    close_session);

        /*---------------------------------------------------------------------------------------------------
         Read all the log entries and verify data
        ---------------------------------------------------------------------------------------------------*/
        (void)memset(entryExpected, 0u, sizeof(LOG_ENTRY_T));
        for (entryNum_u32 = initLen_u32; entryNum_u32 < currLen_u32; ++entryNum_u32)
        {
            QLIB_STATUS_RET_CHECK_GOTO(QLIB_LogAt(ctx, entry, sect_u32, entryNum_u32), status, close_session);
            entryExpected[0] = (uint8_t)(entryNum_u32 & 0xff);
            QLIB_ASSERT_WITH_ERROR_GOTO(0u == memcmp(entry, entryExpected, sizeof(LOG_ENTRY_T)),
                                        QLIB_STATUS__COMMAND_FAIL,
                                        status,
                                        close_session);
        }
    }

    status = QLIB_STATUS__OK;

close_session:
    if (secure_b == (bool)true)
    {
        (void)QLIB_CloseSession(ctx, sect_u32);
    }
remove_key:
    if (secure_b == (bool)true)
    {
        (void)QLIB_RemoveKey(ctx, sect_u32, fullAccess_b);
    }
    return status;
}

#endif // QLIB_ONLY_SECURE_STORAGE
