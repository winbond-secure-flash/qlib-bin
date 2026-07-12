/************************************************************************************************************
* @internal
* @remark     Winbond Electronics Corporation - Confidential
* @copyright  Copyright (c) 2024 by Winbond Electronics Corporation . All rights reserved
* @endinternal
*
* @file       lms.h
* @brief      This file contains lms definitions
*
* ### project lms
*
************************************************************************************************************/

#ifndef _LMS__H_
#define _LMS__H_

#ifdef __cplusplus
extern "C" {
#endif

#include "lms_api.h"
#include "lms_common.h"
/*-----------------------------------------------------------------------------------------------------------
-------------------------------------------------------------------------------------------------------------
                                                 DEFINITIONS
-------------------------------------------------------------------------------------------------------------
-----------------------------------------------------------------------------------------------------------*/
#define LMS_PARAM_N 24u
#define LMS_PARAM_P 51u // Maximum number of chains (including checksum)
#define LMS_PARAM_M 24u

typedef uint8_t         LMS_CHUNK_T[LMS_PARAM_M];     // output of the LMS HASH function
typedef uint8_t         LMS_OTS_CHUNK_T[LMS_PARAM_N]; // output of the OTS HASH function
typedef LMS_OTS_CHUNK_T LMS_OTS_SIG_T[LMS_PARAM_P];   // The OTS signature holds the single signature of OTS leaf
typedef uint8_t         LMS_MSG_NONCE_T[LMS_PARAM_N];
typedef uint8_t         LMS_SEED_T[LMS_PARAM_M];

#define LMS_SIGNATURE_LENGTH(treeHeight) (uint32_t)(LMS_SIGNATURE_SIZE(LMS_PARAM_M, LMS_PARAM_N, LMS_PARAM_P, treeHeight))
#define LMS_PUBLIC_KEY_LENGTH            (uint32_t)(LMS_PUBLIC_KEY_SIZE(LMS_PARAM_M))

#define INVALID_KEY_ID                                                                                  \
    {                                                                                                   \
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, \
    }

/*-----------------------------------------------------------------------------------------------------------
-------------------------------------------------------------------------------------------------------------
                                        INTERFACE FUNCTIONS
-------------------------------------------------------------------------------------------------------------
-----------------------------------------------------------------------------------------------------------*/
/************************************************************************************************************
 * @brief This function sets the cache size
 *
 * @param[in]   cacheSize     number of entries in the cache. should be power of 2
 *
 ************************************************************************************************************/
LMS_ALG_STATUS lmsSetCacheSize(uint32_t cacheSize);

/************************************************************************************************************
 * @brief This function returns the cache buffer
 *
 * @param[in]   keyId     key Id
 * @param[out]  cache     cache data buffer
 * @param[in]   cacheSize number of entries in the cache
************************************************************************************************************/
LMS_ALG_STATUS lmsGetCache(IN const LMS_KEY_ID_T keyId, LMS_CHUNK_T* cache, uint32_t cacheSize);

/************************************************************************************************************
 * @brief   Generates LMS key pair for the specified key ID and seed. keyId is retrieved from @params instead
 *          of generating it internally as in lmsGenerateKeyPair function.
 *          The HSM stores and manages the key-pair internally and shall not export of private keying material.
 *
 * @param[in]   lmsAlg    pointer to a previously populated LMS alg structure.
 * @param[in]   params    pointer to key pair parameters
 * @param[in]   seed      pointer to seed to be used for private key
   *
 * @return
 * ALG_SUCCESS                  - no error occurred\n
 * ALG_ERROR_RNG_FAILURE        - Random Number Generation error\n
 * ALG_ERROR_NOT_SUPPORTED      - parameters sets' ids not supported
 * ALG_ERROR_OUT_OF_MEMORY      - reached max supported keys
 * ALG_ERROR_INVALID_PARAMETER  - one of the parameters is NULL or seedSize is different then 24B
 * ALG_ERROR_(ERROR)            - Other error
************************************************************************************************************/
LMS_ALG_STATUS lmsGenerateKeyPairWithSeed(IN ALG_LMS_T* lmsAlg, IN LMS_GEN_KEY_PAIR_OP_T* params, IN const LMS_SEED_T seed);


#ifdef __cplusplus
}
#endif

#endif // _LMS__H_
