/************************************************************************************************************
* @internal
* @remark     Winbond Electronics Corporation - Confidential
* @copyright  Copyright (c) 2026 by Winbond Electronics Corporation . All rights reserved
* @endinternal
*
* @file       qlib_sample_run.c
* @brief      This file contains the main main execution function
*
*
* @page       qlib_sample_run
*             This sample includes the main sample function, performing sample dispatch based on the sample name. \n
*
************************************************************************************************************/

/*-----------------------------------------------------------------------------------------------------------
                                                   INCLUDES
-----------------------------------------------------------------------------------------------------------*/
#include "qlib.h"
#include "qlib_sample.h"
#include "qlib_sample_run.h"

#include "qlib_sample_fw_update.h"
#include "qlib_sample_safety.h"
#include "qlib_sample_wd.h"
#include "qlib_sample_secure_storage.h"
#include "qlib_sample_sign_verify.h"
#include "qlib_sample_get_ids.h"
#include "qlib_sample_vault.h"
#include "qlib_sample_secure_log.h"
#include "qlib_sample_recovery.h"
#include "qlib_sample_erase.h"
#include "qlib_sample_integrity.h"
#include "qlib_sample_direct_attestation.h"
#include "qlib_sample_cdi.h"
#ifdef OEM_SECTION_PROVISIONING
#include "qlib_sample_oem_section_provisioning.h"
#endif // OEM_SECTION_PROVISIONING
#ifdef W77Q_KDF_KD1
#include "qlib_sample_kd1_config.h"
#endif // W77Q_KDF_KD1
#ifdef QLIB_LMS_SUPPORT
#include "qlib_sample_lms_attestation.h"
#include "qlib_sample_lms_fw_update.h"
#include "qlib_sample_lms_key_backup.h"
#endif // QLIB_LMS_SUPPORT

/*-----------------------------------------------------------------------------------------------------------
                                                  FUNCTIONS
-----------------------------------------------------------------------------------------------------------*/
QLIB_STATUS_T QLIB_SAMPLE_Run(char* sampleName, void* userData_pv)
{
    QLIB_ASSERT_RET(sampleName != NULL, QLIB_STATUS__INVALID_PARAMETER);

#ifndef EXCLUDE_SAMPLE_IDS
    if (strcmp(sampleName, "ids") == 0u)
    {
        return QLIB_SAMPLE_GetIDs(userData_pv);
    }
    else
#endif // EXCLUDE_SAMPLE_IDS
#ifndef QLIB_ONLY_SECURE_STORAGE
        if (strcmp(sampleName, "recovery") == 0u)
    {
        return QLIB_SAMPLE_Recovery(userData_pv);
    }
    else if (strcmp(sampleName, "fw_update") == 0u)
    {
        return QLIB_SAMPLE_FwUpdate(userData_pv);
    }
    else if (strcmp(sampleName, "watchdog") == 0u)
    {
        return QLIB_SAMPLE_WatchDog(userData_pv);
    }
    else if (strcmp(sampleName, "log") == 0u)
    {
        return QLIB_SAMPLE_SecureLog(userData_pv);
    }
    else if (strcmp(sampleName, "vault") == 0u)
    {
        return QLIB_SAMPLE_Vault(userData_pv);
    }
    else if (strcmp(sampleName, "safety") == 0u)
    {
        return QLIB_SAMPLE_Safety(userData_pv);
    }
#ifdef QLIB_LMS_SUPPORT
    else if (strcmp(sampleName, "lms_fw_update") == 0u)
    {
        return QLIB_SAMPLE_LmsFwUpdate(userData_pv);
    }
    else if (strcmp(sampleName, "lms_key_backup") == 0u)
    {
        return QLIB_SAMPLE_LmsKeyBackup(userData_pv);
    }
    else if (strcmp(sampleName, "lms_attest") == 0u)
    {
        return QLIB_SAMPLE_LmsAttest(userData_pv);
    }
#else
#ifdef QLIB_SIGN_DATA_BY_FLASH
    else if (strcmp(sampleName, "sign_verify") == 0u)
    {
        return QLIB_SAMPLE_SignVerify(userData_pv);
    }
#endif // QLIB_SIGN_DATA_BY_FLASH
#endif // QLIB_LMS_SUPPORT
    else if (strcmp(sampleName, "erase") == 0u)
    {
        return QLIB_SAMPLE_Erase(userData_pv);
    }
    else if (strcmp(sampleName, "integrity") == 0u)
    {
        return QLIB_SAMPLE_Integrity(userData_pv);
    }
    else if (strcmp(sampleName, "dice_attest") == 0u)
    {
        return QLIB_SAMPLE_DiceAttestation(userData_pv, false);
    }
    else if (strcmp(sampleName, "direct_attest") == 0u)
    {
        return QLIB_SAMPLE_DirectAttestation(userData_pv);
    }
#ifdef OEM_SECTION_PROVISIONING
    else if (strcmp(sampleName, "section_keys") == 0u)
    {
        QLIB_STATUS_T ret;
        ret = QLIB_SAMPLE_SoC_OEM_SectionProvision(userData_pv);
        // This sample locks the configuration registers. unlock with recovery flow.
        (void)QLIB_SAMPLE_Recovery(userData_pv);
        return ret;
    }
#endif // OEM_SECTION_PROVISIONING
#ifdef W77Q_KDF_KD1
    else if (strcmp(sampleName, "kd1_config") == 0u)
    {
        return QLIB_SAMPLE_KD1Config(userData_pv);
    }
#endif // W77Q_KDF_KD1
    else
#endif // !QLIB_ONLY_SECURE_STORAGE
        if (strcmp(sampleName, "version") == 0u)
        {
            QLIB_SW_VERSION_T versionInfo;
            QLIB_STATUS_RET_CHECK(QLIB_GetVersion(&versionInfo));
            QLIB_SAMPLE_PRINTF("qlib version is %u.%u.%u.%u\r\n",
                               (uint8_t)((versionInfo.qlibVersion >> 24) & 0xFFu),
                               (uint8_t)((versionInfo.qlibVersion >> 16) & 0xFFu),
                               (uint8_t)((versionInfo.qlibVersion >> 8) & 0xFFu),
                               (uint8_t)(versionInfo.qlibVersion & 0xFFu));
            return QLIB_STATUS__OK;
        }
        else
        {
            // default
            return QLIB_SAMPLE_SecureStorage(userData_pv);
        }
}

