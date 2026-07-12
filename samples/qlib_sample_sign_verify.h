/************************************************************************************************************
* @internal
* @remark     Winbond Electronics Corporation - Confidential
* @copyright  Copyright (c) 2022 by Winbond Electronics Corporation . All rights reserved
* @endinternal
*
* @file       qlib_sample_sign_verify.h
* @brief      This file contains QLIB data signing sample definitions
*
* ### project qlib_sample
*
************************************************************************************************************/

#ifndef QLIB_SAMPLE_SIGN_VERIFY_H__
#define QLIB_SAMPLE_SIGN_VERIFY_H__

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#ifdef QLIB_SIGN_DATA_BY_FLASH
/************************************************************************************************************
 * @brief       This routine shows data signing and verification with initialization sequence
 *
 * @param[in]   userData_pv     Pointer to data associated with this qlib instance.
 *
 * @return      0 if no error occurred, QLIB_STATUS__(ERROR) otherwise
************************************************************************************************************/
QLIB_STATUS_T QLIB_SAMPLE_SignVerify(void* userData_pv);

/************************************************************************************************************
 * @brief       This function configures a secure section for signing and verification.
 *              This function assumes the QLIB library and flash device are already initialized and connected
 *
 * @param[out]  qlibContext      [QLIB internal state](md_definitions.html#DEF_CONTEXT)
 *
 * @return      QLIB_STATUS__OK if no error occurred, QLIB_STATUS__(ERROR) otherwise
************************************************************************************************************/
QLIB_STATUS_T QLIB_SAMPLE_SignVerify_manuf_flow(QLIB_CONTEXT_T* qlibContext);

/************************************************************************************************************
 * @brief       This function demonstrates data signing and signature verification using given section keys.
 *              This function assumes the QLIB library and flash device are already initialized and connected
 *
 * @param[out]  qlibContext      [QLIB internal state](md_definitions.html#DEF_CONTEXT)
 *
 * @return      QLIB_STATUS__OK if no error occurred, QLIB_STATUS__(ERROR) otherwise
************************************************************************************************************/
QLIB_STATUS_T QLIB_SAMPLE_SignVerify_field_flow(QLIB_CONTEXT_T* qlibContext);

#endif // QLIB_SIGN_DATA_BY_FLASH

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // QLIB_SAMPLE_SIGN_VERIFY_H__
