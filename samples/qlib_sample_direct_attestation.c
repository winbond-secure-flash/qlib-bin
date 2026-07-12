/************************************************************************************************************
* @internal
* @remark     Winbond Electronics Corporation - Confidential
* @copyright  Copyright (c) 2020 by Winbond Electronics Corporation . All rights reserved
* @endinternal
*
* @file       qlib_sample_direct_attestation.c
* @brief      This file contains QLIB direct attestation sample code
*
* @example    qlib_sample_direct_attestation.c
*
* @page       direct_attestation Remote attestation sample code
* This sample code shows how direct attestation feature can be implemented using w77q/t.\n
*
* @include    samples/qlib_sample_direct_attestation.c
*
************************************************************************************************************/

/*-----------------------------------------------------------------------------------------------------------
                                                   INCLUDES
-----------------------------------------------------------------------------------------------------------*/
#include "qlib.h"
#include "qlib_sample_direct_attestation.h"
#include "qlib_sample.h"

#ifndef QLIB_ONLY_SECURE_STORAGE
#include "qlib_utils_digest.h"

/*-----------------------------------------------------------------------------------------------------------
                                             INTERFACE FUNCTIONS
-----------------------------------------------------------------------------------------------------------*/

QLIB_STATUS_T QLIB_SAMPLE_DirectAttestation(void* userData_pv)
{
    QLIB_CONTEXT_T    qlibContext;
    QLIB_STATUS_T     status    = QLIB_STATUS__OK;
    QLIB_BUS_FORMAT_T busFormat = QLIB_BUS_FORMAT(QLIB_BUS_MODE_1_1_1, (bool)false);

    /*-------------------------------------------------------------------------------------------------------
     Initialize QLIB  - needed when running QLIB either on a device or on a remote server.
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
     configuration for sample
    -------------------------------------------------------------------------------------------------------*/
    QLIB_STATUS_RET_CHECK_GOTO(QLIB_SAMPLE_DirectAttestation_manuf_flow(&qlibContext), status, disconnect);

    /*-------------------------------------------------------------------------------------------------------
     Run sample flow
    -------------------------------------------------------------------------------------------------------*/
    QLIB_STATUS_RET_CHECK_GOTO(QLIB_SAMPLE_DirectAttestation_field_flow(&qlibContext), status, disconnect);

disconnect:
    (void)QLIB_Disconnect(&qlibContext);
    return status;
}

QLIB_STATUS_T QLIB_SAMPLE_DirectAttestation_manuf_flow(QLIB_CONTEXT_T* qlibContext)
{
    QLIB_HW_VER_T                 hwVer;
    QLIB_ID_T                     id;
    uint32_t                      sect_u32;
    uint8_t                       die_u8   = 0u;
    QLIB_FLASH_CONFIG_T           flashCfg = {0u};
    QLIB_DIE_CONFIG_T             dieCfg   = {0u};
    QLIB_SECTIONS_CONF_TABLE_T    cfgLine[QLIB_MAX_DIES_SUPPORTED];
    QLIB_EXTENDED_SECTION_CONF_T* sectCfg;
    uint8_t                       diesNum_u8                        = QLIB_GET_NUM_OF_DIES(qlibContext);
    uint8_t                       writeBuf_u8[QLIB_FLASH_PAGE_SIZE] = {0u};
    QLIB_STATUS_T                 status                            = QLIB_STATUS__OK;
    uint64_t                      digest_u64                        = 0u;

#if QLIB_MAX_DIES_SUPPORTED > 1u
    QLIB_GetActiveDie(qlibContext, &die_u8);
#endif // QLIB_MAX_DIES_SUPPORTED > 1u

    // See what chip is connected and its unique ids. We might need this info to obtain keys for this
    // chips according to formula or via DB. see QLIB_SAMPLE_GetDeviceKey() function
    QLIB_STATUS_RET_CHECK(QLIB_GetHWVersion(qlibContext, &hwVer));
    QLIB_STATUS_RET_CHECK(QLIB_GetId(qlibContext, &id));

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

    // setting layout and policy of the section
    // this section is with digest enabled
    sect_u32 = QLIB_SAMPLE_DIRECT_ATTEST_SECTION;

    sectCfg = cfgLine[die_u8].sectionConfigTable;

    sectCfg[sect_u32].baseAddr = QLIB_SAMPLE_RESERVED_BOOT_SECTION_SIZE;
    sectCfg[sect_u32].size     = SIZE_256KB_;

    // first create the section with digest disabled
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
    QLIB_STATUS_RET_CHECK(QLIB_SAMPLE_GetSectionAccessKey(qlibContext,
                                                          id.std.uniqueID,
                                                          QLIB_KID__FULL_ACCESS_SECTION,
                                                          sect_u32,
                                                          die_u8,
                                                          sectCfg[sect_u32].fullAccessKey));

    // write configuration
    QLIB_STATUS_RET_CHECK(QLIB_ConfigDeviceMultiDie(qlibContext, &flashCfg, &dieCfg, cfgLine, diesNum_u8));

    // write configuration sets die to 0. return to orig die
#if QLIB_MAX_DIES_SUPPORTED > 1u
    QLIB_STATUS_RET_CHECK(QLIB_SetActiveDie(qlibContext, die_u8));
#endif // QLIB_MAX_DIES_SUPPORTED > 1u

    // now set section data and enable digest
    QLIB_STATUS_RET_CHECK(QLIB_EraseSection(qlibContext, sect_u32, (bool)false));
    QLIB_STATUS_RET_CHECK(QLIB_Write(qlibContext, writeBuf_u8, sect_u32, 0u, QLIB_FLASH_PAGE_SIZE, (bool)false));
    QLIB_STATUS_RET_CHECK(QLIB_UTILS_CalcDigestWithPadding(writeBuf_u8,
                                                           QLIB_FLASH_PAGE_SIZE,
                                                           0xFF,
                                                           sectCfg[sect_u32].size - QLIB_FLASH_PAGE_SIZE,
                                                           &digest_u64,
                                                           NULL));

    sectCfg[sect_u32].sectionConfig.policy.plainAccessReadEnable  = 1u;
    sectCfg[sect_u32].sectionConfig.policy.plainAccessWriteEnable = 0u;
    sectCfg[sect_u32].sectionConfig.policy.digestIntegrity        = 1u;
    sectCfg[sect_u32].sectionConfig.policy.writeProt              = 1u;
    sectCfg[sect_u32].sectionConfig.digest                        = digest_u64;
    sectCfg[sect_u32].sectionConfig.postActions.reload            = 0u;
    sectCfg[sect_u32].sectionConfig.postActions.reset             = 0u;
    sectCfg[sect_u32].sectionConfig.postActions.swap              = 0u;

    QLIB_STATUS_RET_CHECK(QLIB_LoadKey(qlibContext, sect_u32, sectCfg[sect_u32].fullAccessKey, (bool)true));
    QLIB_STATUS_RET_CHECK_GOTO(QLIB_OpenSession(qlibContext, sect_u32, QLIB_SESSION_ACCESS_FULL), status, remove_key);
    QLIB_STATUS_RET_CHECK_GOTO(QLIB_ConfigDeviceSection(qlibContext, sect_u32, &sectCfg[sect_u32].sectionConfig),
                               status,
                               close_session);
    QLIB_STATUS_RET_CHECK(QLIB_RemoveKey(qlibContext, sect_u32, (bool)true));

    return QLIB_STATUS__OK;

close_session:
    (void)QLIB_CloseSession(qlibContext, sect_u32);

remove_key:
    (void)QLIB_RemoveKey(qlibContext, sect_u32, (bool)true);

    return status;
}

QLIB_STATUS_T QLIB_SAMPLE_DirectAttestation_field_flow(QLIB_CONTEXT_T* qlibContext)
{
    uint32_t      section_u32;
    QLIB_ID_T     id;
    CONST_KEY_P_T sectionKey;
    uint8_t       die_u8 = 0u;
    QLIB_STATUS_T status = QLIB_STATUS__OK;

    /*-------------------------------------------------------------------------------------------------------
     The section should be set with digest integrity
    -------------------------------------------------------------------------------------------------------*/
    section_u32 = QLIB_SAMPLE_DIRECT_ATTEST_SECTION;

    /*-------------------------------------------------------------------------------------------------------
     Access to the Section must be established before using this command.
     It can be either by granting plain access or via session with restricted or full key.
    -------------------------------------------------------------------------------------------------------*/
    QLIB_STATUS_RET_CHECK(QLIB_GetId(qlibContext, &id));
#if QLIB_MAX_DIES_SUPPORTED > 1u
    QLIB_STATUS_RET_CHECK(QLIB_GetActiveDie(qlibContext, &die_u8));
#endif // QLIB_MAX_DIES_SUPPORTED > 1
    QLIB_STATUS_RET_CHECK(QLIB_SAMPLE_GetSectionKeyPtr(qlibContext,
                                                       id.std.uniqueID,
                                                       QLIB_KID__RESTRICTED_ACCESS_SECTION,
                                                       section_u32,
                                                       die_u8,
                                                       &sectionKey));
    QLIB_STATUS_RET_CHECK(QLIB_LoadKey(qlibContext, section_u32, sectionKey, (bool)false));
    QLIB_STATUS_RET_CHECK_GOTO(QLIB_OpenSession(qlibContext, section_u32, QLIB_SESSION_ACCESS_RESTRICTED), status, remove_key);

    /*-------------------------------------------------------------------------------------------------------
     Check section integrity using digest
    -------------------------------------------------------------------------------------------------------*/
    QLIB_STATUS_RET_CHECK_GOTO(QLIB_CheckIntegrity(qlibContext, section_u32, QLIB_INTEGRITY_DIGEST), status, close_session);

close_session:
    (void)QLIB_CloseSession(qlibContext, section_u32);

remove_key:
    (void)QLIB_RemoveKey(qlibContext, section_u32, (bool)false);
    return status;
}

#endif // QLIB_ONLY_SECURE_STORAGE
