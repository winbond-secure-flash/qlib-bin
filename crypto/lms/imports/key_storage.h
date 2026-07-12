/************************************************************************************************************
* @internal
* @remark     Winbond Electronics Corporation - Confidential
* @copyright  Copyright (c) 2025 by Winbond Electronics Corporation . All rights reserved
* @endinternal
*
* @file       key_storage.h
* @brief      This file includes key storage specific definitions
*
* ### project lms
*
************************************************************************************************************/
#ifndef __LMS_KEY_STORAGE_H__
#define __LMS_KEY_STORAGE_H__

#ifdef __cplusplus
extern "C" {
#endif

/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
/*                                                INCLUDES                                                 */
/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
#include <stdint.h>
#include <stdbool.h>
#include "lms_common.h"

#define SHA256_DIGEST_LENGTH 32

/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
/*                                       PLATFORM SPECIFIC FUNCTIONS                                       */
/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/

/************************************************************************************************************
 * @brief       This function stores the public key in HSM
 *
 * @param[in]  keyId        Key Id to fetch
 * @param[in]  pubKey       public key buffer. content of the buffer is internal to lms implementation
 * @param[in]  pubKeySize   public key buffer size
 * 
 * @return
 * ALG_SUCCESS                         - no error occurred
 * ALG_ERROR_MAX_KID_USED              - No more KID storage allowed
 * ALG_ERROR_INVALID_PARAMETER         - pubKey is null or pubKeySize is illegal
 * ALG_ERROR_SYSTEM_IN_INCORRECT_STATE - public key for the provided keyId already exist
 * ALG_ERROR_(ERROR)                   - Other error
 ************************************************************************************************************/
LMS_ALG_STATUS LMS_StorePublicKey(const IN LMS_KEY_ID_T keyId, const IN void* pubKey, IN uint32_t pubKeySize);

/************************************************************************************************************
 * @brief       This function fetch the public key from HSM
 *
 * @param[in]  keyId            Key Id to fetch
 * @param[out] pubKey           public key. content of the pubKey buffer is internal to lms implementation
 * @param[in]  pubKeyBufferSize public key buffer size
 * @param[out] pubKeyBytesReal  public key returned size
 * 
 * @return
 * ALG_SUCCESS                 - no error occurred
 * ALG_ERROR_KID_NOT_FOUND     - key is not stored
 * ALG_ERROR_INVALID_PARAMETER - insufficient buffer size or NULL buffer
 * ALG_ERROR_(ERROR)           - Other error
 ************************************************************************************************************/
LMS_ALG_STATUS LMS_FetchPublicKey(const IN LMS_KEY_ID_T keyId,
                                  OUT void*             pubKey,
                                  IN uint32_t           pubKeyBufferSize,
                                  OUT uint32_t*         pubKeyBytesReal);

#ifndef VERIFY_ONLY
/************************************************************************************************************
 * @brief       This function stores the private key in HSM
 *
 * @param[in] keyId        Key Id
 * @param[in] privKey      private key buffer. content of the buffer is internal to lms implementation
 * @param[in] privKeySize  private key buffer size
 * 
 * @return
 * ALG_SUCCESS                        - no error occurred
 * ALG_ERROR_MAX_KID_USED             - No more KID storage allowed
 * ALG_ERROR_INVALID_PARAMETER        - privKey is null or zero privKeySize
* ALG_ERROR_SYSTEM_IN_INCORRECT_STATE - private key for the provided keyId already exist
 * ALG_ERROR_PERMISSION               - User is no privilege to access the private key
 * ALG_ERROR_(ERROR)                  - Other error
 ************************************************************************************************************/
LMS_ALG_STATUS
LMS_StorePrivateKey(IN const LMS_KEY_ID_T keyId, const IN void* privKey, IN uint32_t privKeySize);

/************************************************************************************************************
 * @brief       This function fetch the private key from HSM
 *
 * @param[in]  keyId              Key Id to fetch
 * @param[out] privKey            private key. content of the privKey buffer is internal to lms implementation
 * @param[in]  privKeyBufferSize  private key buffer size
 * @param[out] privKeyBytesReal   private key returned size
 * @return
 * ALG_SUCCESS                 - no error occurred
 * ALG_ERROR_KID_NOT_FOUND     - key is not stored
 * ALG_ERROR_INVALID_PARAMETER - insufficient buffer size or NULL buffer
 * ALG_ERROR_PERMISSION        - user does not have privilege to access the private key
 * ALG_ERROR_(ERROR)           - Other error
 ************************************************************************************************************/
LMS_ALG_STATUS LMS_FetchPrivateKey(const IN LMS_KEY_ID_T keyId,
                                   OUT void*             privKey,
                                   IN uint32_t           privKeyBufferSize,
                                   OUT uint32_t*         privKeyBytesReal);

/************************************************************************************************************
 * @brief       This function stores the tree cache in HSM
 *
 * @param[in] keyId        Key Id of the merkle tree
 * @param[in] cache        cache buffer. content of the buffer is internal to lms implementation
 * @param[in] cacheSize    cache size in bytes
 * 
 * @return
 * ALG_SUCCESS                         - no error occurred
 * ALG_ERROR_MAX_KID_USED              - No more KID storage allowed
 * ALG_ERROR_INVALID_PARAMETER         - cache is NULL or zero cacheSize
 * ALG_ERROR_SYSTEM_IN_INCORRECT_STATE - cache for the provided keyId already exist
 * ALG_ERROR_(ERROR)                   - Other error
 ************************************************************************************************************/
LMS_ALG_STATUS LMS_StoreCache(const IN LMS_KEY_ID_T keyId, const IN void* cache, IN uint32_t cacheSize);

/************************************************************************************************************
 * @brief       This function fetch the cache key from HSM
 *
 * @param[in]  keyId              Key Id to fetch
 * @param[out] cache              cache data buffer. content of the cache buffer is internal to lms implementation
 * @param[in]  cacheBufferSize    cache buffer size
 * @param[out] cacheBytesReal     cache returned size in bytes
 * @return
 * ALG_SUCCESS                 - no error occurred
 * ALG_ERROR_KID_NOT_FOUND     - key is not stored
 * ALG_ERROR_INVALID_PARAMETER - Insufficient buffer size or NULL buffer
 * ALG_ERROR_(ERROR)           - Other error
 ************************************************************************************************************/
LMS_ALG_STATUS LMS_FetchCache(const IN LMS_KEY_ID_T keyId,
                              OUT void*             cache,
                              IN uint32_t           cacheBufferSize,
                              OUT uint32_t*         cacheBytesReal);

#endif // VERIFY_ONLY

#ifdef __cplusplus
}
#endif

#endif // __LMS_KEY_STORAGE_H__
