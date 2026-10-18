/************************************************************************************************************
* @internal
* @remark     Winbond Electronics Corporation - Confidential
* @copyright  Copyright (c) 2026 by Winbond Electronics Corporation . All rights reserved
* @endinternal
*
* @file       qlib_sample_kd1_config.c
* @brief      This file contains QLIB sample code for provisioning the device using a KD1-derived
*             pre-provisioned master key
*
* @example    qlib_sample_kd1_config.c
*
* @page       kd1_config KD1-based device provisioning sample code
* This sample code demonstrates the recommended flow for a customer taking a freshly manufactured, KD1
* pre-provisioned Winbond secure flash into use: \n
*   1. Read the device ID (WID). \n
*   2. Derive KD1 (the device master key programmed at Winbond pre-provisioning) from the WID using either
*      the customer CMS, with the NIST SP 800-108 HMAC-SHA256 counter-mode KDF (when W77Q_KDF_KD1 defined)
*      or in other way customer chooses\n
*   3. Use KD1 as the pre-provisioned master key when calling @ref QLIB_ConfigDeviceMultiDie to install
*      the customer's own device master key, one secure section and its full/restricted
*      access keys. \n
*   4. Open a full-access session to the section and perform a write/read round trip to prove that the
*      provisioning succeeded. \n
*
* @include    samples/qlib_sample_kd1_config.c
*
************************************************************************************************************/

/*-----------------------------------------------------------------------------------------------------------
                                                   INCLUDES
-----------------------------------------------------------------------------------------------------------*/
#include "qlib.h"
#include "qlib_sample.h"
#include "qlib_sample_kd1_config.h"

/*-----------------------------------------------------------------------------------------------------------
                                                   DEFINITIONS
-----------------------------------------------------------------------------------------------------------*/

// Section that this sample provisions and uses for the write/read round trip
#define QLIB_SAMPLE_KD1_CONFIG_SECTION 1u

// Size of the section, placed right after the reserved boot area
#define QLIB_SAMPLE_KD1_CONFIG_SECTION_SIZE SIZE_128KB_

/*-----------------------------------------------------------------------------------------------------------
                                          LOCAL FUNCTION PROTOTYPES
-----------------------------------------------------------------------------------------------------------*/

static QLIB_STATUS_T QLIB_SAMPLE_KD1Config_configureDevice_L(QLIB_CONTEXT_T* ctx, const KEY_T kd1_key, QLIB_WID_T wid);
static QLIB_STATUS_T QLIB_SAMPLE_KD1Config_verifyWriteRead_L(QLIB_CONTEXT_T* ctx, QLIB_WID_T wid);

/*-----------------------------------------------------------------------------------------------------------
                                             INTERFACE FUNCTIONS
-----------------------------------------------------------------------------------------------------------*/

QLIB_STATUS_T QLIB_SAMPLE_KD1Config(void* userData_pv)
{
    QLIB_CONTEXT_T    qlibContext;
    QLIB_STATUS_T     status    = QLIB_STATUS__OK;
    QLIB_BUS_FORMAT_T busFormat = QLIB_BUS_FORMAT(QLIB_BUS_MODE_1_1_1, (bool)false);

    /*-------------------------------------------------------------------------------------------------------
     Standard QLIB initialization sequence used by all samples
    -------------------------------------------------------------------------------------------------------*/
    QLIB_SAMPLE_PRINTF("> QLIB_SAMPLE_KD1Config: Initializing QLIB and connecting to the device...\r\n");

    QLIB_STATUS_RET_CHECK(QLIB_InitLib(&qlibContext));
    QLIB_SetUserData(&qlibContext, userData_pv);
    QLIB_STATUS_RET_CHECK(QLIB_Connect(&qlibContext));
    QLIB_STATUS_RET_CHECK_GOTO(QLIB_InitDevice(&qlibContext, busFormat), status, disconnect);

    QLIB_SAMPLE_PRINTF("> QLIB_SAMPLE_KD1Config: Initializing finished successfully\r\n");

    /*-------------------------------------------------------------------------------------------------------
     Provision the device using a KD1-derived pre-provisioned master key
    -------------------------------------------------------------------------------------------------------*/
    QLIB_STATUS_RET_CHECK_GOTO(QLIB_SAMPLE_KD1Config_provision(&qlibContext), status, disconnect);

disconnect:
    (void)QLIB_Disconnect(&qlibContext);
    return status;
}

QLIB_STATUS_T QLIB_SAMPLE_KD1Config_provision(QLIB_CONTEXT_T* ctx)
{
    QLIB_ID_T id;
    KEY_T     kd1_key;
#if QLIB_MAX_DIES_SUPPORTED > 1u && defined W77Q_KDF_KD1
    uint8_t origMaxDie_u8 = ctx->MaxDieId;
#endif // QLIB_MAX_DIES_SUPPORTED > 1u && W77Q_KDF_KD1

    QLIB_ASSERT_RET(NULL != ctx, QLIB_STATUS__INVALID_PARAMETER);

    /*-------------------------------------------------------------------------------------------------------
     Force the active die to die 0. KD1 is a per-die pre-provisioned key but has same value for all dies
     according to die 0 WID.
    -------------------------------------------------------------------------------------------------------*/
#if QLIB_MAX_DIES_SUPPORTED > 1u
    QLIB_SAMPLE_PRINTF("> QLIB_SAMPLE_KD1Config: Set active die to 0\r\n");
    QLIB_STATUS_RET_CHECK(QLIB_SetActiveDie(ctx, 0u));
#ifdef W77Q_KDF_KD1
    ctx->MaxDieId = 0u;
#endif // defined W77Q_KDF_KD1
#endif // QLIB_MAX_DIES_SUPPORTED > 1u

    /*-------------------------------------------------------------------------------------------------------
     Read the flash IDs to obtain the die 0 Winbond ID (WID)
    -------------------------------------------------------------------------------------------------------*/
    QLIB_SAMPLE_PRINTF("> QLIB_SAMPLE_KD1Config: obtain the die 0 Winbond ID (WID)\r\n");
    QLIB_STATUS_RET_CHECK(QLIB_GetId(ctx, &id));

    QLIB_SAMPLE_PRINTF("> QLIB_SAMPLE_KD1Config: die 0 WID = 0x%08lx%08lx\r\n",
                       (unsigned long)id.sec.wid[1],
                       (unsigned long)id.sec.wid[0]);

    /*-------------------------------------------------------------------------------------------------------
     Derive the KD1 pre-provisioned master key from the WID
    -------------------------------------------------------------------------------------------------------*/
    QLIB_SAMPLE_PRINTF("> QLIB_SAMPLE_KD1Config: deriving KD1 from the die 0 WID\r\n");
    QLIB_STATUS_RET_CHECK(QLIB_SAMPLE_GetDeviceKey(ctx, id.sec.wid, QLIB_KID__DEVICE_KEY_PRE_PROVISIONING, kd1_key));
    QLIB_SAMPLE_PRINTF("> QLIB_SAMPLE_KD1Config: retrieved KD1 = 0x");
    for (uint32_t i_u32 = 0u; i_u32 < (uint32_t)sizeof(KEY_T); ++i_u32)
    {
        QLIB_SAMPLE_PRINTF("%02x", (unsigned int)((const uint8_t*)kd1_key)[i_u32]);
    }
    QLIB_SAMPLE_PRINTF("\r\n");

    /*-------------------------------------------------------------------------------------------------------
     Use KD1 to authenticate the device configuration: install the customer's new master key, configure
     section 1 with a full/restricted access key and a secure-only access policy
    -------------------------------------------------------------------------------------------------------*/
    QLIB_SAMPLE_PRINTF("> QLIB_SAMPLE_KD1Config: use Kd1 to config new master key and secure sections\r\n");
    QLIB_STATUS_RET_CHECK(QLIB_SAMPLE_KD1Config_configureDevice_L(ctx, kd1_key, id.sec.wid));

    /*-------------------------------------------------------------------------------------------------------
     Verify that the provisioning succeeded by opening a full-access session and doing a write/read
    -------------------------------------------------------------------------------------------------------*/
    QLIB_SAMPLE_PRINTF("> QLIB_SAMPLE_KD1Config: Verify configured secure section access (read, write)\r\n");
    QLIB_STATUS_RET_CHECK(QLIB_SAMPLE_KD1Config_verifyWriteRead_L(ctx, id.sec.wid));

#if QLIB_MAX_DIES_SUPPORTED > 1u && defined W77Q_KDF_KD1
    ctx->MaxDieId = origMaxDie_u8;
#endif // QLIB_MAX_DIES_SUPPORTED > 1u && W77Q_KDF_KD1
    return QLIB_STATUS__OK;
}

/*-----------------------------------------------------------------------------------------------------------
                                              LOCAL FUNCTIONS
-----------------------------------------------------------------------------------------------------------*/

/************************************************************************************************************
 * @internal
 * @brief       Use KD1 to configure master key, and configure the device with secure section and keys
 *
 * @param[in,out] ctx       QLIB context, already initialized and connected
 * @param[in]     kd1_key   KD1 pre-provisioned master key
 * @param[in]     wid       Device winbond unique ID, used to derive the customer's device master key and section
 *                          access keys via the QLIB_SAMPLE_GetDeviceKey / QLIB_SAMPLE_GetSectionAccessKey
 *                          helpers
 * @endinternal
************************************************************************************************************/
static QLIB_STATUS_T QLIB_SAMPLE_KD1Config_configureDevice_L(QLIB_CONTEXT_T* ctx, const KEY_T kd1_key, QLIB_WID_T wid)
{
    QLIB_FLASH_CONFIG_T           flashCfg = {0u};
    QLIB_DIE_CONFIG_T             dieCfg   = {0u};
    QLIB_SECTIONS_CONF_TABLE_T    cfgLine[QLIB_MAX_DIES_SUPPORTED];
    QLIB_EXTENDED_SECTION_CONF_T* sectCfg;
    uint8_t                       diesNum_u8 = QLIB_GET_NUM_OF_DIES(ctx);
    uint8_t                       die_u8     = 0u;

    /*-------------------------------------------------------------------------------------------------------
     Read the existing configuration so we can modify only what we need
    -------------------------------------------------------------------------------------------------------*/
    QLIB_STATUS_RET_CHECK(QLIB_GetDeviceConfigMultiDie(ctx, &flashCfg, &dieCfg, cfgLine, diesNum_u8));

    /*-------------------------------------------------------------------------------------------------------
     Make sure flash is formatted
    -------------------------------------------------------------------------------------------------------*/
    if ((bool)true == dieCfg.nonSecureFormatEn)
    {
        QLIB_SAMPLE_PRINTF("> QLIB_SAMPLE_KD1Config: Format Flash\r\n");
        QLIB_STATUS_RET_CHECK(QLIB_Format(ctx, NULL, (bool)false, (bool)false));
        QLIB_SAMPLE_PRINTF("> QLIB_SAMPLE_KD1Config: Format Flash Success\r\n");
        // re-read the configuration after format
        QLIB_STATUS_RET_CHECK(QLIB_GetDeviceConfigMultiDie(ctx, &flashCfg, &dieCfg, cfgLine, diesNum_u8));
    }
    else
    {
        QLIB_SAMPLE_PRINTF("> QLIB_SAMPLE_KD1Config: Skip Flash Format\r\n");
    }

    /*-------------------------------------------------------------------------------------------------------
     Install the KD1-derived pre-provisioned master key. This is what authenticates the configuration
     transaction on a freshly manufactured, pre-provisioned device.
    -------------------------------------------------------------------------------------------------------*/
    (void)memcpy((void*)dieCfg.preProvisionedMasterKey, (const void*)kd1_key, sizeof(KEY_T));

    /*-------------------------------------------------------------------------------------------------------
     Install a new customer-chosen device master key, obtained via the standard sample helper
    -------------------------------------------------------------------------------------------------------*/
    QLIB_SAMPLE_PRINTF("> QLIB_SAMPLE_KD1Config: Set new master key\r\n");
    QLIB_STATUS_RET_CHECK(QLIB_SAMPLE_GetDeviceKey(ctx, wid, QLIB_KID__DEVICE_MASTER, dieCfg.deviceMasterKey));

    /*-------------------------------------------------------------------------------------------------------
     Configure section 1 as a secure section (no plain access, full and restricted access keys installed)
    -------------------------------------------------------------------------------------------------------*/
    QLIB_SAMPLE_PRINTF("> QLIB_SAMPLE_KD1Config: Set secure (no plain access) section id %u with full access and restricted "
                       "access "
                       "keys\r\n",
                       (unsigned int)QLIB_SAMPLE_KD1_CONFIG_SECTION);
    sectCfg = cfgLine[die_u8].sectionConfigTable;

    sectCfg[QLIB_SAMPLE_KD1_CONFIG_SECTION].baseAddr = QLIB_SAMPLE_RESERVED_BOOT_SECTION_SIZE;
    sectCfg[QLIB_SAMPLE_KD1_CONFIG_SECTION].size     = QLIB_SAMPLE_KD1_CONFIG_SECTION_SIZE;

    sectCfg[QLIB_SAMPLE_KD1_CONFIG_SECTION].sectionConfig.policy.plainAccessReadEnable   = 0u;
    sectCfg[QLIB_SAMPLE_KD1_CONFIG_SECTION].sectionConfig.policy.plainAccessWriteEnable  = 0u;
    sectCfg[QLIB_SAMPLE_KD1_CONFIG_SECTION].sectionConfig.policy.authPlainAccess         = 0u;
    sectCfg[QLIB_SAMPLE_KD1_CONFIG_SECTION].sectionConfig.policy.checksumIntegrity       = 0u;
    sectCfg[QLIB_SAMPLE_KD1_CONFIG_SECTION].sectionConfig.policy.digestIntegrity         = 0u;
    sectCfg[QLIB_SAMPLE_KD1_CONFIG_SECTION].sectionConfig.policy.digestIntegrityOnAccess = 0u;
    sectCfg[QLIB_SAMPLE_KD1_CONFIG_SECTION].sectionConfig.policy.rollbackProt            = 0u;
    sectCfg[QLIB_SAMPLE_KD1_CONFIG_SECTION].sectionConfig.policy.slog                    = 0u;
    sectCfg[QLIB_SAMPLE_KD1_CONFIG_SECTION].sectionConfig.policy.writeProt               = 0u;

    /*-------------------------------------------------------------------------------------------------------
     Full and restricted access keys for the provisioned section, obtained via the standard sample helper
    -------------------------------------------------------------------------------------------------------*/
    QLIB_STATUS_RET_CHECK(QLIB_SAMPLE_GetSectionAccessKey(ctx,
                                                          wid,
                                                          QLIB_KID__FULL_ACCESS_SECTION,
                                                          QLIB_SAMPLE_KD1_CONFIG_SECTION,
                                                          (uint32_t)die_u8,
                                                          sectCfg[QLIB_SAMPLE_KD1_CONFIG_SECTION].fullAccessKey));

    QLIB_STATUS_RET_CHECK(QLIB_SAMPLE_GetSectionAccessKey(ctx,
                                                          wid,
                                                          QLIB_KID__RESTRICTED_ACCESS_SECTION,
                                                          QLIB_SAMPLE_KD1_CONFIG_SECTION,
                                                          (uint32_t)die_u8,
                                                          sectCfg[QLIB_SAMPLE_KD1_CONFIG_SECTION].restrictedKey));

    /*-------------------------------------------------------------------------------------------------------
     Write the new configuration to the device. This authenticates using preProvisionedMasterKey (=KD1)
     and programs the new deviceMasterKey and section 1 configuration.
    -------------------------------------------------------------------------------------------------------*/
    QLIB_STATUS_RET_CHECK(QLIB_ConfigDeviceMultiDie(ctx, &flashCfg, &dieCfg, cfgLine, diesNum_u8));
    QLIB_SAMPLE_PRINTF("> QLIB_SAMPLE_KD1Config: Configuration success\r\n");
    return QLIB_STATUS__OK;
}

/************************************************************************************************************
 * @internal
 * @brief       Opens a full-access session on the provisioned section, writes one page, reads it back and verify content.
 *
 * @param[in,out] ctx   QLIB context, already provisioned by @ref QLIB_SAMPLE_KD1Config_configureDevice_L
 * @param[in]     wid   Device winbond unique ID, used to derive the section full access key via
 *                      @ref QLIB_SAMPLE_GetSectionAccessKey
 * @endinternal
************************************************************************************************************/
static QLIB_STATUS_T QLIB_SAMPLE_KD1Config_verifyWriteRead_L(QLIB_CONTEXT_T* ctx, QLIB_WID_T wid)
{
    QLIB_STATUS_T status = QLIB_STATUS__OK;
    KEY_T         qlibSampleKd1FullKey;
    uint8_t       writeBuf_u8[QLIB_FLASH_PAGE_SIZE];
    uint8_t       readBuf_u8[QLIB_FLASH_PAGE_SIZE];

    (void)memset(writeBuf_u8, 0xa5, sizeof(writeBuf_u8));
    (void)memset(readBuf_u8, 0x00, sizeof(readBuf_u8));

    /*-------------------------------------------------------------------------------------------------------
     Retrieve the full access key configured for the provisioned section (same helper as in configure)
    -------------------------------------------------------------------------------------------------------*/
    QLIB_STATUS_RET_CHECK(QLIB_SAMPLE_GetSectionAccessKey(ctx,
                                                          wid,
                                                          QLIB_KID__FULL_ACCESS_SECTION,
                                                          QLIB_SAMPLE_KD1_CONFIG_SECTION,
                                                          0u,
                                                          qlibSampleKd1FullKey));

    /*-------------------------------------------------------------------------------------------------------
     Load the full access key for the provisioned section and open a full-access session
    -------------------------------------------------------------------------------------------------------*/
    QLIB_SAMPLE_PRINTF("> QLIB_SAMPLE_KD1Config: Load secure section %u full access key\r\n",
                       (unsigned int)QLIB_SAMPLE_KD1_CONFIG_SECTION);
    QLIB_STATUS_RET_CHECK(QLIB_LoadKey(ctx, QLIB_SAMPLE_KD1_CONFIG_SECTION, qlibSampleKd1FullKey, (bool)true));

    QLIB_SAMPLE_PRINTF("> QLIB_SAMPLE_KD1Config: Open full-access session on secure section %u\r\n",
                       (unsigned int)QLIB_SAMPLE_KD1_CONFIG_SECTION);
    QLIB_STATUS_RET_CHECK_GOTO(QLIB_OpenSession(ctx, QLIB_SAMPLE_KD1_CONFIG_SECTION, QLIB_SESSION_ACCESS_FULL),
                               status,
                               remove_key);

    /*-------------------------------------------------------------------------------------------------------
     Erase the section, write one page, read it back and verify
    -------------------------------------------------------------------------------------------------------*/
    QLIB_SAMPLE_PRINTF("> QLIB_SAMPLE_KD1Config: Erase, write, and read back section %u data\r\n",
                       (unsigned int)QLIB_SAMPLE_KD1_CONFIG_SECTION);
    QLIB_STATUS_RET_CHECK_GOTO(QLIB_EraseSection(ctx, QLIB_SAMPLE_KD1_CONFIG_SECTION, (bool)true), status, close_session);

    QLIB_STATUS_RET_CHECK_GOTO(QLIB_Write(ctx, writeBuf_u8, QLIB_SAMPLE_KD1_CONFIG_SECTION, 0u, QLIB_FLASH_PAGE_SIZE, (bool)true),
                               status,
                               close_session);

    QLIB_STATUS_RET_CHECK_GOTO(QLIB_Read(ctx,
                                         readBuf_u8,
                                         QLIB_SAMPLE_KD1_CONFIG_SECTION,
                                         0u,
                                         QLIB_FLASH_PAGE_SIZE,
                                         (bool)true,
                                         (bool)false),
                               status,
                               close_session);

    QLIB_ASSERT_WITH_ERROR_GOTO(0 == memcmp(readBuf_u8, writeBuf_u8, sizeof(writeBuf_u8)),
                                QLIB_STATUS__TEST_FAIL,
                                status,
                                close_session);

    QLIB_SAMPLE_PRINTF("> QLIB_SAMPLE_KD1Config: section %lu write/read round trip passed\r\n",
                       (unsigned long)QLIB_SAMPLE_KD1_CONFIG_SECTION);

    status = QLIB_STATUS__OK;

close_session:
    (void)QLIB_CloseSession(ctx, QLIB_SAMPLE_KD1_CONFIG_SECTION);

remove_key:
    (void)QLIB_RemoveKey(ctx, QLIB_SAMPLE_KD1_CONFIG_SECTION, (bool)true);

    return status;
}
