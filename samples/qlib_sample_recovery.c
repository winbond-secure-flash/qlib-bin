/************************************************************************************************************
* @internal
* @remark     Winbond Electronics Corporation - Confidential
* @copyright  Copyright (c) 2025 by Winbond Electronics Corporation . All rights reserved
* @endinternal
*
* @file       qlib_sample_recovery.c
* @brief      This file contains configuration recovery code
*
* ### project qlib_samples
*
-----------------------------------------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------------------------------------
                                                   INCLUDES
-----------------------------------------------------------------------------------------------------------*/

#include "qlib.h"
#include "qlib_sample.h"
#include "qlib_sample_recovery.h"

#ifndef QLIB_ONLY_SECURE_STORAGE
/*-----------------------------------------------------------------------------------------------------------
                                                 DEFINITIONS
-----------------------------------------------------------------------------------------------------------*/
#ifdef INIT_FAST_READ_DUMMY_CYCLES
#define RECOVERY_FAST_READ_DUMMY INIT_FAST_READ_DUMMY_CYCLES
#else
#define RECOVERY_FAST_READ_DUMMY FLASH_CR_DUMMY_DEFAULT
#endif // INIT_FAST_READ_DUMMY_CYCLES

/*-----------------------------------------------------------------------------------------------------------
                                         LOCAL FUNCTIONS DECLARATION
-----------------------------------------------------------------------------------------------------------*/

static void QLIB_SAMPLE_initLegacyConfig_L(QLIB_CONTEXT_T*             qlibContext,
                                           QLIB_FLASH_CONFIG_T*        flashCfg,
                                           QLIB_DIE_CONFIG_T*          dieCfg,
                                           QLIB_SECTIONS_CONF_TABLE_T* cfgSectionArray,
                                           uint32_t                    numOfDies_u32);

/*-----------------------------------------------------------------------------------------------------------
                                             INTERFACE FUNCTIONS
-----------------------------------------------------------------------------------------------------------*/
QLIB_STATUS_T QLIB_SAMPLE_Recovery(void* userData_pv)
{
#ifdef QLIB_SUPPORT_XIP
    TOUCH(userData_pv);
    /*-------------------------------------------------------------------------------------------------------
      recovery functionality is designed to run from RAM only since it formats the flash
    -------------------------------------------------------------------------------------------------------*/
    return QLIB_STATUS__NOT_SUPPORTED;
#else
    QLIB_CONTEXT_T    qlibContext;
    QLIB_BUS_FORMAT_T busFormat = QLIB_BUS_FORMAT(QLIB_BUS_MODE_1_1_1, (bool)false);
    QLIB_STATUS_T     status    = QLIB_STATUS__OK;

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
    Flash will obtain legacy configuration
    -------------------------------------------------------------------------------------------------------*/
    QLIB_STATUS_RET_CHECK_GOTO(QLIB_SAMPLE_Recovery_flow(&qlibContext), status, disconnect);

disconnect:
    (void)QLIB_Disconnect(&qlibContext);
    return status;
#endif // QLIB_SUPPORT_XIP
}

QLIB_STATUS_T QLIB_SAMPLE_Recovery_flow(QLIB_CONTEXT_T* qlibContext)
{
    bool                       nonSecureFormatEn_b;
    QLIB_DIE_CONFIG_T          dieCfg     = {0u};
    QLIB_DIE_CONFIG_T          origDieCfg = {0u};
    uint32_t                   sectionId_u32;
    QLIB_ID_T                  id;
    QLIB_FLASH_CONFIG_T        flashCfg;
    QLIB_FLASH_CONFIG_T        tempFlashCfg;
    QLIB_SECTIONS_CONF_TABLE_T cfgSectionArray[QLIB_MAX_DIES_SUPPORTED];
    uint8_t                    dieId_u8;

    QLIB_WATCHDOG_CONF_T watchdogDefault = {
        (bool)false,          // enable
        (bool)false,          // lfOscEn
        (bool)false,          // swResetEn
        (bool)false,          // authenticated
        0u,                   // sectionID
        QLIB_AWDT_TH_12_DAYS, // threshold
        (bool)false,          // lock
        0u,                   // oscRateHz
        (bool)false,          // fallbackEn (in flash that supports Q2_SUPPORT_AWDTCFG_FALLBACK)
    };

    QLIB_SAMPLE_initLegacyConfig_L(qlibContext, &flashCfg, &dieCfg, cfgSectionArray, QLIB_GET_NUM_OF_DIES(qlibContext));

    // See what chip is connected and its unique ids. We might need this info to obtain keys for this
    // chips according to formula or via DB, see QLIB_SAMPLE_GetDeviceKey() function
    QLIB_STATUS_RET_CHECK(QLIB_GetId(qlibContext, &id));

    // get pre-provision key
    QLIB_STATUS_RET_CHECK(QLIB_SAMPLE_GetDeviceKey(qlibContext,
                                                   id.std.uniqueID,
                                                   QLIB_KID__DEVICE_KEY_PRE_PROVISIONING,
                                                   dieCfg.preProvisionedMasterKey));

    // get master key
    QLIB_STATUS_RET_CHECK(
        QLIB_SAMPLE_GetDeviceKey(qlibContext, id.std.uniqueID, QLIB_KID__DEVICE_MASTER, dieCfg.deviceMasterKey));

    //get secret key
    QLIB_STATUS_RET_CHECK(
        QLIB_SAMPLE_GetDeviceKey(qlibContext, id.std.uniqueID, QLIB_KID__DEVICE_SECRET, dieCfg.deviceSecretKey));

    // get SUID
    QLIB_STATUS_RET_CHECK(QLIB_SAMPLE_GetSecureUID(qlibContext, id.std.uniqueID, dieCfg.suid));

    /*-------------------------------------------------------------------------------------------------------
     Ensure device can be formatted and configured
     ------------------------------------------------------------------------------------------------------*/
    QLIB_STATUS_RET_CHECK(
        QLIB_GetDeviceConfigMultiDie(qlibContext, &tempFlashCfg, &origDieCfg, NULL, QLIB_GET_NUM_OF_DIES(qlibContext)));
    nonSecureFormatEn_b = origDieCfg.nonSecureFormatEn;
    if ((bool)false == nonSecureFormatEn_b)
    {

        (void)memcpy(&flashCfg.watchdogDefault, &watchdogDefault, sizeof(QLIB_WATCHDOG_CONF_T));

        QLIB_STATUS_RET_CHECK(
            QLIB_ConfigDeviceMultiDie(qlibContext, &flashCfg, &dieCfg, NULL, QLIB_GET_NUM_OF_DIES(qlibContext)));
    }

    /*-------------------------------------------------------------------------------------------------------
     Format the flash - clean from all it's data and configurations
    -------------------------------------------------------------------------------------------------------*/
    QLIB_STATUS_RET_CHECK(QLIB_Format(qlibContext, NULL, (bool)false, (bool)false));

    /*-------------------------------------------------------------------------------------------------------
     Write the configuration to the flash device
    -------------------------------------------------------------------------------------------------------*/

    (void)memcpy(&flashCfg.watchdogDefault, &watchdogDefault, sizeof(QLIB_WATCHDOG_CONF_T));

    for (dieId_u8 = 0u; dieId_u8 < QLIB_GET_NUM_OF_DIES(qlibContext); ++dieId_u8)
    {
        for (sectionId_u32 = 0u; sectionId_u32 < QLIB_NUM_OF_SECTIONS; ++sectionId_u32)
        {
            if (cfgSectionArray[dieId_u8].sectionConfigTable[sectionId_u32].size > 0u)
            {
                QLIB_STATUS_RET_CHECK(
                    QLIB_SAMPLE_GetSectionAccessKey(qlibContext,
                                                    id.std.uniqueID,
                                                    QLIB_KID__FULL_ACCESS_SECTION,
                                                    sectionId_u32,
                                                    0u,
                                                    cfgSectionArray[dieId_u8].sectionConfigTable[sectionId_u32].fullAccessKey));
                QLIB_STATUS_RET_CHECK(
                    QLIB_SAMPLE_GetSectionAccessKey(qlibContext,
                                                    id.std.uniqueID,
                                                    QLIB_KID__RESTRICTED_ACCESS_SECTION,
                                                    sectionId_u32,
                                                    0u,
                                                    cfgSectionArray[dieId_u8].sectionConfigTable[sectionId_u32].restrictedKey));
            }
        }
    }
    QLIB_STATUS_RET_CHECK(
        QLIB_ConfigDeviceMultiDie(qlibContext, &flashCfg, &dieCfg, cfgSectionArray, QLIB_GET_NUM_OF_DIES(qlibContext)));


#ifndef EXCLUDE_4_BYTES_ADDRESS_MODE
    if (QLIB_FeatureSupported(qlibContext, W77Q_FEATURE_4_BYTES_ADDRESS_MODE) != 0u)
    {
#ifdef SPI_INIT_ADDRESS_MODE_4_BYTES
        QLIB_STATUS_RET_CHECK(QLIB_SetAddressMode(qlibContext, QLIB_STD_ADDR_MODE__4_BYTE));
#else
        QLIB_STATUS_RET_CHECK(QLIB_SetAddressMode(qlibContext, QLIB_STD_ADDR_MODE__3_BYTE));
#endif // SPI_INIT_ADDRESS_MODE_4_BYTES
    }
#endif // EXCLUDE_4_BYTES_ADDRESS_MODE

#ifndef EXCLUDE_FAST_READ_DUMMY_CONFIG
    if (QLIB_FeatureSupported(qlibContext, W77Q_FEATURE_FAST_READ_DUMMY_CONFIG) != 0u)
    {
        QLIB_STATUS_RET_CHECK(QLIB_SetFastReadDummyCycles(qlibContext, flashCfg.fastReadDummyCycles));
    }
#endif // EXCLUDE_FAST_READ_DUMMY_CONFIG

    return QLIB_STATUS__OK;
}


/*-----------------------------------------------------------------------------------------------------------
                                               LOCAL FUNCTIONS
-----------------------------------------------------------------------------------------------------------*/

static void QLIB_SAMPLE_initLegacyConfig_L(QLIB_CONTEXT_T*             qlibContext,
                                           QLIB_FLASH_CONFIG_T*        flashCfg,
                                           QLIB_DIE_CONFIG_T*          dieCfg,
                                           QLIB_SECTIONS_CONF_TABLE_T* cfgSectionArray,
                                           uint32_t                    numOfDies_u32)
{
    uint32_t dieId_u32;

    // map section 0,1 for legacy access to all the flash size
#if !defined(__cplusplus) || (__cplusplus >= 202002L)
#define FLASH_DIE_SECTION_CONFIG_TABLE_DESIGNATOR .sectionConfigTable =
#else
#define FLASH_DIE_SECTION_CONFIG_TABLE_DESIGNATOR
#endif // __cplusplus

#define FLASH_DIE_LEGACY_SECTION_ARRAY                        \
    {                                                         \
        FLASH_DIE_SECTION_CONFIG_TABLE_DESIGNATOR             \
        {                                                     \
            {                                                 \
                /* Section 0 */                               \
                0u, /* baseAddr */                            \
                0u, /* size */                                \
                {                                             \
                    {                                         \
                        /* policy */                          \
                        0u, /* digestIntegrity */             \
                        0u, /* checksumIntegrity */           \
                        0u, /* writeProt */                   \
                        0u, /* rollbackProt */                \
                        1u, /* plainAccessWriteEnable */      \
                        1u, /* plainAccessReadEnable */       \
                        0u, /* authPlainAccess */             \
                        0u, /* digestIntegrityOnAccess */     \
                        0u, /* slog */                        \
                    },                                        \
                    0u,  /* section crc */                    \
                    0u,  /* section digest */                 \
                    0u,  /* version */                        \
                    {0u} /* postActions */                    \
                },                                            \
                (bool)true, /* resetPA */                     \
                {0u},       /* restrictedKey */               \
                {0u},       /* fullAccessKey */               \
                {0u},       /* lmsKey */                      \
            },                                                \
                {                                             \
                    /* Section 1 */                           \
                    0u, /* baseAddr */                        \
                    0u, /* size */                            \
                    {                                         \
                        {                                     \
                            /* policy */                      \
                            0u, /* digestIntegrity */         \
                            0u, /* checksumIntegrity */       \
                            0u, /* writeProt */               \
                            0u, /* rollbackProt */            \
                            1u, /* plainAccessWriteEnable */  \
                            1u, /* plainAccessReadEnable */   \
                            0u, /* authPlainAccess */         \
                            0u, /* digestIntegrityOnAccess */ \
                            0u, /* slog */                    \
                        },                                    \
                        0u,  /* section crc */                \
                        0u,  /* section digest */             \
                        0u,  /* version */                    \
                        {0u} /* postActions */                \
                    },                                        \
                    (bool)false, /* resetPA */                \
                    {0u},        /* restrictedKey */          \
                    {0u},        /* fullAccessKey */          \
                    {0u},        /* lmsKey */                 \
                },                                            \
                {0u}, /* Section 2 */                         \
                {0u}, /* Section 3 */                         \
                {0u}, /* Section 4 */                         \
                {0u}, /* Section 5 */                         \
                {0u}, /* Section 6 */                         \
                {0u}, /* Section 7 */                         \
            {                                                 \
                0u                                            \
            } /* Section 8u (vault) */                        \
        }                                                     \
    }

    QLIB_SECTIONS_CONF_TABLE_T cfgSectionArray_l[QLIB_MAX_DIES_SUPPORTED] =
    { FLASH_DIE_LEGACY_SECTION_ARRAY, // die 0u
#if QLIB_MAX_DIES_SUPPORTED > 1u
      FLASH_DIE_LEGACY_SECTION_ARRAY, // die 1

#endif // QLIB_MAX_DIES_SUPPORTED > 1u
#if QLIB_MAX_DIES_SUPPORTED == 4u
      FLASH_DIE_LEGACY_SECTION_ARRAY, // die 2
      FLASH_DIE_LEGACY_SECTION_ARRAY, // die 3

#endif // QLIB_MAX_DIES_SUPPORTED == 4u
    };

    QLIB_FLASH_CONFIG_T flashCfg_l = {
        {0u}, // watchdogDefault
        {
            // pinMux
            QLIB_IO23_MODE__QUAD, // IO2 and IO3 mux
            (bool)true,           // Dedicated reset-in mux
        },
        {
            24u, // addrLen
            3u,  // sidLen
#ifdef SPI_INIT_ADDRESS_MODE_4_BYTES
            QLIB_STD_ADDR_MODE__4_BYTE, // addrMode
#else
            QLIB_STD_ADDR_MODE__3_BYTE, // addrMode
#endif // SPI_INIT_ADDRESS_MODE_4_BYTES
        },
        RECOVERY_FAST_READ_DUMMY, // fastReadDummyCycles
        (bool)false               // dqsDisable
    };

    QLIB_DIE_CONFIG_T dieCfg_l =
    { {0u},        // deviceMasterKey
      {0u},        // deviceSecretKey
      {0u},        // preProvisionedMasterKey
      {0u},        // suid
      (bool)false, // safeFB
      (bool)false, // Fallback fail
      (bool)false, // Fallback select
      1u,          // Fallback mapping
      (bool)false, // speculCK
      (bool)true,  // nonSecureFormatEn_b
      (bool)false, // rngPlainAccessEn
      (bool)false, // ctagModeMulti
      (bool)false, // devcfgLock
#if QLIB_MAX_DIES_SUPPORTED > 1
      (bool)false, // FBIsPerDieConfig
#endif // QLIB_MAX_DIES_SUPPORTED > 1
    };

    TOUCH(qlibContext);

    if (NULL != flashCfg)
    {
        (void)memcpy((uint8_t*)flashCfg, (uint8_t*)&flashCfg_l, sizeof(QLIB_FLASH_CONFIG_T));
        flashCfg->stdAddrSize.addrLen_u8 =
            (LOG2(QLIB_GetFlashSize(qlibContext)) >= 24u ? 24u : (LOG2(QLIB_GetFlashSize(qlibContext)) == 23u ? 23u : 22u));
    }

    if (NULL != dieCfg)
    {
        (void)memcpy((uint8_t*)dieCfg, (uint8_t*)&dieCfg_l, sizeof(QLIB_DIE_CONFIG_T));
    }

    if (NULL != cfgSectionArray)
    {
        (void)memcpy((uint8_t*)cfgSectionArray, (uint8_t*)cfgSectionArray_l, sizeof(cfgSectionArray_l));

        for (dieId_u32 = 0u; dieId_u32 < numOfDies_u32; ++dieId_u32)
        {
            cfgSectionArray[dieId_u32].sectionConfigTable[0].size = MIN(QLIB_GetFlashSize(qlibContext), QLIB_MAX_SECTION_SIZE);
            cfgSectionArray[dieId_u32].sectionConfigTable[1].size =
                (cfgSectionArray[dieId_u32].sectionConfigTable[0].size == QLIB_GetFlashSize(qlibContext))
                    ? 0u
                    : MIN((QLIB_GetFlashSize(qlibContext) - QLIB_MAX_SECTION_SIZE), QLIB_MAX_SECTION_SIZE);
            cfgSectionArray[dieId_u32].sectionConfigTable[1].baseAddr =
                (cfgSectionArray[dieId_u32].sectionConfigTable[1].size > 0u) ? QLIB_MAX_SECTION_SIZE : 0u;

            if (QLIB_FeatureSupported(qlibContext, W77Q_FEATURE_VAULT) != 0u)
            {
                cfgSectionArray[dieId_u32].sectionConfigTable[QLIB_SECTION_ID_VAULT].size = SIZE_128KB_;
            }
        }
    }
}

#endif // QLIB_ONLY_SECURE_STORAGE
