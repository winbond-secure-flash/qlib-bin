/************************************************************************************************************
* @internal
* @remark     Winbond Electronics Corporation - Confidential
* @copyright  Copyright (c) 2025 by Winbond Electronics Corporation . All rights reserved
* @endinternal
*
* @file       winbond_key_storage_in_file.h
* @brief      This file includes specific definitions of key storage, in files
*
* ### project lms
*
************************************************************************************************************/

#ifndef WINBOND_KEY_STORAGE_IN_FILE_H
#define WINBOND_KEY_STORAGE_IN_FILE_H

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
LMS_ALG_STATUS LMS_StorePublicKeyInFile(const IN LMS_KEY_ID_T keyId, const IN void* pubKey, IN uint32_t pubKeySize);
LMS_ALG_STATUS LMS_FetchPublicKeyInFile(const IN LMS_KEY_ID_T keyId,
                                        OUT void*             pubKey,
                                        IN uint32_t           pubKeyBufferSize,
                                        OUT uint32_t*         pubKeyBytesReal);

#ifndef VERIFY_ONLY
LMS_ALG_STATUS LMS_StorePrivateKeyInFile(const IN LMS_KEY_ID_T keyId, const IN void* privKey, IN uint32_t privKeySize);
LMS_ALG_STATUS LMS_FetchPrivateKeyInFile(const IN LMS_KEY_ID_T keyId,
                                         OUT void*             privKey,
                                         IN uint32_t           privKeyBufferSize,
                                         OUT uint32_t*         privKeyBytesReal);
LMS_ALG_STATUS LMS_StoreCacheInFile(const IN LMS_KEY_ID_T keyId, const IN void* cache, IN uint32_t cacheSize);
LMS_ALG_STATUS LMS_FetchCacheInFile(const IN LMS_KEY_ID_T keyId,
                                    OUT void*             cache,
                                    IN uint32_t           cacheBufferSize,
                                    OUT uint32_t*         cacheBytesReal);
#endif // VERIFY_ONLY
LMS_ALG_STATUS LMS_DeleteKeyDataInFile(IN const LMS_KEY_ID_T keyId);

#endif // WINBOND_KEY_STORAGE_IN_FILE_H
