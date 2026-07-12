/************************************************************************************************************
* @internal
* @remark     Winbond Electronics Corporation - Confidential
* @copyright  Copyright (c) 2023 by Winbond Electronics Corporation . All rights reserved
* @endinternal
*
* @file       qlib_sample_lms_attestation.c
* @brief      This file contains QLIB example for LMS attestation
*
* @example    qlib_sample_lms_attestation.c
*
* @page       LMS_attestation LMS attestation example code
* This sample code shows how to perform LMS attestation signature verification.\n
*
* @include    samples/qlib_sample_lms_attestation.c
*
************************************************************************************************************/

/*-----------------------------------------------------------------------------------------------------------
                                                   INCLUDES
-----------------------------------------------------------------------------------------------------------*/
#include "qlib.h"
#include "qlib_sample.h"
#include "qlib_sample_lms_attestation.h"
#include "qlib_utils_lms.h"

#if !defined(EXCLUDE_LMS_ATTESTATION)
#include "lms_api.h"
/*-----------------------------------------------------------------------------------------------------------
                                                 DEFINITIONS
-----------------------------------------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------------------------------------
    User defined parameters
-----------------------------------------------------------------------------------------------------------*/
// The message being signed
#define LMS_ATTEST_MSG                                                                 \
    {                                                                                  \
        'H', 'e', 'l', 'l', 'o', ' ', 'w', 'o', 'r', 'l', 'd', '!', '!', '!', '!', '!' \
    }
// Random nonce data
#define LMS_ATTEST_NONCE                                                                                                        \
    {                                                                                                                           \
        0x64, 0xC8, 0x93, 0x25, 0x4A, 0x94, 0x2B, 0x56, 0xAC, 0x5B, 0xB6, 0x6F, 0xDE, 0xBF, 0x7D, 0xFA, 0xF7, 0xED, 0xD9, 0xB1, \
            0x61, 0xC2, 0x87, 0x0D                                                                                              \
    }

/*-----------------------------------------------------------------------------------------------------------
    Parameters from the database, corresponding to the specific chip
-----------------------------------------------------------------------------------------------------------*/
#ifdef QLIB_FPGA_TARGET
#define LMS_ATTEST_SAMPLE_LMS_KEY_ID                                                                   \
    {                                                                                                  \
        0x64, 0xC8, 0x93, 0x25, 0x4A, 0x94, 0x2B, 0x56, 0xAC, 0x5B, 0xB6, 0x6F, 0xDE, 0xBF, 0x7D, 0xFA \
    }
#define LMS_ATTEST_PRIVATE_SEED                                                                                                 \
    {                                                                                                                           \
        0x2D, 0x5A, 0xB4, 0x6B, 0xD6, 0xAF, 0x5D, 0xBA, 0x77, 0xEE, 0xDF, 0xBD, 0x79, 0xF2, 0xE7, 0xCD, 0x99, 0x31, 0x62, 0xC4, \
            0x8B, 0x15, 0x2A, 0x54                                                                                              \
    }
#endif // QLIB_FPGA_TARGET

#ifndef LMS_ATTEST_CACHE_SIZE
#define LMS_ATTEST_CACHE_SIZE QLIB_LMS_ATTEST_ALL_NODES
#endif // LMS_ATTEST_CACHE_SIZE

#define LMS_ATTEST_CACHE \
    {                    \
        0u               \
    }

/*-----------------------------------------------------------------------------------------------------------
                                               LOCAL VARIABLES
-----------------------------------------------------------------------------------------------------------*/

// public key data storage. the use of globals is only for demonstration of the flow.
// It should be replaced with some secure storage
static LMS_ATTEST_CHUNK_T   pubCache_L[LMS_ATTEST_CACHE_SIZE];
static LMS_ATTEST_PUB_KEY_T lmsPubKey_L;

/*-----------------------------------------------------------------------------------------------------------
                                         LOCAL FUNCTION PROTOTYPES
-----------------------------------------------------------------------------------------------------------*/
// The following functions are used to store public key and cache which are generated during manufacture stage
// for future use when chip verification is needed in the field flow
static void QLIB_SAMPLE_LmsAttest_storePub_L(LMS_ATTEST_PUB_KEY_T lmsPubKey, LMS_ATTEST_CHUNK_T* pubCache);
static void QLIB_SAMPLE_LmsAttest_retrievePub_L(LMS_ATTEST_PUB_KEY_T lmsPubKey, LMS_ATTEST_CHUNK_T* pubCache);

/*-----------------------------------------------------------------------------------------------------------
                                             INTERFACE FUNCTIONS
-----------------------------------------------------------------------------------------------------------*/
QLIB_STATUS_T QLIB_SAMPLE_LmsAttest(void* userData_pv)
{
    QLIB_CONTEXT_T    qlibContext;
    QLIB_STATUS_T     status    = QLIB_STATUS__OK;
    QLIB_BUS_FORMAT_T busFormat = QLIB_BUS_FORMAT(QLIB_BUS_MODE_1_1_1, (bool)false);

    /*-------------------------------------------------------------------------------------------------------
     Init QLIB  - needed when running QLIB either on a device or on a remote server.
    -------------------------------------------------------------------------------------------------------*/
    QLIB_STATUS_RET_CHECK(QLIB_InitLib(&qlibContext));

    /*-------------------------------------------------------------------------------------------------------
     Set the user data into Qlib's context
    -------------------------------------------------------------------------------------------------------*/
    QLIB_SetUserData(&qlibContext, userData_pv);

    /*-------------------------------------------------------------------------------------------------------
     Take the ownership of flash communication channel (it belongs to local MCU or to remote server, exclusively)
    -------------------------------------------------------------------------------------------------------*/
    QLIB_STATUS_RET_CHECK(QLIB_Connect(&qlibContext));

    /*-------------------------------------------------------------------------------------------------------
     Init Flash Device - not needed when using QLIB on a remote server
    -------------------------------------------------------------------------------------------------------*/
    QLIB_STATUS_RET_CHECK_GOTO(QLIB_InitDevice(&qlibContext, busFormat), status, disconnect);

    /*-------------------------------------------------------------------------------------------------------
     Run manufacture stage code
    -------------------------------------------------------------------------------------------------------*/
    QLIB_STATUS_RET_CHECK_GOTO(QLIB_SAMPLE_LmsAttest_manuf_flow(&qlibContext), status, disconnect);

    /*-------------------------------------------------------------------------------------------------------
     Run sample flow
    -------------------------------------------------------------------------------------------------------*/
    QLIB_STATUS_RET_CHECK_GOTO(QLIB_SAMPLE_LmsAttest_field_flow(&qlibContext), status, disconnect);

    /*-------------------------------------------------------------------------------------------------------
     Release the ownership of flash communication channel (it belongs to local MCU or to remote server, exclusively)
    -------------------------------------------------------------------------------------------------------*/
    QLIB_STATUS_RET_CHECK(QLIB_Disconnect(&qlibContext));

    return QLIB_STATUS__OK;

disconnect:
    (void)QLIB_Disconnect(&qlibContext);
    return status;
}

QLIB_STATUS_T QLIB_SAMPLE_LmsAttest_manuf_flow(QLIB_CONTEXT_T* qlibContext)
{
    QLIB_STATUS_T        status                          = QLIB_STATUS__OK;
    LMS_ATTEST_CHUNK_T   pubCache[LMS_ATTEST_CACHE_SIZE] = LMS_ATTEST_CACHE;
    LMS_ATTEST_PUB_KEY_T lmsPubKey;

    /*-------------------------------------------------------------------------------------------------------
     Run sample code
    -------------------------------------------------------------------------------------------------------*/

#if QLIB_MAX_DIES_SUPPORTED > 1u
    // DIE 0 is used for supply chain protection
    QLIB_STATUS_RET_CHECK(QLIB_SetActiveDie(qlibContext, 0u));
#endif // QLIB_MAX_DIES_SUPPORTED > 1u

    /*-------------------------------------------------------------------------------------------------------
     Generate LMS public key and public cache
    -------------------------------------------------------------------------------------------------------*/
    status = QLIB_LMS_Attest_GetPublicKey(qlibContext, lmsPubKey, pubCache, LMS_ATTEST_CACHE_SIZE);

    if (status == QLIB_STATUS__SYSTEM_IN_INCORRECT_STATE)
    {
#ifndef QLIB_FPGA_TARGET
        QLIB_SAMPLE_PRINTF("Error: The LMS private key must be set for this test");
        /*---------------------------------------------------------------------------------------------------
         please note that setting the LMS private key can be done only once in flash life cycle
         consider carefully before setting it. In order to set the private key use the following command
         QLIB_LMS_KEY_ID_T         keyId = {LMS_PRIV_KEY};
         LMS_ATTEST_PRIVATE_SEED_T seed  = {LMS_PRIV_SEED};
         QLIB_STATUS_RET_CHECK(QLIB_LMS_Attest_SetPrivateKey(qlibContext, seed, keyId));
        ---------------------------------------------------------------------------------------------------*/
        goto exit;
#else  // QLIB_FPGA_TARGET
        /*---------------------------------------------------------------------------------------------------
         Provision private key - only required for FPGA
        ---------------------------------------------------------------------------------------------------*/
        QLIB_LMS_KEY_ID_T         keyId = LMS_ATTEST_SAMPLE_LMS_KEY_ID;
        LMS_ATTEST_PRIVATE_SEED_T seed  = LMS_ATTEST_PRIVATE_SEED;
        QLIB_STATUS_RET_CHECK(QLIB_LMS_Attest_SetPrivateKey(qlibContext, seed, keyId));
        QLIB_STATUS_RET_CHECK(QLIB_LMS_Attest_GetPublicKey(qlibContext, lmsPubKey, pubCache, LMS_ATTEST_CACHE_SIZE));
#endif // QLIB_FPGA_TARGET
    }
    else
    {
        QLIB_STATUS_RET_CHECK(status);
    }

    /*-------------------------------------------------------------------------------------------------------
     public key is stored during manufacture stage (in vault section, DB or other secure location)
     and used later when verifying the signature on field
    -------------------------------------------------------------------------------------------------------*/
    QLIB_SAMPLE_LmsAttest_storePub_L(lmsPubKey, pubCache);

exit:
    return QLIB_STATUS__OK;
}

QLIB_STATUS_T QLIB_SAMPLE_LmsAttest_field_flow(QLIB_CONTEXT_T* qlibContext)
{
    QLIB_STATUS_T        status                          = QLIB_STATUS__OK;
    uint8_t              msg_u8[]                        = LMS_ATTEST_MSG;
    LMS_ATTEST_NONCE_T   nonce                           = LMS_ATTEST_NONCE;
    LMS_ATTEST_CHUNK_T   pubCache[LMS_ATTEST_CACHE_SIZE] = LMS_ATTEST_CACHE;
    LMS_ATTEST_SIG_T     lmsSig;
    QLIB_LMS_KEY_ID_T    wKeyId;
    ALG_LMS_T            lmsAlg = {0u};
    LMS_ATTEST_PUB_KEY_T lmsPubKey;

    /*-------------------------------------------------------------------------------------------------------
     Run sample code
    -------------------------------------------------------------------------------------------------------*/

#if QLIB_MAX_DIES_SUPPORTED > 1u
    // DIE 0 is used for supply chain protection
    QLIB_STATUS_RET_CHECK(QLIB_SetActiveDie(qlibContext, 0u));
#endif // QLIB_MAX_DIES_SUPPORTED > 1u

    /*-------------------------------------------------------------------------------------------------------
     Get the stored public key and public cache
    -------------------------------------------------------------------------------------------------------*/
    QLIB_SAMPLE_LmsAttest_retrievePub_L(lmsPubKey, pubCache);

    /*-------------------------------------------------------------------------------------------------------
     Sign using flash internal private key
    -------------------------------------------------------------------------------------------------------*/
    QLIB_STATUS_RET_CHECK(
        QLIB_LMS_Attest_Sign(qlibContext, msg_u8, sizeof(msg_u8), nonce, pubCache, LMS_ATTEST_CACHE_SIZE, lmsSig, wKeyId));

    /*-------------------------------------------------------------------------------------------------------
     Verify the signature itself
    -------------------------------------------------------------------------------------------------------*/
    {
        QLIB_ASSERT_RET(ALG_SUCCESS == lmsInit(&lmsAlg), QLIB_STATUS__COMMAND_FAIL);
        LMS_VERIFY_SIGNATURE_OP_T verifyParams = {lmsPubKey,
                                                  (uint32_t)sizeof(lmsPubKey),
                                                  msg_u8,
                                                  (uint32_t)sizeof(msg_u8),
                                                  lmsSig,
                                                  (uint32_t)sizeof(LMS_ATTEST_SIG_T)};
        QLIB_ASSERT_WITH_ERROR_GOTO(ALG_SUCCESS == lmsVerifySignature(&lmsAlg, &verifyParams),
                                    QLIB_STATUS__COMMAND_FAIL,
                                    status,
                                    lms_finit);
        (void)lmsFinit(&lmsAlg);
    }

    goto exit;

lms_finit:
    (void)lmsFinit(&lmsAlg);
    goto exit;

exit:
    return status;
}

/*-----------------------------------------------------------------------------------------------------------
                                         LOCAL FUNCTION PROTOTYPES
-----------------------------------------------------------------------------------------------------------*/

/************************************************************************************************************
 * @brief       This routine stores the public key and cache which are generated during manufacture stage
 *              for future use when chip verification is needed in the field flow
 *
 * @param[in]   lmsPubKey     public key to store.
 * @param[in]   pubCache      cache to store
************************************************************************************************************/
static void QLIB_SAMPLE_LmsAttest_storePub_L(LMS_ATTEST_PUB_KEY_T lmsPubKey, LMS_ATTEST_CHUNK_T* pubCache)
{
    (void)memcpy(pubCache_L, pubCache, sizeof(pubCache_L));
    (void)memcpy(lmsPubKey_L, lmsPubKey, sizeof(LMS_ATTEST_PUB_KEY_T));
}

/************************************************************************************************************
 * @brief       This routine retrieves the stored public key and cache which were generated during manufacture stage
 *              for verification in the field flow
 *
 * @param[out]   lmsPubKey     public key to retrieve.
 * @param[out]   pubCache      cache to retrieve
************************************************************************************************************/
static void QLIB_SAMPLE_LmsAttest_retrievePub_L(LMS_ATTEST_PUB_KEY_T lmsPubKey, LMS_ATTEST_CHUNK_T* pubCache)
{
    (void)memcpy(pubCache, pubCache_L, sizeof(pubCache_L));
    (void)memcpy(lmsPubKey, lmsPubKey_L, sizeof(LMS_ATTEST_PUB_KEY_T));
}
#endif //!defined(EXCLUDE_LMS_ATTESTATION)
