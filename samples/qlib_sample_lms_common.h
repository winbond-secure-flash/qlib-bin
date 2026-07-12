/************************************************************************************************************
* @internal
* @remark     Winbond Electronics Corporation - Confidential
* @copyright  Copyright (c) 2025 by Winbond Electronics Corporation . All rights reserved
* @endinternal
*
* @file       qlib_sample_lms_common.h
* @brief      This file contains QLIB LMS common sample definitions
*
* ### project qlib_sample
*
************************************************************************************************************/

#ifndef QLIB_SAMPLE_LMS_COMMON_H__
#define QLIB_SAMPLE_LMS_COMMON_H__

#if !defined(EXCLUDE_LMS) && !defined(LMS_SAMPLE_PREPARE_IN_ADVANCE) && !defined(VERIFY_ONLY)

#include "qlib.h"
#include "lms_api.h"

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

/************************************************************************************************************
 * @brief       return the public key associated with the key id
 *
 * @param[in]     keyId_pu8
 * @param[out]    publicKey
 *
 * @return      QLIB_STATUS__OK on success or QLIB_STATUS__[ERROR] otherwise
************************************************************************************************************/
QLIB_STATUS_T QLIB_LMS_HsmGetKey(const uint8_t* keyId_pu8, LMS_PUB_KEY_T publicKey);

/************************************************************************************************************
 * @brief       generate key pair and return the key id
 *
 * @param[out] keyId_pu8 associated with the created key pair
 *
 * @return      QLIB_STATUS__OK on success or QLIB_STATUS__[ERROR] otherwise
************************************************************************************************************/
QLIB_STATUS_T QLIB_LMS_HsmGenerateKeyPair(uint8_t* keyId_pu8);

/************************************************************************************************************
 * @brief       sign a message
 *
 * @param[in,out] signParams signature parameters
 *
 * @return      QLIB_STATUS__OK on success or QLIB_STATUS__[ERROR] otherwise
************************************************************************************************************/
QLIB_STATUS_T QLIB_LMS_HsmSign(LMS_SIGN_MESSAGE_OP_T* signParams);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // !defined(EXCLUDE_LMS) && !defined(LMS_SAMPLE_PREPARE_IN_ADVANCE) && !defined(VERIFY_ONLY)
#endif // QLIB_SAMPLE_LMS_COMMON_H__
