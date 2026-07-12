/************************************************************************************************************
* @internal
* @remark     Winbond Electronics Corporation - Confidential
* @copyright  Copyright (c) 2025 by Winbond Electronics Corporation . All rights reserved
* @endinternal
*
* @file       winbond_key_storage_in_ram.c
* @brief      This file includes specific definitions of key storage, in RAM
*
* ### project lms
*
************************************************************************************************************/

/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
/*                                                INCLUDES                                                 */
/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
#include "winbond_key_storage_in_ram.h"
#include "lms.h"
#include "sha256.h"
#include <string.h>

/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
/*                                              DEFINITIONS                                                */
/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
#ifndef MAX_KEYS
#define MAX_KEYS 1u
#endif
#define MAX_CACHE_SIZE 2048u

/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
/*                                                  TYPES                                                  */
/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
typedef struct
{
    LMS_KEY_ID_T keyId;
    uint8_t      pubKey[sizeof(LMS_CHUNK_T) + sizeof(uint32_t)];
    uint32_t     pubKeySize;
} PubKeyStorage;

#ifndef VERIFY_ONLY
typedef struct
{
    LMS_KEY_ID_T keyId;
    uint8_t      privKey[sizeof(LMS_SEED_T) + sizeof(uint32_t) + sizeof(uint32_t)];
    uint32_t     privKeySize;
} PrivKeyStorage;

typedef struct
{
    LMS_KEY_ID_T keyId;
    uint8_t      cache[sizeof(LMS_CHUNK_T) * MAX_CACHE_SIZE];
    uint32_t     cacheSize;
} CacheStorage;
#endif // VERIFY_ONLY

/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
/*                                                GLOBALS                                                  */
/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
static PubKeyStorage pubKeyStorage[MAX_KEYS] = {0};
static uint32_t      pubKeyCount             = 0;
#ifndef VERIFY_ONLY
static PrivKeyStorage privKeyStorage[MAX_KEYS] = {0};
static uint32_t       privKeyCount             = 0;
static CacheStorage   cacheStorage[MAX_KEYS]   = {0};
static uint32_t       cacheCount               = 0;
#endif // VERIFY_ONLY

/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
/*                                        LOCAL FUNCTION PROTOTYPES                                        */
/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
static int findPubKeyIndex_L(const LMS_KEY_ID_T keyId);
static int findPubKeyFreeIndex_L(void);
#ifndef VERIFY_ONLY
static int findPrivKeyIndex_L(const LMS_KEY_ID_T keyId);
static int findCacheIndex_L(const LMS_KEY_ID_T keyId);
static int findPrivKeyFreeIndex_L(void);
static int findCacheFreeIndex_L(void);
#endif // VERIFY_ONLY

/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
/*                                           INTERFACE FUNCTIONS                                           */
/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
LMS_ALG_STATUS LMS_StorePublicKeyInRAM(const IN LMS_KEY_ID_T keyId, const IN void* pubKey, IN uint32_t pubKeySize)
{
    int index = findPubKeyIndex_L(keyId);

    if (index != -1)
    {
        return ALG_ERROR_SYSTEM_IN_INCORRECT_STATE;
    }

    if (pubKeyCount >= MAX_KEYS)
    {
        return ALG_ERROR_MAX_KID_USED;
    }

    index = findPubKeyFreeIndex_L();
    if (index == -1)
    {
        return ALG_ERROR_MAX_KID_USED;
    }

    (void)memcpy((void*)pubKeyStorage[index].keyId, (const void*)keyId, sizeof(LMS_KEY_ID_T));
    (void)memcpy((void*)pubKeyStorage[index].pubKey, (const void*)pubKey, pubKeySize);
    pubKeyStorage[index].pubKeySize = pubKeySize;
    pubKeyCount++;

    return ALG_SUCCESS;
}

LMS_ALG_STATUS LMS_FetchPublicKeyInRAM(const IN LMS_KEY_ID_T keyId,
                                       OUT void*             pubKey,
                                       IN uint32_t           pubKeyBufferSize,
                                       OUT uint32_t*         pubKeyBytesReal)
{
    int index = findPubKeyIndex_L(keyId);

    if (index == -1)
    {
        return ALG_ERROR_KID_NOT_FOUND;
    }

    if (pubKeyBufferSize < pubKeyStorage[index].pubKeySize)
    {
        return ALG_ERROR_INVALID_PARAMETER;
    }

    (void)memcpy((void*)pubKey, (const void*)pubKeyStorage[index].pubKey, pubKeyStorage[index].pubKeySize);
    *pubKeyBytesReal = pubKeyStorage[index].pubKeySize;

    return ALG_SUCCESS;
}

#ifndef VERIFY_ONLY
LMS_ALG_STATUS LMS_StorePrivateKeyInRAM(const IN LMS_KEY_ID_T keyId, const IN void* privKey, IN uint32_t privKeySize)
{
    int  index    = findPrivKeyIndex_L(keyId);
    bool isNewKey = false;

    if (index == -1)
    { // Key was not found, search for free index
        isNewKey = true;

        if (privKeyCount >= MAX_KEYS)
        {
            return ALG_ERROR_MAX_KID_USED;
        }

        index = findPrivKeyFreeIndex_L();
        if (index == -1)
        {
            return ALG_ERROR_MAX_KID_USED;
        }
    }

    if (index < 0 || index >= (int)MAX_KEYS)
    {
        return ALG_ERROR_MAX_KID_USED;
    }

    if (isNewKey == (bool)true)
    {
        privKeyStorage[index].privKeySize = privKeySize;
        privKeyCount++;
    }

    // Update key data according the the index (new or existing)
    (void)memcpy(privKeyStorage[index].keyId, keyId, sizeof(LMS_KEY_ID_T));
    (void)memcpy((void*)privKeyStorage[index].privKey, (const void*)privKey, privKeySize);

    return ALG_SUCCESS;
}

LMS_ALG_STATUS LMS_FetchPrivateKeyInRAM(const IN LMS_KEY_ID_T keyId,
                                        OUT void*             privKey,
                                        IN uint32_t           privKeyBufferSize,
                                        OUT uint32_t*         privKeyBytesReal)
{
    int index = findPrivKeyIndex_L(keyId);

    if (index == -1)
    {
        return ALG_ERROR_KID_NOT_FOUND;
    }

    if (privKeyBufferSize < privKeyStorage[index].privKeySize)
    {
        return ALG_ERROR_INVALID_PARAMETER;
    }

    (void)memcpy((void*)privKey, (const void*)privKeyStorage[index].privKey, privKeyStorage[index].privKeySize);
    *privKeyBytesReal = privKeyStorage[index].privKeySize;

    return ALG_SUCCESS;
}

LMS_ALG_STATUS LMS_StoreCacheInRAM(const IN LMS_KEY_ID_T keyId, const IN void* cache, IN uint32_t cacheSize)
{
    int index = findCacheIndex_L(keyId);

    if (index != -1)
    {
        return ALG_ERROR_SYSTEM_IN_INCORRECT_STATE;
    }

    if (cacheCount >= MAX_KEYS)
    {
        return ALG_ERROR_MAX_KID_USED;
    }

    index = findCacheFreeIndex_L();
    if (index == -1)
    {
        return ALG_ERROR_MAX_KID_USED;
    }
    (void)memcpy(cacheStorage[index].keyId, keyId, sizeof(LMS_KEY_ID_T));
    (void)memcpy((void*)cacheStorage[index].cache, (const void*)cache, cacheSize);
    cacheStorage[index].cacheSize = cacheSize;
    cacheCount++;

    return ALG_SUCCESS;
}

LMS_ALG_STATUS LMS_FetchCacheInRAM(const IN LMS_KEY_ID_T keyId,
                                   OUT void*             cache,
                                   IN uint32_t           cacheBufferSize,
                                   OUT uint32_t*         cacheBytesReal)
{
    int index = findCacheIndex_L(keyId);

    if (index == -1)
    {
        return ALG_ERROR_KID_NOT_FOUND;
    }

    if (cacheBufferSize < cacheStorage[index].cacheSize)
    {
        return ALG_ERROR_INVALID_PARAMETER;
    }

    (void)memcpy((void*)cache, (const void*)cacheStorage[index].cache, cacheStorage[index].cacheSize);
    *cacheBytesReal = cacheStorage[index].cacheSize;

    return ALG_SUCCESS;
}
#endif // VERIFY_ONLY

LMS_ALG_STATUS LMS_DeleteKeyDataInRAM(IN const LMS_KEY_ID_T keyId)
{
    int index;

    index = findPubKeyIndex_L(keyId);
    if (index != -1)
    {
        (void)memset(&pubKeyStorage[index], 0, sizeof(PubKeyStorage));
        pubKeyCount--;
    }
#ifndef VERIFY_ONLY
    index = findPrivKeyIndex_L(keyId);
    if (index != -1)
    {
        (void)memset(&privKeyStorage[index], 0, sizeof(PrivKeyStorage));
        privKeyCount--;
    }
    index = findCacheIndex_L(keyId);
    if (index != -1)
    {
        (void)memset(&cacheStorage[index], 0, sizeof(CacheStorage));
        cacheCount--;
    }
#endif
    return ALG_SUCCESS;
}

LMS_ALG_STATUS LMS_DeleteAllDataInRAM(void)
{
    (void)memset(pubKeyStorage, 0, sizeof(PubKeyStorage) * MAX_KEYS);
    pubKeyCount = 0;
#ifndef VERIFY_ONLY
    (void)memset(privKeyStorage, 0, sizeof(PrivKeyStorage) * MAX_KEYS);
    privKeyCount = 0;
    (void)memset(cacheStorage, 0, sizeof(CacheStorage) * MAX_KEYS);
    cacheCount = 0;
#endif
    return ALG_SUCCESS;
}

/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
/*                                             LOCAL FUNCTIONS                                             */
/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
static int findPubKeyIndex_L(const LMS_KEY_ID_T keyId)
{
    for (uint32_t i = 0; i < MAX_KEYS; ++i)
    {
        if (0 == memcmp(pubKeyStorage[i].keyId, keyId, sizeof(LMS_KEY_ID_T)))
        {
            return (int)i;
        }
    }
    return -1;
}

#ifndef VERIFY_ONLY
static int findPrivKeyIndex_L(const LMS_KEY_ID_T keyId)
{
    for (uint32_t i = 0; i < MAX_KEYS; ++i)
    {
        if (0 == memcmp(privKeyStorage[i].keyId, keyId, sizeof(LMS_KEY_ID_T)))
        {
            return (int)i;
        }
    }
    return -1;
}

static int findCacheIndex_L(const LMS_KEY_ID_T keyId)
{
    for (uint32_t i = 0; i < MAX_KEYS; ++i)
    {
        if (0 == memcmp(cacheStorage[i].keyId, keyId, sizeof(LMS_KEY_ID_T)))
        {
            return (int)i;
        }
    }
    return -1;
}
#endif // VERIFY_ONLY

static int findPubKeyFreeIndex_L(void)
{
    LMS_KEY_ID_T keyIdInvalid = INVALID_KEY_ID;
    for (uint32_t i = 0; i < MAX_KEYS; ++i)
    {
        if (0 == memcmp(pubKeyStorage[i].keyId, keyIdInvalid, sizeof(LMS_KEY_ID_T)))
        {
            return (int)i;
        }
    }
    return -1;
}

#ifndef VERIFY_ONLY
static int findPrivKeyFreeIndex_L(void)
{
    LMS_KEY_ID_T keyIdInvalid = INVALID_KEY_ID;
    for (uint32_t i = 0; i < MAX_KEYS; ++i)
    {
        if (0 == memcmp(privKeyStorage[i].keyId, keyIdInvalid, sizeof(LMS_KEY_ID_T)))
        {
            return (int)i;
        }
    }
    return -1;
}

static int findCacheFreeIndex_L(void)
{
    LMS_KEY_ID_T keyIdInvalid = INVALID_KEY_ID;
    for (uint32_t i = 0; i < MAX_KEYS; ++i)
    {
        if (0 == memcmp(cacheStorage[i].keyId, keyIdInvalid, sizeof(LMS_KEY_ID_T)))
        {
            return (int)i;
        }
    }
    return -1;
}
#endif // VERIFY_ONLY
