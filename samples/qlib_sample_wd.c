/************************************************************************************************************
* @internal
* @remark     Winbond Electronics Corporation - Confidential
* @copyright  Copyright (c) 2019 by Winbond Electronics Corporation . All rights reserved
* @endinternal
*
* @file       qlib_sample_wd.c
* @brief      This file contains QLIB WD related functions implementation
*
* @example    qlib_sample_wd.c
*
* @page       watchdog watchdog sample code
* This sample code shows watchdog protection
*
* @include    samples/qlib_sample_wd.c
*
************************************************************************************************************/

/*-----------------------------------------------------------------------------------------------------------
                                                  INCLUDES
-----------------------------------------------------------------------------------------------------------*/
#include "qlib.h"
#include "qlib_sample.h"
#include "qlib_sample_wd.h"

#ifndef QLIB_ONLY_SECURE_STORAGE
/*-----------------------------------------------------------------------------------------------------------
                                                 DEFINITIONS
-----------------------------------------------------------------------------------------------------------*/
#define QLIB_AWDTCFG__OSC_RATE_KHZ_DEFAULT 54u

/*-----------------------------------------------------------------------------------------------------------
                                             INTERFACE FUNCTIONS
-----------------------------------------------------------------------------------------------------------*/

QLIB_STATUS_T QLIB_SAMPLE_WatchDogConfig(QLIB_CONTEXT_T* qlibContext,
                                         bool            enable_b,
                                         bool            secure_b,
                                         QLIB_AWDT_TH_T  threshold,
                                         uint32_t        section_u32)
{
    QLIB_WATCHDOG_CONF_T watchdogCFG;
    QLIB_WATCHDOG_CONF_T prevWatchdogCFG;
    QLIB_STATUS_T        status = QLIB_STATUS__OK;

    watchdogCFG.enable        = enable_b;
    watchdogCFG.authenticated = secure_b;
    watchdogCFG.sectionID     = section_u32;
    watchdogCFG.threshold     = threshold;

    /*-------------------------------------------------------------------------------------------------------
     The rest of WD features are not among the parameters of this sample and are board related.
     This is why we use constant values.
    -------------------------------------------------------------------------------------------------------*/
    watchdogCFG.lfOscEn    = (bool)true;
    watchdogCFG.swResetEn  = (bool)false;
    watchdogCFG.lock       = (bool)false;
    watchdogCFG.oscRateHz  = 0u;
    watchdogCFG.fallbackEn = (bool)false;

    /*-------------------------------------------------------------------------------------------------------
     In case of secure watchdog we need to open session to the section WD which it is associated with.
    -------------------------------------------------------------------------------------------------------*/
    QLIB_STATUS_RET_CHECK(QLIB_Watchdog_ConfigGet(qlibContext, &prevWatchdogCFG));
    if ((bool)true == prevWatchdogCFG.authenticated)
    {
        QLIB_STATUS_RET_CHECK(QLIB_OpenSession(qlibContext, prevWatchdogCFG.sectionID, QLIB_SESSION_ACCESS_FULL));
    }

    /*-------------------------------------------------------------------------------------------------------
     Configure the watchdog with the parameters defined above.
    -------------------------------------------------------------------------------------------------------*/
    QLIB_STATUS_RET_CHECK_GOTO(QLIB_Watchdog_ConfigSet(qlibContext, &watchdogCFG), status, close_session);

    /*-------------------------------------------------------------------------------------------------------
     Now WD is configured and the counter on flash starts to count to 2^^threshold, incrementing every
     second ( not accurate ). Once it gets there reset pin is set, so you need to touch (pet) the
     watchdog to zero the timer.
    -------------------------------------------------------------------------------------------------------*/

close_session:
    if ((bool)true == prevWatchdogCFG.authenticated)
    {
        (void)QLIB_CloseSession(qlibContext, prevWatchdogCFG.sectionID);
    }

    return status;
}

QLIB_STATUS_T QLIB_SAMPLE_WatchDogTouch(QLIB_CONTEXT_T* qlibContext, bool secure_b, uint32_t section_u32)
{
    QLIB_STATUS_T status = QLIB_STATUS__OK;

    if (secure_b == (bool)true)
    {
        /*---------------------------------------------------------------------------------------------------
         In case of secure watchdog we need to open session to the section WD which it is associated with.
        ---------------------------------------------------------------------------------------------------*/
        QLIB_STATUS_RET_CHECK(QLIB_OpenSession(qlibContext, section_u32, QLIB_SESSION_ACCESS_FULL));
    }

    /*-------------------------------------------------------------------------------------------------------
     Touch the watchdog to zero the flash counter.
    -------------------------------------------------------------------------------------------------------*/
    QLIB_STATUS_RET_CHECK_GOTO(QLIB_Watchdog_Touch(qlibContext), status, close_session);

    /*-------------------------------------------------------------------------------------------------------
     Now the counter on flash is zeroed. This prevents from the counter to reach the threshold and
     reset the chip.
    -------------------------------------------------------------------------------------------------------*/
close_session:
    if ((bool)true == secure_b)
    {
        (void)QLIB_CloseSession(qlibContext, section_u32);
    }

    return status;
}

QLIB_STATUS_T QLIB_SAMPLE_WatchDogGet(QLIB_CONTEXT_T* qlibContext,
                                      bool            secure_b,
                                      uint32_t        section_u32,
                                      uint32_t*       time_pu32,
                                      bool*           expired_pb)
{
    QLIB_STATUS_T status = QLIB_STATUS__OK;

    if (secure_b == (bool)true)
    {
        /*---------------------------------------------------------------------------------------------------
         in case of secure watchdog you we need to open session to the section WD is associated with
        ---------------------------------------------------------------------------------------------------*/
        QLIB_STATUS_RET_CHECK(QLIB_OpenSession(qlibContext, section_u32, QLIB_SESSION_ACCESS_FULL));
    }

    QLIB_STATUS_RET_CHECK_GOTO(QLIB_Watchdog_Get(qlibContext, time_pu32, expired_pb), status, close_session);
    /*-------------------------------------------------------------------------------------------------------
     if *expired_pb is true then WD was not touched and already expired, in this case time_pu32 value you get
     is irrelevant
    -------------------------------------------------------------------------------------------------------*/
close_session:
    if ((bool)true == secure_b)
    {
        (void)QLIB_CloseSession(qlibContext, section_u32);
    }

    return status;
}

QLIB_STATUS_T QLIB_SAMPLE_WatchDog(void* userData_pv)
{
    QLIB_CONTEXT_T    qlibContext;
    QLIB_BUS_FORMAT_T busFormat = QLIB_BUS_FORMAT(QLIB_BUS_MODE_1_1_1, (bool)false);
    //KEY_T             deviceMasterKey;
    //QLIB_ID_T         id;

    /*-------------------------------------------------------------------------------------------------------
     Initialization part
    -------------------------------------------------------------------------------------------------------*/

    /*-------------------------------------------------------------------------------------------------------
     Init QLIB  - needed when running QLIB either on a device or on a remote server.
    -------------------------------------------------------------------------------------------------------*/
    QLIB_STATUS_RET_CHECK(QLIB_InitLib(&qlibContext));

    /*-------------------------------------------------------------------------------------------------------
     Set user data - store a pointer to data the user might need for its platform
    -------------------------------------------------------------------------------------------------------*/
    QLIB_SetUserData(&qlibContext, userData_pv);

    /*-------------------------------------------------------------------------------------------------------
     Take the ownership of flash communication channel (it belongs to local MCU or to remote server, exclusively)
    -------------------------------------------------------------------------------------------------------*/
    QLIB_STATUS_RET_CHECK(QLIB_Connect(&qlibContext));

    /*-------------------------------------------------------------------------------------------------------
     Init Flash Device - not needed when using QLIB on a remote server
    -------------------------------------------------------------------------------------------------------*/
    QLIB_STATUS_RET_CHECK(QLIB_InitDevice(&qlibContext, busFormat));

    /*-------------------------------------------------------------------------------------------------------
     Run sample code configuration
    -------------------------------------------------------------------------------------------------------*/
    QLIB_STATUS_RET_CHECK(QLIB_SAMPLE_WatchDog_manuf_flow(&qlibContext));

    /*-------------------------------------------------------------------------------------------------------
     Run sample flow
    -------------------------------------------------------------------------------------------------------*/
    QLIB_STATUS_RET_CHECK(QLIB_SAMPLE_WatchDog_field_flow(&qlibContext));

    /*-------------------------------------------------------------------------------------------------------
     Release the ownership of flash communication channel (it belongs to local MCU or to remote server, exclusively)
    -------------------------------------------------------------------------------------------------------*/
    (void)QLIB_Disconnect(&qlibContext);

    return QLIB_STATUS__OK;
}

QLIB_STATUS_T QLIB_SAMPLE_WatchDog_manuf_flow(QLIB_CONTEXT_T* qlibContext)
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
    // chips according to formula or via DB
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

    /*-------------------------------------------------------------------------------------------------------
     This is the default WD configuration stored in section configuration loaded on power up.
     Be careful with it, since if you configure and enable default WD threshold which is very short, flash may signal reset
     even before board initialization is over.
    -------------------------------------------------------------------------------------------------------*/
    sect_u32                               = 1u;
    flashCfg.watchdogDefault.sectionID     = sect_u32;
    flashCfg.watchdogDefault.authenticated = (bool)false;
    flashCfg.watchdogDefault.enable        = (bool)true;
    flashCfg.watchdogDefault.fallbackEn    = (bool)false;
    flashCfg.watchdogDefault.lfOscEn       = (bool)true;
    flashCfg.watchdogDefault.lock          = (bool)false;
    flashCfg.watchdogDefault.oscRateHz     = 0u;
    flashCfg.watchdogDefault.swResetEn     = (bool)false;
    flashCfg.watchdogDefault.threshold     = QLIB_AWDT_TH_12_DAYS;

    // adding a section for the watchdog
#if QLIB_MAX_DIES_SUPPORTED > 1u
    QLIB_STATUS_RET_CHECK(QLIB_GetActiveDie(qlibContext, &die_u8));
#endif // QLIB_MAX_DIES_SUPPORTED > 1u
    sectCfg = cfgLine[die_u8].sectionConfigTable;

    sectCfg[sect_u32].baseAddr = QLIB_SAMPLE_RESERVED_BOOT_SECTION_SIZE;
    sectCfg[sect_u32].size     = SIZE_256KB_;

    sectCfg[sect_u32].sectionConfig.policy.plainAccessReadEnable   = 1u;
    sectCfg[sect_u32].sectionConfig.policy.plainAccessWriteEnable  = 0u;
    sectCfg[sect_u32].sectionConfig.policy.authPlainAccess         = 1u;
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

    /*-------------------------------------------------------------------------------------------------------
     Flash loads the above watchdog configuration only at power up, therefore a hard reset is required for
     the configuration to apply
    -------------------------------------------------------------------------------------------------------*/

#if QLIB_MAX_DIES_SUPPORTED > 1u
    // write configuration sets die to 0. return to orig die
    QLIB_STATUS_RET_CHECK(QLIB_SetActiveDie(qlibContext, die_u8));
#endif // QLIB_MAX_DIES_SUPPORTED > 1u

    return QLIB_STATUS__OK;
}

QLIB_STATUS_T QLIB_SAMPLE_WatchDog_field_flow(QLIB_CONTEXT_T* qlibContext)
{
    QLIB_STATUS_T status      = QLIB_STATUS__OK;
    uint32_t      section_u32 = 1u;
    KEY_T         key;
    QLIB_ID_T     id;
    uint8_t       die_u8 = 0u;
    uint32_t      time_u32;
    bool          expired_b;

    /*-------------------------------------------------------------------------------------------------------
     obtain access key and load the key to library
     If secured WD is used it is associated with the section number and we must have correct full access key to that section_u32.
    -------------------------------------------------------------------------------------------------------*/
    QLIB_STATUS_RET_CHECK(QLIB_GetId(qlibContext, &id));
#if QLIB_MAX_DIES_SUPPORTED > 1u
    QLIB_STATUS_RET_CHECK(QLIB_GetActiveDie(qlibContext, &die_u8));
#endif // QLIB_MAX_DIES_SUPPORTED > 1u
    QLIB_STATUS_RET_CHECK(
        QLIB_SAMPLE_GetSectionAccessKey(qlibContext, id.sec.wid, QLIB_KID__FULL_ACCESS_SECTION, section_u32, die_u8, key));
    QLIB_STATUS_RET_CHECK(QLIB_LoadKey(qlibContext, section_u32, key, (bool)true));

    /*-------------------------------------------------------------------------------------------------------
     We configure WD to signal every 4 minutes (approximately) . Now its counter starts to count up, and if not
     "touched", then after 4 minutes the reset bit will be raised.
    -------------------------------------------------------------------------------------------------------*/
    QLIB_STATUS_RET_CHECK_GOTO(QLIB_SAMPLE_WatchDogConfig(qlibContext,
                                                          (bool)true,
                                                          (bool)true,
                                                          QLIB_AWDT_TH_4_MINUTES,
                                                          section_u32),
                               status,
                               remove_key);

    /*-------------------------------------------------------------------------------------------------------
     Check whether the WD counter is expired
    -------------------------------------------------------------------------------------------------------*/
    QLIB_STATUS_RET_CHECK_GOTO(QLIB_SAMPLE_WatchDogGet(qlibContext, (bool)true, section_u32, &time_u32, &expired_b),
                               status,
                               remove_key);

    /*-------------------------------------------------------------------------------------------------------
     Touching the WD will zero the counter and WD will bark only after 4 minutes again...
     Configure this in timer loop to continuously pet the WD.
    -------------------------------------------------------------------------------------------------------*/
    QLIB_STATUS_RET_CHECK_GOTO(QLIB_SAMPLE_WatchDogTouch(qlibContext, (bool)true, section_u32), status, remove_key);

    /*-------------------------------------------------------------------------------------------------------
     Disable WD
    -------------------------------------------------------------------------------------------------------*/
    QLIB_STATUS_RET_CHECK_GOTO(QLIB_SAMPLE_WatchDogConfig(qlibContext,
                                                          (bool)false,
                                                          (bool)true,
                                                          QLIB_AWDT_TH_4_MINUTES,
                                                          section_u32),
                               status,
                               remove_key);

    /*-------------------------------------------------------------------------------------------------------
     Clean the key reference from QLIB. optional
    -------------------------------------------------------------------------------------------------------*/
    QLIB_STATUS_RET_CHECK(QLIB_RemoveKey(qlibContext, section_u32, (bool)true));

    return QLIB_STATUS__OK;

remove_key:
    (void)QLIB_RemoveKey(qlibContext, section_u32, (bool)true);
    return status;
}


#endif // QLIB_ONLY_SECURE_STORAGE
