/************************************************************************************************************
* @internal
* @remark     Winbond Electronics Corporation - Confidential
* @copyright  Copyright (c) 2026 by Winbond Electronics Corporation . All rights reserved
* @endinternal
*
* @file       qlib_sample_oem_section_provisioning.c
* @brief      This file contains QLIB sample code for SoC to OEM section key provisioning flow
*
* @example    qlib_sample_oem_section_provisioning.c
*
* @page       oem_section_provisioning OEM section provisioning sample code
* This sample code shows how the SoC configures flash sections and derives a section provisioning key,
* then how the OEM uses that key to provision section keys and policy.\n
*
* @include    samples/qlib_sample_oem_section_provisioning.c
*
************************************************************************************************************/

/*-----------------------------------------------------------------------------------------------------------
                                                   INCLUDES
-----------------------------------------------------------------------------------------------------------*/
#include "qlib.h"
#include "qlib_sample.h"
#include "qlib_sample_oem_section_provisioning.h"

#ifndef QLIB_ONLY_SECURE_STORAGE
#ifdef OEM_SECTION_PROVISIONING
/*-----------------------------------------------------------------------------------------------------------
                                                 DEFINITIONS
-----------------------------------------------------------------------------------------------------------*/
#define QLIB_SAMPLE_SOC_SECTION_ID   (0u)
#define QLIB_SAMPLE_OEM_SECTION_ID   (2u)
#define QLIB_SAMPLE_SOC_SECTION_SIZE (SIZE_1MB_)
#define QLIB_SAMPLE_OEM_SECTION_SIZE (SIZE_512KB_)

/*-----------------------------------------------------------------------------------------------------------
                                             INTERFACE FUNCTIONS
-----------------------------------------------------------------------------------------------------------*/

QLIB_STATUS_T QLIB_SAMPLE_SOC_flow(QLIB_CONTEXT_T* qlibContext, uint32_t* oemSectionId_pu32, KEY_T sectionProvisionKey)
{
    QLIB_ID_T                     id;
    QLIB_FLASH_CONFIG_T           flashCfg = {0u};
    QLIB_DIE_CONFIG_T             dieCfg   = {0u};
    QLIB_SECTIONS_CONF_TABLE_T    cfgLine[QLIB_MAX_DIES_SUPPORTED];
    QLIB_EXTENDED_SECTION_CONF_T* sectCfg_ps;
    uint8_t                       die_u8     = 0u;
    uint8_t                       diesNum_u8 = QLIB_GET_NUM_OF_DIES(qlibContext);

    QLIB_ASSERT_RET(qlibContext != NULL, QLIB_STATUS__INVALID_PARAMETER);
    QLIB_ASSERT_RET(sectionProvisionKey != NULL, QLIB_STATUS__INVALID_PARAMETER);
    QLIB_ASSERT_RET(oemSectionId_pu32 != NULL, QLIB_STATUS__INVALID_PARAMETER);

    /*-------------------------------------------------------------------------------------------------------
     Get flash IDs - used for key derivation
    -------------------------------------------------------------------------------------------------------*/
    QLIB_STATUS_RET_CHECK(QLIB_GetId(qlibContext, &id));

    /*-------------------------------------------------------------------------------------------------------
     Read current device configuration (read-modify-write)
    -------------------------------------------------------------------------------------------------------*/
    QLIB_STATUS_RET_CHECK(QLIB_GetDeviceConfigMultiDie(qlibContext, &flashCfg, &dieCfg, cfgLine, diesNum_u8));

    /*-------------------------------------------------------------------------------------------------------
     Set the pre-provisioned (current) master key so the device accepts the new configuration
    -------------------------------------------------------------------------------------------------------*/
    QLIB_STATUS_RET_CHECK(
        QLIB_SAMPLE_GetDeviceKey(qlibContext, id.sec.wid, QLIB_KID__DEVICE_KEY_PRE_PROVISIONING, dieCfg.preProvisionedMasterKey));

    /*-------------------------------------------------------------------------------------------------------
     Set the new device master key
    -------------------------------------------------------------------------------------------------------*/
    QLIB_STATUS_RET_CHECK(QLIB_SAMPLE_GetDeviceKey(qlibContext, id.sec.wid, QLIB_KID__DEVICE_MASTER, dieCfg.deviceMasterKey));

#if QLIB_MAX_DIES_SUPPORTED > 1u
    QLIB_STATUS_RET_CHECK(QLIB_SetActiveDie(qlibContext, die_u8));
#endif // QLIB_MAX_DIES_SUPPORTED > 1u

    sectCfg_ps = cfgLine[die_u8].sectionConfigTable;

    /*-------------------------------------------------------------------------------------------------------
     Configure section 0: set base address, size, policy and access keys.
     Section 0 is the SoC-owned firmware section (rollback protected, plain-read allowed).
    -------------------------------------------------------------------------------------------------------*/
    sectCfg_ps[QLIB_SAMPLE_SOC_SECTION_ID].baseAddr = 0u;
    sectCfg_ps[QLIB_SAMPLE_SOC_SECTION_ID].size     = QLIB_SAMPLE_SOC_SECTION_SIZE;

    sectCfg_ps[QLIB_SAMPLE_SOC_SECTION_ID].sectionConfig.policy.plainAccessReadEnable   = 1u;
    sectCfg_ps[QLIB_SAMPLE_SOC_SECTION_ID].sectionConfig.policy.plainAccessWriteEnable  = 0u;
    sectCfg_ps[QLIB_SAMPLE_SOC_SECTION_ID].sectionConfig.policy.authPlainAccess         = 0u;
    sectCfg_ps[QLIB_SAMPLE_SOC_SECTION_ID].sectionConfig.policy.checksumIntegrity       = 0u;
    sectCfg_ps[QLIB_SAMPLE_SOC_SECTION_ID].sectionConfig.policy.digestIntegrity         = 0u;
    sectCfg_ps[QLIB_SAMPLE_SOC_SECTION_ID].sectionConfig.policy.digestIntegrityOnAccess = 0u;
    sectCfg_ps[QLIB_SAMPLE_SOC_SECTION_ID].sectionConfig.policy.rollbackProt            = 1u;
    sectCfg_ps[QLIB_SAMPLE_SOC_SECTION_ID].sectionConfig.policy.slog                    = 0u;
    sectCfg_ps[QLIB_SAMPLE_SOC_SECTION_ID].sectionConfig.policy.writeProt               = 0u;

    QLIB_STATUS_RET_CHECK(QLIB_SAMPLE_GetSectionAccessKey(qlibContext,
                                                          id.sec.wid,
                                                          QLIB_KID__FULL_ACCESS_SECTION,
                                                          QLIB_SAMPLE_SOC_SECTION_ID,
                                                          die_u8,
                                                          sectCfg_ps[QLIB_SAMPLE_SOC_SECTION_ID].fullAccessKey));

    QLIB_STATUS_RET_CHECK(QLIB_SAMPLE_GetSectionAccessKey(qlibContext,
                                                          id.sec.wid,
                                                          QLIB_KID__RESTRICTED_ACCESS_SECTION,
                                                          QLIB_SAMPLE_SOC_SECTION_ID,
                                                          die_u8,
                                                          sectCfg_ps[QLIB_SAMPLE_SOC_SECTION_ID].restrictedKey));

    /*-------------------------------------------------------------------------------------------------------
     Configure OEM section : set base address and size only.
     Keys and policy for OEM section are provisioned by the OEM using the provisioning key.
    -------------------------------------------------------------------------------------------------------*/
    sectCfg_ps[QLIB_SAMPLE_OEM_SECTION_ID].baseAddr = QLIB_SAMPLE_SOC_SECTION_SIZE;
    sectCfg_ps[QLIB_SAMPLE_OEM_SECTION_ID].size     = QLIB_SAMPLE_OEM_SECTION_SIZE;

    /*-------------------------------------------------------------------------------------------------------
     Set address size to suit the plain section size
    -------------------------------------------------------------------------------------------------------*/
    flashCfg.stdAddrSize.addrLen_u8 = 22u;

    if (QLIB_FeatureSupported(qlibContext, W77Q_FEATURE_DEVCFG_LOCK))
    {
        /*---------------------------------------------------------------------------------------------------
         Lock the section configuration
        ---------------------------------------------------------------------------------------------------*/
        dieCfg.devcfgLock = (bool)true;
    }

    /*-------------------------------------------------------------------------------------------------------
     Write the complete device configuration to flash
    -------------------------------------------------------------------------------------------------------*/
    QLIB_STATUS_RET_CHECK(QLIB_ConfigDeviceMultiDie(qlibContext, &flashCfg, &dieCfg, cfgLine, diesNum_u8));

    /*-------------------------------------------------------------------------------------------------------
     Derive the OEM section provisioning key from Kd and return it to the caller for secure transfer to OEM.
     Also output the OEM section ID so the caller knows which section to provision.
    -------------------------------------------------------------------------------------------------------*/
    QLIB_STATUS_RET_CHECK(QLIB_GetSectionProvisionKey(QLIB_SAMPLE_OEM_SECTION_ID, dieCfg.deviceMasterKey, sectionProvisionKey));

    *oemSectionId_pu32 = QLIB_SAMPLE_OEM_SECTION_ID;

    return QLIB_STATUS__OK;
}

QLIB_STATUS_T QLIB_SAMPLE_OEM_flow(QLIB_CONTEXT_T* qlibContext, uint32_t sectionId_u32, const KEY_T sectionProvisionKey)
{
    QLIB_ID_T           id;
    QLIB_DIE_CONFIG_T   dieCfg;
    QLIB_FLASH_CONFIG_T flashCfg;
    QLIB_SECTION_CONF_T sectionConf = {0u};
    QLIB_STATUS_T       status      = QLIB_STATUS__OK;
    KEY_T               sectionFullAccessKey;
    KEY_T               sectionRestrictedKey;
#ifndef EXCLUDE_LMS
    LMS_PUB_KEY_T  sectionLmsKey;
    const uint8_t* pLmsKey_pu8 = QLIB_FeatureSupported(qlibContext, W77Q_FEATURE_LMS) == 1u ? sectionLmsKey : NULL;
#else
    const uint8_t* pLmsKey_pu8 = NULL;
#endif // EXCLUDE_LMS
    uint8_t die_u8 = 0u;

    QLIB_ASSERT_RET(qlibContext != NULL, QLIB_STATUS__INVALID_PARAMETER);
    QLIB_ASSERT_RET(sectionProvisionKey != NULL, QLIB_STATUS__INVALID_PARAMETER);

    /*-------------------------------------------------------------------------------------------------------
     Get flash IDs for key derivation
    -------------------------------------------------------------------------------------------------------*/
    QLIB_STATUS_RET_CHECK(QLIB_GetId(qlibContext, &id));

    /*-------------------------------------------------------------------------------------------------------
     Derive the OEM section access keys (full and restricted)
    -------------------------------------------------------------------------------------------------------*/
    QLIB_STATUS_RET_CHECK(QLIB_SAMPLE_GetSectionAccessKey(qlibContext,
                                                          id.sec.wid,
                                                          QLIB_KID__FULL_ACCESS_SECTION,
                                                          sectionId_u32,
                                                          die_u8,
                                                          sectionFullAccessKey));

    QLIB_STATUS_RET_CHECK(QLIB_SAMPLE_GetSectionAccessKey(qlibContext,
                                                          id.sec.wid,
                                                          QLIB_KID__RESTRICTED_ACCESS_SECTION,
                                                          sectionId_u32,
                                                          die_u8,
                                                          sectionRestrictedKey));

#ifndef EXCLUDE_LMS
    QLIB_STATUS_RET_CHECK(QLIB_SAMPLE_GetSectionLmsKey(qlibContext, id.sec.wid, sectionId_u32, die_u8, sectionLmsKey));
#endif // EXCLUDE_LMS

    /*-------------------------------------------------------------------------------------------------------
     Provision OEM section keys using the provisioning key received from SoC
    -------------------------------------------------------------------------------------------------------*/
    QLIB_STATUS_RET_CHECK(QLIB_SetSectionKeys(qlibContext,
                                              sectionId_u32,
                                              sectionProvisionKey,
                                              sectionFullAccessKey,
                                              sectionRestrictedKey,
                                              pLmsKey_pu8));

    /*-------------------------------------------------------------------------------------------------------
     Load the full access key and open a FULL session to configure the section policy
    -------------------------------------------------------------------------------------------------------*/
    QLIB_STATUS_RET_CHECK(QLIB_LoadKey(qlibContext, sectionId_u32, sectionFullAccessKey, (bool)true));

    QLIB_STATUS_RET_CHECK_GOTO(QLIB_OpenSession(qlibContext, sectionId_u32, QLIB_SESSION_ACCESS_CONFIG_ONLY), status, remove_key);

    /*-------------------------------------------------------------------------------------------------------
     Set the OEM section policy
    -------------------------------------------------------------------------------------------------------*/
    sectionConf.policy.plainAccessReadEnable   = 0u;
    sectionConf.policy.plainAccessWriteEnable  = 0u;
    sectionConf.policy.authPlainAccess         = 0u;
    sectionConf.policy.checksumIntegrity       = 0u;
    sectionConf.policy.digestIntegrity         = 0u;
    sectionConf.policy.digestIntegrityOnAccess = 0u;
    sectionConf.policy.rollbackProt            = 0u;
    sectionConf.policy.slog                    = 0u;
    sectionConf.policy.writeProt               = 0u;

    /*-------------------------------------------------------------------------------------------------------
     Keep the session open after configuration so the lock check ahead is done with authenticated command
    -------------------------------------------------------------------------------------------------------*/
    sectionConf.postActions.reload = 1u;

    QLIB_STATUS_RET_CHECK_GOTO(QLIB_ConfigDeviceSection(qlibContext, sectionId_u32, &sectionConf), status, close_session);

    if (QLIB_FeatureSupported(qlibContext, W77Q_FEATURE_DEVCFG_LOCK))
    {
        /*---------------------------------------------------------------------------------------------------
         Verify DEVCFG.CFG_LOCK is locked so the SoC cannot modify section mapping.
         GMC is read with authentication since a session is open.
        ---------------------------------------------------------------------------------------------------*/
        QLIB_STATUS_RET_CHECK_GOTO(QLIB_GetDeviceConfigMultiDie(qlibContext,
                                                                &flashCfg,
                                                                &dieCfg,
                                                                NULL,
                                                                QLIB_GET_NUM_OF_DIES(qlibContext)),
                                   status,
                                   close_session);

        QLIB_ASSERT_WITH_ERROR_GOTO(dieCfg.devcfgLock == (bool)true,
                                    QLIB_STATUS__SYSTEM_IN_INCORRECT_STATE,
                                    status,
                                    close_session);
    }
    QLIB_STATUS_RET_CHECK_GOTO(QLIB_CloseSession(qlibContext, sectionId_u32), status, remove_key);

    QLIB_STATUS_RET_CHECK(QLIB_RemoveKey(qlibContext, sectionId_u32, (bool)true));

    return QLIB_STATUS__OK;

close_session:
    (void)QLIB_CloseSession(qlibContext, sectionId_u32);
remove_key:
    (void)QLIB_RemoveKey(qlibContext, sectionId_u32, (bool)true);

    return status;
}

QLIB_STATUS_T QLIB_SAMPLE_SoC_OEM_SectionProvision(void* userData_pv)
{
    QLIB_CONTEXT_T    qlibContext;
    QLIB_STATUS_T     status    = QLIB_STATUS__OK;
    QLIB_BUS_FORMAT_T busFormat = QLIB_BUS_FORMAT(QLIB_BUS_MODE_1_1_1, (bool)false);
    KEY_T             sectionProvisionKey;
    uint32_t          oemSectionId_u32 = 0u;

    /*-------------------------------------------------------------------------------------------------------
     Init QLIB - needed when running QLIB either on a device or on a remote server.
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
     SoC flow: configure SoC and OEM sections, set master key, derive section provisioning key and section ID
    -------------------------------------------------------------------------------------------------------*/
    QLIB_STATUS_RET_CHECK_GOTO(QLIB_SAMPLE_SOC_flow(&qlibContext, &oemSectionId_u32, sectionProvisionKey), status, disconnect);

    /*-------------------------------------------------------------------------------------------------------
     OEM flow: receive provisioning key and section ID, set section keys and policy, verify CFG_LOCK
    -------------------------------------------------------------------------------------------------------*/
    QLIB_STATUS_RET_CHECK_GOTO(QLIB_SAMPLE_OEM_flow(&qlibContext, oemSectionId_u32, sectionProvisionKey), status, disconnect);

disconnect:
    (void)QLIB_Disconnect(&qlibContext);
    return status;
}
#endif // OEM_SECTION_PROVISIONING
#endif // QLIB_ONLY_SECURE_STORAGE
