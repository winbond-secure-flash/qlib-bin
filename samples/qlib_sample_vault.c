/************************************************************************************************************
* @internal
* @remark     Winbond Electronics Corporation - Confidential
* @copyright  Copyright (c) 2023 by Winbond Electronics Corporation . All rights reserved
* @endinternal
*
* @file       qlib_sample_vault.c
* @brief      This file contains QLIB vault sample code
*
* @example    qlib_sample_vault.c
*
* @page       Vault sample code
* This sample code shows how to use the vault.\n
* The first function shows erase/write/read from W77Q/T's vault section as secure flash using full key.\n
* The second function shows erase/write/read from W77Q/T's vault section as secure flash using restricted key.\n
*
* @include    samples/qlib_sample_vault.c
*
************************************************************************************************************/

/*-----------------------------------------------------------------------------------------------------------
                                                  INCLUDES
-----------------------------------------------------------------------------------------------------------*/
#include <stdio.h>

#include "qlib.h"
#include "qlib_sample.h"
#include "qlib_sample_vault.h"

#ifndef QLIB_ONLY_SECURE_STORAGE

/*-----------------------------------------------------------------------------------------------------------
                                                 DEFINITIONS
-----------------------------------------------------------------------------------------------------------*/
#define QLIB_SECTION_ID_VAULT 8UL

/*-----------------------------------------------------------------------------------------------------------
                                             INTERFACE FUNCTIONS
-----------------------------------------------------------------------------------------------------------*/
QLIB_STATUS_T QLIB_SAMPLE_Vault(void* userData_pv)
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
    Check if the device supports the vault feature, if not - skip the sample
    -------------------------------------------------------------------------------------------------------*/
    if (QLIB_FeatureSupported(&qlibContext, W77Q_FEATURE_VAULT) == 0u)
    {
        QLIB_SAMPLE_PRINTF("Chip does not support Vault feature and sample is skipped\r\n");
        goto disconnect;
    }

    /*-------------------------------------------------------------------------------------------------------
     Run sample code configuration
    -------------------------------------------------------------------------------------------------------*/
    QLIB_STATUS_RET_CHECK(QLIB_SAMPLE_Vault_manuf_flow(&qlibContext));

    /*-------------------------------------------------------------------------------------------------------
     Run sample flow
    -------------------------------------------------------------------------------------------------------*/
    QLIB_STATUS_RET_CHECK(QLIB_SAMPLE_Vault_field_flow(&qlibContext));

    /*-------------------------------------------------------------------------------------------------------
     Release the ownership of flash communication channel (it belongs to local MCU or to remote server, exclusively)
    -------------------------------------------------------------------------------------------------------*/
    QLIB_STATUS_RET_CHECK(QLIB_Disconnect(&qlibContext));

    goto exit;

disconnect:
    (void)QLIB_Disconnect(&qlibContext);
exit:
    return status;
}

QLIB_STATUS_T QLIB_SAMPLE_Vault_manuf_flow(QLIB_CONTEXT_T* qlibContext)
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
    QLIB_STATUS_RET_CHECK(QLIB_SAMPLE_GetDeviceKey(qlibContext,
                                                   id.std.uniqueID,
                                                   QLIB_KID__DEVICE_KEY_PRE_PROVISIONING,
                                                   dieCfg.preProvisionedMasterKey));

    // set new master key
    QLIB_STATUS_RET_CHECK(
        QLIB_SAMPLE_GetDeviceKey(qlibContext, id.std.uniqueID, QLIB_KID__DEVICE_MASTER, dieCfg.deviceMasterKey));

    // setting layout and policy of the vault section - only secure access
    sect_u32 = QLIB_SECTION_ID_VAULT;

    sectCfg = cfgLine[die_u8].sectionConfigTable;

    sectCfg[sect_u32].size = SIZE_128KB_;

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
                                                          id.std.uniqueID,
                                                          QLIB_KID__FULL_ACCESS_SECTION,
                                                          sect_u32,
                                                          die_u8,
                                                          sectCfg[sect_u32].fullAccessKey));

    // get restricted access key to configure into chip.
    QLIB_STATUS_RET_CHECK(QLIB_SAMPLE_GetSectionAccessKey(qlibContext,
                                                          id.std.uniqueID,
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

QLIB_STATUS_T QLIB_SAMPLE_Vault_field_flow(QLIB_CONTEXT_T* qlibContext)
{
    /*-------------------------------------------------------------------------------------------------------
     Run sample code
    -------------------------------------------------------------------------------------------------------*/
    QLIB_STATUS_RET_CHECK(QLIB_SAMPLE_VaultFullKey(qlibContext));
    QLIB_STATUS_RET_CHECK(QLIB_SAMPLE_VaultRestrictedKey(qlibContext));

    return QLIB_STATUS__OK;
}

QLIB_STATUS_T QLIB_SAMPLE_VaultFullKey(QLIB_CONTEXT_T* qlibContext)
{
    QLIB_STATUS_T status      = QLIB_STATUS__OK;
    uint32_t      section_u32 = QLIB_SECTION_ID_VAULT;
    CONST_KEY_P_T key;
    uint8_t       writeBuf_u8[QLIB_FLASH_PAGE_SIZE];
    uint8_t       readBuf_u8[QLIB_FLASH_PAGE_SIZE];
    uint8_t       eraseBuf_u8[QLIB_FLASH_PAGE_SIZE];
    QLIB_ID_T     id;
    uint8_t       die_u8 = 0u;

    (void)memset(readBuf_u8, 0u, sizeof(readBuf_u8));
    (void)memset(writeBuf_u8, 0xa5, sizeof(writeBuf_u8));
    (void)memset(eraseBuf_u8, 0xFF, sizeof(eraseBuf_u8));

    /*-------------------------------------------------------------------------------------------------------
     obtain access keys and load the key to library
    -------------------------------------------------------------------------------------------------------*/
    QLIB_STATUS_RET_CHECK(QLIB_GetId(qlibContext, &id));
#if QLIB_MAX_DIES_SUPPORTED > 1u
    QLIB_STATUS_RET_CHECK(QLIB_GetActiveDie(qlibContext, &die_u8));
#endif // QLIB_MAX_DIES_SUPPORTED > 1u
    QLIB_STATUS_RET_CHECK(
        QLIB_SAMPLE_GetSectionKeyPtr(qlibContext, id.std.uniqueID, QLIB_KID__FULL_ACCESS_SECTION, section_u32, die_u8, &key));

    QLIB_STATUS_RET_CHECK(QLIB_LoadKey(qlibContext, section_u32, key, (bool)true));

    /*-------------------------------------------------------------------------------------------------------
     Open secure session for the secure commands
    -------------------------------------------------------------------------------------------------------*/
    QLIB_STATUS_RET_CHECK_GOTO(QLIB_OpenSession(qlibContext, section_u32, QLIB_SESSION_ACCESS_FULL), status, remove_key);

    /*-------------------------------------------------------------------------------------------------------
     Secure erase
    -------------------------------------------------------------------------------------------------------*/
    QLIB_STATUS_RET_CHECK_GOTO(QLIB_Erase(qlibContext, section_u32, 0u, QLIB_FLASH_SECTOR_SIZE, (bool)true),
                               status,
                               close_session);

    /*-------------------------------------------------------------------------------------------------------
     Secure write
    -------------------------------------------------------------------------------------------------------*/
    QLIB_STATUS_RET_CHECK_GOTO(QLIB_Write(qlibContext, writeBuf_u8, section_u32, 0u, sizeof(writeBuf_u8), (bool)true),
                               status,
                               close_session);

    /*-------------------------------------------------------------------------------------------------------
     Non-secure read is not allowed
    -------------------------------------------------------------------------------------------------------*/
    QLIB_SAMPLE_ALLOW_TO_FAIL__START();
    status = QLIB_Read(qlibContext, readBuf_u8, section_u32, 0u, sizeof(readBuf_u8), (bool)false, (bool)false);
    QLIB_SAMPLE_ALLOW_TO_FAIL__END();
    QLIB_ASSERT_WITH_ERROR_GOTO(status == QLIB_STATUS__DEVICE_PRIVILEGE_ERR, QLIB_STATUS__TEST_FAIL, status, close_session);

    /*-------------------------------------------------------------------------------------------------------
     Verify data
    -------------------------------------------------------------------------------------------------------*/
    QLIB_ASSERT_WITH_ERROR_GOTO(0u != memcmp(readBuf_u8, writeBuf_u8, sizeof(writeBuf_u8)),
                                QLIB_STATUS__TEST_FAIL,
                                status,
                                close_session);

    /*-------------------------------------------------------------------------------------------------------
     Non-secure erase is not allowed
    -------------------------------------------------------------------------------------------------------*/
    QLIB_SAMPLE_ALLOW_TO_FAIL__START();
    status = QLIB_Erase(qlibContext, section_u32, 0u, QLIB_FLASH_SECTOR_SIZE, (bool)false);
    QLIB_SAMPLE_ALLOW_TO_FAIL__END();
    QLIB_ASSERT_WITH_ERROR_GOTO(status == QLIB_STATUS__DEVICE_PRIVILEGE_ERR, QLIB_STATUS__TEST_FAIL, status, close_session);

    /*-------------------------------------------------------------------------------------------------------
     Secure read
    -------------------------------------------------------------------------------------------------------*/
    QLIB_STATUS_RET_CHECK_GOTO(QLIB_Read(qlibContext, readBuf_u8, section_u32, 0u, sizeof(readBuf_u8), (bool)true, (bool)false),
                               status,
                               close_session);

    /*-------------------------------------------------------------------------------------------------------
     Verify data
    -------------------------------------------------------------------------------------------------------*/
    QLIB_ASSERT_WITH_ERROR_GOTO(0u == memcmp(readBuf_u8, writeBuf_u8, sizeof(writeBuf_u8)),
                                QLIB_STATUS__TEST_FAIL,
                                status,
                                close_session);

    /*-------------------------------------------------------------------------------------------------------
     Note: user can verify section data with integrity check as well by using CALC_SIG command and comparing
     the digest with an expected digest value
    -------------------------------------------------------------------------------------------------------*/

    status = QLIB_STATUS__OK;

close_session:
    (void)QLIB_CloseSession(qlibContext, section_u32);

remove_key:
    (void)QLIB_RemoveKey(qlibContext, section_u32, (bool)true);

    return status;
}

QLIB_STATUS_T QLIB_SAMPLE_VaultRestrictedKey(QLIB_CONTEXT_T* qlibContext)
{
    QLIB_STATUS_T status      = QLIB_STATUS__OK;
    uint32_t      section_u32 = QLIB_SECTION_ID_VAULT;
    CONST_KEY_P_T fullAccessKey;
    CONST_KEY_P_T restrictedAccessKey;
    uint8_t       writeBuf_u8[QLIB_FLASH_PAGE_SIZE];
    uint8_t       readBuf_u8[QLIB_FLASH_PAGE_SIZE];
    uint8_t       eraseBuf_u8[QLIB_FLASH_PAGE_SIZE];
    bool          isFullAccess_b = (bool)true;
    QLIB_ID_T     id;
    uint8_t       die_u8 = 0u;

    (void)memset(readBuf_u8, 0u, sizeof(readBuf_u8));
    (void)memset(writeBuf_u8, 0xa5, sizeof(writeBuf_u8));
    (void)memset(eraseBuf_u8, 0xFF, sizeof(eraseBuf_u8));

    /*-------------------------------------------------------------------------------------------------------
     Set flash data at start of test with full key
    -------------------------------------------------------------------------------------------------------*/
    /*-------------------------------------------------------------------------------------------------------
     obtain access keys and load the keys to library
    -------------------------------------------------------------------------------------------------------*/
#if QLIB_MAX_DIES_SUPPORTED > 1u
    QLIB_STATUS_RET_CHECK(QLIB_GetActiveDie(qlibContext, &die_u8));
#endif // QLIB_MAX_DIES_SUPPORTED > 1u
    QLIB_STATUS_RET_CHECK(QLIB_SAMPLE_GetSectionKeyPtr(qlibContext,
                                                       id.std.uniqueID,
                                                       QLIB_KID__FULL_ACCESS_SECTION,
                                                       section_u32,
                                                       die_u8,
                                                       &fullAccessKey));
    QLIB_STATUS_RET_CHECK(QLIB_LoadKey(qlibContext, section_u32, fullAccessKey, (bool)true));
    QLIB_STATUS_RET_CHECK_GOTO(QLIB_OpenSession(qlibContext, section_u32, QLIB_SESSION_ACCESS_FULL), status, remove_key);
    QLIB_STATUS_RET_CHECK_GOTO(QLIB_Erase(qlibContext, section_u32, 0u, QLIB_FLASH_SECTOR_SIZE, (bool)true),
                               status,
                               close_session);
    QLIB_STATUS_RET_CHECK_GOTO(QLIB_Write(qlibContext, writeBuf_u8, section_u32, 0u, sizeof(writeBuf_u8), (bool)true),
                               status,
                               close_session);
    QLIB_STATUS_RET_CHECK_GOTO(QLIB_CloseSession(qlibContext, section_u32), status, remove_key);
    QLIB_STATUS_RET_CHECK(QLIB_RemoveKey(qlibContext, section_u32, (bool)true));

    /*-------------------------------------------------------------------------------------------------------
     Load the restricted key for the secure read operations
    -------------------------------------------------------------------------------------------------------*/
    QLIB_STATUS_RET_CHECK(QLIB_SAMPLE_GetSectionKeyPtr(qlibContext,
                                                       id.std.uniqueID,
                                                       QLIB_KID__RESTRICTED_ACCESS_SECTION,
                                                       section_u32,
                                                       die_u8,
                                                       &restrictedAccessKey));
    QLIB_STATUS_RET_CHECK(QLIB_LoadKey(qlibContext, section_u32, restrictedAccessKey, (bool)false));
    isFullAccess_b = (bool)false;

    /*-------------------------------------------------------------------------------------------------------
     Open secure session for the secure commands
    -------------------------------------------------------------------------------------------------------*/
    QLIB_STATUS_RET_CHECK_GOTO(QLIB_OpenSession(qlibContext, section_u32, QLIB_SESSION_ACCESS_RESTRICTED), status, remove_key);

    /*-------------------------------------------------------------------------------------------------------
     Secure read
    -------------------------------------------------------------------------------------------------------*/
    QLIB_STATUS_RET_CHECK_GOTO(QLIB_Read(qlibContext, readBuf_u8, section_u32, 0u, sizeof(readBuf_u8), (bool)true, (bool)false),
                               status,
                               close_session);

    /*-------------------------------------------------------------------------------------------------------
     Verify data
    -------------------------------------------------------------------------------------------------------*/
    QLIB_ASSERT_WITH_ERROR_GOTO(0u == memcmp(readBuf_u8, writeBuf_u8, sizeof(writeBuf_u8)),
                                QLIB_STATUS__TEST_FAIL,
                                status,
                                close_session);

    /*-------------------------------------------------------------------------------------------------------
     Secure erase is not allowed
    -------------------------------------------------------------------------------------------------------*/
    QLIB_SAMPLE_ALLOW_TO_FAIL__START();
    status = QLIB_Erase(qlibContext, section_u32, 0u, QLIB_FLASH_SECTOR_SIZE, (bool)true);
    QLIB_SAMPLE_ALLOW_TO_FAIL__END();
    QLIB_ASSERT_WITH_ERROR_GOTO(status == QLIB_STATUS__DEVICE_SESSION_ERR, QLIB_STATUS__TEST_FAIL, status, close_session);

    /*-------------------------------------------------------------------------------------------------------
     Secure read
    -------------------------------------------------------------------------------------------------------*/
    QLIB_STATUS_RET_CHECK_GOTO(QLIB_Read(qlibContext, readBuf_u8, section_u32, 0u, sizeof(readBuf_u8), (bool)true, (bool)false),
                               status,
                               close_session);

    /*-------------------------------------------------------------------------------------------------------
     Verify data
    -------------------------------------------------------------------------------------------------------*/
    QLIB_ASSERT_WITH_ERROR_GOTO(0u == memcmp(readBuf_u8, writeBuf_u8, sizeof(writeBuf_u8)),
                                QLIB_STATUS__TEST_FAIL,
                                status,
                                close_session);

    /*-------------------------------------------------------------------------------------------------------
     Secure write is not allowed
    -------------------------------------------------------------------------------------------------------*/
    QLIB_SAMPLE_ALLOW_TO_FAIL__START();
    status = QLIB_Write(qlibContext, writeBuf_u8, section_u32, 0u, sizeof(writeBuf_u8), (bool)true);
    QLIB_SAMPLE_ALLOW_TO_FAIL__END();
    QLIB_ASSERT_WITH_ERROR_GOTO(status == QLIB_STATUS__DEVICE_SESSION_ERR, QLIB_STATUS__TEST_FAIL, status, close_session);

    status = QLIB_STATUS__OK;

close_session:
    (void)QLIB_CloseSession(qlibContext, section_u32);

remove_key:
    (void)QLIB_RemoveKey(qlibContext, section_u32, isFullAccess_b);

    return status;
}

#endif // QLIB_ONLY_SECURE_STORAGE
