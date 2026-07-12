/************************************************************************************************************
* @internal
* @remark     Winbond Electronics Corporation - Confidential
* @copyright  Copyright (c) 2020 by Winbond Electronics Corporation . All rights reserved
* @endinternal
*
* @file       qlib_sample_secure_storage.c
* @brief      This file contains QLIB secure storage sample code
*
* @example    qlib_sample_secure_storage.c
*
* @page       secure_storage Secure storage sample code
* This sample code shows how to perform secure storage.\n
* The first function shows erase/write/read from W77Q/T as legacy flash.\n
* The second function shows erase/write/read from W77Q/T as secure flash using full key.\n
* The third function shows erase/write/read from W77Q/T as secure flash using restricted key.\n
*
* @include    samples/qlib_sample_secure_storage.c
*
************************************************************************************************************/

/*-----------------------------------------------------------------------------------------------------------
                                                   INCLUDES
-----------------------------------------------------------------------------------------------------------*/
#include <stdio.h>

#include "qlib.h"
#include "qlib_sample.h"
#include "qlib_sample_secure_storage.h"
#include "defs_utils.h"

/*-----------------------------------------------------------------------------------------------------------
                                             INTERFACE FUNCTIONS
-----------------------------------------------------------------------------------------------------------*/
QLIB_STATUS_T QLIB_SAMPLE_SecureStorage(void* userData_pv)
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

#ifndef QLIB_ONLY_SECURE_STORAGE
    /*-------------------------------------------------------------------------------------------------------
     configuration for sample
    -------------------------------------------------------------------------------------------------------*/
    QLIB_STATUS_RET_CHECK_GOTO(QLIB_SAMPLE_SecureStorage_manuf_flow(&qlibContext), status, disconnect);
#endif // QLIB_ONLY_SECURE_STORAGE

    /*-------------------------------------------------------------------------------------------------------
     sample flow
    -------------------------------------------------------------------------------------------------------*/
    QLIB_STATUS_RET_CHECK_GOTO(QLIB_SAMPLE_SecureStorage_field_flow(&qlibContext), status, disconnect);

disconnect:
    (void)QLIB_Disconnect(&qlibContext);
    return status;
}

#ifndef QLIB_ONLY_SECURE_STORAGE
QLIB_STATUS_T QLIB_SAMPLE_SecureStorage_manuf_flow(QLIB_CONTEXT_T* ctx)
{
    QLIB_HW_VER_T                 hwVer;
    QLIB_ID_T                     id;
    uint32_t                      sect_u32;
    uint8_t                       die_u8   = 0u;
    QLIB_FLASH_CONFIG_T           flashCfg = {0u};
    QLIB_DIE_CONFIG_T             dieCfg   = {0u};
    QLIB_SECTIONS_CONF_TABLE_T    cfgLine[QLIB_MAX_DIES_SUPPORTED];
    QLIB_EXTENDED_SECTION_CONF_T* sectCfg;
    uint8_t                       diesNum_u8 = QLIB_GET_NUM_OF_DIES(ctx);

    // See what chip is connected and its unique ids. We might need this info to obtain keys for this
    // chips according to formula or via DB see QLIB_SAMPLE_GetDeviceKey() function
    QLIB_STATUS_RET_CHECK(QLIB_GetHWVersion(ctx, &hwVer));
    QLIB_STATUS_RET_CHECK(QLIB_GetId(ctx, &id));

    // We do read / modify /write configuration since we want to change only what is needed
    // read configuration
    QLIB_STATUS_RET_CHECK(QLIB_GetDeviceConfigMultiDie(ctx, &flashCfg, &dieCfg, cfgLine, diesNum_u8));

    // modify configuration

    // set pre provision master key if needed
    QLIB_STATUS_RET_CHECK(
        QLIB_SAMPLE_GetDeviceKey(ctx, id.std.uniqueID, QLIB_KID__DEVICE_KEY_PRE_PROVISIONING, dieCfg.preProvisionedMasterKey));

    // set new master key
    QLIB_STATUS_RET_CHECK(QLIB_SAMPLE_GetDeviceKey(ctx, id.std.uniqueID, QLIB_KID__DEVICE_MASTER, dieCfg.deviceMasterKey));

#if QLIB_MAX_DIES_SUPPORTED > 1u
    QLIB_STATUS_RET_CHECK(QLIB_GetActiveDie(ctx, &die_u8));
#endif // QLIB_MAX_DIES_SUPPORTED > 1u

    // setting layout and policy of the section 1
    // this section plain read enabled and plain write enabled
    sect_u32 = 1u;

    sectCfg = cfgLine[die_u8].sectionConfigTable;

    sectCfg[sect_u32].baseAddr = QLIB_SAMPLE_RESERVED_BOOT_SECTION_SIZE;
    sectCfg[sect_u32].size     = SIZE_256KB_;

    sectCfg[sect_u32].sectionConfig.policy.plainAccessReadEnable   = 1u;
    sectCfg[sect_u32].sectionConfig.policy.plainAccessWriteEnable  = 1u;
    sectCfg[sect_u32].sectionConfig.policy.authPlainAccess         = 0u;
    sectCfg[sect_u32].sectionConfig.policy.checksumIntegrity       = 0u;
    sectCfg[sect_u32].sectionConfig.policy.digestIntegrity         = 0u;
    sectCfg[sect_u32].sectionConfig.policy.digestIntegrityOnAccess = 0u;
    sectCfg[sect_u32].sectionConfig.policy.rollbackProt            = 0u;
    sectCfg[sect_u32].sectionConfig.policy.slog                    = 0u;
    sectCfg[sect_u32].sectionConfig.policy.writeProt               = 0u;

    // Full access key configuration.This is the key to use later for the section configuration
    // changes or write access
    QLIB_STATUS_RET_CHECK(QLIB_SAMPLE_GetSectionAccessKey(ctx,
                                                          id.std.uniqueID,
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
    sectCfg[sect_u32].sectionConfig.policy.slog                    = 0u;
    sectCfg[sect_u32].sectionConfig.policy.writeProt               = 0u;

    // get full access key to configure into chip.
    QLIB_STATUS_RET_CHECK(QLIB_SAMPLE_GetSectionAccessKey(ctx,
                                                          id.std.uniqueID,
                                                          QLIB_KID__FULL_ACCESS_SECTION,
                                                          sect_u32,
                                                          die_u8,
                                                          sectCfg[sect_u32].fullAccessKey));

    // setting layout of the section 3
    // this section plain read disabled and plain write disabled
    sect_u32 = 3u;

    sectCfg[sect_u32].baseAddr = sectCfg[sect_u32 - 1].baseAddr + sectCfg[sect_u32 - 1].size;
    sectCfg[sect_u32].size     = SIZE_256KB_;

    sectCfg[sect_u32].sectionConfig.policy.plainAccessReadEnable   = 0u;
    sectCfg[sect_u32].sectionConfig.policy.plainAccessWriteEnable  = 0u;
    sectCfg[sect_u32].sectionConfig.policy.authPlainAccess         = 0u;
    sectCfg[sect_u32].sectionConfig.policy.checksumIntegrity       = 0u;
    sectCfg[sect_u32].sectionConfig.policy.digestIntegrity         = 0u;
    sectCfg[sect_u32].sectionConfig.policy.digestIntegrityOnAccess = 0u;
    sectCfg[sect_u32].sectionConfig.policy.rollbackProt            = 0u;
    sectCfg[sect_u32].sectionConfig.policy.slog                    = 0u;
    sectCfg[sect_u32].sectionConfig.policy.writeProt               = 0u;

    // get full access key to configure into chip.
    QLIB_STATUS_RET_CHECK(QLIB_SAMPLE_GetSectionAccessKey(ctx,
                                                          id.std.uniqueID,
                                                          QLIB_KID__FULL_ACCESS_SECTION,
                                                          sect_u32,
                                                          die_u8,
                                                          sectCfg[sect_u32].fullAccessKey));

    // get restricted access key to configure into chip.
    QLIB_STATUS_RET_CHECK(QLIB_SAMPLE_GetSectionAccessKey(ctx,
                                                          id.std.uniqueID,
                                                          QLIB_KID__RESTRICTED_ACCESS_SECTION,
                                                          sect_u32,
                                                          die_u8,
                                                          sectCfg[sect_u32].restrictedKey));

    // setting layout of the section 4
    // this section is with authenticated plain read and write access
    sect_u32 = 4u;

    sectCfg[sect_u32].baseAddr = sectCfg[sect_u32 - 1].baseAddr + sectCfg[sect_u32 - 1].size;
    sectCfg[sect_u32].size     = SIZE_256KB_;

    sectCfg[sect_u32].sectionConfig.policy.plainAccessReadEnable   = 1u;
    sectCfg[sect_u32].sectionConfig.policy.plainAccessWriteEnable  = 1u;
    sectCfg[sect_u32].sectionConfig.policy.authPlainAccess         = 1u;
    sectCfg[sect_u32].sectionConfig.policy.checksumIntegrity       = 0u;
    sectCfg[sect_u32].sectionConfig.policy.digestIntegrity         = 0u;
    sectCfg[sect_u32].sectionConfig.policy.digestIntegrityOnAccess = 0u;
    sectCfg[sect_u32].sectionConfig.policy.rollbackProt            = 0u;
    sectCfg[sect_u32].sectionConfig.policy.slog                    = 0u;
    sectCfg[sect_u32].sectionConfig.policy.writeProt               = 0u;

    // get full access key to configure into chip.
    QLIB_STATUS_RET_CHECK(QLIB_SAMPLE_GetSectionAccessKey(ctx,
                                                          id.std.uniqueID,
                                                          QLIB_KID__FULL_ACCESS_SECTION,
                                                          sect_u32,
                                                          die_u8,
                                                          sectCfg[sect_u32].fullAccessKey));

    // write configuration
    QLIB_STATUS_RET_CHECK(QLIB_ConfigDeviceMultiDie(ctx, &flashCfg, &dieCfg, cfgLine, diesNum_u8));

#if QLIB_MAX_DIES_SUPPORTED > 1u
    // write configuration sets die to 0. return to orig die
    QLIB_STATUS_RET_CHECK(QLIB_SetActiveDie(ctx, die_u8));
#endif // QLIB_MAX_DIES_SUPPORTED > 1u

    return QLIB_STATUS__OK;
}
#endif // QLIB_ONLY_SECURE_STORAGE

QLIB_STATUS_T QLIB_SAMPLE_SecureStorage_field_flow(QLIB_CONTEXT_T* ctx)
{
#ifndef QLIB_ONLY_SECURE_STORAGE
    QLIB_STATUS_RET_CHECK(QLIB_SAMPLE_SecureStorage_field_flow_plainRW(ctx));
    QLIB_STATUS_RET_CHECK(QLIB_SAMPLE_SecureStorage_field_flow_plainR(ctx));
    QLIB_STATUS_RET_CHECK(QLIB_SAMPLE_SecureStorage_field_flow_authPlainR(ctx));
#endif // QLIB_ONLY_SECURE_STORAGE
    QLIB_STATUS_RET_CHECK(QLIB_SAMPLE_SecureStorage_field_flow_secure(ctx));
    return QLIB_STATUS__OK;
}

#ifndef QLIB_ONLY_SECURE_STORAGE
QLIB_STATUS_T QLIB_SAMPLE_SecureStorage_field_flow_plainRW(QLIB_CONTEXT_T* ctx)
{
    uint32_t sect_u32;
    uint8_t  writeBuf_u8[QLIB_FLASH_PAGE_SIZE];
    uint8_t  readBuf_u8[QLIB_FLASH_PAGE_SIZE];

    (void)memset(readBuf_u8, 0u, sizeof(readBuf_u8));
    (void)memset(writeBuf_u8, 0xa5, sizeof(writeBuf_u8));

    // Section 1 should be rw via plain access

    sect_u32 = 1u;
    // plain erase is possible
    QLIB_STATUS_RET_CHECK(QLIB_EraseSection(ctx, sect_u32, (bool)false));

    // plain write is possible
    QLIB_STATUS_RET_CHECK(QLIB_Write(ctx, writeBuf_u8, sect_u32, 0u, QLIB_FLASH_PAGE_SIZE, (bool)false));

    // plain read is possible
    QLIB_STATUS_RET_CHECK(QLIB_Read(ctx, readBuf_u8, sect_u32, 0u, QLIB_FLASH_PAGE_SIZE, (bool)false, (bool)false));

    // verify data
    QLIB_ASSERT_RET(0u == memcmp(readBuf_u8, writeBuf_u8, sizeof(writeBuf_u8)), QLIB_STATUS__TEST_FAIL);

    return QLIB_STATUS__OK;
}
#endif // QLIB_ONLY_SECURE_STORAGE

#ifndef QLIB_ONLY_SECURE_STORAGE
QLIB_STATUS_T QLIB_SAMPLE_SecureStorage_field_flow_plainR(QLIB_CONTEXT_T* ctx)
{
    QLIB_HW_VER_T hwVer;
    QLIB_ID_T     id;
    uint8_t       die_u8 = 0u;
    uint32_t      section_u32;
    CONST_KEY_P_T keyF;
    QLIB_STATUS_T status;
    uint8_t       writeBuf_u8[QLIB_FLASH_PAGE_SIZE];
    uint8_t       readBuf_u8[QLIB_FLASH_PAGE_SIZE];
    uint8_t       eraseBuf_u8[QLIB_FLASH_PAGE_SIZE];

    (void)memset(readBuf_u8, 0u, sizeof(writeBuf_u8));
    (void)memset(writeBuf_u8, 0xa5, sizeof(writeBuf_u8));
    (void)memset(eraseBuf_u8, 0xFF, sizeof(eraseBuf_u8));

    // See what chip is connected and its unique ids. We might need this info to obtain keys for this
    // chips according to formula or via DB see QLIB_SAMPLE_GetSectionKeyPtr() function
    QLIB_STATUS_RET_CHECK(QLIB_GetHWVersion(ctx, &hwVer));
    QLIB_STATUS_RET_CHECK(QLIB_GetId(ctx, &id));

#if QLIB_MAX_DIES_SUPPORTED > 1u
    QLIB_STATUS_RET_CHECK(QLIB_GetActiveDie(ctx, &die_u8));
#endif // QLIB_MAX_DIES_SUPPORTED > 1u

    /*-------------------------------------------------------------------------------------------------------
    Section 2 should be plain read, and secure write only with full access key
    -------------------------------------------------------------------------------------------------------*/
    section_u32 = 2u;

    /*-------------------------------------------------------------------------------------------------------
     Non-secure read is allowed
    -------------------------------------------------------------------------------------------------------*/
    QLIB_STATUS_RET_CHECK_GOTO(QLIB_Read(ctx, readBuf_u8, section_u32, 0u, QLIB_FLASH_PAGE_SIZE, (bool)false, (bool)false),
                               status,
                               close_session);

    /*-------------------------------------------------------------------------------------------------------
     obtain full access key and load the key to library
    -------------------------------------------------------------------------------------------------------*/
    QLIB_STATUS_RET_CHECK(
        QLIB_SAMPLE_GetSectionKeyPtr(ctx, id.std.uniqueID, QLIB_KID__FULL_ACCESS_SECTION, section_u32, die_u8, &keyF));
    QLIB_STATUS_RET_CHECK(QLIB_LoadKey(ctx, section_u32, keyF, (bool)true));

    /*-------------------------------------------------------------------------------------------------------
     Open full access secure session for the secure commands
    -------------------------------------------------------------------------------------------------------*/
    QLIB_STATUS_RET_CHECK_GOTO(QLIB_OpenSession(ctx, section_u32, QLIB_SESSION_ACCESS_FULL), status, remove_key);

    /*-------------------------------------------------------------------------------------------------------
     Non-secure write is not allowed
    -------------------------------------------------------------------------------------------------------*/
    QLIB_SAMPLE_ALLOW_TO_FAIL__START();
    status = QLIB_Write(ctx, writeBuf_u8, section_u32, 0u, QLIB_FLASH_PAGE_SIZE, (bool)false);
    QLIB_SAMPLE_ALLOW_TO_FAIL__END();
    QLIB_ASSERT_WITH_ERROR_GOTO(QLIB_STATUS__DEVICE_PRIVILEGE_ERR == status, QLIB_STATUS__TEST_FAIL, status, close_session);

    /*-------------------------------------------------------------------------------------------------------
     Non-secure erase is not allowed
    -------------------------------------------------------------------------------------------------------*/
    QLIB_SAMPLE_ALLOW_TO_FAIL__START();
    status = QLIB_EraseSection(ctx, section_u32, (bool)false);
    QLIB_SAMPLE_ALLOW_TO_FAIL__END();
    QLIB_ASSERT_WITH_ERROR_GOTO(QLIB_STATUS__DEVICE_PRIVILEGE_ERR == status, QLIB_STATUS__TEST_FAIL, status, close_session);

    /*-------------------------------------------------------------------------------------------------------
     Secure erase
    -------------------------------------------------------------------------------------------------------*/
    QLIB_STATUS_RET_CHECK_GOTO(QLIB_Erase(ctx, section_u32, 0u, QLIB_FLASH_SECTOR_SIZE, (bool)true), status, close_session);

    /*-------------------------------------------------------------------------------------------------------
     Non-secure read
    -------------------------------------------------------------------------------------------------------*/
    QLIB_STATUS_RET_CHECK_GOTO(QLIB_Read(ctx, readBuf_u8, section_u32, 0u, sizeof(readBuf_u8), (bool)false, (bool)false),
                               status,
                               close_session);

    /*-------------------------------------------------------------------------------------------------------
     Verify sector is cleared
    -------------------------------------------------------------------------------------------------------*/
    QLIB_ASSERT_WITH_ERROR_GOTO(0u == memcmp(readBuf_u8, eraseBuf_u8, sizeof(eraseBuf_u8)),
                                QLIB_STATUS__TEST_FAIL,
                                status,
                                close_session);

    /*-------------------------------------------------------------------------------------------------------
     Secure write
    -------------------------------------------------------------------------------------------------------*/
    QLIB_STATUS_RET_CHECK_GOTO(QLIB_Write(ctx, writeBuf_u8, section_u32, 0u, QLIB_FLASH_PAGE_SIZE, (bool)true),
                               status,
                               close_session);

    /*-------------------------------------------------------------------------------------------------------
     Non-secure read
    -------------------------------------------------------------------------------------------------------*/
    QLIB_STATUS_RET_CHECK_GOTO(QLIB_Read(ctx, readBuf_u8, section_u32, 0u, sizeof(readBuf_u8), (bool)false, (bool)false),
                               status,
                               close_session);
    /*-------------------------------------------------------------------------------------------------------
     Verify sector data
    -------------------------------------------------------------------------------------------------------*/
    QLIB_ASSERT_WITH_ERROR_GOTO(0u == memcmp(readBuf_u8, writeBuf_u8, sizeof(writeBuf_u8)),
                                QLIB_STATUS__TEST_FAIL,
                                status,
                                close_session);

    /*-------------------------------------------------------------------------------------------------------
     Close the secure session
    -------------------------------------------------------------------------------------------------------*/
    QLIB_STATUS_RET_CHECK_GOTO(QLIB_CloseSession(ctx, section_u32), status, remove_key);

    status = QLIB_STATUS__OK;
    goto remove_key;

close_session:
    (void)QLIB_CloseSession(ctx, section_u32);

remove_key:
    (void)QLIB_RemoveKey(ctx, section_u32, (bool)true);

    return status;
}
#endif // QLIB_ONLY_SECURE_STORAGE

QLIB_STATUS_T QLIB_SAMPLE_SecureStorage_field_flow_secure(QLIB_CONTEXT_T* ctx)
{
    QLIB_HW_VER_T hwVer;
    QLIB_ID_T     id;
    uint8_t       die_u8 = 0u;
    uint32_t      section_u32;
    CONST_KEY_P_T keyF;
    CONST_KEY_P_T keyR;
    QLIB_STATUS_T status;
    uint8_t       writeBuf_u8[QLIB_FLASH_PAGE_SIZE];
    uint8_t       readBuf_u8[QLIB_FLASH_PAGE_SIZE];
    uint32_t      openedSessionSection_u32;
    bool          fullAccess_b;

    // See what chip is connected and its unique ids. We might need this info to obtain keys for this
    // chips according to formula or via DB see QLIB_SAMPLE_GetSectionAccessKey() function
    QLIB_STATUS_RET_CHECK(QLIB_GetHWVersion(ctx, &hwVer));
    QLIB_STATUS_RET_CHECK(QLIB_GetId(ctx, &id));

#if QLIB_MAX_DIES_SUPPORTED > 1u
    QLIB_STATUS_RET_CHECK(QLIB_GetActiveDie(ctx, &die_u8));
#endif // QLIB_MAX_DIES_SUPPORTED > 1u

    /*-------------------------------------------------------------------------------------------------------
     Section 3 should be read and write with full access key but only read with restricted key
    -------------------------------------------------------------------------------------------------------*/
    section_u32 = 3u;

#ifndef QLIB_ONLY_SECURE_STORAGE
    /*-------------------------------------------------------------------------------------------------------
     plain access attempt should fail
    -------------------------------------------------------------------------------------------------------*/
    QLIB_SAMPLE_ALLOW_TO_FAIL__START();
    status = QLIB_Read(ctx, readBuf_u8, section_u32, 0u, QLIB_FLASH_PAGE_SIZE, (bool)false, (bool)false);
    QLIB_SAMPLE_ALLOW_TO_FAIL__END();
    QLIB_ASSERT_RET(QLIB_STATUS__DEVICE_PRIVILEGE_ERR == status, QLIB_STATUS__TEST_FAIL);

    QLIB_SAMPLE_ALLOW_TO_FAIL__START();
    status = QLIB_Write(ctx, writeBuf_u8, section_u32, 0u, QLIB_FLASH_PAGE_SIZE, (bool)false);
    QLIB_SAMPLE_ALLOW_TO_FAIL__END();
    QLIB_ASSERT_RET(QLIB_STATUS__DEVICE_PRIVILEGE_ERR == status, QLIB_STATUS__TEST_FAIL);

    // plain erase should fail
    QLIB_SAMPLE_ALLOW_TO_FAIL__START();
    status = QLIB_EraseSection(ctx, section_u32, (bool)false);
    QLIB_SAMPLE_ALLOW_TO_FAIL__END();
    QLIB_ASSERT_RET(QLIB_STATUS__DEVICE_PRIVILEGE_ERR == status, QLIB_STATUS__TEST_FAIL);
#endif // QLIB_ONLY_SECURE_STORAGE

    /*-------------------------------------------------------------------------------------------------------
     obtain access keys and load the keys to library
    -------------------------------------------------------------------------------------------------------*/
    QLIB_STATUS_RET_CHECK(
        QLIB_SAMPLE_GetSectionKeyPtr(ctx, id.std.uniqueID, QLIB_KID__RESTRICTED_ACCESS_SECTION, section_u32, die_u8, &keyR));
    QLIB_STATUS_RET_CHECK(
        QLIB_SAMPLE_GetSectionKeyPtr(ctx, id.std.uniqueID, QLIB_KID__FULL_ACCESS_SECTION, section_u32, die_u8, &keyF));

    QLIB_STATUS_RET_CHECK(QLIB_LoadKey(ctx, section_u32, keyR, (bool)false));
    QLIB_STATUS_RET_CHECK_GOTO(QLIB_LoadKey(ctx, section_u32, keyF, (bool)true), status, remove_key);

    /*-------------------------------------------------------------------------------------------------------
     Check that no session is opened, should return UINT32_MAX as openedSessionSection_u32
    -------------------------------------------------------------------------------------------------------*/
    QLIB_STATUS_RET_CHECK_GOTO(QLIB_CurrOpenedSession(ctx, die_u8, &openedSessionSection_u32, &fullAccess_b), status, remove_key);
    QLIB_ASSERT_RET(UINT32_MAX == openedSessionSection_u32, QLIB_STATUS__TEST_FAIL);

    /*-------------------------------------------------------------------------------------------------------
     Open restricted access secure session for secure read
    -------------------------------------------------------------------------------------------------------*/
    QLIB_STATUS_RET_CHECK_GOTO(QLIB_OpenSession(ctx, section_u32, QLIB_SESSION_ACCESS_RESTRICTED), status, remove_key);

    /*-------------------------------------------------------------------------------------------------------
     Check that a secure session is opened, should return the section opened and if it is fullAccess_b or not
    -------------------------------------------------------------------------------------------------------*/
    QLIB_STATUS_RET_CHECK_GOTO(QLIB_CurrOpenedSession(ctx, die_u8, &openedSessionSection_u32, &fullAccess_b), status, remove_key);
    QLIB_ASSERT_RET(section_u32 == openedSessionSection_u32, QLIB_STATUS__TEST_FAIL);
    QLIB_ASSERT_RET(false == fullAccess_b, QLIB_STATUS__TEST_FAIL);

    /*-------------------------------------------------------------------------------------------------------
     section_u32 is readable via restricted session
    -------------------------------------------------------------------------------------------------------*/
    QLIB_STATUS_RET_CHECK_GOTO(QLIB_Read(ctx, readBuf_u8, section_u32, 0u, QLIB_FLASH_PAGE_SIZE, (bool)true, (bool)false),
                               status,
                               close_session);

    /*-------------------------------------------------------------------------------------------------------
     section_u32 not writable/erasable via restricted session
    -------------------------------------------------------------------------------------------------------*/
    QLIB_SAMPLE_ALLOW_TO_FAIL__START();
    status = QLIB_Write(ctx, writeBuf_u8, section_u32, 0u, QLIB_FLASH_PAGE_SIZE, (bool)true);
    QLIB_SAMPLE_ALLOW_TO_FAIL__END();
    QLIB_ASSERT_WITH_ERROR_GOTO(QLIB_STATUS__DEVICE_SESSION_ERR == status, QLIB_STATUS__TEST_FAIL, status, close_session);

    QLIB_SAMPLE_ALLOW_TO_FAIL__START();
    status = QLIB_EraseSection(ctx, section_u32, (bool)true);
    QLIB_SAMPLE_ALLOW_TO_FAIL__END();
    QLIB_ASSERT_WITH_ERROR_GOTO(QLIB_STATUS__DEVICE_SESSION_ERR == status, QLIB_STATUS__TEST_FAIL, status, close_session);

    QLIB_STATUS_RET_CHECK_GOTO(QLIB_CloseSession(ctx, section_u32), status, remove_key);

    /*-------------------------------------------------------------------------------------------------------
     Open full access secure session for secure write
    -------------------------------------------------------------------------------------------------------*/
    QLIB_STATUS_RET_CHECK_GOTO(QLIB_OpenSession(ctx, section_u32, QLIB_SESSION_ACCESS_FULL), status, remove_key);

    /*-------------------------------------------------------------------------------------------------------
     all secure access is allowed
    -------------------------------------------------------------------------------------------------------*/
    QLIB_STATUS_RET_CHECK_GOTO(QLIB_Read(ctx, readBuf_u8, section_u32, 0u, QLIB_FLASH_PAGE_SIZE, (bool)true, (bool)false),
                               status,
                               close_session);
    QLIB_STATUS_RET_CHECK_GOTO(QLIB_Write(ctx, writeBuf_u8, section_u32, 0u, QLIB_FLASH_PAGE_SIZE, (bool)true),
                               status,
                               close_session);
    QLIB_STATUS_RET_CHECK_GOTO(QLIB_EraseSection(ctx, section_u32, (bool)true), status, close_session);

    QLIB_STATUS_RET_CHECK_GOTO(QLIB_CloseSession(ctx, section_u32), status, remove_key);

    status = QLIB_STATUS__OK;
    goto remove_key;

close_session:
    (void)QLIB_CloseSession(ctx, section_u32);

remove_key:
    (void)QLIB_RemoveKey(ctx, section_u32, (bool)true);
    (void)QLIB_RemoveKey(ctx, section_u32, (bool)false);

    return status;
}

#ifndef QLIB_ONLY_SECURE_STORAGE
QLIB_STATUS_T QLIB_SAMPLE_SecureStorage_field_flow_authPlainR(QLIB_CONTEXT_T* ctx)
{
    QLIB_HW_VER_T hwVer;
    QLIB_ID_T     id;
    uint8_t       die_u8 = 0u;
    uint32_t      section_u32;
    CONST_KEY_P_T keyF;
    QLIB_STATUS_T status = QLIB_STATUS__OK;
    uint8_t       writeBuf_u8[QLIB_FLASH_PAGE_SIZE];
    uint8_t       readBuf_u8[QLIB_FLASH_PAGE_SIZE];
    uint8_t       eraseBuf_u8[QLIB_FLASH_PAGE_SIZE];

    (void)memset(readBuf_u8, 0u, sizeof(writeBuf_u8));
    (void)memset(writeBuf_u8, 0xa5, sizeof(writeBuf_u8));
    (void)memset(eraseBuf_u8, 0xFF, sizeof(eraseBuf_u8));

    // See what chip is connected and its unique ids. We might need this info to obtain keys for this
    // chips according to formula or via DB see QLIB_SAMPLE_GetSectionKeyPtr() function
    QLIB_STATUS_RET_CHECK(QLIB_GetHWVersion(ctx, &hwVer));
    QLIB_STATUS_RET_CHECK(QLIB_GetId(ctx, &id));

#if QLIB_MAX_DIES_SUPPORTED > 1u
    QLIB_STATUS_RET_CHECK(QLIB_GetActiveDie(ctx, &die_u8));
#endif // QLIB_MAX_DIES_SUPPORTED > 1u

    /*-------------------------------------------------------------------------------------------------------
     Section 4 should be authenticated plain read and write
    -------------------------------------------------------------------------------------------------------*/
    section_u32 = 4u;

    /*-------------------------------------------------------------------------------------------------------
     obtain full access key and load the key to library
    -------------------------------------------------------------------------------------------------------*/
    QLIB_STATUS_RET_CHECK(
        QLIB_SAMPLE_GetSectionKeyPtr(ctx, id.std.uniqueID, QLIB_KID__FULL_ACCESS_SECTION, section_u32, die_u8, &keyF));

    QLIB_STATUS_RET_CHECK(QLIB_LoadKey(ctx, section_u32, keyF, (bool)true));

    /*-------------------------------------------------------------------------------------------------------
     Open full access secure session for the secure commands
    -------------------------------------------------------------------------------------------------------*/
    QLIB_STATUS_RET_CHECK_GOTO(QLIB_OpenSession(ctx, section_u32, QLIB_SESSION_ACCESS_FULL), status, remove_key);

    /*-------------------------------------------------------------------------------------------------------
     Secure erase
    -------------------------------------------------------------------------------------------------------*/
    QLIB_STATUS_RET_CHECK_GOTO(QLIB_Erase(ctx, section_u32, 0u, QLIB_FLASH_SECTOR_SIZE, (bool)true), status, close_session);

    /*-------------------------------------------------------------------------------------------------------
     Secure write
    -------------------------------------------------------------------------------------------------------*/
    QLIB_STATUS_RET_CHECK_GOTO(QLIB_Write(ctx, writeBuf_u8, section_u32, 0u, (uint32_t)sizeof(writeBuf_u8), (bool)true),
                               status,
                               close_session);

    /*-------------------------------------------------------------------------------------------------------
     Non-secure authenticated read is allowed when session is open
    -------------------------------------------------------------------------------------------------------*/
    QLIB_STATUS_RET_CHECK_GOTO(QLIB_Read(ctx,
                                         readBuf_u8,
                                         section_u32,
                                         0u,
                                         (uint32_t)sizeof(readBuf_u8),
                                         (bool)false,
                                         (bool)false),
                               status,
                               close_session);

    /*-------------------------------------------------------------------------------------------------------
     Verify data
    -------------------------------------------------------------------------------------------------------*/
    QLIB_ASSERT_WITH_ERROR_GOTO(0u == memcmp(readBuf_u8, writeBuf_u8, (uint32_t)sizeof(writeBuf_u8)),
                                QLIB_STATUS__TEST_FAIL,
                                status,
                                close_session);

    /*-------------------------------------------------------------------------------------------------------
     Close the secure session
    -------------------------------------------------------------------------------------------------------*/
    QLIB_STATUS_RET_CHECK_GOTO(QLIB_CloseSession(ctx, section_u32), status, remove_key);

    /*-------------------------------------------------------------------------------------------------------
     Non-secure read for authenticated plain access read section is still allowed after session was closed
    -------------------------------------------------------------------------------------------------------*/
    QLIB_STATUS_RET_CHECK_GOTO(QLIB_Read(ctx,
                                         readBuf_u8,
                                         section_u32,
                                         0u,
                                         (uint32_t)sizeof(readBuf_u8),
                                         (bool)false,
                                         (bool)false),
                               status,
                               remove_key);

    /*-------------------------------------------------------------------------------------------------------
     Revoke Plain Access privileges
    -------------------------------------------------------------------------------------------------------*/
    QLIB_STATUS_RET_CHECK_GOTO(QLIB_PlainAccessRevoke(ctx, section_u32, QLIB_PA_REVOKE_ALL_ACCESS), status, remove_key);

    /*-------------------------------------------------------------------------------------------------------
     In W77Q 16Mb to 128Mb flash, non-secure read for authenticated plain access section is not allowed after
     revoke.
     In W77Q/T 256Mb to 1Gb flash, the read function implicitly grants plain access.
    -------------------------------------------------------------------------------------------------------*/
    QLIB_SAMPLE_ALLOW_TO_FAIL__START();
    status = QLIB_Read(ctx, readBuf_u8, section_u32, 0u, (uint32_t)sizeof(readBuf_u8), (bool)false, (bool)false);
    QLIB_SAMPLE_ALLOW_TO_FAIL__END();
    QLIB_ASSERT_WITH_ERROR_GOTO(status == (QLIB_FeatureSupported(ctx, W77Q_FEATURE_PA_GRANT_REVOKE) == 0u
                                               ? QLIB_STATUS__DEVICE_PRIVILEGE_ERR
                                               : QLIB_STATUS__OK),
                                QLIB_STATUS__TEST_FAIL,
                                status,
                                remove_key);

    status = QLIB_STATUS__OK;
    goto remove_key;

close_session:
    (void)QLIB_CloseSession(ctx, section_u32);

remove_key:
    (void)QLIB_RemoveKey(ctx, section_u32, (bool)true);

    return status;
}
#endif // QLIB_ONLY_SECURE_STORAGE
