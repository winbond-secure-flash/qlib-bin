/************************************************************************************************************
* @internal
* @remark     Winbond Electronics Corporation - Confidential
* @copyright  Copyright (c) 2026 by Winbond Electronics Corporation . All rights reserved
* @endinternal
*
* @file       qlib_sample_integrity.c
* @brief      This file contains QLIB integrity check sample code
*
* @example    qlib_sample_integrity.c
*
* @page       integrity Integrity sample code
* This sample code shows how to configure CRC and digest integrity protection on different sections and verify it
*
* @include    samples/qlib_sample_integrity.c
*
************************************************************************************************************/

/*-----------------------------------------------------------------------------------------------------------
                                                   INCLUDES
-----------------------------------------------------------------------------------------------------------*/
#include "qlib.h"
#include "qlib_sample.h"

#ifndef QLIB_ONLY_SECURE_STORAGE
#include "qlib_sample_integrity.h"

/*-----------------------------------------------------------------------------------------------------------
                                                  DEFINITIONS
-----------------------------------------------------------------------------------------------------------*/
#define QLIB_SAMPLE_INTEGRITY_CHECK_SECTION 1u
#define QLIB_SAMPLE_INTEGRITY_TEST_SECTION  2u

/*
 * Pre-calculated CRC and digest for SIZE_512KB_ of 0xFF (erased flash). Used both when
 * configuring the section and when verifying integrity.
 */
#define QLIB_SAMPLE_INTEGRITY_CRC_EXPECTED    0x504bf849u
#define QLIB_SAMPLE_INTEGRITY_DIGEST_EXPECTED 0x9f58675d7251e2edu

/*-----------------------------------------------------------------------------------------------------------
                                         LOCAL FUNCTION DECLARATIONS
-----------------------------------------------------------------------------------------------------------*/
static QLIB_STATUS_T QLIB_SAMPLE_CheckDigestIntegrity_L(QLIB_CONTEXT_T* qlibContext, bool expectPass_b);

/*-----------------------------------------------------------------------------------------------------------
                                             INTERFACE FUNCTIONS
-----------------------------------------------------------------------------------------------------------*/

QLIB_STATUS_T QLIB_SAMPLE_Integrity(void* userData_pv)
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
     Take the ownership of flash communication channel
    -------------------------------------------------------------------------------------------------------*/
    QLIB_STATUS_RET_CHECK(QLIB_Connect(&qlibContext));

    /*-------------------------------------------------------------------------------------------------------
     Init Flash Device - not needed when using QLIB on a remote server
    -------------------------------------------------------------------------------------------------------*/
    QLIB_STATUS_RET_CHECK_GOTO(QLIB_InitDevice(&qlibContext, busFormat), status, disconnect);

    /*-------------------------------------------------------------------------------------------------------
     Configuration for sample - set up sections -
                                section 1 with both CRC and digest integrity,
                                and section 2 with plain read/write access (used to corrupt section 1)
    -------------------------------------------------------------------------------------------------------*/
    QLIB_STATUS_RET_CHECK_GOTO(QLIB_SAMPLE_Integrity_manuf_flow(&qlibContext), status, disconnect);

    /*-------------------------------------------------------------------------------------------------------
     Sample flow - verify integrity and demonstrate corruption detection
    -------------------------------------------------------------------------------------------------------*/
    QLIB_STATUS_RET_CHECK_GOTO(QLIB_SAMPLE_Integrity_field_flow(&qlibContext), status, disconnect);

disconnect:
    (void)QLIB_Disconnect(&qlibContext);
    return status;
}

QLIB_STATUS_T QLIB_SAMPLE_Integrity_manuf_flow(QLIB_CONTEXT_T* qlibContext)
{
    QLIB_HW_VER_T                 hwVer;
    QLIB_ID_T                     id;
    uint8_t                       die_u8 = 0u;
    QLIB_FLASH_CONFIG_T           flashCfg;
    QLIB_DIE_CONFIG_T             dieCfg;
    QLIB_SECTIONS_CONF_TABLE_T    cfgLine[QLIB_MAX_DIES_SUPPORTED];
    QLIB_EXTENDED_SECTION_CONF_T* sectCfg;
    uint8_t                       diesNum_u8 = QLIB_GET_NUM_OF_DIES(qlibContext);

    (void)memset(&flashCfg, 0, sizeof(flashCfg));
    (void)memset(&dieCfg, 0, sizeof(dieCfg));
    (void)memset(cfgLine, 0, sizeof(cfgLine));

    /*-------------------------------------------------------------------------------------------------------
     See what chip is connected and its unique ids. We might need this info to obtain keys for this
     chip according to formula or via DB. See QLIB_SAMPLE_GetDeviceKey() function.
    -------------------------------------------------------------------------------------------------------*/
    QLIB_STATUS_RET_CHECK(QLIB_GetHWVersion(qlibContext, &hwVer));
    QLIB_STATUS_RET_CHECK(QLIB_GetId(qlibContext, &id));

    /*-------------------------------------------------------------------------------------------------------
     We do read / modify / write configuration since we want to change only what is needed.
    -------------------------------------------------------------------------------------------------------*/
    QLIB_STATUS_RET_CHECK(QLIB_GetDeviceConfigMultiDie(qlibContext, &flashCfg, &dieCfg, cfgLine, diesNum_u8));

    /*-------------------------------------------------------------------------------------------------------
     Set pre-provision master key if needed
    -------------------------------------------------------------------------------------------------------*/
    QLIB_STATUS_RET_CHECK(
        QLIB_SAMPLE_GetDeviceKey(qlibContext, id.sec.wid, QLIB_KID__DEVICE_KEY_PRE_PROVISIONING, dieCfg.preProvisionedMasterKey));

    /*-------------------------------------------------------------------------------------------------------
     Set new master key
    -------------------------------------------------------------------------------------------------------*/
    QLIB_STATUS_RET_CHECK(QLIB_SAMPLE_GetDeviceKey(qlibContext, id.sec.wid, QLIB_KID__DEVICE_MASTER, dieCfg.deviceMasterKey));

#if QLIB_MAX_DIES_SUPPORTED > 1u
    QLIB_STATUS_RET_CHECK(QLIB_GetActiveDie(qlibContext, &die_u8));
#endif // QLIB_MAX_DIES_SUPPORTED > 1u

    sectCfg = cfgLine[die_u8].sectionConfigTable;

    /*-------------------------------------------------------------------------------------------------------
     preliminary step: delete the sample test area, since the sample uses pre-calculated CRC and digest
     for SIZE_512KB_ of 0xFF (erased flash). If the flash area is already deleted, this is redundant.
    -------------------------------------------------------------------------------------------------------*/
    sectCfg[QLIB_SAMPLE_INTEGRITY_CHECK_SECTION].baseAddr = QLIB_SAMPLE_RESERVED_BOOT_SECTION_SIZE;
    sectCfg[QLIB_SAMPLE_INTEGRITY_CHECK_SECTION].size     = SIZE_512KB_;

    memset(&sectCfg[QLIB_SAMPLE_INTEGRITY_CHECK_SECTION].sectionConfig.policy, 0, sizeof(QLIB_POLICY_T));
    sectCfg[QLIB_SAMPLE_INTEGRITY_CHECK_SECTION].sectionConfig.policy.plainAccessReadEnable  = 1u;
    sectCfg[QLIB_SAMPLE_INTEGRITY_CHECK_SECTION].sectionConfig.policy.plainAccessWriteEnable = 1u;

    QLIB_STATUS_RET_CHECK(QLIB_SAMPLE_GetSectionAccessKey(qlibContext,
                                                          id.sec.wid,
                                                          QLIB_KID__FULL_ACCESS_SECTION,
                                                          QLIB_SAMPLE_INTEGRITY_CHECK_SECTION,
                                                          die_u8,
                                                          sectCfg[QLIB_SAMPLE_INTEGRITY_CHECK_SECTION].fullAccessKey));

    QLIB_STATUS_RET_CHECK(QLIB_ConfigDeviceMultiDie(qlibContext, &flashCfg, &dieCfg, cfgLine, diesNum_u8));
    QLIB_STATUS_RET_CHECK(QLIB_EraseSection(qlibContext, QLIB_SAMPLE_INTEGRITY_CHECK_SECTION, (bool)false));

    /*-------------------------------------------------------------------------------------------------------
     Now configure the sections for the sample.
     Section 1 (check section): CRC and digest integrity protected, plain read enabled, plain write disabled.
     Both the CRC and the digest are pre-calculated for SIZE_512KB_ of 0xFF (erased flash).
     CRC integrity is verified by the device without a session; digest integrity requires a restricted session.
    -------------------------------------------------------------------------------------------------------*/
    sectCfg[QLIB_SAMPLE_INTEGRITY_CHECK_SECTION].baseAddr = QLIB_SAMPLE_RESERVED_BOOT_SECTION_SIZE;
    sectCfg[QLIB_SAMPLE_INTEGRITY_CHECK_SECTION].size     = SIZE_512KB_;

    sectCfg[QLIB_SAMPLE_INTEGRITY_CHECK_SECTION].sectionConfig.policy.plainAccessReadEnable  = 1u;
    sectCfg[QLIB_SAMPLE_INTEGRITY_CHECK_SECTION].sectionConfig.policy.plainAccessWriteEnable = 0u;
    sectCfg[QLIB_SAMPLE_INTEGRITY_CHECK_SECTION].sectionConfig.policy.checksumIntegrity      = 1u;
    sectCfg[QLIB_SAMPLE_INTEGRITY_CHECK_SECTION].sectionConfig.policy.digestIntegrity        = 1u;
    sectCfg[QLIB_SAMPLE_INTEGRITY_CHECK_SECTION].sectionConfig.policy.rollbackProt           = 0u;
    sectCfg[QLIB_SAMPLE_INTEGRITY_CHECK_SECTION].sectionConfig.policy.slog                   = 0u;
    sectCfg[QLIB_SAMPLE_INTEGRITY_CHECK_SECTION].sectionConfig.policy.digestIntegrityOnAccess =
        (QLIB_FeatureSupported(qlibContext, W77Q_FEATURE_AUTH_AC) != (bool)false) ? 1u : 0u;
    sectCfg[QLIB_SAMPLE_INTEGRITY_CHECK_SECTION].sectionConfig.crc    = QLIB_SAMPLE_INTEGRITY_CRC_EXPECTED;
    sectCfg[QLIB_SAMPLE_INTEGRITY_CHECK_SECTION].sectionConfig.digest = QLIB_SAMPLE_INTEGRITY_DIGEST_EXPECTED;

    QLIB_STATUS_RET_CHECK(QLIB_SAMPLE_GetSectionAccessKey(qlibContext,
                                                          id.sec.wid,
                                                          QLIB_KID__RESTRICTED_ACCESS_SECTION,
                                                          QLIB_SAMPLE_INTEGRITY_CHECK_SECTION,
                                                          die_u8,
                                                          sectCfg[QLIB_SAMPLE_INTEGRITY_CHECK_SECTION].restrictedKey));

    QLIB_STATUS_RET_CHECK(QLIB_SAMPLE_GetSectionAccessKey(qlibContext,
                                                          id.sec.wid,
                                                          QLIB_KID__FULL_ACCESS_SECTION,
                                                          QLIB_SAMPLE_INTEGRITY_CHECK_SECTION,
                                                          die_u8,
                                                          sectCfg[QLIB_SAMPLE_INTEGRITY_CHECK_SECTION].fullAccessKey));

    /*-------------------------------------------------------------------------------------------------------
     Section 2 (test section): plain read and write enabled. This section overlaps section 1 memory area
     and will be used to corrupt section 1 data to demonstrate integrity detection.
    -------------------------------------------------------------------------------------------------------*/
    sectCfg[QLIB_SAMPLE_INTEGRITY_TEST_SECTION].baseAddr = QLIB_SAMPLE_RESERVED_BOOT_SECTION_SIZE;
    sectCfg[QLIB_SAMPLE_INTEGRITY_TEST_SECTION].size     = SIZE_512KB_;

    sectCfg[QLIB_SAMPLE_INTEGRITY_TEST_SECTION].sectionConfig.policy.plainAccessReadEnable   = 1u;
    sectCfg[QLIB_SAMPLE_INTEGRITY_TEST_SECTION].sectionConfig.policy.plainAccessWriteEnable  = 1u;
    sectCfg[QLIB_SAMPLE_INTEGRITY_TEST_SECTION].sectionConfig.policy.authPlainAccess         = 0u;
    sectCfg[QLIB_SAMPLE_INTEGRITY_TEST_SECTION].sectionConfig.policy.checksumIntegrity       = 0u;
    sectCfg[QLIB_SAMPLE_INTEGRITY_TEST_SECTION].sectionConfig.policy.digestIntegrity         = 0u;
    sectCfg[QLIB_SAMPLE_INTEGRITY_TEST_SECTION].sectionConfig.policy.digestIntegrityOnAccess = 0u;
    sectCfg[QLIB_SAMPLE_INTEGRITY_TEST_SECTION].sectionConfig.policy.rollbackProt            = 0u;
    sectCfg[QLIB_SAMPLE_INTEGRITY_TEST_SECTION].sectionConfig.policy.slog                    = 0u;
    sectCfg[QLIB_SAMPLE_INTEGRITY_TEST_SECTION].sectionConfig.policy.writeProt               = 0u;

    QLIB_STATUS_RET_CHECK(QLIB_SAMPLE_GetSectionAccessKey(qlibContext,
                                                          id.sec.wid,
                                                          QLIB_KID__FULL_ACCESS_SECTION,
                                                          QLIB_SAMPLE_INTEGRITY_TEST_SECTION,
                                                          die_u8,
                                                          sectCfg[QLIB_SAMPLE_INTEGRITY_TEST_SECTION].fullAccessKey));

    /*-------------------------------------------------------------------------------------------------------
     Write configuration to device
    -------------------------------------------------------------------------------------------------------*/
    QLIB_STATUS_RET_CHECK(QLIB_ConfigDeviceMultiDie(qlibContext, &flashCfg, &dieCfg, cfgLine, diesNum_u8));

#if QLIB_MAX_DIES_SUPPORTED > 1u
    QLIB_STATUS_RET_CHECK(QLIB_SetActiveDie(qlibContext, die_u8));
#endif // QLIB_MAX_DIES_SUPPORTED > 1u

    return QLIB_STATUS__OK;
}

QLIB_STATUS_T QLIB_SAMPLE_Integrity_field_flow(QLIB_CONTEXT_T* qlibContext)
{
    QLIB_ID_T     id;
    uint8_t       die_u8 = 0u;
    KEY_T         digestSectKey;
    uint8_t       corruptedData_u8[] = {0xDEu, 0xADu, 0xBEu, 0xEFu};
    QLIB_STATUS_T ret                = QLIB_STATUS__OK;

    QLIB_STATUS_RET_CHECK(QLIB_GetId(qlibContext, &id));
#if QLIB_MAX_DIES_SUPPORTED > 1u
    QLIB_STATUS_RET_CHECK(QLIB_GetActiveDie(qlibContext, &die_u8));
#endif // QLIB_MAX_DIES_SUPPORTED > 1u

    /*-------------------------------------------------------------------------------------------------------
     Obtain the restricted key for the digest section (needed to open a session for digest checks)
    -------------------------------------------------------------------------------------------------------*/
    QLIB_STATUS_RET_CHECK(QLIB_SAMPLE_GetSectionAccessKey(qlibContext,
                                                          id.sec.wid,
                                                          QLIB_KID__RESTRICTED_ACCESS_SECTION,
                                                          QLIB_SAMPLE_INTEGRITY_CHECK_SECTION,
                                                          die_u8,
                                                          digestSectKey));

    QLIB_STATUS_RET_CHECK(QLIB_LoadKey(qlibContext, QLIB_SAMPLE_INTEGRITY_CHECK_SECTION, digestSectKey, (bool)false));
    QLIB_STATUS_RET_CHECK_GOTO(QLIB_OpenSession(qlibContext, QLIB_SAMPLE_INTEGRITY_CHECK_SECTION, QLIB_SESSION_ACCESS_RESTRICTED),
                               ret,
                               remove_key);

    /*-------------------------------------------------------------------------------------------------------
     Verify section 1 CRC integrity is valid (flash is erased, matches pre-configured CRC)
    -------------------------------------------------------------------------------------------------------*/
    QLIB_ASSERT_WITH_ERROR_GOTO(QLIB_STATUS__OK ==
                                    QLIB_CheckIntegrity(qlibContext, QLIB_SAMPLE_INTEGRITY_CHECK_SECTION, QLIB_INTEGRITY_CRC),
                                QLIB_STATUS__TEST_FAIL,
                                ret,
                                close_session);

    /*-------------------------------------------------------------------------------------------------------
     Verify section 1 digest integrity is valid (flash is erased, matches pre-configured digest).
     Unlike CRC, digest check requires an authenticated session.
    -------------------------------------------------------------------------------------------------------*/
    QLIB_STATUS_RET_CHECK_GOTO(QLIB_SAMPLE_CheckDigestIntegrity_L(qlibContext, (bool)true), ret, close_session);

    /*-------------------------------------------------------------------------------------------------------
     Corrupt section 1 data through section 2 (overlapping plain write)
    -------------------------------------------------------------------------------------------------------*/
    QLIB_STATUS_RET_CHECK_GOTO(QLIB_Write(qlibContext,
                                          corruptedData_u8,
                                          QLIB_SAMPLE_INTEGRITY_TEST_SECTION,
                                          0u,
                                          sizeof(corruptedData_u8),
                                          (bool)false),
                               ret,
                               close_session);

    QLIB_SAMPLE_ALLOW_TO_FAIL__START();
    /*-------------------------------------------------------------------------------------------------------
     Verify section 1 CRC integrity now fails (data was corrupted)
    -------------------------------------------------------------------------------------------------------*/
    ret = QLIB_CheckIntegrity(qlibContext, QLIB_SAMPLE_INTEGRITY_CHECK_SECTION, QLIB_INTEGRITY_CRC);
    QLIB_SAMPLE_ALLOW_TO_FAIL__END();

    QLIB_ASSERT_WITH_ERROR_GOTO(QLIB_STATUS__OK != ret, QLIB_STATUS__TEST_FAIL, ret, close_session);
    /*-------------------------------------------------------------------------------------------------------
     Verify section 3 digest integrity now fails (data was corrupted)
    -------------------------------------------------------------------------------------------------------*/
    QLIB_STATUS_RET_CHECK_GOTO(QLIB_SAMPLE_CheckDigestIntegrity_L(qlibContext, (bool)false), ret, close_session);

close_session:
    (void)QLIB_CloseSession(qlibContext, QLIB_SAMPLE_INTEGRITY_CHECK_SECTION);

remove_key:
    (void)QLIB_RemoveKey(qlibContext, QLIB_SAMPLE_INTEGRITY_CHECK_SECTION, (bool)false);
    return ret;
}

/*-----------------------------------------------------------------------------------------------------------
                                               LOCAL FUNCTIONS
-----------------------------------------------------------------------------------------------------------*/

/************************************************************************************************************
 * @brief       Opens a restricted session on the digest section and checks its digest integrity.
 *
 * Two methods are available, selected at compile time via QLIB_SAMPLE_INTEGRITY_USE_GET_SIGNED_DATA:
 *
 * - Method 0 (default): QLIB_CheckIntegrity - the device verifies the digest against the configured
 *   value and returns an error status directly on mismatch.
 *
 * - Method 1: QLIB_GetSignedData - retrieves the current section digest as raw bytes and compares
 *   them manually against QLIB_SAMPLE_INTEGRITY_DIGEST_EXPECTED. QLIB_GetSignedData always returns
 *   QLIB_STATUS__OK regardless of data correctness; integrity is determined by the comparison.
 *
 * @param[in]   qlibContext      [QLIB internal state](md_definitions.html#DEF_CONTEXT)
 * @param[in]   expectPass_b     If true, check is expected to pass; if false, check is expected to fail
 *
 * @return      QLIB_STATUS__OK if the integrity result matched expectPass_b, QLIB_STATUS__(ERROR) otherwise
************************************************************************************************************/
static QLIB_STATUS_T QLIB_SAMPLE_CheckDigestIntegrity_L(QLIB_CONTEXT_T* qlibContext, bool expectPass_b)
{
    QLIB_STATUS_T ret = QLIB_STATUS__OK;
    uint8_t       digestBuff_u8[sizeof(uint64_t)];
    uint64_t      expectedDigest_u64 = QLIB_SAMPLE_INTEGRITY_DIGEST_EXPECTED;
    bool          digestMatches_b    = (bool)false;

    /*-----------------------------------------------------------------------------------------------------
     Method 0: QLIB_CheckIntegrity
     The device compares the section digest against the configured value and signals the result through
     the return status: QLIB_STATUS__OK on match, QLIB_STATUS__SECURITY_ERR on mismatch.
    -----------------------------------------------------------------------------------------------------*/
    QLIB_SAMPLE_ALLOW_TO_FAIL__START();
    ret = QLIB_CheckIntegrity(qlibContext, QLIB_SAMPLE_INTEGRITY_CHECK_SECTION, QLIB_INTEGRITY_DIGEST);
    QLIB_SAMPLE_ALLOW_TO_FAIL__END();

    if (expectPass_b)
    {
        QLIB_ASSERT_RET(QLIB_STATUS__OK == ret, QLIB_STATUS__TEST_FAIL);
    }
    else
    {
        QLIB_ASSERT_RET(QLIB_STATUS__OK != ret, QLIB_STATUS__TEST_FAIL);
    }
    /*-----------------------------------------------------------------------------------------------------
     Method 1: QLIB_GetSignedData + manual comparison
     Retrieves the current section digest as raw bytes. Unlike QLIB_CheckIntegrity, this always returns
     QLIB_STATUS__OK - integrity must be determined by comparing the output against the expected value.
    -----------------------------------------------------------------------------------------------------*/
    QLIB_STATUS_RET_CHECK(QLIB_GetSignedData(qlibContext,
                                             QLIB_SAMPLE_INTEGRITY_CHECK_SECTION,
                                             QLIB_SIGNED_DATA_TYPE_SECTION_DIGEST,
                                             digestBuff_u8,
                                             sizeof(digestBuff_u8),
                                             NULL));

    digestMatches_b = (memcmp(digestBuff_u8, &expectedDigest_u64, sizeof(expectedDigest_u64)) == 0);

    QLIB_ASSERT_RET(expectPass_b == digestMatches_b, QLIB_STATUS__TEST_FAIL);

    return QLIB_STATUS__OK;
}
#endif // QLIB_ONLY_SECURE_STORAGE
