/************************************************************************************************************
* @internal
* @remark     Winbond Electronics Corporation - Confidential
* @copyright  Copyright (c) 2025 by Winbond Electronics Corporation . All rights reserved
* @endinternal
*
* @file       winbond_key_storage.c
* @brief      This file includes key storage specific implementations
*
* ### project lms
*
************************************************************************************************************/

/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
/*                                                INCLUDES                                                 */
/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
#include "winbond_key_storage.h"

#ifdef USE_RAM_STORAGE
#include "winbond_key_storage_in_ram.h"
#else
#include "winbond_key_storage_in_file.h"
#endif

/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
/*                                           INTERFACE FUNCTIONS                                           */
/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
LMS_ALG_STATUS LMS_StorePublicKey(const IN LMS_KEY_ID_T keyId, const IN void* pubKey, IN uint32_t pubKeySize)
{
#ifdef USE_RAM_STORAGE
    return LMS_StorePublicKeyInRAM(keyId, pubKey, pubKeySize);
#else
    return LMS_StorePublicKeyInFile(keyId, pubKey, pubKeySize);
#endif
}

LMS_ALG_STATUS LMS_FetchPublicKey(const IN LMS_KEY_ID_T keyId,
                                  OUT void*             pubKey,
                                  IN uint32_t           pubKeyBufferSize,
                                  OUT uint32_t*         pubKeyBytesReal)
{
#ifdef USE_RAM_STORAGE
    return LMS_FetchPublicKeyInRAM(keyId, pubKey, pubKeyBufferSize, pubKeyBytesReal);
#else
    return LMS_FetchPublicKeyInFile(keyId, pubKey, pubKeyBufferSize, pubKeyBytesReal);
#endif
}

#ifndef VERIFY_ONLY
LMS_ALG_STATUS LMS_StorePrivateKey(const IN LMS_KEY_ID_T keyId, const IN void* privKey, IN uint32_t privKeySize)
{
#ifdef USE_RAM_STORAGE
    return LMS_StorePrivateKeyInRAM(keyId, privKey, privKeySize);
#else
    return LMS_StorePrivateKeyInFile(keyId, privKey, privKeySize);
#endif
}

LMS_ALG_STATUS LMS_FetchPrivateKey(const IN LMS_KEY_ID_T keyId,
                                   OUT void*             privKey,
                                   IN uint32_t           privKeyBufferSize,
                                   OUT uint32_t*         privKeyBytesReal)
{
#ifdef USE_RAM_STORAGE
    return LMS_FetchPrivateKeyInRAM(keyId, privKey, privKeyBufferSize, privKeyBytesReal);
#else
    return LMS_FetchPrivateKeyInFile(keyId, privKey, privKeyBufferSize, privKeyBytesReal);
#endif
}

#ifndef VERIFY_ONLY
LMS_ALG_STATUS LMS_ReservePrivateKey(const IN LMS_KEY_ID_T keyId,
                                     const IN void*         expectedPrivKey,
                                     const IN void*         reservedPrivKey,
                                     IN uint32_t            privKeySize)
{
#ifdef USE_RAM_STORAGE
    return LMS_ReservePrivateKeyInRAM(keyId, expectedPrivKey, reservedPrivKey, privKeySize);
#else
    return LMS_ReservePrivateKeyInFile(keyId, expectedPrivKey, reservedPrivKey, privKeySize);
#endif
}
#endif // VERIFY_ONLY

LMS_ALG_STATUS LMS_StoreCache(const IN LMS_KEY_ID_T keyId, const IN void* cache, IN uint32_t cacheSize)
{
#ifdef USE_RAM_STORAGE
    return LMS_StoreCacheInRAM(keyId, cache, cacheSize);
#else
    return LMS_StoreCacheInFile(keyId, cache, cacheSize);
#endif
}

LMS_ALG_STATUS LMS_FetchCache(const IN LMS_KEY_ID_T keyId,
                              OUT void*             cache,
                              IN uint32_t           cacheBufferSize,
                              OUT uint32_t*         cacheBytesReal)
{
#ifdef USE_RAM_STORAGE
    return LMS_FetchCacheInRAM(keyId, cache, cacheBufferSize, cacheBytesReal);
#else
    return LMS_FetchCacheInFile(keyId, cache, cacheBufferSize, cacheBytesReal);
#endif
}

#endif // VERIFY_ONLY

LMS_ALG_STATUS LMS_DeleteKeyData(IN const LMS_KEY_ID_T keyId)
{
#ifdef USE_RAM_STORAGE
    return LMS_DeleteKeyDataInRAM(keyId);
#else
    return LMS_DeleteKeyDataInFile(keyId);
#endif
}

LMS_ALG_STATUS LMS_DeleteAllData(void)
{
#ifdef USE_RAM_STORAGE
    return LMS_DeleteAllDataInRAM();
#else
    return ALG_ERROR_NOT_SUPPORTED; // currently not implemented
#endif
}
