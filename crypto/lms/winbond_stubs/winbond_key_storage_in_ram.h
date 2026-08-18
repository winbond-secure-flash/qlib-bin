/************************************************************************************************************
* @internal
* @remark     Winbond Electronics Corporation - Confidential
* @copyright  Copyright (c) 2025 by Winbond Electronics Corporation . All rights reserved
* @endinternal
*
* @file       winbond_key_storage_in_ram.h
* @brief      This file includes specific definitions of key storage, in RAM
*
* ### project lms
*
************************************************************************************************************/

#ifndef WINBOND_KEY_STORAGE_IN_RAM_H
#define WINBOND_KEY_STORAGE_IN_RAM_H

/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
/*                                                INCLUDES                                                 */
/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
#include "key_storage.h"

/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
/*                                           INTERFACE FUNCTIONS                                           */
/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
LMS_ALG_STATUS LMS_StorePublicKeyInRAM(const IN LMS_KEY_ID_T keyId, const IN void* pubKey, IN uint32_t pubKeySize);
LMS_ALG_STATUS LMS_FetchPublicKeyInRAM(const IN LMS_KEY_ID_T keyId,
                                       OUT void*             pubKey,
                                       IN uint32_t           pubKeyBufferSize,
                                       OUT uint32_t*         pubKeyBytesReal);

#ifndef VERIFY_ONLY
LMS_ALG_STATUS LMS_StorePrivateKeyInRAM(const IN LMS_KEY_ID_T keyId, const IN void* privKey, IN uint32_t privKeySize);
LMS_ALG_STATUS LMS_FetchPrivateKeyInRAM(const IN LMS_KEY_ID_T keyId,
                                        OUT void*             privKey,
                                        IN uint32_t           privKeyBufferSize,
                                        OUT uint32_t*         privKeyBytesReal);
LMS_ALG_STATUS LMS_ReservePrivateKeyInRAM(const IN LMS_KEY_ID_T keyId,
                                          const IN void*         expectedPrivKey,
                                          const IN void*         reservedPrivKey,
                                          IN uint32_t            privKeySize);
LMS_ALG_STATUS LMS_StoreCacheInRAM(const IN LMS_KEY_ID_T keyId, const IN void* cache, IN uint32_t cacheSize);
LMS_ALG_STATUS LMS_FetchCacheInRAM(const IN LMS_KEY_ID_T keyId,
                                   OUT void*             cache,
                                   IN uint32_t           cacheBufferSize,
                                   OUT uint32_t*         cacheBytesReal);
LMS_ALG_STATUS LMS_DeleteKeyDataInRAM(IN const LMS_KEY_ID_T keyId);
LMS_ALG_STATUS LMS_DeleteAllDataInRAM(void);

#endif // VERIFY_ONLY

#endif // WINBOND_KEY_STORAGE_IN_RAM_H
