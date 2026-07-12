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

static const KEY_ARRAY_T rKeys_L = {QLIB_SAMPLE_RESTRICTED_K_0,
                                    QLIB_SAMPLE_RESTRICTED_K_1,
                                    QLIB_SAMPLE_RESTRICTED_K_2,
                                    QLIB_SAMPLE_RESTRICTED_K_3,
                                    QLIB_SAMPLE_RESTRICTED_K_4,
                                    QLIB_SAMPLE_RESTRICTED_K_5,
                                    QLIB_SAMPLE_RESTRICTED_K_6,
                                    QLIB_SAMPLE_RESTRICTED_K_7,
                                    QLIB_SAMPLE_VAULT_RESTRICTED};
static const KEY_ARRAY_T fKeys_L = {QLIB_SAMPLE_FULL_ACCESS_K_0,
                                    QLIB_SAMPLE_FULL_ACCESS_K_1,
                                    QLIB_SAMPLE_FULL_ACCESS_K_2,
                                    QLIB_SAMPLE_FULL_ACCESS_K_3,
                                    QLIB_SAMPLE_FULL_ACCESS_K_4,
                                    QLIB_SAMPLE_FULL_ACCESS_K_5,
                                    QLIB_SAMPLE_FULL_ACCESS_K_6,
                                    QLIB_SAMPLE_FULL_ACCESS_K_7,
                                    QLIB_SAMPLE_VAULT_FULL_ACCESS};

/*-----------------------------------------------------------------------------------------------------------
                                             INTERFACE FUNCTIONS
-----------------------------------------------------------------------------------------------------------*/

QLIB_STATUS_T QLIB_SAMPLE_GetSectionAccessKey(QLIB_CONTEXT_T* ctx,
                                              UNIQUE_ID_T     uid,
                                              QLIB_KID_TYPE_T keyType,
                                              uint32_t        section_u32,
                                              uint32_t        die_u32,
                                              KEY_T           key)
{
    CONST_KEY_P_T keyPtr;
    QLIB_STATUS_RET_CHECK(QLIB_SAMPLE_GetSectionKeyPtr(ctx, uid, keyType, section_u32, die_u32, &keyPtr));
    (void)memcpy(key, keyPtr, sizeof(KEY_T));
    return QLIB_STATUS__OK;
}

#ifndef QLIB_ONLY_SECURE_STORAGE
QLIB_STATUS_T QLIB_SAMPLE_GetSectionLmsKey(QLIB_CONTEXT_T* ctx,
                                           UNIQUE_ID_T     uid,
                                           uint32_t        section_u32,
                                           uint32_t        die_u32,
                                           LMS_PUB_KEY_T   key)
{
    (void)ctx;
    (void)uid;
    (void)die_u32;
    LMS_PUB_KEY_T keys[QLIB_NUM_OF_SECTIONS] = {QLIB_SAMPLE_LMS_K_0,
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

QLIB_STATUS_T QLIB_SAMPLE_GetSectionKeyPtr(QLIB_CONTEXT_T* ctx,
                                           UNIQUE_ID_T     uid,
                                           QLIB_KID_TYPE_T keyType,
                                           uint32_t        section_u32,
                                           uint32_t        die_u32,
                                           CONST_KEY_P_T*  key)
{
    (void)ctx;
    (void)uid;
    (void)die_u32;

    QLIB_ASSERT_RET((keyType == QLIB_KID__FULL_ACCESS_SECTION) || (keyType == QLIB_KID__RESTRICTED_ACCESS_SECTION),
                    QLIB_STATUS__INVALID_PARAMETER);
    QLIB_ASSERT_RET(section_u32 < QLIB_NUM_OF_SECTIONS, QLIB_STATUS__INVALID_PARAMETER);
    QLIB_ASSERT_RET(key != NULL, QLIB_STATUS__INVALID_PARAMETER);

    if (keyType == QLIB_KID__FULL_ACCESS_SECTION)
    {
        *key = fKeys_L[section_u32];
    }
    else
    {
        *key = rKeys_L[section_u32];
    }

    return QLIB_STATUS__OK;
}

#ifndef QLIB_ONLY_SECURE_STORAGE
QLIB_STATUS_T QLIB_SAMPLE_GetDeviceKey(QLIB_CONTEXT_T* ctx, UNIQUE_ID_T uid, QLIB_KID_TYPE_T keyType, KEY_T deviceKey)
{
    (void)uid;
    if (keyType == QLIB_KID__DEVICE_KEY_PRE_PROVISIONING)
    {
        if (QLIB_FeatureSupported(ctx, W77Q_FEATURE_PRE_PROV_MASTER_KEY) != (bool)false)
        {
            KEY_T key = QLIB_SAMPLE_KD1;
            (void)memcpy(deviceKey, key, sizeof(KEY_T));
        }
        else
        {
            (void)memset(deviceKey, 0u, sizeof(KEY_T));
        }
    }
    else if (keyType == QLIB_KID__DEVICE_MASTER)
    {
        KEY_T key = QLIB_SAMPLE_KD;
        (void)memcpy(deviceKey, key, sizeof(KEY_T));
    }
    else if (keyType == QLIB_KID__DEVICE_SECRET)
    {
        KEY_T key = QLIB_SAMPLE_KDS;
        (void)memcpy(deviceKey, key, sizeof(KEY_T));
    }
    else
    {
        return QLIB_STATUS__INVALID_PARAMETER;
    }
    return QLIB_STATUS__OK;
}

QLIB_STATUS_T QLIB_SAMPLE_GetSecureUID(QLIB_CONTEXT_T* ctx, UNIQUE_ID_T uid, TYPE_128BIT suid)
{
    (void)ctx;
    (void)uid;
    TYPE_128BIT sampleSuid = QLIB_SAMPLE_SUID;

    (void)memcpy(suid, sampleSuid, sizeof(TYPE_128BIT));
    return QLIB_STATUS__OK;
}
#endif // QLIB_ONLY_SECURE_STORAGE
