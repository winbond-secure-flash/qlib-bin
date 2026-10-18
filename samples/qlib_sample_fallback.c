/************************************************************************************************************
* @internal
* @remark     Winbond Electronics Corporation - Confidential
* @copyright  Copyright (c) 2020 by Winbond Electronics Corporation . All rights reserved
* @endinternal
*
* @file       qlib_sample_fallback.c
* @brief      This file contains QLIB fallback sample code
*
* @example    qlib_sample_fallback.c
*
* @page       fallback Fallback sample code
* This sample code shows how to perform fallback operations.\n
* The first function shows erase/write/read from W77Q/T as legacy flash.\n
* The second function shows erase/write/read from W77Q/T as secure flash using full key.\n
* The third function shows erase/write/read from W77Q/T as secure flash using restricted key.\n
*
* @include    samples/qlib_sample_fallback.c
*
************************************************************************************************************/

/*-----------------------------------------------------------------------------------------------------------
                                                   INCLUDES
-----------------------------------------------------------------------------------------------------------*/
#include <stdio.h>

#include "qlib.h"
#include "qlib_sample.h"
#include "qlib_sample_fallback.h"

#ifndef QLIB_ONLY_SECURE_STORAGE
#include "defs_utils.h"

/*-----------------------------------------------------------------------------------------------------------
                                                   DEFINITIONS
-----------------------------------------------------------------------------------------------------------*/
#define FW_STRING_BOOT                                 \
    {                                                  \
        0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF \
    }
#define FW_STRING_FB                                   \
    {                                                  \
        0xFB, 0xFB, 0xFB, 0xFB, 0xFB, 0xFB, 0xFB, 0xFB \
    }
#define FW_STRING_SIZE 8

/*-----------------------------------------------------------------------------------------------------------
                                             INTERFACE FUNCTIONS
-----------------------------------------------------------------------------------------------------------*/
QLIB_STATUS_T QLIB_SAMPLE_Fallback(void* userData_pv)
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

#ifndef QLIB_ONLY_FALLBACK
    /*-------------------------------------------------------------------------------------------------------
     configuration for sample
    -------------------------------------------------------------------------------------------------------*/
    QLIB_STATUS_RET_CHECK_GOTO(QLIB_SAMPLE_Fallback_manuf_flow(&qlibContext), status, disconnect);
#endif // QLIB_ONLY_FALLBACK

    /*-------------------------------------------------------------------------------------------------------
     sample flow
    -------------------------------------------------------------------------------------------------------*/
    QLIB_STATUS_RET_CHECK_GOTO(QLIB_SAMPLE_Fallback_field_flow(&qlibContext), status, disconnect);
disconnect:
    (void)QLIB_Disconnect(&qlibContext);
    return status;
}

QLIB_STATUS_T QLIB_SAMPLE_Fallback_config_section(QLIB_CONTEXT_T* qlibContext,
                                                  QLIB_ID_T*      id,
                                                  uint32_t        die_u32,
                                                  uint32_t        sect_u32,
                                                  uint32_t*       crc_pu32,
                                                  uint64_t*       digest_pu64)
{
    KEY_T keyF;

    // section configuration is done via read, modify write
    QLIB_SECTION_CONF_T cfgNew = {0};
    QLIB_STATUS_RET_CHECK(QLIB_GetDeviceSectionConfig(qlibContext, sect_u32, NULL, NULL, &cfgNew));

    // section will be write protected
    cfgNew.policy.plainAccessReadEnable  = 1;
    cfgNew.policy.plainAccessWriteEnable = 0;

    // crc integrity on reboot set
    if (crc_pu32 != NULL)
    {
        cfgNew.policy.checksumIntegrity = 1;
        cfgNew.crc                      = *crc_pu32;
    }

    // digest integrity during configuration set
    if (digest_pu64 != NULL)
    {
        cfgNew.policy.digestIntegrity = 1;
        cfgNew.digest                 = *digest_pu64;
    }

    // we want new policy to be effective immediately
    cfgNew.postActions.reload = 1;

    // obtain full, access key, load its pointer to library, open session
    QLIB_STATUS_RET_CHECK(
        QLIB_SAMPLE_GetSectionAccessKey(qlibContext, id->sec.wid, QLIB_KID__FULL_ACCESS_SECTION, sect_u32, die_u32, keyF));

    // at this point we want to swap between sections
    QLIB_STATUS_RET_CHECK(QLIB_LoadKey(qlibContext, sect_u32, keyF, true));
    QLIB_STATUS_RET_CHECK(QLIB_OpenSession(qlibContext, sect_u32, QLIB_SESSION_ACCESS_FULL));

    QLIB_STATUS_RET_CHECK(QLIB_ConfigDeviceSection(qlibContext, sect_u32, &cfgNew));

    // note that since .reload = 1 in case of successful section configuration session is
    // reopened, so we close it manually
    QLIB_STATUS_RET_CHECK(QLIB_CloseSession(qlibContext, sect_u32));
    QLIB_STATUS_RET_CHECK(QLIB_RemoveKey(qlibContext, sect_u32, true));

    return QLIB_STATUS__OK;
}

#ifndef QLIB_ONLY_FALLBACK
QLIB_STATUS_T QLIB_SAMPLE_Fallback_manuf_flow(QLIB_CONTEXT_T* qlibContext)
{
    QLIB_HW_VER_T                 hwVer;
    QLIB_ID_T                     id;
    uint32_t                      sect_u32;
    uint8_t                       die_u8   = 0u;
    QLIB_FLASH_CONFIG_T           flashCfg = {0u};
    QLIB_DIE_CONFIG_T             dieCfg   = {0u};
    QLIB_SECTIONS_CONF_TABLE_T    cfgLine[QLIB_MAX_DIES_SUPPORTED];
    QLIB_EXTENDED_SECTION_CONF_T* sectCfg;
    uint8_t                       diesNum_u8             = QLIB_GET_NUM_OF_DIES(qlibContext);
    uint32_t                      crc_u32                = 0u;
    uint64_t                      digest_u64             = 0u;
    uint8_t                       digest_bytes_u8[8]     = {0x4d, 0x34, 0x89, 0xdf, 0xac, 0x39, 0x53, 0xec};
    uint8_t                       buf_u8[FW_STRING_SIZE] = FW_STRING_FB;

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

    // setting layout and policy of the section 1
    // this section plain read enabled and plain write enabled

    dieCfg.safeFB = 1u; // enable safe fallback for the die
    sect_u32      = 0u;
    sectCfg       = cfgLine[die_u8].sectionConfigTable;

    sectCfg[sect_u32].baseAddr                                     = 0u;
    sectCfg[sect_u32].size                                         = SIZE_1MB_;
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
                                                          id.sec.wid,
                                                          QLIB_KID__FULL_ACCESS_SECTION,
                                                          sect_u32,
                                                          die_u8,
                                                          sectCfg[sect_u32].fullAccessKey));
    sect_u32                                                       = 1u;
    sectCfg[sect_u32].baseAddr                                     = 0u;
    sectCfg[sect_u32].size                                         = SIZE_2MB_;
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
                                                          id.sec.wid,
                                                          QLIB_KID__FULL_ACCESS_SECTION,
                                                          sect_u32,
                                                          die_u8,
                                                          sectCfg[sect_u32].fullAccessKey));

    // fall back section, we make plain access for the sake of example
    sect_u32                                                       = 7u;
    sectCfg[sect_u32].baseAddr                                     = SIZE_2MB_;
    sectCfg[sect_u32].size                                         = SIZE_1MB_;
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
                                                          id.sec.wid,
                                                          QLIB_KID__FULL_ACCESS_SECTION,
                                                          sect_u32,
                                                          die_u8,
                                                          sectCfg[sect_u32].fullAccessKey));

    // get restricted access key to configure into chip.
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

    QLIB_STATUS_RET_CHECK(QLIB_EraseSection(qlibContext, 0, false));
    QLIB_STATUS_RET_CHECK(QLIB_EraseSection(qlibContext, 1, false));
    QLIB_STATUS_RET_CHECK(QLIB_EraseSection(qlibContext, 7, false));
    // At this point section 0 is plain write. Now we can:
    // 1. Write to it fw
    // 2. Calculate crc of the section with fw.
    //    Section crc is calculated and compared to this value every boot. If integrity damaged will
    //    result in fallback mapping to section 7
    // 3. calculate sha256 of the section with fw.
    //    section sha256 is calculated and compared to this value during configuration. This enables you
    //    write your firmware where/as long as you want since if integrity is damaged configuration will
    //    fail.
    // 4. configure these values adding write protected policy.
    // For the sake of example we use precalculated crc32/sha256 for 1M of 0xff, assuming flash is empty,
    // so this code runs successfully on formatted flash

    // crc32/sha256 for 1M of 0xff. For sha we work with bytes' sequence to avoid endianity issues
    crc_u32 = 0x956bac74;

    // sha256 for 1M of 0xff. We work with bytes' sequence to avoid endianity issues
    memcpy(&digest_u64, digest_bytes_u8, sizeof(digest_u64));

    // final security configuration of the boot section
    QLIB_STATUS_RET_CHECK(QLIB_SAMPLE_Fallback_config_section(qlibContext, &id, die_u8, 0, &crc_u32, &digest_u64));

    // write a unique string to the fallback section for later reading during test flow
    QLIB_STATUS_RET_CHECK(QLIB_Write(qlibContext, buf_u8, 7, 0, FW_STRING_SIZE, false));

    return QLIB_STATUS__OK;
}
#endif // QLIB_ONLY_FALLBACK

QLIB_STATUS_T QLIB_SAMPLE_Fallback_field_flow(QLIB_CONTEXT_T* qlibContext)
{
    QLIB_HW_VER_T  hwVer;
    QLIB_ID_T      id;
    uint8_t        boot_str_u8[FW_STRING_SIZE] = FW_STRING_BOOT;
    uint8_t        fb_str_u8[FW_STRING_SIZE]   = FW_STRING_FB;
    uint8_t        on_flash_u8[FW_STRING_SIZE];
    uint8_t        hacker_data_u8[] = {0xde, 0xad, 0xbe, 0xef};
    QLIB_REG_SSR_T ssr;

    // See what chip is connected and its unique ids. We might need this info to obtain keys for this
    // chips according to formula or via DB see QLIB_SAMPLE_GetSectionAccessKey() function
    QLIB_STATUS_RET_CHECK(QLIB_GetHWVersion(qlibContext, &hwVer));
    QLIB_STATUS_RET_CHECK(QLIB_GetId(qlibContext, &id));

    // ensure we have expected data on boot section
    QLIB_STATUS_RET_CHECK(QLIB_Read(qlibContext, on_flash_u8, 0, 0, FW_STRING_SIZE, false, false));
    QLIB_ASSERT_RET(0 == memcmp(boot_str_u8, on_flash_u8, FW_STRING_SIZE), QLIB_STATUS__TEST_FAIL);

    // now lets test fall back feature. To achieve this, we corrupt the boot section data
    // integrity through section 1.

    QLIB_STATUS_RET_CHECK(QLIB_Write(qlibContext, hacker_data_u8, 1, 0, sizeof(hacker_data_u8), false));

    // In real life crc check will be triggered by hard reset aka reboot. To simulate a power cycle
    // in test flow we use reset function to initiate the CRC check
    QLIB_STATUS_RET_CHECK(QLIB_ResetFlash(qlibContext));

    // now flash should map plain read to the fallback section
    QLIB_STATUS_RET_CHECK(QLIB_Read(qlibContext, on_flash_u8, 0, 0, FW_STRING_SIZE, false, false));
    QLIB_ASSERT_RET(0 == memcmp(fb_str_u8, on_flash_u8, FW_STRING_SIZE), QLIB_STATUS__TEST_FAIL);

    // we can test chip is in fall back mode according to ssr bit
    QLIB_STATUS_RET_CHECK(QLIB_GetStatus(qlibContext, &ssr));
    QLIB_ASSERT_RET(ssr.asStruct.FB_REMAP == 1, QLIB_STATUS__TEST_FAIL);

    return QLIB_STATUS__OK;
}


#endif // QLIB_ONLY_SECURE_STORAGE
