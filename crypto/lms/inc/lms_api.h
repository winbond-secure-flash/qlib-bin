/************************************************************************************************************
* @internal
* @remark     Winbond Electronics Corporation - Confidential
* @copyright  Copyright (c) 2024 by Winbond Electronics Corporation . All rights reserved
* @endinternal
*
* @file       lms_api.h
* @brief      This file contains lms definitions
*
* ### project lms
*
************************************************************************************************************/

#ifndef _LMS_API_H_
#define _LMS_API_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>

/*-----------------------------------------------------------------------------------------------------------
-------------------------------------------------------------------------------------------------------------
                                                 DEFINITIONS
-------------------------------------------------------------------------------------------------------------
-----------------------------------------------------------------------------------------------------------*/
#if defined(_MSC_VER) && !defined(__clang__)
#ifdef LMS_DLL_EXPORT
#define WINBOND_LMS_API __declspec(dllexport) _cdecl
#else
#ifdef LMS_DLL_IMPORT
#define WINBOND_LMS_API __declspec(dllimport) _cdecl
#else
#define WINBOND_LMS_API
#endif
#endif
#else
#define WINBOND_LMS_API
#endif //LMS_DLL_EXPORT

#define IN
#define OUT

// parameters sets' ids according to standard
#define LMOTS_SHA256_N24_W4_PARAM_SET_ID 0x0007u
#define LMS_SHA256_M24_H20_PARAM_SET_ID  0x000Du
#define LMS_SHA256_M24_H10_PARAM_SET_ID  0x000Bu

typedef struct VERSION_T
{
    int  major;
    int  minor;
    int  patch;
    int  internal;
    char vendor[32];
} VERSION_T;

typedef enum _alg_errors
{
    /// @brief no error occurred, signature is valid, etc.
    ALG_SUCCESS = 0,
    /// @brief The key pair with the provided ID does not exist
    ALG_ERROR_KID_NOT_FOUND,
    /// @brief All one-time signatures have been used
    ALG_ERROR_ALL_OTS_USED,
    /// @brief signature verification failure due to wrong signature
    ALG_ERROR_WRONG_SIGNATURE,
    // @brief Invalid parameter
    ALG_ERROR_INVALID_PARAMETER,
    // @brief ParameterSetId not supported
    ALG_ERROR_NOT_SUPPORTED,
    // @brief reached maximal number of key pairs
    ALG_ERROR_MAX_KID_USED,
    // @brief random kid was already used (imported LMS_GetRandom did not produce unique kid)
    ALG_ERROR_RANDOM_KID_USED,
    // @brief no permission to access key
    ALG_ERROR_PERMISSION,
    // @brief corrupted data
    ALG_ERROR_CORRUPTED_DATA,
    // @brief incorrect state
    ALG_ERROR_SYSTEM_IN_INCORRECT_STATE,

    // ... more error codes to be added by the implementer
    ALG_ERROR_OTHER,
} LMS_ALG_STATUS;

typedef struct algorithm_lms_settings
{
    /// @brief  for future compatibility
    uint32_t IN lmsApiId;
    /// @brief  Saved user data pointer
    void* userData;

    // ... possible custom values definition here for example cache size, etc.
} ALG_LMS_T;

typedef struct _lms_op_generate_key_pair_params
{
    /// @brief unique ID for for the key-pair identification in the system.
    /// Always 16 byte long.
    uint8_t OUT* keyId;
    /// @brief ID of LMOTS used. Secure Flash uses LMOTS_SHA256_N24_W4 = 0x7
    const uint32_t IN lmotsParameterSetId;
    /// @brief  ID of LMS used. Secure Flash uses LMS_SHA256_M24_H20 = 0xd
    const uint32_t IN lmsParameterSetId;
#ifdef __cplusplus
    _lms_op_generate_key_pair_params(uint8_t* keyId, uint32_t lmotsParameterSetId, uint32_t lmsParameterSetId)
        : keyId(keyId)
        , lmotsParameterSetId(lmotsParameterSetId)
        , lmsParameterSetId(lmsParameterSetId)
    {
    }
#endif
} LMS_GEN_KEY_PAIR_OP_T;

typedef struct _lms_get_public_key_params
{
    const uint8_t IN* keyId;
    uint8_t IN OUT*   publicKey;
    const uint32_t IN publicKeyBufferLen;
    uint32_t OUT      publicKeyBytesReal;
#ifdef __cplusplus
    _lms_get_public_key_params(const uint8_t* keyId, uint8_t* publicKey, uint32_t publicKeyBufferLen, uint32_t publicKeyBytesReal)
        : keyId(keyId)
        , publicKey(publicKey)
        , publicKeyBufferLen(publicKeyBufferLen)
        , publicKeyBytesReal(publicKeyBytesReal)
    {
    }
#endif
} LMS_GET_PUB_KEY_OP_T;

typedef struct _lms_sign_message_params
{
    const uint8_t IN* keyId;              // 16-byte long string for the key-pair
    const uint8_t IN* message;            // message to be signed
    const uint32_t IN messageLen;         // length of the message in bytes
    uint8_t IN OUT*   signature;          // pointer to the signature buffer
    const uint32_t IN signatureBufferLen; // size of the signature buffer
    uint32_t OUT      signatureBytesReal; // signature actual size in bytes

#ifdef __cplusplus
    // Default constructor
    _lms_sign_message_params(uint8_t* keyId,
                             uint8_t* message,
                             uint32_t messageLen,
                             uint8_t* signature,
                             uint32_t signatureBufferLen,
                             uint32_t signatureBytesReal)
        : keyId(keyId)
        , message(message)
        , messageLen(messageLen)
        , signature(signature)
        , signatureBufferLen(signatureBufferLen)
        , signatureBytesReal(signatureBytesReal)
    {
    }
#endif
} LMS_SIGN_MESSAGE_OP_T;

typedef struct _lms_verify_signature_params
{
    const uint8_t IN* publicKey;    //LMS public key
    uint32_t IN       publicKeyLen; // length of the LMS public key in bytes
    const uint8_t IN* message;      // message signed
    uint32_t IN       messageLen;   // length of the message in bytes
    const uint8_t IN* signature;    // pointer to the signature buffer
    uint32_t IN       signatureLen; // size of the signature in bytes
// Default constructor
#ifdef __cplusplus
    _lms_verify_signature_params(uint8_t* publicKey,
                                 uint32_t publicKeyLen,
                                 uint8_t* message,
                                 uint32_t messageLen,
                                 uint8_t* signature,
                                 uint32_t signatureLen)
        : publicKey(publicKey)
        , publicKeyLen(publicKeyLen)
        , message(message)
        , messageLen(messageLen)
        , signature(signature)
        , signatureLen(signatureLen)
    {
    }

#endif
} LMS_VERIFY_SIGNATURE_OP_T;

typedef struct _lms_key_id_from_public_key_params
{
    const uint8_t IN* publicKey;
    uint32_t IN       publicKeyLen;
    uint8_t IN OUT*   keyId; // 16-byte long buffer for the key-id
#ifdef __cplusplus
    _lms_key_id_from_public_key_params(uint8_t* publicKey, uint32_t publicKeyLen, uint8_t* keyId)
        : publicKey(publicKey)
        , publicKeyLen(publicKeyLen)
        , keyId(keyId)
    {
    }
#endif
} LMS_KEY_ID_FROM_PUBLIC_KEY_OP_T;

/************************************************************************************************************
 * LMS macros according to RFC 8554
 * m - Number of bytes associated with each node. defined by lmsParameterSetId in LMS_GEN_KEY_PAIR_OP_T
 * n - The number of bytes of the output of the hash function. defined by lmotsParameterSetId in LMS_GEN_KEY_PAIR_OP_T
 * p - The number of n-byte string elements that make up the LM-OTS signature, determined by n and w of lmotsParameterSetId in LMS_GEN_KEY_PAIR_OP_T
 * h - The height of the tree, determined by lmsParameterSetId in LMS_GEN_KEY_PAIR_OP_T
************************************************************************************************************/
#define LMS_SIGNATURE_SIZE(m, n, p, h)            \
    (sizeof(uint32_t) + /* otsIdxQ */             \
     sizeof(uint32_t) + /* lmotsParameterSetId */ \
     (n) +              /* randomString */        \
     ((p) * (n)) +      /* otsSignature */        \
     sizeof(uint32_t) + /* lmsParameterSetId */   \
     ((h) * (m)))       /* path */

#define LMS_KEY_ID_SIZE (16u)
#define LMS_PUBLIC_KEY_SIZE(m)                           \
    (sizeof(uint32_t) +        /* lmsParameterSetId */   \
     sizeof(uint32_t) +        /* lmotsParameterSetId */ \
     (size_t)LMS_KEY_ID_SIZE + /* key Id */              \
     (size_t)(m))              /* tree root */

/************************************************************************************************************
 * @brief       Returns the lms and lms api versions
 *
 * @param[out]   pLmsVersion      Pointer to lms version to return.
 *
 * @return      void
************************************************************************************************************/
void WINBOND_LMS_API lmsGetVersion(OUT VERSION_T* pLmsVersion);

/************************************************************************************************************
 * @brief       Initializes the lmsAlg structure with its parameters
 *
 * @param[in]   lmsAlg      Pointer to data associated with this LMS instance.
 *
 * @return      ALG_SUCCESS if no error occurred, ALG_ERROR_(ERROR) otherwise
************************************************************************************************************/
LMS_ALG_STATUS WINBOND_LMS_API lmsInit(IN OUT ALG_LMS_T* lmsAlg);

/************************************************************************************************************
 * @brief       Finalizes LMS
 *
 * @param[in]   lmsAlg      Pointer to data associated with this LMS instance.
 *
 * @return      ALG_SUCCESS if no error occurred, ALG_ERROR_(ERROR) otherwise
************************************************************************************************************/
LMS_ALG_STATUS WINBOND_LMS_API lmsFinit(IN OUT ALG_LMS_T* lmsAlg);

/************************************************************************************************************
 * @brief   Generates random key ID and generate LMS key pair for this key ID.
 *          The HSM stores and manages the key-pair internally and shall not export of private keying material.
 *
 * @param[in]   lmsAlg    pointer to a previously populated LMS alg structure.
 * @param[in]   params    pointer to key pair parameters
 *
 * @return
 * ALG_SUCCESS                  - no error occurred\n
 * ALG_ERROR_RNG_FAILURE        - Random Number Generation error\n
 * ALG_ERROR_NOT_SUPPORTED      - parameters sets' ids not supported
 * ALG_ERROR_OUT_OF_MEMORY      - reached max supported keys
 * ALG_ERROR_INVALID_PARAMETER  - one of the parameters is NULL
 * ALG_ERROR_(ERROR)            - Other error
************************************************************************************************************/
LMS_ALG_STATUS WINBOND_LMS_API lmsGenerateKeyPair(IN ALG_LMS_T* lmsAlg, IN LMS_GEN_KEY_PAIR_OP_T* params);

/************************************************************************************************************
 * @brief   This function gets LMS public key for specific kid.
 *
 * @param[in]   lmsAlg    pointer to a previously populated LMS alg structure.
 * @param[in]   params    pointer to parameters
 *
 * @return
 * ALG_SUCCESS                  - no error occurred\n
 * ALG_ERROR_KID_NOT_FOUND      - did not generate key pair for this key Id\n
 * ALG_ERROR_INVALID_PARAMETER  - one of the parameters is NULL or not enough public key buffer size
 * ALG_ERROR_(ERROR)            - Other error
************************************************************************************************************/
LMS_ALG_STATUS WINBOND_LMS_API lmsGetPublicKey(IN ALG_LMS_T* lmsAlg, IN OUT LMS_GET_PUB_KEY_OP_T* params);

/************************************************************************************************************
 * @brief   This function calculates LMS signature
 *
 * @param[in]   lmsAlg    pointer to a previously populated LMS alg structure.
 * @param[in]   params    pointer to parameters
 *
 * @return
 * ALG_SUCCESS                  - no error occurred\n
 * ALG_ERROR_KID_NOT_FOUND      - did not generate key pair for this key Id\n
 * ALG_ERROR_RNG_FAILURE        - 
 * ALG_ERROR_INVALID_PARAMETER  - one of the parameters is NULL or not enough public key buffer size
 * ALG_ERROR_ALL_OTS_USED       - all tree leaves were used in previous sign operations 
 * ALG_ERROR_(ERROR)            - Other error
************************************************************************************************************/
LMS_ALG_STATUS WINBOND_LMS_API lmsSignMessage(IN ALG_LMS_T* lmsAlg, IN OUT LMS_SIGN_MESSAGE_OP_T* params);

/************************************************************************************************************
 * @brief   This function verifies the LMS signature.
 *
 * @param[in]   lmsAlg    pointer to a previously populated LMS alg structure.
 * @param[in]   params    pointer to parameters
 *
 * @return
 * ALG_SUCCESS                  - no error occurred\n
 * ALG_ERROR_KID_NOT_FOUND      - did not find key pair for this key Id\n 
 * ALG_ERROR_INVALID_PARAMETER  - one of the parameters is NULL or not enough buffer size
 * ALG_ERROR_WRONG_SIGN         - signature verification failed
 * ALG_ERROR_(ERROR)            - Other error
************************************************************************************************************/
LMS_ALG_STATUS WINBOND_LMS_API lmsVerifySignature(IN ALG_LMS_T* lmsAlg, IN LMS_VERIFY_SIGNATURE_OP_T* params);

/************************************************************************************************************
 * @brief   This function retrieves the key ID (I) from a given LMS public key.
 *          public key is of <lmsParameterSetId || lmotsParameterSetId || keyId(I) || treeRoot> format as defined in rfc8554
 *
 * @param[in]   lmsAlg    pointer to a previously populated LMS alg structure.
 * @param[in]   params    pointer to parameters
 *
 * @return
 * ALG_SUCCESS                  - no error occurred\n
 * ALG_ERROR_INVALID_PARAMETER  - one of the parameters is NULL or public key buffer size is bad
 * ALG_ERROR_(ERROR)            - Other error
************************************************************************************************************/
LMS_ALG_STATUS WINBOND_LMS_API lmsGetKeyIdFromPublicKey(IN ALG_LMS_T* lmsAlg, IN OUT LMS_KEY_ID_FROM_PUBLIC_KEY_OP_T* params);

#ifdef __cplusplus
}
#endif

#endif // _LMS_API_H_
