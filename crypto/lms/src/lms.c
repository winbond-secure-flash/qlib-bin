/************************************************************************************************************
* @internal
* @remark     Winbond Electronics Corporation - Confidential
* @copyright  Copyright (c) 2024 by Winbond Electronics Corporation . All rights reserved
* @endinternal
*
* @file       lms.c
* @brief      This file contains SW LMS implementation
************************************************************************************************************/

/*-----------------------------------------------------------------------------------------------------------
-------------------------------------------------------------------------------------------------------------
                                                  INCLUDES
-------------------------------------------------------------------------------------------------------------
-----------------------------------------------------------------------------------------------------------*/
#include "lms_api.h"
#include "lms.h"
#include "lms_version.h"
#include "nonce.h"
#include "sha256.h"
#include "random.h"
#include "key_storage.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#ifdef _WIN32
#include <io.h>
#elif defined(__linux__)
#include <unistd.h>
#endif

/*-----------------------------------------------------------------------------------------------------------
-------------------------------------------------------------------------------------------------------------
                                                 DEFINITIONS
-------------------------------------------------------------------------------------------------------------
-----------------------------------------------------------------------------------------------------------*/

#ifndef LMS_MAX_NUM_KEYS
#define LMS_MAX_NUM_KEYS 1u // Maximum Key IDs supported
#endif
#define LMS_MAX_TREE_HEIGHT 20u
#define LMS_MAX_CACHE_SIZE  2048u

/*---------------------------------------------------------------------------------------------------------*/
/* convert from an unsigned 32-bit number and a four-byte string in network order (BE)                     */
/*---------------------------------------------------------------------------------------------------------*/
#define LMS_SET_U32_IN_BUF_BE(val, buf)               \
    {                                                 \
        (buf)[0] = (uint8_t)(((val) >> 24u) & 0xFFu); \
        (buf)[1] = (uint8_t)(((val) >> 16u) & 0xFFu); \
        (buf)[2] = (uint8_t)(((val) >> 8u) & 0xFFu);  \
        (buf)[3] = (uint8_t)((val)&0xFFu);            \
    }
#define LMS_GET_U32_FROM_BUF_BE(val, buf)                                                                             \
    {                                                                                                                 \
        (val) = (((uint32_t)(((const uint8_t*)(buf))[0])) << 24) | (((uint32_t)(((const uint8_t*)(buf))[1])) << 16) | \
                (((uint32_t)(((const uint8_t*)(buf))[2])) << 8) | ((uint32_t)(((const uint8_t*)(buf))[3]));           \
    }

#define LMS_NUM_OF_CHAINS 48u // Number of chains (only for input)
#define LMS_PARAM_W       4u  // Input size in bits
#define LMS_MAX_DIGIT     ((1U << LMS_PARAM_W) - 1u)

#define LMS_D_LEAF 0x8282u // LMS hashing constant for LMS leaf
#define LMS_D_PBLC 0x8080u // LMS hashing constant for OTS public key
#define LMS_D_MESG 0x8181u // LMS hashing constant for OTS message
#define LMS_D_INTR 0x8383u // LMS hashing constant for LMS node

// macro to extract digit i from input message
#define LMS_COEF(input, i) \
    (LMS_MAX_DIGIT & (((input)[((i)*LMS_PARAM_W) / 8u]) >> (8u - (LMS_PARAM_W * ((i) % (8u / LMS_PARAM_W)) + LMS_PARAM_W))))

#define LMS_LEAF_NODES_START_INDEX(treeHeight) (((uint32_t)0x1) << (treeHeight))
#define LMS_NUM_OF_LEAVES(treeHeight)          LMS_LEAF_NODES_START_INDEX((treeHeight))
#define LMS_ALL_NODES(treeHeight)              (((uint32_t)0x1) << ((treeHeight) + 1u))

#define LMS_FIRST_CACHED_INDEX(treeHeight)            \
    (((LMS_G.cacheSize) >= LMS_ALL_NODES(treeHeight)) \
         ? (1u)                                       \
         : (((LMS_G.cacheSize) > 0u) ? (LMS_G.cacheSize) : LMS_ALL_NODES(treeHeight)))

#define LMS_NODE_IS_EMPTY(node) \
    ((node)[0] == 0u && (node)[1] == 0u && (node)[2] == 0u && (node)[3] == 0u && (node)[4] == 0u && (node)[5] == 0u)

#define LMS_NODE_IN_CACHE(treeHeight, pubCache, nodeIndex)                       \
    (((nodeIndex) >= LMS_FIRST_CACHED_INDEX((treeHeight))) &&                    \
     ((nodeIndex) < (LMS_FIRST_CACHED_INDEX((treeHeight)) + LMS_G.cacheSize)) && \
     (!LMS_NODE_IS_EMPTY((pubCache)[(nodeIndex)-LMS_FIRST_CACHED_INDEX((treeHeight))])))

#define LMS_SHA256(output, data, dataSize)              \
    {                                                   \
        void* ctx;                                      \
        (void)(LMS_SHA256_Init(&ctx));                  \
        (void)(LMS_SHA256_Update(ctx, data, dataSize)); \
        (void)(LMS_SHA256_Finish(ctx, output));         \
    }

typedef uint32_t SHA256_OUTPUT_T[8];
typedef uint32_t LMS_CACHE_NODE_T[sizeof(LMS_CHUNK_T) / sizeof(uint32_t)];

typedef struct
{
    ALG_LMS_T lmsAlg;
#ifndef VERIFY_ONLY
    uint32_t cacheSize;
#endif // VERIFY_ONLY
} LMS_DATA_T;

typedef struct
{
    uint32_t    nodeIndex;
    LMS_CHUNK_T hash;
} LMS_HASH_NODE_T;

typedef struct
{
    LMS_SEED_T seed;
    uint32_t   otsIdx;
    uint32_t   treeHeight;
} LMS_KEY_PRIV_T;

typedef struct
{
    LMS_CACHE_NODE_T pubRoot;
    uint32_t         treeHeight;
} LMS_KEY_PUB_T;

/*-----------------------------------------------------------------------------------------------------------
-------------------------------------------------------------------------------------------------------------
                                                 GLOBALS
-------------------------------------------------------------------------------------------------------------
-----------------------------------------------------------------------------------------------------------*/
static LMS_DATA_T LMS_G;

/*-----------------------------------------------------------------------------------------------------------
-------------------------------------------------------------------------------------------------------------
                                          FUNCTION DECLARATION
-------------------------------------------------------------------------------------------------------------
-----------------------------------------------------------------------------------------------------------*/
#ifndef VERIFY_ONLY
LMS_ALG_STATUS lmsGetRoot_L(const LMS_KEY_ID_T    keyId,
                            uint32_t              rootIndex,
                            const LMS_KEY_PRIV_T* privKey,
                            LMS_CACHE_NODE_T*     cache,
                            uint8_t*              root,
                            bool                  cacheRO);
static void    lmsGetLeaf_L(const LMS_KEY_ID_T keyId, const LMS_KEY_PRIV_T* privKey, uint32_t leafIndex, LMS_CHUNK_T leafHash);
static void    lmsGetLeafPublicKey_L(const LMS_KEY_ID_T    keyId,
                                     const LMS_KEY_PRIV_T* privKey,
                                     uint32_t              leafNum,
                                     LMS_OTS_CHUNK_T       otsPubKey);
static void    lmsOtsGetPriv_L(const uint8_t* seed, uint32_t q, uint16_t chainIndex, LMS_OTS_CHUNK_T priv);

static void lmsSign_L(const LMS_KEY_ID_T       keyId,
                      const IN LMS_KEY_PRIV_T* privKey,
                      const LMS_OTS_CHUNK_T    msgHash,
                      LMS_OTS_SIG_T            otsSig);
#endif // VERIFY_ONLY
static void LmsOtsGetPublicCandidate_L(const LMS_KEY_ID_T    keyId,
                                       uint32_t              leafNum,
                                       const LMS_OTS_SIG_T   otsSig,
                                       const LMS_OTS_CHUNK_T msgHash,
                                       LMS_OTS_CHUNK_T       pubCandidate);
static void lmsOtsCalcLeafHash_L(IN const LMS_KEY_ID_T    keyId,
                                 IN uint32_t              nodeIndex,
                                 IN const LMS_OTS_CHUNK_T leafPubKey,
                                 OUT LMS_CHUNK_T          rootCandidate);

static void lmsCalcMsgHash(IN const LMS_KEY_ID_T    keyId,
                           IN uint32_t              otsIdxQ,
                           IN const LMS_MSG_NONCE_T nonce,
                           IN const uint8_t*        msg,
                           IN uint32_t              msgSize,
                           OUT LMS_OTS_CHUNK_T      msgHash);
static void lmsHashChain_L(const LMS_OTS_CHUNK_T input,
                           const LMS_KEY_ID_T    keyId,
                           uint32_t              q,
                           uint16_t              chainIndex,
                           uint8_t               iterStart,
                           uint8_t               iter,
                           LMS_OTS_CHUNK_T       output);

static void lmsOtsCalcNodePub_L(const LMS_KEY_ID_T keyId,
                                uint32_t           q,
                                const LMS_CHUNK_T  left,
                                const LMS_CHUNK_T  right,
                                OUT LMS_CHUNK_T    pub);
#ifndef VERIFY_ONLY
static LMS_ALG_STATUS lmsGenerateKeyPair_L(IN ALG_LMS_T* lmsAlg, IN LMS_GEN_KEY_PAIR_OP_T* params, const uint8_t* seed);
static void           lmsOtsSignDigit_L(const LMS_KEY_ID_T       keyId,
                                        const IN LMS_KEY_PRIV_T* privKey,
                                        IN uint8_t               msgDigit,
                                        IN uint16_t              chainIndex,
                                        IN OUT uint32_t*         checksum,
                                        OUT LMS_OTS_CHUNK_T      digitSig);
static LMS_ALG_STATUS lmsGenerateUniqueKeyId_L(uint8_t* keyId);

static LMS_ALG_STATUS lmsOtsPubChain(const LMS_KEY_ID_T keyId,
                                     const LMS_SEED_T   seed,
                                     uint32_t           otsIndex,
                                     uint16_t           chainIndex,
                                     LMS_OTS_CHUNK_T    pubChunk);
#endif // VERIFY_ONLY

static LMS_ALG_STATUS lmsPubKeySerialize_L(OUT uint8_t*          pubKey,
                                           IN uint32_t           pubKeyBufSize,
                                           IN uint32_t           lmsParameterSetId,
                                           IN uint32_t           lmotsParameterSetId,
                                           IN const LMS_KEY_ID_T keyId,
                                           IN const LMS_CHUNK_T  treeRoot);
static LMS_ALG_STATUS lmsPubKeyDeserialize_L(IN const uint8_t* pubKey,
                                             IN uint32_t       pubKeyBufSize,
                                             OUT uint32_t*     lmsParameterSetId,
                                             OUT uint32_t*     lmotsParameterSetId,
                                             OUT LMS_KEY_ID_T  keyId,
                                             OUT LMS_CHUNK_T   treeRoot);
#ifndef VERIFY_ONLY
static LMS_ALG_STATUS lmsSignatureSerialize_L(OUT uint8_t*             sigBuf,
                                              IN uint32_t              sigBufSize,
                                              IN uint32_t              otsIdxQ,
                                              IN uint32_t              lmotsParameterSetId,
                                              IN const LMS_MSG_NONCE_T randomString,
                                              IN const LMS_OTS_SIG_T   otsSignature,
                                              IN uint32_t              lmsParameterSetId,
                                              IN const LMS_CHUNK_T*    path,
                                              IN uint32_t              numPathNodes);
#endif // VERIFY_ONLY
static LMS_ALG_STATUS lmsSignatureDeserialize_L(IN const uint8_t*   sigBuf,
                                                IN uint32_t         sigBufSize,
                                                OUT uint32_t*       otsIdxQ,
                                                OUT uint32_t*       lmotsParameterSetId,
                                                OUT LMS_MSG_NONCE_T randomString,
                                                OUT LMS_OTS_SIG_T   otsSignature,
                                                OUT uint32_t*       lmsParameterSetId,
                                                OUT LMS_CHUNK_T*    path,
                                                IN uint32_t         numPathNodes);
/*-----------------------------------------------------------------------------------------------------------
-------------------------------------------------------------------------------------------------------------
                                             INTERFACE FUNCTIONS
-------------------------------------------------------------------------------------------------------------
-----------------------------------------------------------------------------------------------------------*/

void lmsGetVersion(VERSION_T* pLmsVersion)
{
    if (NULL != pLmsVersion)
    {
        pLmsVersion->major    = LMS_VERSION_MAJOR;
        pLmsVersion->minor    = LMS_VERSION_MINOR;
        pLmsVersion->patch    = LMS_VERSION_PATCH;
        pLmsVersion->internal = LMS_VERSION_INTERNAL;
#ifdef _WIN32
        (void)strncpy_s(pLmsVersion->vendor, sizeof(pLmsVersion->vendor), "Winbond", strlen("Winbond") + 1u);
#else  // _WIN32
        (void)strncpy(pLmsVersion->vendor, "Winbond", strlen("Winbond") + 1u);
#endif // _WIN32
    }
}

LMS_ALG_STATUS lmsInit(IN OUT ALG_LMS_T* lmsAlg)
{
    lmsAlg->lmsApiId = LMS_API_VERSION;
    (void)lmsAlg->userData;
    (void)memset(&LMS_G, 0, sizeof(LMS_DATA_T));
    (void)memcpy(&LMS_G.lmsAlg, lmsAlg, sizeof(ALG_LMS_T));
#ifndef VERIFY_ONLY
    LMS_G.cacheSize = LMS_MAX_CACHE_SIZE;
#endif // VERIFY_ONLY
    return ALG_SUCCESS;
}

LMS_ALG_STATUS lmsFinit(IN OUT ALG_LMS_T* lmsAlg)
{
    if (lmsAlg == NULL)
    {
        return ALG_ERROR_INVALID_PARAMETER;
    }
    (void)memset(&LMS_G, 0, sizeof(LMS_DATA_T));
    return ALG_SUCCESS;
}

#ifndef VERIFY_ONLY
LMS_ALG_STATUS lmsGenerateKeyPair(IN ALG_LMS_T* lmsAlg, IN LMS_GEN_KEY_PAIR_OP_T* params)
{
    LMS_ALG_STATUS ret;

    if (params == NULL || lmsAlg == NULL || params->keyId == NULL)
    {
        return ALG_ERROR_INVALID_PARAMETER;
    }
    // Generate random key-id
    if ((ret = lmsGenerateUniqueKeyId_L(params->keyId)) != ALG_SUCCESS)
    {
        return ret;
    }
    return lmsGenerateKeyPair_L(lmsAlg, params, NULL);
}
#endif

LMS_ALG_STATUS lmsGetPublicKey(IN ALG_LMS_T* lmsAlg, IN OUT LMS_GET_PUB_KEY_OP_T* params)
{
    LMS_ALG_STATUS ret;
    LMS_KEY_PUB_T  pub;
    uint32_t       size;

    if (lmsAlg == NULL || params == NULL || params->keyId == NULL || params->publicKey == NULL ||
        params->publicKeyBufferLen < LMS_PUBLIC_KEY_LENGTH)
    {
        return ALG_ERROR_INVALID_PARAMETER;
    }
    ret = LMS_FetchPublicKey(params->keyId, (void*)(&pub), (uint32_t)(sizeof(LMS_KEY_PUB_T)), &size);
    if (ret != ALG_SUCCESS)
    {
        return ret;
    }
    if (size != sizeof(LMS_KEY_PUB_T))
    {
        return ALG_ERROR_CORRUPTED_DATA;
    }
    if ((pub.treeHeight != 10u) && (pub.treeHeight != 20u))
    {
        return ALG_ERROR_CORRUPTED_DATA;
    }
    ret = lmsPubKeySerialize_L(params->publicKey,
                               params->publicKeyBufferLen,
                               (pub.treeHeight == 10u ? LMS_SHA256_M24_H10_PARAM_SET_ID : LMS_SHA256_M24_H20_PARAM_SET_ID),
                               LMOTS_SHA256_N24_W4_PARAM_SET_ID,
                               params->keyId,
                               (uint8_t*)pub.pubRoot);

    if (ret != ALG_SUCCESS)
    {
        return ret;
    }

    params->publicKeyBytesReal = LMS_PUBLIC_KEY_LENGTH;

    return ALG_SUCCESS;
}

#ifndef VERIFY_ONLY
LMS_ALG_STATUS lmsSignMessage(IN ALG_LMS_T* lmsAlg, IN OUT LMS_SIGN_MESSAGE_OP_T* params)
{
    LMS_OTS_CHUNK_T  msgHash;
    uint32_t         i, r;
    uint32_t         index;
    LMS_ALG_STATUS   ret;
    LMS_MSG_NONCE_T  nonce;
    LMS_OTS_SIG_T    sig;
    uint32_t         lmsParameterSetId;
    LMS_CHUNK_T      path[LMS_MAX_TREE_HEIGHT];
    LMS_KEY_PRIV_T   priv = {0u};
    uint32_t         size = 0u;
    LMS_CACHE_NODE_T cache[LMS_MAX_CACHE_SIZE];
    uint32_t         cacheSize = 0;

    if (lmsAlg == NULL || params == NULL || params->keyId == NULL || params->message == NULL || params->messageLen == 0u)
    {
        return ALG_ERROR_INVALID_PARAMETER;
    }

    if ((ret = LMS_FetchPrivateKey(params->keyId, &priv, (uint32_t)(sizeof(LMS_KEY_PRIV_T)), &size)) != ALG_SUCCESS)
    {
        return ret;
    }
    if (size != sizeof(LMS_KEY_PRIV_T))
    {
        return ALG_ERROR_CORRUPTED_DATA;
    }
    if ((priv.treeHeight != 10u) && (priv.treeHeight != 20u))
    {
        return ALG_ERROR_CORRUPTED_DATA;
    }
    if (params->signatureBufferLen < LMS_SIGNATURE_LENGTH(priv.treeHeight))
    {
        return ALG_ERROR_INVALID_PARAMETER;
    }
    lmsParameterSetId = (priv.treeHeight == 10u ? LMS_SHA256_M24_H10_PARAM_SET_ID : LMS_SHA256_M24_H20_PARAM_SET_ID);

    if ((LMS_FetchCache(params->keyId, (void*)cache, (LMS_G.cacheSize * (uint32_t)(sizeof(LMS_CACHE_NODE_T))), &cacheSize) !=
         ALG_SUCCESS) ||
        (cacheSize != LMS_G.cacheSize * (uint32_t)(sizeof(LMS_CACHE_NODE_T))))
    {
        (void)memset(cache, 0, sizeof(LMS_CACHE_NODE_T) * LMS_MAX_CACHE_SIZE);
    }

    // increment leaf number
    priv.otsIdx++;

    if (priv.otsIdx >= LMS_NUM_OF_LEAVES(priv.treeHeight))
    {
        return ALG_ERROR_ALL_OTS_USED;
    }

    //save the private key after otsIdx increment
    if ((ret = LMS_StorePrivateKey(params->keyId, (const void*)(&priv), (uint32_t)(sizeof(LMS_KEY_PRIV_T)))) != ALG_SUCCESS)
    {
        return ret;
    }

    // generate nonce
    for (i = 0; i < sizeof(LMS_MSG_NONCE_T); i++)
    {
        nonce[i] = (uint8_t)LMS_GetNonce();
    }

    // Hash the msg
    lmsCalcMsgHash(params->keyId, priv.otsIdx, nonce, params->message, params->messageLen, msgHash);
    lmsSign_L(params->keyId, &priv, msgHash, sig);

    // Calculate path
    r = LMS_LEAF_NODES_START_INDEX(priv.treeHeight) + priv.otsIdx;
    for (i = 0; i < priv.treeHeight; ++i)
    {
        index = (r / ((uint32_t)1 << i)) ^ (uint32_t)1;
        if ((ret = lmsGetRoot_L(params->keyId, index, &priv, cache, path[i], false)) != ALG_SUCCESS)
        {
            return ret;
        }
    }

    // Set signature
    ret = lmsSignatureSerialize_L(params->signature,
                                  params->signatureBufferLen,
                                  priv.otsIdx,
                                  LMOTS_SHA256_N24_W4_PARAM_SET_ID,
                                  (const uint8_t*)nonce,
                                  (const LMS_CHUNK_T*)sig,
                                  lmsParameterSetId,
                                  (const LMS_CHUNK_T*)path,
                                  priv.treeHeight);

    if (ret != ALG_SUCCESS)
    {
        return ret;
    }

    params->signatureBytesReal = LMS_SIGNATURE_LENGTH(priv.treeHeight);
    return ALG_SUCCESS;
}
#endif // VERIFY_ONLY

LMS_ALG_STATUS lmsVerifySignature(IN ALG_LMS_T* lmsAlg, IN LMS_VERIFY_SIGNATURE_OP_T* params)
{
    uint32_t        i;
    uint32_t        nodeIndex;
    LMS_OTS_CHUNK_T msgHash;
    LMS_OTS_CHUNK_T leafOtsPub;
    LMS_CHUNK_T     rootCandidate;
    uint32_t        leafIndex;
    uint32_t        treeHeight;
    uint32_t        lmsParameterSetId;
    uint32_t        lmotsParameterSetId;
    LMS_MSG_NONCE_T randomString;
    LMS_OTS_SIG_T   otsSignature;
    LMS_CHUNK_T     path[LMS_MAX_TREE_HEIGHT];
    LMS_ALG_STATUS  ret;
    LMS_KEY_ID_T    keyId;
    LMS_CHUNK_T     rootPubKey;

    if (params == NULL)
    {
        return ALG_ERROR_INVALID_PARAMETER;
    }

    ret = lmsSignatureDeserialize_L(params->signature,
                                    params->signatureLen,
                                    &leafIndex,
                                    &lmotsParameterSetId,
                                    randomString,
                                    otsSignature,
                                    &lmsParameterSetId,
                                    path,
                                    LMS_MAX_TREE_HEIGHT);

    if (ret != ALG_SUCCESS)
    {
        return ret;
    }

    if (lmsParameterSetId == LMS_SHA256_M24_H20_PARAM_SET_ID)
    {
        treeHeight = 20;
    }
    else if (lmsParameterSetId == LMS_SHA256_M24_H10_PARAM_SET_ID)
    {
        treeHeight = 10;
    }
    else
    {
        return ALG_ERROR_NOT_SUPPORTED;
    }
    if (lmsAlg == NULL || params->message == NULL || params->messageLen == 0u ||
        lmotsParameterSetId != LMOTS_SHA256_N24_W4_PARAM_SET_ID || params->publicKey == NULL ||
        params->publicKeyLen < LMS_PUBLIC_KEY_LENGTH)
    {
        return ALG_ERROR_INVALID_PARAMETER;
    }

    ret = lmsPubKeyDeserialize_L(params->publicKey, params->publicKeyLen, NULL, NULL, keyId, rootPubKey);
    if (ret != ALG_SUCCESS)
    {
        return ret;
    }
    // Calculate MSG hash
    lmsCalcMsgHash(keyId, leafIndex, randomString, params->message, params->messageLen, msgHash);

    // Calculate OTS public key candidate
    LmsOtsGetPublicCandidate_L(keyId, leafIndex, (const LMS_OTS_CHUNK_T*)(otsSignature), msgHash, leafOtsPub);

    // Complete full LMS public key using given path
    i         = 0;
    nodeIndex = LMS_LEAF_NODES_START_INDEX(treeHeight) + leafIndex;

    // Get 1st root candidate
    lmsOtsCalcLeafHash_L(keyId, nodeIndex, leafOtsPub, rootCandidate);

    /*-----------------------------------------------------------------------------------------------------*/
    /* Traverse the path                                                                                   */
    /*-----------------------------------------------------------------------------------------------------*/
    while (nodeIndex > 1u)
    {
        if ((nodeIndex & 1u) == 1u)
        {
            lmsOtsCalcNodePub_L(keyId, (nodeIndex >> 1), path[i], rootCandidate, rootCandidate);
        }
        else
        {
            lmsOtsCalcNodePub_L(keyId, (nodeIndex >> 1), rootCandidate, path[i], rootCandidate);
        }
        nodeIndex >>= 1;
        i++;
    }

    if (memcmp(rootPubKey, rootCandidate, sizeof(LMS_CHUNK_T)) == 0)
    {
        return ALG_SUCCESS;
    }

    return ALG_ERROR_WRONG_SIGNATURE;
}

LMS_ALG_STATUS WINBOND_LMS_API lmsGetKeyIdFromPublicKey(IN ALG_LMS_T* lmsAlg, IN OUT LMS_KEY_ID_FROM_PUBLIC_KEY_OP_T* params)
{
    LMS_ALG_STATUS ret;
    if (lmsAlg == NULL || params == NULL || params->keyId == NULL || params->publicKey == NULL ||
        params->publicKeyLen != LMS_PUBLIC_KEY_LENGTH)
    {
        return ALG_ERROR_INVALID_PARAMETER;
    }
    ret = lmsPubKeyDeserialize_L(params->publicKey, params->publicKeyLen, NULL, NULL, params->keyId, NULL);
    if (ret != ALG_SUCCESS)
    {
        return ret;
    }
    return ALG_SUCCESS;
}

#ifndef VERIFY_ONLY
LMS_ALG_STATUS lmsSetCacheSize(uint32_t cacheSize)
{
    if ((cacheSize & (cacheSize - 1u)) != 0u)
    {
        return ALG_ERROR_NOT_SUPPORTED;
    }
    if (cacheSize > LMS_MAX_CACHE_SIZE)
    {
        return ALG_ERROR_NOT_SUPPORTED;
    }

    LMS_G.cacheSize = cacheSize;
    return ALG_SUCCESS;
}

LMS_ALG_STATUS lmsGetCache(IN const LMS_KEY_ID_T keyId, LMS_CHUNK_T* cache, uint32_t cacheSize)
{
    LMS_ALG_STATUS ret;
    uint32_t       size;

    if (cacheSize != LMS_G.cacheSize)
    {
        return ALG_ERROR_INVALID_PARAMETER;
    }
    if ((ret = LMS_FetchCache(keyId, (void*)cache, cacheSize * (uint32_t)(sizeof(LMS_CACHE_NODE_T)), &size)) != ALG_SUCCESS)
    {
        return ret;
    }
    if (size != (LMS_G.cacheSize * sizeof(LMS_CACHE_NODE_T)))
    {
        return ALG_ERROR_CORRUPTED_DATA;
    }

    return ALG_SUCCESS;
}

LMS_ALG_STATUS lmsGenerateKeyPairWithSeed(IN ALG_LMS_T* lmsAlg, IN LMS_GEN_KEY_PAIR_OP_T* params, IN const LMS_SEED_T seed)
{
    LMS_KEY_ID_T keyIdInvalid = INVALID_KEY_ID;
    if (seed == NULL)
    {
        return ALG_ERROR_INVALID_PARAMETER;
    }
    if (params->keyId == NULL)
    {
        return ALG_ERROR_INVALID_PARAMETER;
    }
    if (memcmp(params->keyId, keyIdInvalid, sizeof(LMS_KEY_ID_T)) == 0)
    {
        return ALG_ERROR_INVALID_PARAMETER;
    }
    return lmsGenerateKeyPair_L(lmsAlg, params, seed);
}

/************************************************************************************************************
 * @brief This function calculates OTS Chain Public Key (Ki)
 *
 * @param[in]   keyId        key Id
 * @param[in]   seed         private key seed
 * @param[in]   otsIndex     leaf index
 * @param[in]   chainIndex   chain index
 * @param[out]  pubChunk     OTS Chain Public Key (Ki)
************************************************************************************************************/
static
    LMS_ALG_STATUS
    lmsOtsPubChain(const LMS_KEY_ID_T keyId,
                   const LMS_SEED_T   seed,
                   uint32_t           otsIndex,
                   uint16_t           chainIndex,
                   LMS_OTS_CHUNK_T    pubChunk)
{
    LMS_OTS_CHUNK_T priv;

    if (seed == NULL || pubChunk == NULL)
    {
        return ALG_ERROR_INVALID_PARAMETER;
    }
    /*-----------------------------------------------------------------------------------------------------*/
    /* Generate private key chunk                                                                          */
    /*-----------------------------------------------------------------------------------------------------*/
    lmsOtsGetPriv_L(seed, otsIndex, chainIndex, priv);

    /*-----------------------------------------------------------------------------------------------------*/
    /* Generate public key chunk                                                                           */
    /*-----------------------------------------------------------------------------------------------------*/
    lmsHashChain_L(priv, keyId, otsIndex, chainIndex, 0, LMS_MAX_DIGIT, pubChunk);

    return ALG_SUCCESS;
}
#endif // VERIFY_ONLY

/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
/*                                             LOCAL FUNCTIONS                                             */
/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/

/************************************************************************************************************
 * @brief This function calculates OTS public key for signature verification
 *
 * @param[in]   keyId              LMS key id
 * @param[in]   leafNum            Leaf number
 * @param[in]   otsSig             OTS signature
 * @param[in]   msgHash            Message hash
 * @param[out]  pubCandidate       Output OTS public key candidate
************************************************************************************************************/
static void LmsOtsGetPublicCandidate_L(const LMS_KEY_ID_T    keyId,
                                       uint32_t              leafNum,
                                       const LMS_OTS_SIG_T   otsSig,
                                       const LMS_OTS_CHUNK_T msgHash,
                                       LMS_OTS_CHUNK_T       pubCandidate)
{
    uint8_t         i;
    uint8_t         digit;
    uint16_t        checksum   = LMS_NUM_OF_CHAINS * LMS_MAX_DIGIT;
    SHA256_OUTPUT_T hashOutput = {0u};
    void*           hashCtx;
    uint16_t        d_pblc = LMS_D_PBLC;
    LMS_OTS_CHUNK_T pubChunks[LMS_PARAM_P];
    uint8_t         beOtsIdxQ[sizeof(uint32_t)];
    LMS_SET_U32_IN_BUF_BE(leafNum, beOtsIdxQ);

    for (i = 0; i < LMS_PARAM_P; ++i)
    {
        if (i < LMS_NUM_OF_CHAINS)
        {
            digit = LMS_COEF(msgHash, i);
            checksum -= digit;
        }
        else
        {
            uint8_t nibbleIndex = (uint8_t)(((LMS_PARAM_P - LMS_NUM_OF_CHAINS) - (i - LMS_NUM_OF_CHAINS)) - 1u);
            digit               = (uint8_t)((checksum >> (nibbleIndex * LMS_PARAM_W)) & 0xFu);
        }

        lmsHashChain_L(otsSig[i], keyId, leafNum, (uint16_t)i, digit, LMS_MAX_DIGIT - digit, pubChunks[i]);
    }
    (void)LMS_SHA256_Init(&hashCtx);
    (void)LMS_SHA256_Update(hashCtx, keyId, (uint32_t)sizeof(LMS_KEY_ID_T));
    (void)LMS_SHA256_Update(hashCtx, beOtsIdxQ, (uint32_t)sizeof(uint32_t));
    (void)LMS_SHA256_Update(hashCtx, &d_pblc, (uint32_t)sizeof(uint16_t));
    (void)LMS_SHA256_Update(hashCtx, pubChunks, (uint32_t)sizeof(pubChunks));
    (void)LMS_SHA256_Finish(hashCtx, hashOutput);

    (void)memcpy(pubCandidate, (uint8_t*)hashOutput, sizeof(LMS_OTS_CHUNK_T));
}

#ifndef VERIFY_ONLY
/************************************************************************************************************
 * @brief This function calculates the root of Merkle tree, optionally populating node cache
 *
 * @param[in]       keyId              lms tree key Id
 * @param[in]       rootIndex          Required root index
 * @param[in]       privKey            private key data
 * @param[in,out]   cache              cache
 * @param[out]      root               Output Merkle tree root
 * @param[in]       cacheRO            if true, cache used in read-only mode
************************************************************************************************************/
LMS_ALG_STATUS lmsGetRoot_L(const LMS_KEY_ID_T    keyId,
                            uint32_t              rootIndex,
                            const LMS_KEY_PRIV_T* privKey,
                            LMS_CACHE_NODE_T*     cache,
                            uint8_t*              root,
                            bool                  cacheRO)
{
    /*-----------------------------------------------------------------------------------------------------*/
    /* Check if node is cached                                                                             */
    /*-----------------------------------------------------------------------------------------------------*/
    if (LMS_NODE_IN_CACHE(privKey->treeHeight, cache, rootIndex))
    {
        (void)memcpy(root, (uint8_t*)cache[rootIndex - LMS_FIRST_CACHED_INDEX(privKey->treeHeight)], sizeof(LMS_CHUNK_T));
    }
    /*-----------------------------------------------------------------------------------------------------*/
    /* if not cached - calculate                                                                           */
    /*-----------------------------------------------------------------------------------------------------*/
    else
    {
        uint32_t        nodesToHash[LMS_MAX_TREE_HEIGHT + 1u];
        uint32_t        nodesToHashSize;
        LMS_HASH_NODE_T nodesHashed[LMS_MAX_TREE_HEIGHT + 1u];
        uint32_t        nodesHashedSize = 0u;
        LMS_CHUNK_T     nodePubKey      = {0u};
        uint32_t        nodeIndex;

        nodesToHash[0]  = rootIndex;
        nodesToHashSize = 1u;
        while (nodesToHashSize > 0u)
        {
            bool nodeInCache;
            // pop one node from stack
            nodeIndex = nodesToHash[nodesToHashSize - 1u];
            nodesToHashSize -= 1u;

            nodeInCache = (LMS_NODE_IN_CACHE(privKey->treeHeight, cache, nodeIndex)) ? true : false;
            if ((nodeIndex >= LMS_LEAF_NODES_START_INDEX(privKey->treeHeight)) || (nodeInCache == true))
            {
                bool     loopExit;
                uint8_t* pNodePubKey = NULL;

                if (nodeInCache == false)
                {
                    // this is a leaf
                    lmsGetLeaf_L(keyId, privKey, nodeIndex, nodePubKey);
                    pNodePubKey = nodePubKey;
                }
                else
                {
                    // node in cache
                    pNodePubKey = (uint8_t*)(cache[nodeIndex - LMS_FIRST_CACHED_INDEX(privKey->treeHeight)]);
                }

                do
                {
                    loopExit = true;
                    // add to hashed nodes stack
                    if (nodesHashedSize > LMS_MAX_TREE_HEIGHT)
                    {
                        // this is incorrect state. not supposed to get here
                        (void)memset(root, 0, sizeof(LMS_CHUNK_T));
                        return ALG_ERROR_SYSTEM_IN_INCORRECT_STATE;
                    }

                    nodesHashed[nodesHashedSize].nodeIndex = nodeIndex;
                    (void)memcpy(nodesHashed[nodesHashedSize].hash, pNodePubKey, sizeof(LMS_CHUNK_T)); /*lint !e420 PCLP-5299u */
                    nodesHashedSize = nodesHashedSize + 1u;

                    // Add result to cache
                    if ((cacheRO == false) && (nodeInCache == false) && (LMS_G.cacheSize > 0u))
                    {
                        if ((nodeIndex >= LMS_FIRST_CACHED_INDEX(privKey->treeHeight)) &&
                            (nodeIndex < (LMS_FIRST_CACHED_INDEX(privKey->treeHeight) + LMS_G.cacheSize)))
                        {
                            (void)memcpy((uint8_t*)cache[nodeIndex - LMS_FIRST_CACHED_INDEX(privKey->treeHeight)],
                                         nodePubKey,
                                         sizeof(LMS_CHUNK_T));
                        }
                    }

                    if (nodesHashedSize >= 2u)
                    {
                        if ((nodesHashed[nodesHashedSize - 2u].nodeIndex + 1u) == nodesHashed[nodesHashedSize - 1u].nodeIndex)
                        {
                            loopExit = false;
                            // if the last 2 nodes in stack are brothers, compute their father node
                            nodeIndex = nodesHashed[nodesHashedSize - 1u].nodeIndex >> 1u;

                            ///Calculate node hash
                            lmsOtsCalcNodePub_L(keyId,
                                                nodeIndex,
                                                nodesHashed[nodesHashedSize - 2u].hash,
                                                nodesHashed[nodesHashedSize - 1u].hash,
                                                nodePubKey);
                            pNodePubKey = nodePubKey;
                            nodeInCache = false;

                            // pop
                            nodesHashedSize -= 2u;
                        }
                    }

                } while (loopExit == false);
            }
            else
            {
                // internal node - add sons to stack
                nodesToHash[nodesToHashSize++] = 2u * nodeIndex + 1u;
                nodesToHash[nodesToHashSize++] = 2u * nodeIndex;
            }
        }

        // set result
        (void)memcpy(root, nodePubKey, sizeof(LMS_CHUNK_T));
    }
    return ALG_SUCCESS;
}

/************************************************************************************************************
 * @brief This function calculates a public leaf of Merkle tree
 *
 * @param[in]  keyId              lms tree key Id
 * @param[in]  privKey            private key data
 * @param[in]  leafIndex          Leaf index
 * @param[out] leafHash           Output public leaf hash
************************************************************************************************************/
static void lmsGetLeaf_L(const LMS_KEY_ID_T keyId, const LMS_KEY_PRIV_T* privKey, uint32_t leafIndex, LMS_CHUNK_T leafHash)
{
    uint32_t        q;
    LMS_OTS_CHUNK_T otsPub;

    /*-----------------------------------------------------------------------------------------------------*/
    /* Get leaf number                                                                                     */
    /*-----------------------------------------------------------------------------------------------------*/
    q = leafIndex - LMS_LEAF_NODES_START_INDEX(privKey->treeHeight);

    /*-----------------------------------------------------------------------------------------------------*/
    /* Get OTS public key                                                                                  */
    /*-----------------------------------------------------------------------------------------------------*/
    lmsGetLeafPublicKey_L(keyId, privKey, q, otsPub);

    /*-----------------------------------------------------------------------------------------------------*/
    /* Calculate leaf hash                                                                                 */
    /*-----------------------------------------------------------------------------------------------------*/
    lmsOtsCalcLeafHash_L(keyId, leafIndex, otsPub, leafHash);
}

/************************************************************************************************************
 * @brief This function calculates OTS public key for given leaf index
 *
 * @param[in]   keyId        lms tree key Id
 * @param[in]   privKey      private key data
 * @param[in]   leafNum      Leaf number that goes into OTS hash calculations
 * @param[out]  otsPubKey    OTS public key for the given @p leafIndex
************************************************************************************************************/
static void lmsGetLeafPublicKey_L(const LMS_KEY_ID_T    keyId,
                                  const LMS_KEY_PRIV_T* privKey,
                                  uint32_t              leafNum,
                                  LMS_OTS_CHUNK_T       otsPubKey)
{
    uint8_t         pubHashBuf[sizeof(LMS_KEY_ID_T) + sizeof(uint32_t) + sizeof(uint16_t) +
                       LMS_PARAM_P * sizeof(LMS_CHUNK_T)]; // keyId || q || d_pblc || pubChunks
    uint8_t*        pBuf;
    uint32_t        i;
    SHA256_OUTPUT_T hashOutput;
    LMS_OTS_CHUNK_T chunk;

    (void)memset(pubHashBuf, 0, sizeof(pubHashBuf));
    pBuf = &pubHashBuf[sizeof(LMS_KEY_ID_T) + sizeof(uint32_t) + sizeof(uint16_t)];
    for (i = 0; i < LMS_PARAM_P; ++i)
    {
        (void)lmsOtsPubChain(keyId, privKey->seed, leafNum, (uint16_t)i, chunk);

        (void)memcpy(pBuf, chunk, sizeof(LMS_OTS_CHUNK_T));
        pBuf += sizeof(LMS_OTS_CHUNK_T);
    }

    pBuf = pubHashBuf;
    (void)memcpy(pBuf, keyId, sizeof(LMS_KEY_ID_T));
    pBuf += sizeof(LMS_KEY_ID_T);
    LMS_SET_U32_IN_BUF_BE(leafNum, pBuf);
    pBuf += sizeof(uint32_t);
    *pBuf++ = (uint8_t)(LMS_D_PBLC & 0xFFu);
    *pBuf   = (uint8_t)((LMS_D_PBLC >> 8u) & 0xFFu);

    LMS_SHA256(hashOutput, pubHashBuf, (uint32_t)sizeof(pubHashBuf));
    (void)memcpy(otsPubKey, (uint8_t*)hashOutput, sizeof(LMS_OTS_CHUNK_T));
}

/************************************************************************************************************
 * @brief This routine generates a private key chunk
 *
 * @param[in]   seed         PARAM_M bytes private key seed
 * @param[in]   q            leaf index
 * @param[in]   chainIndex   chain(digit) index
 * @param[out]  priv         private key chunk
************************************************************************************************************/
static void lmsOtsGetPriv_L(const uint8_t* seed, uint32_t q, uint16_t chainIndex, LMS_OTS_CHUNK_T priv)
{
    uint8_t
        buf[LMS_PARAM_M + (4u * (uint32_t)sizeof(uint32_t)) + (4u * (uint32_t)sizeof(uint16_t))]; // NVKroot || sc_vect || dc_vect
    uint8_t*        pBuf = buf;
    SHA256_OUTPUT_T hash;

    // NVKroot
    (void)memcpy(pBuf, seed, LMS_PARAM_M); /*lint !e420 PCLP-5299u */
    pBuf += LMS_PARAM_M;
    // sc_vect
    *pBuf++ = (uint8_t)((q >> 24u) & 0xFFu);
    (void)memset(pBuf, 0, 3);
    pBuf += 3;
    *pBuf++ = (uint8_t)((q >> 16u) & 0xFFu);
    (void)memset(pBuf, 0, 3);
    pBuf += 3;
    *pBuf++ = (uint8_t)((q >> 8u) & 0xFFu);
    (void)memset(pBuf, 0, 3);
    pBuf += 3;
    *pBuf++ = (uint8_t)(q & 0xFFu);
    (void)memset(pBuf, 0, 3);
    pBuf += 3;
    // dc_vect
    *pBuf++ = (uint8_t)((chainIndex >> 8u) & 0xFFu);
    (void)memset(pBuf, 0, 3);
    pBuf += 3;
    *pBuf++ = (uint8_t)(chainIndex & 0xFFu);
    (void)memset(pBuf, 0, 3);

    LMS_SHA256(hash, buf, (uint32_t)sizeof(buf));
    (void)memcpy(priv, (uint8_t*)hash, sizeof(LMS_CHUNK_T));
}
#endif // VERIFY_ONLY

static void lmsOtsCalcNodePub_L(const LMS_KEY_ID_T keyId,
                                uint32_t           q,
                                const LMS_CHUNK_T  left,
                                const LMS_CHUNK_T  right,
                                OUT LMS_CHUNK_T    pub)
{
    // keyId || q || d_intr || left || right
    uint16_t        d_intr = LMS_D_INTR;
    SHA256_OUTPUT_T hash;
    void*           hashCtx;
    uint8_t         beOtsIdxQ[(uint32_t)sizeof(uint32_t)];
    LMS_SET_U32_IN_BUF_BE(q, beOtsIdxQ);

    (void)LMS_SHA256_Init(&hashCtx);
    //keyId
    (void)LMS_SHA256_Update(hashCtx, keyId, (uint32_t)sizeof(LMS_KEY_ID_T));
    // q
    (void)LMS_SHA256_Update(hashCtx, beOtsIdxQ, (uint32_t)sizeof(uint32_t));
    // d_intr
    (void)LMS_SHA256_Update(hashCtx, &d_intr, (uint32_t)sizeof(uint16_t));
    // left
    (void)LMS_SHA256_Update(hashCtx, left, (uint32_t)sizeof(LMS_CHUNK_T));
    // right
    (void)LMS_SHA256_Update(hashCtx, right, (uint32_t)sizeof(LMS_CHUNK_T));

    (void)LMS_SHA256_Finish(hashCtx, hash);
    (void)memcpy(pub, (uint8_t*)hash, sizeof(LMS_CHUNK_T));
}

static void lmsCalcMsgHash(IN const LMS_KEY_ID_T    keyId,
                           IN uint32_t              otsIdxQ,
                           IN const LMS_MSG_NONCE_T nonce,
                           IN const uint8_t*        msg,
                           IN uint32_t              msgSize,
                           OUT LMS_OTS_CHUNK_T      msgHash)
{
    SHA256_OUTPUT_T hash;
    void*           hashCtx;
    uint16_t        d_mesg = LMS_D_MESG;
    uint8_t         beOtsIdxQ[(uint32_t)sizeof(uint32_t)];
    LMS_SET_U32_IN_BUF_BE(otsIdxQ, beOtsIdxQ);

    (void)LMS_SHA256_Init(&hashCtx);
    (void)LMS_SHA256_Update(hashCtx, keyId, (uint32_t)sizeof(LMS_KEY_ID_T));
    (void)LMS_SHA256_Update(hashCtx, beOtsIdxQ, (uint32_t)sizeof(uint32_t));
    (void)LMS_SHA256_Update(hashCtx, &d_mesg, (uint32_t)sizeof(uint16_t));
    (void)LMS_SHA256_Update(hashCtx, nonce, (uint32_t)sizeof(LMS_MSG_NONCE_T));
    (void)LMS_SHA256_Update(hashCtx, msg, msgSize);
    (void)LMS_SHA256_Finish(hashCtx, hash);

    (void)memcpy(msgHash, (uint8_t*)hash, sizeof(LMS_OTS_CHUNK_T));
}

#ifndef VERIFY_ONLY
static void lmsOtsSignDigit_L(const LMS_KEY_ID_T       keyId,
                              const IN LMS_KEY_PRIV_T* privKey,
                              IN uint8_t               msgDigit,
                              IN uint16_t              chainIndex,
                              IN OUT uint32_t*         checksum,
                              OUT LMS_OTS_CHUNK_T      digitSig)
{
    uint32_t        q = privKey->otsIdx;
    LMS_OTS_CHUNK_T priv;
    uint8_t         digit;

    /*-----------------------------------------------------------------------------------------------------*/
    /* Get the digit to sign                                                                               */
    /*-----------------------------------------------------------------------------------------------------*/
    if (chainIndex < LMS_NUM_OF_CHAINS)
    {
        /*-------------------------------------------------------------------------------------------------*/
        /* Set digit                                                                                       */
        /*-------------------------------------------------------------------------------------------------*/
        digit = msgDigit;
        /*-------------------------------------------------------------------------------------------------*/
        /* Add checksum                                                                                    */
        /*-------------------------------------------------------------------------------------------------*/
        (*checksum) -= msgDigit;
    }
    else
    {
        /*-------------------------------------------------------------------------------------------------*/
        /* Sign checksum                                                                                   */
        /*-------------------------------------------------------------------------------------------------*/
        uint8_t nibbleIndex = ((LMS_PARAM_P - LMS_NUM_OF_CHAINS) - ((uint8_t)chainIndex - LMS_NUM_OF_CHAINS)) - 1u;
        digit               = (uint8_t)(((*checksum) >> (nibbleIndex * LMS_PARAM_W)) & 0xFu);
    }

    /*-----------------------------------------------------------------------------------------------------*/
    /* Generate private key chunk                                                                          */
    /*-----------------------------------------------------------------------------------------------------*/
    lmsOtsGetPriv_L(privKey->seed, q, chainIndex, priv);

    /*-----------------------------------------------------------------------------------------------------*/
    /* Sign the digit                                                                                      */
    /*-----------------------------------------------------------------------------------------------------*/
    lmsHashChain_L(priv, keyId, q, (uint16_t)chainIndex, 0, digit, digitSig);
}

/************************************************************************************************************
 * @brief This function calculates the OTS signature for give message hash using internal flash state
 *
 * @param[in]   keyId          Key Id
 * @param[in]   privKey        private Key data
 * @param[in]   msgHash        Message hash to sign
 * @param[out]  otsSig         OTS signature
************************************************************************************************************/
static void lmsSign_L(const LMS_KEY_ID_T       keyId,
                      const IN LMS_KEY_PRIV_T* privKey,
                      const LMS_OTS_CHUNK_T    msgHash,
                      LMS_OTS_SIG_T            otsSig)
{
    uint32_t i;
    uint8_t  digit;
    uint32_t checksum = LMS_NUM_OF_CHAINS * LMS_MAX_DIGIT;

    for (i = 0; i < LMS_PARAM_P; ++i)
    {
        if (i < LMS_NUM_OF_CHAINS)
        {
            digit = LMS_COEF(msgHash, i);
        }
        else
        {
            digit = 0;
        }

        lmsOtsSignDigit_L(keyId, privKey, digit, (uint16_t)i, &checksum, otsSig[i]);
    }
}
#endif // VERIFY_ONLY

static void lmsOtsCalcLeafHash_L(IN const LMS_KEY_ID_T    keyId,
                                 IN uint32_t              nodeIndex,
                                 IN const LMS_OTS_CHUNK_T leafPubKey,
                                 OUT LMS_CHUNK_T          rootCandidate)
{
    uint16_t        d_leaf = LMS_D_LEAF;
    void*           hashCtx;
    SHA256_OUTPUT_T hash;
    uint8_t         beNodeIdx[(uint32_t)sizeof(uint32_t)];
    LMS_SET_U32_IN_BUF_BE(nodeIndex, beNodeIdx);
    (void)LMS_SHA256_Init(&hashCtx);

    //keyId
    (void)LMS_SHA256_Update(hashCtx, keyId, (uint32_t)sizeof(LMS_KEY_ID_T));
    // nodeIndex
    (void)LMS_SHA256_Update(hashCtx, beNodeIdx, (uint32_t)sizeof(uint32_t));
    // d_leaf
    (void)LMS_SHA256_Update(hashCtx, &d_leaf, (uint32_t)sizeof(uint16_t));
    // otsPub
    (void)LMS_SHA256_Update(hashCtx, leafPubKey, (uint32_t)sizeof(LMS_OTS_CHUNK_T));

    (void)LMS_SHA256_Finish(hashCtx, hash);
    (void)memcpy(rootCandidate, (uint8_t*)hash, sizeof(LMS_CHUNK_T));
}

static void lmsHashChain_L(const LMS_OTS_CHUNK_T input,
                           const LMS_KEY_ID_T    keyId,
                           uint32_t              q,
                           uint16_t              chainIndex,
                           uint8_t               iterStart,
                           uint8_t               iter,
                           LMS_OTS_CHUNK_T       output)
{
    uint8_t         buf[(uint32_t)sizeof(LMS_KEY_ID_T) + (uint32_t)sizeof(uint32_t) + (uint32_t)sizeof(uint16_t) +
                (uint32_t)sizeof(uint8_t) + (uint32_t)sizeof(LMS_OTS_CHUNK_T)]; // keyId || q || index || iter || prevChunk
    uint8_t*        pBuf = buf;
    uint8_t*        pIter;
    uint8_t*        pPrev;
    SHA256_OUTPUT_T hash;
    uint8_t         j;

    // Set buffer static parameters
    // I
    (void)memcpy(buf, keyId, (uint32_t)sizeof(LMS_KEY_ID_T));
    pBuf += (uint32_t)sizeof(LMS_KEY_ID_T);
    // ots index
    LMS_SET_U32_IN_BUF_BE(q, pBuf);
    pBuf += (uint32_t)sizeof(uint32_t);
    // chain index
    *pBuf++ = (uint8_t)((chainIndex >> 8u) & 0xFFu);
    *pBuf++ = (uint8_t)(chainIndex & 0xFFu);

    pIter = pBuf;
    pPrev = pBuf + (uint32_t)sizeof(uint8_t);
    // Set OTS private key (Xi) as initial prev chunk
    (void)memcpy(pPrev, input, (uint32_t)sizeof(LMS_OTS_CHUNK_T));

    // Perform iterations
    for (j = 0; j < iter; j++)
    {
        *pIter = j + iterStart;
        LMS_SHA256(hash, buf, (uint32_t)sizeof(buf));
        (void)memcpy(pPrev, (uint8_t*)hash, (uint32_t)sizeof(LMS_OTS_CHUNK_T));
    }

    // signed digit result
    (void)memcpy(output, pPrev, sizeof(LMS_OTS_CHUNK_T)); /*lint !e419 PCLP-5299u */
}

#ifndef VERIFY_ONLY
static LMS_ALG_STATUS lmsGenerateKeyPair_L(IN ALG_LMS_T* lmsAlg, IN LMS_GEN_KEY_PAIR_OP_T* params, const uint8_t* seed)
{
    uint8_t          i;
    uint32_t         requestedTreeHeight;
    LMS_ALG_STATUS   ret;
    LMS_KEY_PRIV_T   priv = {0u};
    LMS_KEY_PUB_T    pub;
    LMS_CACHE_NODE_T cache[LMS_MAX_CACHE_SIZE];

    // Error checks
    if (params == NULL || lmsAlg == NULL || params->keyId == NULL)
    {
        return ALG_ERROR_INVALID_PARAMETER;
    }
    if (params->lmotsParameterSetId != LMOTS_SHA256_N24_W4_PARAM_SET_ID)
    {
        return ALG_ERROR_NOT_SUPPORTED;
    }
    if ((params->lmsParameterSetId != LMS_SHA256_M24_H20_PARAM_SET_ID) &&
        (params->lmsParameterSetId != LMS_SHA256_M24_H10_PARAM_SET_ID))
    {
        return ALG_ERROR_NOT_SUPPORTED;
    }
    requestedTreeHeight = (params->lmsParameterSetId == LMS_SHA256_M24_H20_PARAM_SET_ID ? 20u : 10u);

    // Create new key entry with private key
    if (seed == NULL)
    {
        // generate random seed
        for (i = 0; i < LMS_PARAM_M; i++)
        {
            priv.seed[i] = (uint8_t)LMS_GetRandom();
        }
    }
    else
    {
        (void)memcpy(priv.seed, seed, LMS_PARAM_M); /*lint !e420 PCLP-5299u */
    }
    priv.treeHeight = requestedTreeHeight;

    // store private key
    if ((ret = LMS_StorePrivateKey(params->keyId, (const void*)(&priv), (uint32_t)(sizeof(LMS_KEY_PRIV_T)))) != ALG_SUCCESS)
    {
        return ret;
    }

    // create public key (and cache)

    // Calculate root public key, start from fresh cache
    (void)memset(cache, 0, sizeof(LMS_CACHE_NODE_T) * LMS_MAX_CACHE_SIZE);
    if ((ret = lmsGetRoot_L(params->keyId, 1, &priv, cache, (uint8_t*)pub.pubRoot, false)) != ALG_SUCCESS)
    {
        return ret;
    }

    // store cache
    if (LMS_G.cacheSize > 0u)
    {
        if ((ret = LMS_StoreCache(params->keyId, (const void*)(cache), (uint32_t)(sizeof(LMS_CACHE_NODE_T)) * LMS_G.cacheSize)) !=
            ALG_SUCCESS)
        {
            return ret;
        }
    }
    pub.treeHeight = requestedTreeHeight;

    // store public key
    if ((ret = LMS_StorePublicKey(params->keyId, (const void*)(&pub), (uint32_t)(sizeof(LMS_KEY_PUB_T)))) != ALG_SUCCESS)
    {
        return ret;
    }

    return ALG_SUCCESS;
}

static LMS_ALG_STATUS lmsGenerateUniqueKeyId_L(uint8_t* keyId)
{
    uint8_t        i;
    LMS_KEY_PRIV_T priv = {0u};
    LMS_KEY_PUB_T  pub;
    uint32_t       size = 0u;
    LMS_ALG_STATUS ret;
    LMS_KEY_ID_T   keyIdInvalid = INVALID_KEY_ID;

    // Generate random key-id
    do
    {
        uint8_t* pKeyIdByte = keyId;
        for (i = 0; i < LMS_KEY_ID_SIZE; i++)
        {
            *pKeyIdByte = (uint8_t)LMS_GetRandom();
            pKeyIdByte++;
        }
    } while (memcmp(keyId, keyIdInvalid, sizeof(LMS_KEY_ID_T)) == 0);

    // verify key is unique
    ret = LMS_FetchPrivateKey(keyId, &priv, (uint32_t)(sizeof(LMS_KEY_PRIV_T)), &size);
    if (ret == ALG_SUCCESS)
    {
        return ALG_ERROR_RANDOM_KID_USED;
    }
    if (ret != ALG_ERROR_KID_NOT_FOUND)
    {
        return ret;
    }
    ret = LMS_FetchPublicKey(keyId, &pub, (uint32_t)(sizeof(LMS_KEY_PUB_T)), &size);
    if (ret == ALG_SUCCESS)
    {
        return ALG_ERROR_RANDOM_KID_USED;
    }
    if (ret != ALG_ERROR_KID_NOT_FOUND)
    {
        return ret;
    }
    return ALG_SUCCESS;
}

/************************************************************************************************************
* @brief This function serialize parameters in the signature buffer as defined in rfc8554
*
* @param[out]  sigBuf                  the signature buffer
* @param[in]   sigBufSize              provided buffer size
* @param[in]   otsIdxQ                 leaf index
* @param[in]   lmotsParameterSetId     OTS parameter set Id
* @param[in]   randomString            random nonce
* @param[in]   otsSignature            OTS signature
* @param[in]   lmsParameterSetId       LMS parameter set Id
* @param[in]   path                    OTS public key of nodes in path
* @param[in]   numPathNodes            number of path nodes
************************************************************************************************************/
static LMS_ALG_STATUS lmsSignatureSerialize_L(OUT uint8_t*             sigBuf,
                                              IN uint32_t              sigBufSize,
                                              IN uint32_t              otsIdxQ,
                                              IN uint32_t              lmotsParameterSetId,
                                              IN const LMS_MSG_NONCE_T randomString,
                                              IN const LMS_OTS_SIG_T   otsSignature,
                                              IN uint32_t              lmsParameterSetId,
                                              IN const LMS_CHUNK_T*    path,
                                              IN uint32_t              numPathNodes)
{
    uint8_t* ptr = sigBuf;
    uint32_t treeHeight;

    if (lmsParameterSetId == LMS_SHA256_M24_H10_PARAM_SET_ID)
    {
        treeHeight = 10u;
    }
    else if (lmsParameterSetId == LMS_SHA256_M24_H20_PARAM_SET_ID)
    {
        treeHeight = 20u;
    }
    else
    {
        return ALG_ERROR_INVALID_PARAMETER;
    }

    if (sigBuf == NULL || sigBufSize < LMS_SIGNATURE_LENGTH(treeHeight) || randomString == NULL || otsSignature == NULL ||
        path == NULL || numPathNodes != treeHeight)
    {
        return ALG_ERROR_INVALID_PARAMETER;
    }

    LMS_SET_U32_IN_BUF_BE(otsIdxQ, ptr);
    ptr += (uint32_t)sizeof(uint32_t);
    LMS_SET_U32_IN_BUF_BE(lmotsParameterSetId, ptr);
    ptr += (uint32_t)sizeof(uint32_t);
    (void)memcpy(ptr, randomString, sizeof(LMS_MSG_NONCE_T)); /*lint !e420 PCLP-5299u */
    ptr += (uint32_t)sizeof(LMS_MSG_NONCE_T);
    (void)memcpy(ptr, (const uint8_t*)otsSignature, sizeof(LMS_OTS_SIG_T)); /*lint !e420 PCLP-5299u */
    ptr += (uint32_t)sizeof(LMS_OTS_SIG_T);
    LMS_SET_U32_IN_BUF_BE(lmsParameterSetId, ptr);
    ptr += (uint32_t)sizeof(uint32_t);
    (void)memcpy(ptr, (const uint8_t*)path, sizeof(LMS_CHUNK_T) * numPathNodes);

    return ALG_SUCCESS;
}
#endif // VERIFY_ONLY

/************************************************************************************************************
* @brief This function deserialize parameters from the signature buffer as defined in rfc8554
*        Output parameters may be NULL if not needed
* 
* @param[in]    sigBuf                  the signature buffer
* @param[in]    sigBufSize              provided buffer size
* @param[out]   otsIdxQ                 leaf index
* @param[out]   lmotsParameterSetId     OTS parameter set Id
* @param[out]   randomString            random nonce
* @param[out]   otsSignature            OTS signature
* @param[out]   lmsParameterSetId       LMS parameter set Id
* @param[out]   path                    OTS public key of nodes in path
* @param[out]   numPathNodes            number of nodes in path array
************************************************************************************************************/
static LMS_ALG_STATUS lmsSignatureDeserialize_L(IN const uint8_t*   sigBuf,
                                                IN uint32_t         sigBufSize,
                                                OUT uint32_t*       otsIdxQ,
                                                OUT uint32_t*       lmotsParameterSetId,
                                                OUT LMS_MSG_NONCE_T randomString,
                                                OUT LMS_OTS_SIG_T   otsSignature,
                                                OUT uint32_t*       lmsParameterSetId,
                                                OUT LMS_CHUNK_T*    path,
                                                IN uint32_t         numPathNodes)
{
    const uint8_t* ptr = sigBuf;
    uint32_t       treeHeight;
    uint32_t       id;

    if (sigBuf == NULL || sigBufSize < (uint32_t)LMS_SIGNATURE_LENGTH(0u))
    {
        return ALG_ERROR_INVALID_PARAMETER;
    }

    if (otsIdxQ != NULL)
    {
        LMS_GET_U32_FROM_BUF_BE(*otsIdxQ, ptr);
    }
    ptr += (uint32_t)sizeof(uint32_t);
    if (lmotsParameterSetId != NULL)
    {
        LMS_GET_U32_FROM_BUF_BE(*lmotsParameterSetId, ptr);
    }
    ptr += (uint32_t)sizeof(uint32_t);
    if (randomString != NULL)
    {
        (void)memcpy(randomString, ptr, (uint32_t)sizeof(LMS_MSG_NONCE_T)); /*lint !e419 PCLP-5299u */
    }
    ptr += (uint32_t)sizeof(LMS_MSG_NONCE_T);
    if (otsSignature != NULL)
    {
        (void)memcpy((uint8_t*)otsSignature, ptr, (uint32_t)sizeof(LMS_OTS_SIG_T)); /*lint !e419 PCLP-5299u */
    }
    ptr += (uint32_t)sizeof(LMS_OTS_SIG_T);
    if (lmsParameterSetId != NULL)
    {
        LMS_GET_U32_FROM_BUF_BE(*lmsParameterSetId, ptr);
    }
    else
    {
        LMS_GET_U32_FROM_BUF_BE(id, ptr);
        lmsParameterSetId = &id;
    }
    ptr += (uint32_t)sizeof(uint32_t);

    if (*lmsParameterSetId == LMS_SHA256_M24_H20_PARAM_SET_ID)
    {
        treeHeight = 20;
    }
    else if (*lmsParameterSetId == LMS_SHA256_M24_H10_PARAM_SET_ID)
    {
        treeHeight = 10;
    }
    else
    {
        return ALG_ERROR_NOT_SUPPORTED;
    }
    if (sigBufSize < LMS_SIGNATURE_LENGTH(treeHeight))
    {
        return ALG_ERROR_INVALID_PARAMETER;
    }
    if (path != NULL)
    {
        if (numPathNodes < treeHeight)
        {
            return ALG_ERROR_INVALID_PARAMETER;
        }
        (void)memcpy((uint8_t*)path, ptr, treeHeight * sizeof(LMS_CHUNK_T));
    }
    return ALG_SUCCESS;
}

/************************************************************************************************************
* @brief This function serialize parameters in the public key buffer as defined in rfc8554
*
* @param[out]  pubKey              the public key buffer
* @param[in]   pubKeyBufSize       provided buffer size
* @param[in]   lmsParameterSetId   LMS parameter set Id
* @param[in]   lmotsParameterSetId OTS parameter set Id
* @param[in]   keyId               key ID
* @param[in]   treeRoot            Public key of tree root
************************************************************************************************************/
static LMS_ALG_STATUS lmsPubKeySerialize_L(OUT uint8_t*          pubKey,
                                           IN uint32_t           pubKeyBufSize,
                                           IN uint32_t           lmsParameterSetId,
                                           IN uint32_t           lmotsParameterSetId,
                                           IN const LMS_KEY_ID_T keyId,
                                           IN const LMS_CHUNK_T  treeRoot)
{
    uint8_t* ptr = pubKey;

    if (lmsParameterSetId == LMS_SHA256_M24_H20_PARAM_SET_ID || lmsParameterSetId == LMS_SHA256_M24_H10_PARAM_SET_ID)
    {
        if (pubKey == NULL || pubKeyBufSize < LMS_PUBLIC_KEY_SIZE(LMS_PARAM_M) || keyId == NULL)
        {
            return ALG_ERROR_INVALID_PARAMETER;
        }
    }
    else
    {
        return ALG_ERROR_INVALID_PARAMETER;
    }

    LMS_SET_U32_IN_BUF_BE(lmsParameterSetId, ptr);
    ptr += (uint32_t)sizeof(uint32_t);
    LMS_SET_U32_IN_BUF_BE(lmotsParameterSetId, ptr);
    ptr += (uint32_t)sizeof(uint32_t);
    (void)memcpy(ptr, keyId, sizeof(LMS_KEY_ID_T)); /*lint !e420 PCLP-5299u */
    ptr += (uint32_t)sizeof(LMS_KEY_ID_T);
    (void)memcpy(ptr, (const uint8_t*)treeRoot, (uint32_t)sizeof(LMS_CHUNK_T));

    return ALG_SUCCESS;
}

/************************************************************************************************************
* @brief This function deserialize parameters from the public key buffer as defined in rfc8554
*        Output parameters may be NULL if not needed
*
* @param[in]  pubKey              the public key buffer
* @param[in]  pubKeyBufSize       provided buffer size
* @param[out] lmsParameterSetId   LMS parameter set Id
* @param[out] lmotsParameterSetId OTS parameter set Id
* @param[out] keyId               key ID
* @param[out] treeRoot            Public key of tree root
************************************************************************************************************/
static LMS_ALG_STATUS lmsPubKeyDeserialize_L(IN const uint8_t* pubKey,
                                             IN uint32_t       pubKeyBufSize,
                                             OUT uint32_t*     lmsParameterSetId,
                                             OUT uint32_t*     lmotsParameterSetId,
                                             OUT LMS_KEY_ID_T  keyId,
                                             OUT LMS_CHUNK_T   treeRoot)
{
    const uint8_t* ptr = pubKey;

    if (pubKey == NULL || pubKeyBufSize < LMS_PUBLIC_KEY_SIZE(LMS_PARAM_M))
    {
        return ALG_ERROR_INVALID_PARAMETER;
    }

    if (lmsParameterSetId != NULL)
    {
        LMS_GET_U32_FROM_BUF_BE(*lmsParameterSetId, ptr);
    }
    ptr += (uint32_t)sizeof(uint32_t);
    if (lmotsParameterSetId != NULL)
    {
        LMS_GET_U32_FROM_BUF_BE(*lmotsParameterSetId, ptr);
    }
    ptr += (uint32_t)sizeof(uint32_t);
    if (keyId != NULL)
    {
        (void)memcpy(keyId, ptr, (uint32_t)sizeof(LMS_KEY_ID_T)); /*lint !e419 PCLP-5299u */
    }
    ptr += (uint32_t)sizeof(LMS_KEY_ID_T);
    if (treeRoot != NULL)
    {
        (void)memcpy(treeRoot, ptr, sizeof(LMS_CHUNK_T)); /*lint !e419 PCLP-5299u */
    }

    return ALG_SUCCESS;
}
