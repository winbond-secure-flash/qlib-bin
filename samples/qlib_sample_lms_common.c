/************************************************************************************************************
* @internal
* @remark     Winbond Electronics Corporation - Confidential
* @copyright  Copyright (c) 2025 by Winbond Electronics Corporation . All rights reserved
* @endinternal
*
* @file       qlib_sample_lms_common.c
* @brief      This file contains QLIB LMS sample code
*
* @include    samples/qlib_sample_lms_common.h
*
************************************************************************************************************/

/*-----------------------------------------------------------------------------------------------------------
                                                   INCLUDES
-----------------------------------------------------------------------------------------------------------*/
#include "qlib.h"
#include "qlib_sample.h"

#if !defined(EXCLUDE_LMS) && !defined(LMS_SAMPLE_PREPARE_IN_ADVANCE) && !defined(VERIFY_ONLY)
#include "qlib_sample_lms_common.h"
#include "lms_api.h"

QLIB_STATUS_T QLIB_LMS_HsmGetKey(const uint8_t* keyId_pu8, LMS_PUB_KEY_T publicKey)
{
    ALG_LMS_T     lmsAlg;
    QLIB_STATUS_T status = QLIB_STATUS__OK;
#ifdef __cplusplus
    LMS_GET_PUB_KEY_OP_T getPubKeyParams(keyId_pu8, publicKey, sizeof(LMS_PUB_KEY_T), 0u);
#else
    LMS_GET_PUB_KEY_OP_T  getPubKeyParams = {keyId_pu8, publicKey, sizeof(LMS_PUB_KEY_T), 0u};
#endif // __cplusplus

    QLIB_ASSERT_RET(lmsInit(&lmsAlg) == ALG_SUCCESS, QLIB_STATUS__COMMAND_FAIL);
    QLIB_ASSERT_WITH_ERROR_GOTO(lmsGetPublicKey(&lmsAlg, &getPubKeyParams) == ALG_SUCCESS,
                                QLIB_STATUS__COMMAND_FAIL,
                                status,
                                lms_finit);
    QLIB_ASSERT_WITH_ERROR_GOTO(getPubKeyParams.publicKeyBytesReal == LMS_PUBLIC_KEY_SIZE(24u),
                                QLIB_STATUS__COMMAND_FAIL,
                                status,
                                lms_finit);
lms_finit:
    (void)lmsFinit(&lmsAlg);
    return status;
}

QLIB_STATUS_T QLIB_LMS_HsmGenerateKeyPair(uint8_t* keyId_pu8)
{
    ALG_LMS_T     lmsAlg;
    QLIB_STATUS_T status = QLIB_STATUS__OK;
#ifdef __cplusplus
    LMS_GEN_KEY_PAIR_OP_T genKeyParams(keyId_pu8, LMOTS_SHA256_N24_W4_PARAM_SET_ID, LMS_SHA256_M24_H20_PARAM_SET_ID);
#else
    LMS_GEN_KEY_PAIR_OP_T genKeyParams    = {keyId_pu8, LMOTS_SHA256_N24_W4_PARAM_SET_ID, LMS_SHA256_M24_H20_PARAM_SET_ID};
#endif // __cplusplus

    QLIB_ASSERT_RET(lmsInit(&lmsAlg) == ALG_SUCCESS, QLIB_STATUS__COMMAND_FAIL);

    QLIB_ASSERT_WITH_ERROR_GOTO(lmsGenerateKeyPair(&lmsAlg, &genKeyParams) == ALG_SUCCESS,
                                QLIB_STATUS__COMMAND_FAIL,
                                status,
                                lms_finit);

lms_finit:
    (void)lmsFinit(&lmsAlg);
    return status;
}

QLIB_STATUS_T QLIB_LMS_HsmSign(LMS_SIGN_MESSAGE_OP_T* signParams)
{
    ALG_LMS_T     lmsAlg;
    QLIB_STATUS_T status = QLIB_STATUS__OK;
    QLIB_ASSERT_RET(ALG_SUCCESS == lmsInit(&lmsAlg), QLIB_STATUS__COMMAND_FAIL);

    QLIB_ASSERT_WITH_ERROR_GOTO(lmsSignMessage(&lmsAlg, signParams) == ALG_SUCCESS, QLIB_STATUS__COMMAND_FAIL, status, lms_finit);
    QLIB_ASSERT_WITH_ERROR_GOTO(signParams->signatureBytesReal == QLIB_LMS_SIGNATURE_SIZE(QLIB_LMS_PARAM_M,
                                                                                          QLIB_LMS_PARAM_N,
                                                                                          QLIB_LMS_PARAM_P,
                                                                                          QLIB_LMS_TREE_HEIGHT),
                                QLIB_STATUS__COMMAND_FAIL,
                                status,
                                lms_finit);

lms_finit:
    (void)lmsFinit(&lmsAlg);
    return status;
}
#endif // !defined(EXCLUDE_LMS) && !defined(LMS_SAMPLE_PREPARE_IN_ADVANCE) && !defined(VERIFY_ONLY)
