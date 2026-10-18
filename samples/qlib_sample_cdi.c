/************************************************************************************************************
* @internal
* @remark     Winbond Electronics Corporation - Confidential
* @copyright  Copyright (c) 2020 by Winbond Electronics Corporation . All rights reserved
* @endinternal
*
* @file       qlib_sample_cdi.c
* @brief      This file contains QLIB cdi sample code
*
* @example    qlib_sample_cdi.c
*
* @page       cdi_attestation cdi attestation sample code
* This sample code shows how to use cdi feature for secure boot implementation .\n
*
* @include    samples/qlib_sample_cdi.c
*
************************************************************************************************************/

/*-----------------------------------------------------------------------------------------------------------
                                                   INCLUDES
-----------------------------------------------------------------------------------------------------------*/
#include <stdio.h>

#include "qlib.h"
#include "qlib_sample_cdi.h"
#include "qlib_sample.h"

#ifndef QLIB_ONLY_SECURE_STORAGE
#include "qlib_utils_digest.h"

/*-----------------------------------------------------------------------------------------------------------
                                             INTERFACE FUNCTIONS
-----------------------------------------------------------------------------------------------------------*/
QLIB_STATUS_T QLIB_SAMPLE_DiceAttestation(void* userData_pv, bool runFieldFlow_b)
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

    if (runFieldFlow_b == (bool)false)
    {
        /*---------------------------------------------------------------------------------------------------
         configuration for sample
        ---------------------------------------------------------------------------------------------------*/
        QLIB_STATUS_RET_CHECK_GOTO(QLIB_SAMPLE_DiceAttestation_manuf_flow(&qlibContext), status, disconnect);
    }
    else
    {
        /*---------------------------------------------------------------------------------------------------
         sample flow
        ---------------------------------------------------------------------------------------------------*/
        QLIB_STATUS_RET_CHECK_GOTO(QLIB_SAMPLE_DiceAttestation_field_flow(&qlibContext), status, disconnect);
    }
disconnect:
    (void)QLIB_Disconnect(&qlibContext);
    return status;
}

QLIB_STATUS_T QLIB_SAMPLE_DiceAttestation_manuf_flow(QLIB_CONTEXT_T* qlibContext)
{
    QLIB_HW_VER_T                 hwVer;
    QLIB_ID_T                     id;
    uint32_t                      sect_u32 = 0u;
    uint8_t                       die_u8   = 0u;
    QLIB_FLASH_CONFIG_T           flashCfg = {0u};
    QLIB_DIE_CONFIG_T             dieCfg   = {0u};
    QLIB_SECTIONS_CONF_TABLE_T    cfgLine[QLIB_MAX_DIES_SUPPORTED];
    QLIB_EXTENDED_SECTION_CONF_T* sectCfg;
    uint8_t                       diesNum_u8        = QLIB_GET_NUM_OF_DIES(qlibContext);
    uint32_t                      startConfSect_u32 = 0u;
    QLIB_STATUS_T                 status            = QLIB_STATUS__OK;
    uint8_t                       writeBuf_u8[QLIB_FLASH_PAGE_SIZE];
    uint64_t                      digest_u64 = 0u;

#if QLIB_MAX_DIES_SUPPORTED > 1u
    QLIB_GetActiveDie(qlibContext, &die_u8);
#endif // QLIB_MAX_DIES_SUPPORTED > 1u

    (void)memset(writeBuf_u8, 0xa5, sizeof(writeBuf_u8));

    // See what chip is connected and its unique ids. We might need this info to obtain keys for this
    // chips according to formula or via DB. see QLIB_SAMPLE_GetDeviceKey() function
    QLIB_STATUS_RET_CHECK(QLIB_GetHWVersion(qlibContext, &hwVer));
    QLIB_STATUS_RET_CHECK(QLIB_GetId(qlibContext, &id));

    // We do read / modify /write configuration since we want to change only what is needed
    // read configuration
    QLIB_STATUS_RET_CHECK(QLIB_GetDeviceConfigMultiDie(qlibContext, &flashCfg, &dieCfg, cfgLine, diesNum_u8));

    // modify configuration

    // set pre provision master key if needed
    QLIB_STATUS_RET_CHECK(
        QLIB_SAMPLE_GetDeviceKey(qlibContext, id.sec.wid, QLIB_KID__DEVICE_KEY_PRE_PROVISIONING, dieCfg.preProvisionedMasterKey));

    // set new master key
    QLIB_STATUS_RET_CHECK(QLIB_SAMPLE_GetDeviceKey(qlibContext, id.sec.wid, QLIB_KID__DEVICE_MASTER, dieCfg.deviceMasterKey));

    // set device secret key as it participates in the CDI calculation
    QLIB_STATUS_RET_CHECK(QLIB_SAMPLE_GetDeviceKey(qlibContext, id.sec.wid, QLIB_KID__DEVICE_SECRET, dieCfg.deviceSecretKey));

    // set Secure device ID as it participate in the CDI calculation
    QLIB_STATUS_RET_CHECK(QLIB_SAMPLE_GetSecureUID(qlibContext, id.sec.wid, dieCfg.suid));

    // configure sections 0, 1, 2 with digest
    // first configure without digest, then erase section_u32 contents, write some data, calculate digest and enable it on flash

#ifdef QLIB_SUPPORT_XIP
    // if sample is run in XIP mode, we can not change the boot section configuration.
    // verify section 0 was already configured with digest for this sample
    QLIB_ASSERT_RET((cfgLine[die_u8].sectionConfigTable[0].size > 0u) &&
                        (cfgLine[die_u8].sectionConfigTable[0].sectionConfig.policy.digestIntegrity == 1u),
                    QLIB_STATUS__SYSTEM_IN_INCORRECT_STATE);
    startConfSect_u32 = 1u;

#endif // QLIB_SUPPORT_XIP

#ifdef QLIB_SAMPLE_CDI_PRESERVE_BOOT_SECTION
    // When running on a board with a live boot loader in section 0, skip reconfiguring the boot section policies.
    // Its digest will be computed from existing content below if needed.
    // Set boot section size to 4MB (enough for ~2.2MB boot loader) instead of the full flash.
    startConfSect_u32                                                         = 1u;
    cfgLine[die_u8].sectionConfigTable[QLIB_SAMPLE_CDI_BOOT_SECTION].baseAddr = 0u;
    cfgLine[die_u8].sectionConfigTable[QLIB_SAMPLE_CDI_BOOT_SECTION].size     = QLIB_SAMPLE_RESERVED_BOOT_SECTION_SIZE;
    // Get boot section full access key (needed for digest configuration below)
    QLIB_STATUS_RET_CHECK(
        QLIB_SAMPLE_GetSectionAccessKey(qlibContext,
                                        id.sec.wid,
                                        QLIB_KID__FULL_ACCESS_SECTION,
                                        QLIB_SAMPLE_CDI_BOOT_SECTION,
                                        die_u8,
                                        cfgLine[die_u8].sectionConfigTable[QLIB_SAMPLE_CDI_BOOT_SECTION].fullAccessKey));
#endif // QLIB_SAMPLE_CDI_PRESERVE_BOOT_SECTION

    sectCfg = cfgLine[die_u8].sectionConfigTable;
    for (sect_u32 = startConfSect_u32; sect_u32 < QLIB_SAMPLE_CDI_NUM_SECTIONS; sect_u32++)
    {
        sectCfg[sect_u32].baseAddr = (sect_u32 == 0u ? 0u : QLIB_SAMPLE_RESERVED_BOOT_SECTION_SIZE);
        sectCfg[sect_u32].size     = SIZE_256KB_;

        sectCfg[sect_u32].sectionConfig.policy.checksumIntegrity       = 0u;
        sectCfg[sect_u32].sectionConfig.policy.digestIntegrity         = 0u;
        sectCfg[sect_u32].sectionConfig.policy.digestIntegrityOnAccess = 0u;
        sectCfg[sect_u32].sectionConfig.policy.rollbackProt            = 0u;
        sectCfg[sect_u32].sectionConfig.policy.slog                    = 0u;
        sectCfg[sect_u32].sectionConfig.policy.writeProt               = 0u;

        // Full access key configuration.This is the key to use later for the section configuration and write
        QLIB_STATUS_RET_CHECK(QLIB_SAMPLE_GetSectionAccessKey(qlibContext,
                                                              id.sec.wid,
                                                              QLIB_KID__FULL_ACCESS_SECTION,
                                                              sect_u32,
                                                              die_u8,
                                                              sectCfg[sect_u32].fullAccessKey));
    }

    // write configuration
    QLIB_STATUS_RET_CHECK(QLIB_ConfigDeviceMultiDie(qlibContext, &flashCfg, &dieCfg, cfgLine, diesNum_u8));

    // write configuration sets die to 0. return to orig die
#if QLIB_MAX_DIES_SUPPORTED > 1u
    QLIB_STATUS_RET_CHECK(QLIB_SetActiveDie(qlibContext, die_u8));
#endif // QLIB_MAX_DIES_SUPPORTED > 1u

    // now set sections data and add Write Protected and Authenticity Protected
    QLIB_STATUS_RET_CHECK(QLIB_UTILS_CalcDigestWithPadding(writeBuf_u8,
                                                           QLIB_FLASH_PAGE_SIZE,
                                                           0xFF,
                                                           SIZE_256KB_ - QLIB_FLASH_PAGE_SIZE,
                                                           &digest_u64,
                                                           NULL));
    for (sect_u32 = QLIB_SAMPLE_CDI_BOOT_SECTION; sect_u32 < QLIB_SAMPLE_CDI_NUM_SECTIONS; sect_u32++)
    {
        sectCfg = cfgLine[die_u8].sectionConfigTable;

        // If digestIntegrity is already enabled for this section, no need to reconfigure
        if (sectCfg[sect_u32].sectionConfig.policy.digestIntegrity == 1u)
        {
            continue;
        }

        sectCfg[sect_u32].sectionConfig.policy.digestIntegrity = 1u;
#ifdef QLIB_SAMPLE_CDI_PRESERVE_BOOT_SECTION
        // Don't write-protect the boot section — it must remain writable for boot loader updates
        if (sect_u32 != QLIB_SAMPLE_CDI_BOOT_SECTION)
#endif // QLIB_SAMPLE_CDI_PRESERVE_BOOT_SECTION
            sectCfg[sect_u32].sectionConfig.policy.writeProt = 1u;
        QLIB_STATUS_RET_CHECK(QLIB_LoadKey(qlibContext, sect_u32, sectCfg[sect_u32].fullAccessKey, (bool)true));
        QLIB_STATUS_RET_CHECK_GOTO(QLIB_OpenSession(qlibContext, sect_u32, QLIB_SESSION_ACCESS_FULL), status, remove_key);

#ifdef QLIB_SAMPLE_CDI_PRESERVE_BOOT_SECTION
        if (sect_u32 == QLIB_SAMPLE_CDI_BOOT_SECTION)
        {
            // Boot section: read existing content and compute its digest incrementally.
            // Do NOT erase or write — preserve the boot loader.
            // Section can be large (e.g. 16MB), so we read and hash in page-sized chunks.
            static uint8_t readBuf_u8[QLIB_FLASH_PAGE_SIZE];
            uint32_t       sectionSize_u32 = sectCfg[sect_u32].size;
            uint32_t       offset_u32      = 0u;
            void*          hashCtx_pv      = NULL;
            TYPE_256BIT    hashResult;

            QLIB_SAMPLE_PRINTF("[CDI manuf] section_u32 %u: hashing existing content (%u bytes) in chunks\n",
                               sect_u32,
                               sectionSize_u32);
            QLIB_ASSERT_RET(PLAT_HASH_Init(&hashCtx_pv, QLIB_HASH_OPT_NONE) == 0u, QLIB_STATUS__HARDWARE_FAILURE);
            while (offset_u32 < sectionSize_u32)
            {
                uint32_t chunkSize_u32 = ((sectionSize_u32 - offset_u32) < QLIB_FLASH_PAGE_SIZE) ? (sectionSize_u32 - offset_u32)
                                                                                                 : QLIB_FLASH_PAGE_SIZE;
                status = QLIB_Read(qlibContext, readBuf_u8, sect_u32, offset_u32, chunkSize_u32, (bool)false, (bool)false);
                if (QLIB_STATUS__OK != status)
                {
                    goto close_session;
                }
                QLIB_ASSERT_RET(PLAT_HASH_Update(hashCtx_pv, readBuf_u8, chunkSize_u32) == 0u, QLIB_STATUS__HARDWARE_FAILURE);
                offset_u32 += chunkSize_u32;
            }
            QLIB_ASSERT_RET(PLAT_HASH_Finish(hashCtx_pv, hashResult) == 0u, QLIB_STATUS__HARDWARE_FAILURE);
            (void)memcpy((void*)&digest_u64, (void*)&hashResult[6], sizeof(uint64_t));
            QLIB_SAMPLE_PRINTF("[CDI manuf] section_u32 %u: digest_u64 computed OK\n", sect_u32);
        }
        else
#endif // QLIB_SAMPLE_CDI_PRESERVE_BOOT_SECTION
        {
            QLIB_STATUS_RET_CHECK_GOTO(QLIB_EraseSection(qlibContext, sect_u32, (bool)true), status, close_session);
            QLIB_STATUS_RET_CHECK_GOTO(QLIB_Write(qlibContext, writeBuf_u8, sect_u32, 0u, QLIB_FLASH_PAGE_SIZE, (bool)true),
                                       status,
                                       close_session);
        }

        sectCfg[sect_u32].sectionConfig.digest             = digest_u64;
        sectCfg[sect_u32].sectionConfig.postActions.reload = 1u;
        sectCfg[sect_u32].sectionConfig.postActions.reset  = 0u;
        sectCfg[sect_u32].sectionConfig.postActions.swap   = 0u;

        QLIB_STATUS_RET_CHECK_GOTO(QLIB_ConfigDeviceSection(qlibContext, sect_u32, &sectCfg[sect_u32].sectionConfig),
                                   status,
                                   close_session);

        QLIB_STATUS_RET_CHECK_GOTO(QLIB_CloseSession(qlibContext, sect_u32), status, remove_key);
        QLIB_STATUS_RET_CHECK(QLIB_RemoveKey(qlibContext, sect_u32, (bool)true));
    }

    return QLIB_STATUS__OK;

close_session:
    (void)QLIB_CloseSession(qlibContext, sect_u32);
remove_key:
    (void)QLIB_RemoveKey(qlibContext, sect_u32, (bool)true);
    return status;
}

QLIB_STATUS_T QLIB_SAMPLE_DiceAttestation_field_flow(QLIB_CONTEXT_T* qlibContext)
{
    TYPE_256BIT   nextCdi;
    TYPE_256BIT   prevCdi;
    QLIB_HW_VER_T hwVer;
    QLIB_ID_T     id;
    uint8_t       die_u8 = 0u;
    uint32_t      section_u32;
    KEY_T         keyF;

#if QLIB_MAX_DIES_SUPPORTED > 1u
    QLIB_GetActiveDie(qlibContext, &die_u8);
#endif // QLIB_MAX_DIES_SUPPORTED > 1u

    // See what chip is connected and its unique ids. We might need this info to obtain keys for this
    // chips according to formula or via DB. see QLIB_SAMPLE_GetSectionAccessKey() function
    QLIB_STATUS_RET_CHECK(QLIB_GetHWVersion(qlibContext, &hwVer));
    QLIB_STATUS_RET_CHECK(QLIB_GetId(qlibContext, &id));

    section_u32 = 0u;

    /*-------------------------------------------------------------------------------------------------------
     obtain full access key
    -------------------------------------------------------------------------------------------------------*/
    QLIB_STATUS_RET_CHECK(
        QLIB_SAMPLE_GetSectionAccessKey(qlibContext, id.sec.wid, QLIB_KID__FULL_ACCESS_SECTION, section_u32, die_u8, keyF));

    /*-------------------------------------------------------------------------------------------------------
     to retrieve first CDI in the fw trust chain, section 0u is used
    -------------------------------------------------------------------------------------------------------*/
    QLIB_STATUS_RET_CHECK(QLIB_SAMPLE_GetCdi(qlibContext, nextCdi, NULL, section_u32, keyF));

#if (QLIB_SAMPLE_CDI_NUM_SECTIONS < 3u)
#error "CDI sample assumes sections 0,1,2 configured with digest"
#endif // (QLIB_SAMPLE_CDI_NUM_SECTIONS < 3u)

    /*-------------------------------------------------------------------------------------------------------
      assuming the second firmware resides in section 1u and its policy has digest enabled we use previous(first)
      cdi to obtain next(second) value
     ------------------------------------------------------------------------------------------------------*/
    section_u32 = 1u;
    QLIB_STATUS_RET_CHECK(
        QLIB_SAMPLE_GetSectionAccessKey(qlibContext, id.sec.wid, QLIB_KID__FULL_ACCESS_SECTION, section_u32, die_u8, keyF));
    (void)memcpy(prevCdi, nextCdi, sizeof(TYPE_256BIT));
    QLIB_STATUS_RET_CHECK(QLIB_SAMPLE_GetCdi(qlibContext, nextCdi, prevCdi, section_u32, keyF));

    /*-------------------------------------------------------------------------------------------------------
      assuming the third firmware resides in section 2u and its policy has digest enabled we use previous(second)
      cdi to obtain next (third) value
     ------------------------------------------------------------------------------------------------------*/
    section_u32 = 2u;
    QLIB_STATUS_RET_CHECK(
        QLIB_SAMPLE_GetSectionAccessKey(qlibContext, id.sec.wid, QLIB_KID__FULL_ACCESS_SECTION, section_u32, die_u8, keyF));
    (void)memcpy(prevCdi, nextCdi, sizeof(TYPE_256BIT));
    QLIB_STATUS_RET_CHECK(QLIB_SAMPLE_GetCdi(qlibContext, nextCdi, prevCdi, section_u32, keyF));

    /*-------------------------------------------------------------------------------------------------------
      At this point nextCdi contains last CDI that was derived from three stage boot chain. it can be used further
      in any attestation mechanism. (for instance server ...)
     ------------------------------------------------------------------------------------------------------*/

    return QLIB_STATUS__OK;
}

QLIB_STATUS_T QLIB_SAMPLE_GetCdi(QLIB_CONTEXT_T* qlibContext,
                                 TYPE_256BIT     nextCdi,
                                 TYPE_256BIT     prevCdi,
                                 uint32_t        sectionId_u32,
                                 const KEY_T     sectionFullAccessKey)
{
    QLIB_STATUS_T status = QLIB_STATUS__OK;

    /*-------------------------------------------------------------------------------------------------------
     Load the key for the secure write operations
    -------------------------------------------------------------------------------------------------------*/
    QLIB_STATUS_RET_CHECK(QLIB_LoadKey(qlibContext, sectionId_u32, sectionFullAccessKey, (bool)true));

    /*-------------------------------------------------------------------------------------------------------
     Open secure session for the secure commands
    -------------------------------------------------------------------------------------------------------*/
    QLIB_STATUS_RET_CHECK_GOTO(QLIB_OpenSession(qlibContext, sectionId_u32, QLIB_SESSION_ACCESS_FULL), status, remove_key);

    /*-------------------------------------------------------------------------------------------------------
     Retrieving cdi value
    -------------------------------------------------------------------------------------------------------*/
    QLIB_STATUS_RET_CHECK_GOTO(QLIB_CalcCDI(qlibContext, nextCdi, prevCdi, sectionId_u32), status, close_session);

close_session:
    (void)QLIB_CloseSession(qlibContext, sectionId_u32);

remove_key:
    (void)QLIB_RemoveKey(qlibContext, sectionId_u32, (bool)true);

    return status;
}

#endif // QLIB_ONLY_SECURE_STORAGE
