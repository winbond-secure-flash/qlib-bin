/************************************************************************************************************
* @internal
* @remark     Winbond Electronics Corporation - Confidential
* @copyright  Copyright (c) 2025 by Winbond Electronics Corporation . All rights reserved
* @endinternal
*
* @file       qlib_sample_fw.c
* @brief      This file contains QLIB sample specific functions
*
* @example    qlib_sample_fw.c
*
* This sample code shows fw specific function implementation .\n
*
* @include    samples/qlib_sample_fw.c
*
************************************************************************************************************/

/*-----------------------------------------------------------------------------------------------------------
                                                   INCLUDES
-----------------------------------------------------------------------------------------------------------*/

#include "qlib_sample.h"
#if defined W77Q_KDF_KD1 || defined QLIB_KDF_DEV_KEYS
#include "qlib_utils_cmkdf_gen_flash_key.h"
#endif // defined W77Q_KDF_KD1 || defined QLIB_KDF_DEV_KEYS

/*-----------------------------------------------------------------------------------------------------------
                                                   LOCAL VARIABLES
-----------------------------------------------------------------------------------------------------------*/

#ifdef W77Q_KDF_KD1
static const uint8_t cms_au8_L[] = QLIB_SAMPLE_CMS;
#endif // W77Q_KDF_KD1
#ifdef QLIB_KDF_DEV_KEYS
static const uint8_t devSecret_au8_L[] = QLIB_SAMPLE_DEV_SECRET;
#endif // QLIB_KDF_DEV_KEYS

/*-----------------------------------------------------------------------------------------------------------
                                             INTERFACE FUNCTIONS
-----------------------------------------------------------------------------------------------------------*/

QLIB_STATUS_T QLIB_SAMPLE_GetSectionAccessKey(QLIB_CONTEXT_T* ctx,
                                              QLIB_WID_T      wid,
                                              QLIB_KID_TYPE_T keyType,
                                              uint32_t        section_u32,
                                              uint32_t        die_u32,
                                              KEY_T           key)
{
    QLIB_ASSERT_RET((keyType == QLIB_KID__FULL_ACCESS_SECTION) || (keyType == QLIB_KID__RESTRICTED_ACCESS_SECTION),
                    QLIB_STATUS__INVALID_PARAMETER);
    QLIB_ASSERT_RET(section_u32 < QLIB_NUM_OF_SECTIONS, QLIB_STATUS__INVALID_PARAMETER);
    QLIB_ASSERT_RET(key != NULL, QLIB_STATUS__INVALID_PARAMETER);
    QLIB_ASSERT_RET(ctx != NULL, QLIB_STATUS__INVALID_PARAMETER);
    QLIB_ASSERT_RET(die_u32 < QLIB_GET_NUM_OF_DIES(ctx), QLIB_STATUS__INVALID_PARAMETER);
    QLIB_ASSERT_RET(wid != NULL, QLIB_STATUS__INVALID_PARAMETER);
#ifdef QLIB_KDF_DEV_KEYS
    QLIB_ASSERT_RET(QLIB_UTILS_CMKDF_KeyDerive(devSecret_au8_L,
                                               (uint8_t*)wid,
                                               keyType == QLIB_KID__FULL_ACCESS_SECTION ? QLIB_UTILS_CMKDF_LABEL_FK
                                                                                        : QLIB_UTILS_CMKDF_LABEL_RK,
                                               (QLIB_NUM_OF_SECTIONS * die_u32) + section_u32,
                                               QLIB_NUM_OF_SECTIONS * QLIB_GET_NUM_OF_DIES(ctx),
                                               (uint8_t*)key,
                                               sizeof(KEY_T)) == 0,
                    QLIB_STATUS__COMMAND_FAIL);
#else
    {
        static const KEY_ARRAY_T rKeys = {QLIB_SAMPLE_RESTRICTED_K_0,
                                          QLIB_SAMPLE_RESTRICTED_K_1,
                                          QLIB_SAMPLE_RESTRICTED_K_2,
                                          QLIB_SAMPLE_RESTRICTED_K_3,
                                          QLIB_SAMPLE_RESTRICTED_K_4,
                                          QLIB_SAMPLE_RESTRICTED_K_5,
                                          QLIB_SAMPLE_RESTRICTED_K_6,
                                          QLIB_SAMPLE_RESTRICTED_K_7,
                                          QLIB_SAMPLE_VAULT_RESTRICTED};
        static const KEY_ARRAY_T fKeys = {QLIB_SAMPLE_FULL_ACCESS_K_0,
                                          QLIB_SAMPLE_FULL_ACCESS_K_1,
                                          QLIB_SAMPLE_FULL_ACCESS_K_2,
                                          QLIB_SAMPLE_FULL_ACCESS_K_3,
                                          QLIB_SAMPLE_FULL_ACCESS_K_4,
                                          QLIB_SAMPLE_FULL_ACCESS_K_5,
                                          QLIB_SAMPLE_FULL_ACCESS_K_6,
                                          QLIB_SAMPLE_FULL_ACCESS_K_7,
                                          QLIB_SAMPLE_VAULT_FULL_ACCESS};

        (void)memcpy(key, keyType == QLIB_KID__FULL_ACCESS_SECTION ? fKeys[section_u32] : rKeys[section_u32], sizeof(KEY_T));
    }
#endif // QLIB_KDF_DEV_KEYS
    return QLIB_STATUS__OK;
}

#ifndef QLIB_ONLY_SECURE_STORAGE
QLIB_STATUS_T QLIB_SAMPLE_GetSectionLmsKey(QLIB_CONTEXT_T* ctx,
                                           QLIB_WID_T      wid,
                                           uint32_t        section_u32,
                                           uint32_t        die_u32,
                                           LMS_PUB_KEY_T   key)
{
    (void)ctx;
    (void)wid;
    (void)die_u32;
    static const LMS_PUB_KEY_T keys[QLIB_NUM_OF_SECTIONS] = {QLIB_SAMPLE_LMS_K_0,
                                                             QLIB_SAMPLE_LMS_K_1,
                                                             QLIB_SAMPLE_LMS_K_2,
                                                             QLIB_SAMPLE_LMS_K_3,
                                                             QLIB_SAMPLE_LMS_K_4,
                                                             QLIB_SAMPLE_LMS_K_5,
                                                             QLIB_SAMPLE_LMS_K_6,
                                                             QLIB_SAMPLE_LMS_K_7,
                                                             QLIB_SAMPLE_LMS_K_8};

    QLIB_ASSERT_RET(section_u32 < QLIB_NUM_OF_SECTIONS, QLIB_STATUS__INVALID_PARAMETER);

    (void)memcpy(key, keys[section_u32], sizeof(LMS_PUB_KEY_T));

    return QLIB_STATUS__OK;
}
#endif // QLIB_ONLY_SECURE_STORAGE

#ifndef QLIB_ONLY_SECURE_STORAGE
QLIB_STATUS_T QLIB_SAMPLE_GetDeviceKey(QLIB_CONTEXT_T* ctx, QLIB_WID_T wid, QLIB_KID_TYPE_T keyType, KEY_T deviceKey)
{
    QLIB_ASSERT_RET(ctx != NULL, QLIB_STATUS__INVALID_PARAMETER);
    QLIB_ASSERT_RET(wid != NULL, QLIB_STATUS__INVALID_PARAMETER);
    QLIB_ASSERT_RET(deviceKey != NULL, QLIB_STATUS__INVALID_PARAMETER);

    if (keyType == QLIB_KID__DEVICE_KEY_PRE_PROVISIONING)
    {
        if (QLIB_FeatureSupported(ctx, W77Q_FEATURE_PRE_PROV_MASTER_KEY) != (bool)false)
        {
#ifdef W77Q_KDF_KD1
            QLIB_ASSERT_RET(QLIB_UTILS_CMKDF_KeyDerive(cms_au8_L,
                                                       (uint8_t*)wid,
                                                       QLIB_UTILS_CMKDF_LABEL_KD1,
                                                       0u,
                                                       1u,
                                                       (uint8_t*)deviceKey,
                                                       sizeof(KEY_T)) == 0,
                            QLIB_STATUS__COMMAND_FAIL);
#else
            KEY_T key = QLIB_SAMPLE_KD1;
            (void)memcpy(deviceKey, key, sizeof(KEY_T));
#endif // W77Q_KDF_KD1
        }
        else
        {
            (void)memset(deviceKey, 0u, sizeof(KEY_T));
        }
    }
    else if (keyType == QLIB_KID__DEVICE_MASTER)
    {
#ifdef QLIB_KDF_DEV_KEYS
        QLIB_ASSERT_RET(QLIB_UTILS_CMKDF_KeyDerive(devSecret_au8_L,
                                                   (uint8_t*)wid,
                                                   QLIB_UTILS_CMKDF_LABEL_KD,
                                                   0u,
                                                   1u,
                                                   (uint8_t*)deviceKey,
                                                   sizeof(KEY_T)) == 0,
                        QLIB_STATUS__COMMAND_FAIL);
#else
        KEY_T key = QLIB_SAMPLE_KD;
        (void)memcpy(deviceKey, key, sizeof(KEY_T));
#endif // QLIB_KDF_DEV_KEYS
    }
    else if (keyType == QLIB_KID__DEVICE_SECRET)
    {
#ifdef QLIB_KDF_DEV_KEYS
        QLIB_ASSERT_RET(QLIB_UTILS_CMKDF_KeyDerive(devSecret_au8_L,
                                                   (uint8_t*)wid,
                                                   QLIB_UTILS_CMKDF_LABEL_KS,
                                                   0u,
                                                   1u,
                                                   (uint8_t*)deviceKey,
                                                   sizeof(KEY_T)) == 0,
                        QLIB_STATUS__COMMAND_FAIL);
#else
        KEY_T key = QLIB_SAMPLE_KDS;
        (void)memcpy(deviceKey, key, sizeof(KEY_T));
#endif // QLIB_KDF_DEV_KEYS
    }
    else
    {
        return QLIB_STATUS__INVALID_PARAMETER;
    }
    return QLIB_STATUS__OK;
}

QLIB_STATUS_T QLIB_SAMPLE_GetSecureUID(QLIB_CONTEXT_T* ctx, QLIB_WID_T wid, TYPE_128BIT suid)
{
    (void)ctx;
    (void)wid;
    TYPE_128BIT sampleSuid = QLIB_SAMPLE_SUID;

    (void)memcpy(suid, sampleSuid, sizeof(TYPE_128BIT));
    return QLIB_STATUS__OK;
}
#endif // QLIB_ONLY_SECURE_STORAGE
