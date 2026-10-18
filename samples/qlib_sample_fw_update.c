/************************************************************************************************************
* @internal
* @remark     Winbond Electronics Corporation - Confidential
* @copyright  Copyright (c) 2019 by Winbond Electronics Corporation . All rights reserved
* @endinternal
*
* @file       qlib_sample_fw_update.c
* @brief      This file contains QLIB firmware update related functions implementation
*
* @example    qlib_sample_fw_update.c
*
* @page       fw_update FW update sample code
* This sample code shows how to use the firmware update feature
*
* @include    samples/qlib_sample_fw_update.c
*
************************************************************************************************************/

/*-----------------------------------------------------------------------------------------------------------
                                                   INCLUDES
-----------------------------------------------------------------------------------------------------------*/
#include "qlib.h"
#include "qlib_sample.h"
#include "qlib_sample_fw_update.h"

#ifndef QLIB_ONLY_SECURE_STORAGE
#include "qlib_utils_crc.h"
#include "qlib_utils_digest.h"

/*-----------------------------------------------------------------------------------------------------------
                                         LOCAL FUNCTION DECLARATIONS
-----------------------------------------------------------------------------------------------------------*/
static QLIB_STATUS_T QLIB_SAMPLE_SecCompare_L(QLIB_CONTEXT_T* qlibContext,
                                              const uint8_t*  fw_pu8,
                                              uint32_t        fw_size_u32,
                                              uint32_t        section_u32,
                                              uint32_t        sectionOffset_u32,
                                              bool            secure_b,
                                              bool            auth_b);

static QLIB_STATUS_T QLIB_SAMPLE_FirmwareUpdate_L(QLIB_CONTEXT_T* qlibContext,
                                                  const uint32_t* fw_pu32,
                                                  uint32_t        fw_size_u32,
                                                  uint32_t        section_u32,
                                                  uint32_t*       newVersion_pu32,
                                                  uint64_t*       checkDigest_pu64,
                                                  uint32_t*       checkCrc_pu32,
                                                  bool            shouldCalcCrcDigest_b,
                                                  bool            swap_b,
                                                  bool            reset_b);

/*-----------------------------------------------------------------------------------------------------------
                                             INTERFACE FUNCTIONS
-----------------------------------------------------------------------------------------------------------*/
QLIB_STATUS_T QLIB_SAMPLE_FwUpdate(void* userData_pv)
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
    QLIB_STATUS_RET_CHECK_GOTO(QLIB_SAMPLE_FirmwareUpdate_manuf_flow(&qlibContext), status, disconnect);

    /*-------------------------------------------------------------------------------------------------------
     sample flow
    -------------------------------------------------------------------------------------------------------*/
    QLIB_STATUS_RET_CHECK_GOTO(QLIB_SAMPLE_FirmwareUpdate_field_flow(&qlibContext), status, disconnect);

disconnect:
    (void)QLIB_Disconnect(&qlibContext);
    return status;
}

QLIB_STATUS_T QLIB_SAMPLE_FirmwareUpdate_manuf_flow(QLIB_CONTEXT_T* qlibContext)
{
    QLIB_HW_VER_T                 hwVer;
    QLIB_ID_T                     id;
    uint8_t                       die_u8   = 0u;
    uint32_t                      sect_u32 = 1u;
    QLIB_FLASH_CONFIG_T           flashCfg = {0u};
    QLIB_DIE_CONFIG_T             dieCfg   = {0u};
    QLIB_SECTIONS_CONF_TABLE_T    cfgLine[QLIB_MAX_DIES_SUPPORTED];
    QLIB_EXTENDED_SECTION_CONF_T* sectCfg;
    uint8_t                       diesNum_u8 = QLIB_GET_NUM_OF_DIES(qlibContext);

    // See what chip is connected and its unique ids. We might need this info to obtain keys for this
    // chips according to formula or via DB see QLIB_SAMPLE_GetDeviceKey() function
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

#if QLIB_MAX_DIES_SUPPORTED > 1u
    QLIB_STATUS_RET_CHECK(QLIB_GetActiveDie(qlibContext, &die_u8));
#endif // QLIB_MAX_DIES_SUPPORTED > 1u

    sectCfg = cfgLine[die_u8].sectionConfigTable;

    sectCfg[sect_u32].baseAddr = QLIB_SAMPLE_RESERVED_BOOT_SECTION_SIZE;
    sectCfg[sect_u32].size     = SIZE_512KB_;

    // section_u32 configured as rollback protected
    sectCfg[sect_u32].sectionConfig.policy.plainAccessReadEnable   = 1u;
    sectCfg[sect_u32].sectionConfig.policy.plainAccessWriteEnable  = 0u;
    sectCfg[sect_u32].sectionConfig.policy.authPlainAccess         = 0u;
    sectCfg[sect_u32].sectionConfig.policy.checksumIntegrity       = 0u;
    sectCfg[sect_u32].sectionConfig.policy.digestIntegrity         = 0u;
    sectCfg[sect_u32].sectionConfig.policy.digestIntegrityOnAccess = 0u;
    sectCfg[sect_u32].sectionConfig.policy.rollbackProt            = 1u;
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

    // write configuration
    QLIB_STATUS_RET_CHECK(QLIB_ConfigDeviceMultiDie(qlibContext, &flashCfg, &dieCfg, cfgLine, diesNum_u8));

#if QLIB_MAX_DIES_SUPPORTED > 1u
    // write configuration sets die to 0. return to orig die
    QLIB_STATUS_RET_CHECK(QLIB_SetActiveDie(qlibContext, die_u8));
#endif // QLIB_MAX_DIES_SUPPORTED > 1u
    return QLIB_STATUS__OK;
}

QLIB_STATUS_T QLIB_SAMPLE_FirmwareUpdate_field_flow(QLIB_CONTEXT_T* qlibContext)
{
    QLIB_ID_T           id;
    const char          fw_pu8[] = {'i', ' ', 'a', 'm', ' ', 'a', ' ', 'n', 'e', 'w', ' ', 'f', 'w'};
    KEY_T               sectionKey;
    uint8_t             die_u8      = 0u;
    uint32_t            section_u32 = 1u;
    uint32_t            fw32_u32[1u + (sizeof(fw_pu8) / 4u)];
    QLIB_SECTION_CONF_T sectionConfig  = {0u};
    uint32_t            newVersion_u32 = 0u;
    QLIB_STATUS_T       ret            = QLIB_STATUS__OK;

    /*-------------------------------------------------------------------------------------------------------
     Pre-configured CRC and Digest for the firmware section new sample firmware
    -------------------------------------------------------------------------------------------------------*/
    uint64_t digest_u64 = 0x0a5c3ee298dd239bu;
    uint32_t crc_u32    = 0x30fc5598u;

    /*-------------------------------------------------------------------------------------------------------
     Moving FW to aligned location
    -------------------------------------------------------------------------------------------------------*/
    (void)memset(fw32_u32, 0xff, sizeof(fw32_u32));
    (void)memcpy((void*)fw32_u32, (void*)fw_pu8, sizeof(fw_pu8));

    /*-------------------------------------------------------------------------------------------------------
     Get current section version
    -------------------------------------------------------------------------------------------------------*/
    QLIB_STATUS_RET_CHECK(QLIB_GetDeviceSectionConfig(qlibContext, section_u32, NULL, NULL, &sectionConfig));
    newVersion_u32 = sectionConfig.version + 1u;

    /*-------------------------------------------------------------------------------------------------------
     First, we set (load) keys to the section to be updated. The key is necessary to manage secure session to the flash.
     In this sample we update the section full access key,
     otherwise update operation will be blocked by a flash
    -------------------------------------------------------------------------------------------------------*/
    QLIB_STATUS_RET_CHECK(QLIB_GetId(qlibContext, &id));
#if QLIB_MAX_DIES_SUPPORTED > 1u
    QLIB_STATUS_RET_CHECK(QLIB_GetActiveDie(qlibContext, &die_u8));
#endif // QLIB_MAX_DIES_SUPPORTED > 1u
    QLIB_STATUS_RET_CHECK(
        QLIB_SAMPLE_GetSectionAccessKey(qlibContext, id.sec.wid, QLIB_KID__FULL_ACCESS_SECTION, section_u32, die_u8, sectionKey));
    QLIB_STATUS_RET_CHECK(QLIB_LoadKey(qlibContext, section_u32, sectionKey, (bool)true));

    QLIB_STATUS_RET_CHECK_GOTO(QLIB_SAMPLE_FirmwareUpdate_L(qlibContext,
                                                            fw32_u32,
                                                            (uint32_t)sizeof(fw32_u32),
                                                            section_u32,
                                                            &newVersion_u32,
                                                            &digest_u64,
                                                            &crc_u32,
                                                            (bool)false,
                                                            (bool)true,
                                                            (bool)false),
                               ret,
                               remove_key);

    /*-------------------------------------------------------------------------------------------------------
     previous version will not be accepted despite correct crypto
     this is rollback protection feature, aka even if hacker controls correct update flow
     flash will not accept if version is less then current one
    -------------------------------------------------------------------------------------------------------*/
    newVersion_u32 = newVersion_u32 - 1u;
    QLIB_SAMPLE_ALLOW_TO_FAIL__START();
    ret = QLIB_SAMPLE_FirmwareUpdate_L(qlibContext,
                                       fw32_u32,
                                       (uint32_t)sizeof(fw32_u32),
                                       section_u32,
                                       &newVersion_u32,
                                       &digest_u64,
                                       &crc_u32,
                                       (bool)false,
                                       (bool)true,
                                       (bool)false);
    QLIB_SAMPLE_ALLOW_TO_FAIL__END();
    QLIB_ASSERT_WITH_ERROR_GOTO(QLIB_STATUS__OK != ret, QLIB_STATUS__TEST_FAIL, ret, remove_key);

    /*-------------------------------------------------------------------------------------------------------
     Clean the key reference from QLIB. optional
    -------------------------------------------------------------------------------------------------------*/
    QLIB_STATUS_RET_CHECK(QLIB_RemoveKey(qlibContext, section_u32, (bool)true));

    return QLIB_STATUS__OK;

remove_key:
    (void)QLIB_RemoveKey(qlibContext, section_u32, (bool)true);
    return ret;
}

static QLIB_STATUS_T QLIB_SAMPLE_FirmwareUpdate_L(QLIB_CONTEXT_T* qlibContext,
                                                  const uint32_t* fw_pu32,
                                                  uint32_t        fw_size_u32,
                                                  uint32_t        section_u32,
                                                  uint32_t*       newVersion_pu32,
                                                  uint64_t*       checkDigest_pu64,
                                                  uint32_t*       checkCrc_pu32,
                                                  bool            shouldCalcCrcDigest_b,
                                                  bool            swap_b,
                                                  bool            reset_b)
{
    QLIB_STATUS_T ret = QLIB_STATUS__OK;

    /*-------------------------------------------------------------------------------------------------------
     When ran on embedded target and NOT remotely CRC and DIGEST are pre-calculated
     ------------------------------------------------------------------------------------------------------*/

#ifndef QLIB_SUPPORT_XIP
    /*-------------------------------------------------------------------------------------------------------
     Prepare everything needed to update section - this function will calculate digest/crc if needed.
     If the CRC/Digest are already known, this process is not needed.
    -------------------------------------------------------------------------------------------------------*/
    if (shouldCalcCrcDigest_b == (bool)true)
    {
        QLIB_STATUS_RET_CHECK(QLIB_SAMPLE_GetUpdateSectionParams(qlibContext,
                                                                 section_u32,
                                                                 fw_pu32,
                                                                 fw_size_u32,
                                                                 checkDigest_pu64,
                                                                 checkCrc_pu32,
                                                                 NULL));
    }
#else
    QLIB_ASSERT_RET(shouldCalcCrcDigest_b == (bool)false, QLIB_STATUS__SYSTEM_IN_INCORRECT_STATE);
#endif // QLIB_SUPPORT_XIP

    ret = QLIB_SAMPLE_SectionUpdate(qlibContext,
                                    (const uint8_t*)fw_pu32,
                                    fw_size_u32,
                                    section_u32,
                                    checkDigest_pu64,
                                    checkCrc_pu32,
                                    newVersion_pu32,
                                    swap_b,
                                    reset_b == (bool)true ? QLIB_SECTION_CONF_ACTION__RESET : QLIB_SECTION_CONF_ACTION__RELOAD);

    /*-------------------------------------------------------------------------------------------------------
     If swap is set to QLIB_SWAP_AND_RESET device should undergo reset -> if this code runs locally we reset here
     so the following code relevant for remote execution only
    -------------------------------------------------------------------------------------------------------*/
    if ((reset_b == (bool)true) && (ret == QLIB_STATUS__COMMUNICATION_ERR))
    {
        /*---------------------------------------------------------------------------------------------------
         Device should undergo reset and therefore does not answer. Let QLIB_STATUS__COMMUNICATION_ERR
         be the error that is returned by remote server TM layer, when there is no answer from IoT device.
        ---------------------------------------------------------------------------------------------------*/
        ret = QLIB_STATUS__OK;
        goto exit;
    }

    QLIB_ASSERT_RET(ret == QLIB_STATUS__OK, QLIB_STATUS__SYSTEM_IN_INCORRECT_STATE);

    /*-------------------------------------------------------------------------------------------------------
     Verify the FW update operation. Securely read from the active half.
    -------------------------------------------------------------------------------------------------------*/
    if (swap_b == (bool)true)
    {
        QLIB_STATUS_RET_CHECK(
            QLIB_SAMPLE_SecCompare_L(qlibContext, (const uint8_t*)fw_pu32, fw_size_u32, section_u32, 0u, (bool)true, (bool)true));
    }

exit:

    return ret;
}

#ifndef QLIB_SUPPORT_XIP
QLIB_STATUS_T QLIB_SAMPLE_GetUpdateSectionParams(QLIB_CONTEXT_T* qlibContext,
                                                 uint32_t        section_u32,
                                                 const uint32_t* buff_pu32,
                                                 uint32_t        buffSize_u32,
                                                 uint64_t*       digestIntegrity_pu64,
                                                 uint32_t*       checksumIntegrity_pu32,
                                                 uint32_t*       oldVersion_pu32)
{
    uint32_t            sectionLen_u32;
    QLIB_SECTION_CONF_T sectionConfig = {0u};
    QLIB_STATUS_T       ret           = QLIB_STATUS__OK;

    QLIB_STATUS_RET_CHECK(QLIB_OpenSession(qlibContext, section_u32, QLIB_SESSION_ACCESS_FULL));

    /*-------------------------------------------------------------------------------------------------------
     To calculate crc/digest section length.
    -------------------------------------------------------------------------------------------------------*/
    QLIB_STATUS_RET_CHECK(QLIB_GetDeviceSectionConfig(qlibContext, section_u32, NULL, &sectionLen_u32, &sectionConfig));
    if (NULL != oldVersion_pu32)
    {
        *oldVersion_pu32 = sectionConfig.version;
    }

    /*-------------------------------------------------------------------------------------------------------
     Get section information and check that the section is defined as rollback protected section.
    -------------------------------------------------------------------------------------------------------*/
    QLIB_ASSERT_WITH_ERROR_GOTO(1u == sectionConfig.policy.rollbackProt,
                                QLIB_STATUS__SYSTEM_IN_INCORRECT_STATE,
                                ret,
                                close_session);

    if (digestIntegrity_pu64 != NULL || checksumIntegrity_pu32 != NULL)
    {
        if (digestIntegrity_pu64 != NULL)
        {
#ifndef QLIB_RAM_SMALL_FOR_FULL_SECTION
            static uint32_t sectionBuf_u32[QLIB_SAMPLE_MAX_FW_SIZE / 4];
            (void)memset(sectionBuf_u32, 0xFF, sectionLen_u32 / 2u);
            (void)memcpy(sectionBuf_u32, buff_pu32, buffSize_u32);
            QLIB_STATUS_RET_CHECK_GOTO(QLIB_UTILS_CalcDigest(sectionBuf_u32, sectionLen_u32 / 2u, digestIntegrity_pu64),
                                       ret,
                                       close_session);
#else
            // there is not enough RAM to hold all the section buffer
            QLIB_STATUS_RET_CHECK(QLIB_STATUS__NOT_SUPPORTED);
#endif // QLIB_RAM_SMALL_FOR_FULL_SECTION
        }

        if (checksumIntegrity_pu32 != NULL)
        {
            QLIB_STATUS_RET_CHECK_GOTO(QLIB_UTILS_CalcCRCWithPadding(buff_pu32,
                                                                     buffSize_u32,
                                                                     0xFFu,
                                                                     (sectionLen_u32 / 2u) - buffSize_u32,
                                                                     checksumIntegrity_pu32),
                                       ret,
                                       close_session);
        }
    }

close_session:

    (void)QLIB_CloseSession(qlibContext, section_u32);

    return ret;
}
#endif // QLIB_SUPPORT_XIP

QLIB_STATUS_T QLIB_SAMPLE_SectionUpdate(QLIB_CONTEXT_T*            qlibContext,
                                        const uint8_t*             buff_pu8,
                                        uint32_t                   buffSize_u32,
                                        uint32_t                   section_u32,
                                        uint64_t*                  digestIntegrity_pu64,
                                        uint32_t*                  checksumIntegrity_pu32,
                                        uint32_t*                  newVersion_pu32,
                                        bool                       swap_b,
                                        QLIB_SECTION_CONF_ACTION_T action)
{
    QLIB_STATUS_T       ret = QLIB_STATUS__OK;
    uint32_t            sectionLen_u32;
    QLIB_POLICY_T       policy;
    QLIB_SECTION_CONF_T sectionConfig = {0u};

    /*-------------------------------------------------------------------------------------------------------
     Open the session. Assumed that the key to that section is already loaded.
    -------------------------------------------------------------------------------------------------------*/
    QLIB_STATUS_RET_CHECK(QLIB_OpenSession(qlibContext, section_u32, QLIB_SESSION_ACCESS_FULL));

    /*-------------------------------------------------------------------------------------------------------
     Check that the section is defined as rollback protected and error exit if not.
    -------------------------------------------------------------------------------------------------------*/
    QLIB_STATUS_RET_CHECK(QLIB_GetDeviceSectionConfig(qlibContext, section_u32, NULL, &sectionLen_u32, &sectionConfig));
    (void)memcpy(&policy, &sectionConfig.policy, sizeof(QLIB_POLICY_T));

    QLIB_ASSERT_WITH_ERROR_GOTO(1u == policy.rollbackProt, QLIB_STATUS__SYSTEM_IN_INCORRECT_STATE, ret, close_session);

    /*-------------------------------------------------------------------------------------------------------
     Perform secure write operations to write DATA into inactive partition of the section (high).
     The buffer size should be not bigger than half of the section length as in case of rollback protected section,
     the section is divided to two parts.
     Note: If section plain write is enabled, non-secure erase/write operations might be used instead.
    -------------------------------------------------------------------------------------------------------*/
    QLIB_ASSERT_WITH_ERROR_GOTO(buffSize_u32 <= (sectionLen_u32 / 2u), QLIB_STATUS__INVALID_DATA_SIZE, ret, close_session);
    QLIB_STATUS_RET_CHECK_GOTO(QLIB_Erase(qlibContext, section_u32, (sectionLen_u32 / 2u), (sectionLen_u32 / 2u), (bool)true),
                               ret,
                               close_session);
    QLIB_STATUS_RET_CHECK_GOTO(QLIB_Write(qlibContext, buff_pu8, section_u32, (sectionLen_u32 / 2u), buffSize_u32, (bool)true),
                               ret,
                               close_session);

    /*-------------------------------------------------------------------------------------------------------
     Now, in order to finish we want to swap the new fw with the current one.
     If configured as CRC/digest protected, we need to give correct digest/crc to that command.
    -------------------------------------------------------------------------------------------------------*/
    policy.checksumIntegrity = checksumIntegrity_pu32 != NULL ? 1u : 0u;
    policy.digestIntegrity   = digestIntegrity_pu64 != NULL ? 1u : 0u;
    if (QLIB_FeatureSupported(qlibContext, W77Q_FEATURE_AUTH_AC) != (bool)false)
    {
        policy.digestIntegrityOnAccess = policy.digestIntegrity;
    }

    if (0u == policy.digestIntegrity)
    {
        QLIB_SAMPLE_PRINTF("Warning: Update section with digest integrity disabled");
    }

    if (0u == policy.checksumIntegrity)
    {
        QLIB_SAMPLE_PRINTF("Warning: Update section with checksum (CRC) integrity disabled");
    }

    /*-------------------------------------------------------------------------------------------------------
     Last command to finish, note if section updated is section 0u - (the one we run from):
     QLIB_SWAP_AND_RESET should be used as a last parameter in order for reset to occur so we can run from
     the new updated FW.
    -------------------------------------------------------------------------------------------------------*/
    QLIB_SAMPLE_PRINTF("\nSENDING QLIB_ConfigSection with crc 0x%x \r\n",
                       checksumIntegrity_pu32 == NULL ? 0u : *checksumIntegrity_pu32);

    (void)memcpy(&sectionConfig.policy, &policy, sizeof(QLIB_POLICY_T));
    if (NULL != checksumIntegrity_pu32)
    {
        sectionConfig.crc = *checksumIntegrity_pu32;
    }
    if (NULL != digestIntegrity_pu64)
    {
        sectionConfig.digest = *digestIntegrity_pu64;
    }
    if (NULL != newVersion_pu32)
    {
        sectionConfig.version = *newVersion_pu32;
    }

    sectionConfig.postActions.swap   = BOOLEAN_TO_INT(swap_b);
    sectionConfig.postActions.reload = (action == QLIB_SECTION_CONF_ACTION__RELOAD ? 1u : 0u);
    sectionConfig.postActions.reset  = (action == QLIB_SECTION_CONF_ACTION__RESET ? 1u : 0u);

    QLIB_STATUS_RET_CHECK_GOTO(QLIB_ConfigDeviceSection(qlibContext, section_u32, &sectionConfig), ret, close_session);

close_session:

    /*-------------------------------------------------------------------------------------------------------
     even when the device is after reset, close section will work as needed from remote since it does not
     involve TM transactions
    -------------------------------------------------------------------------------------------------------*/
    (void)QLIB_CloseSession(qlibContext, section_u32);

    return ret;
}

/*-----------------------------------------------------------------------------------------------------------
                                               LOCAL FUNCTIONS
-----------------------------------------------------------------------------------------------------------*/

/************************************************************************************************************
 * @brief       This routine make memcmp over secure flash
 *
 * @param[out]  qlibContext         [QLIB internal state](md_definitions.html#DEF_CONTEXT)
 * @param[in]   fw_pu8              FW buffer
 * @param[in]   fw_size_u32         FW buffer size in bytes
 * @param[in]   section_u32         [Section index](md_definitions.html#DEF_SECTION)
 * @param[in]   sectionOffset_u32   [Section offset](md_definitions.html#DEF_OFFSET)
 * @param[in]   secure_b            If read is secure
 * @param[in]   auth_b              If read is authenticated
 *
 * @return      0 if fw_pu8[fw_size_u32] is equal to flash content from section_u32[sectionOffset_u32]
************************************************************************************************************/
static QLIB_STATUS_T QLIB_SAMPLE_SecCompare_L(QLIB_CONTEXT_T* qlibContext,
                                              const uint8_t*  fw_pu8,
                                              uint32_t        fw_size_u32,
                                              uint32_t        section_u32,
                                              uint32_t        sectionOffset_u32,
                                              bool            secure_b,
                                              bool            auth_b)
{
    QLIB_STATUS_T ret          = QLIB_STATUS__OK;
    uint32_t      compared_u32 = 0u;
    uint32_t      takeSize_u32;
    uint32_t      take_u32[4];

    QLIB_STATUS_RET_CHECK(QLIB_OpenSession(qlibContext, section_u32, QLIB_SESSION_ACCESS_FULL));

    while (compared_u32 < fw_size_u32)
    {
        takeSize_u32 = MIN(fw_size_u32 - compared_u32, (uint32_t)sizeof(take_u32));
        QLIB_STATUS_RET_CHECK_GOTO(QLIB_Read(qlibContext,
                                             (uint8_t*)take_u32,
                                             section_u32,
                                             sectionOffset_u32,
                                             takeSize_u32,
                                             secure_b,
                                             auth_b),
                                   ret,
                                   close_session);
        QLIB_ASSERT_WITH_ERROR_GOTO(0u == memcmp(&fw_pu8[compared_u32], (uint8_t*)take_u32, takeSize_u32),
                                    QLIB_STATUS__COMMAND_FAIL,
                                    ret,
                                    close_session);
        compared_u32 += takeSize_u32;
    }

close_session:

    (void)QLIB_CloseSession(qlibContext, section_u32);
    return ret;
}

#endif // QLIB_ONLY_SECURE_STORAGE
