/************************************************************************************************************
* @internal
* @remark     Winbond Electronics Corporation - Confidential
* @copyright  Copyright (c) 2020 by Winbond Electronics Corporation . All rights reserved
* @endinternal
*
* @file       qlib_sample_secure_storage.h
* @brief      This file contains QLIB secure storage sample definitions
*
* ### project qlib_sample
*
************************************************************************************************************/

#ifndef QLIB_SAMPLE_SECURE_STORAGE_H__
#define QLIB_SAMPLE_SECURE_STORAGE_H__

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

/************************************************************************************************************
 * @brief       This function demonstrates secure storage functionality with initialization sequence
 *              It shows read/write/erase commands to sections with different access privileges.
 *
 * @param[in]   userData_pv     Pointer to data associated with this qlib instance.
 *
 * @return      QLIB_STATUS__OK if no error occurred, QLIB_STATUS__(ERROR) otherwise
************************************************************************************************************/
QLIB_STATUS_T QLIB_SAMPLE_SecureStorage(void* userData_pv);

#ifndef QLIB_ONLY_SECURE_STORAGE
/************************************************************************************************************
* @brief      This function configures several sections in the flash, with different access privileges.
*             This function assumes the QLIB library and flash device are already initialized and user is connected
*
* @param[out]  ctx      [QLIB internal state](md_definitions.html#DEF_CONTEXT)
*
* @return      QLIB_STATUS__OK if no error occurred, QLIB_STATUS__(ERROR) otherwise
************************************************************************************************************/
QLIB_STATUS_T QLIB_SAMPLE_SecureStorage_manuf_flow(QLIB_CONTEXT_T* ctx);
#endif // QLIB_ONLY_SECURE_STORAGE

/************************************************************************************************************
* @brief      This function shows storage commands to sections with different access privileges.
*             This function assumes the QLIB library and flash device are already initialized and user is connected
*
* @param[out]  ctx      [QLIB internal state](md_definitions.html#DEF_CONTEXT)
*
* @return      QLIB_STATUS__OK if no error occurred, QLIB_STATUS__(ERROR) otherwise
************************************************************************************************************/
QLIB_STATUS_T QLIB_SAMPLE_SecureStorage_field_flow(QLIB_CONTEXT_T* ctx);

#ifndef QLIB_ONLY_SECURE_STORAGE
/************************************************************************************************************
* @brief      This function shows storage commands to section with plain read and write access.
*             This function assumes the QLIB library and flash device are already initialized and user is connected
*
* @param[out]  ctx      [QLIB internal state](md_definitions.html#DEF_CONTEXT)
*
* @return      QLIB_STATUS__OK if no error occurred, QLIB_STATUS__(ERROR) otherwise
************************************************************************************************************/
QLIB_STATUS_T QLIB_SAMPLE_SecureStorage_field_flow_plainRW(QLIB_CONTEXT_T* ctx);

/************************************************************************************************************
* @brief      This function shows storage commands to section with plain read access and no plain write access.
*             This function assumes the QLIB library and flash device are already initialized and user is connected
*
* @param[out]  ctx      [QLIB internal state](md_definitions.html#DEF_CONTEXT)
*
* @return      QLIB_STATUS__OK if no error occurred, QLIB_STATUS__(ERROR) otherwise
************************************************************************************************************/
QLIB_STATUS_T QLIB_SAMPLE_SecureStorage_field_flow_plainR(QLIB_CONTEXT_T* ctx);

/************************************************************************************************************
* @brief      This function shows storage commands to section with authenticated read access.
*             This function assumes the QLIB library and flash device are already initialized and user is connected
*
* @param[out]  ctx      [QLIB internal state](md_definitions.html#DEF_CONTEXT)
*
* @return      QLIB_STATUS__OK if no error occurred, QLIB_STATUS__(ERROR) otherwise
************************************************************************************************************/
QLIB_STATUS_T QLIB_SAMPLE_SecureStorage_field_flow_authPlainR(QLIB_CONTEXT_T* ctx);
#endif // QLIB_ONLY_SECURE_STORAGE

/************************************************************************************************************
* @brief      This function shows storage commands to secure section.
*             This function assumes the QLIB library and flash device are already initialized and user is connected
*
* @param[out]  ctx      [QLIB internal state](md_definitions.html#DEF_CONTEXT)
*
* @return      QLIB_STATUS__OK if no error occurred, QLIB_STATUS__(ERROR) otherwise
************************************************************************************************************/
QLIB_STATUS_T QLIB_SAMPLE_SecureStorage_field_flow_secure(QLIB_CONTEXT_T* ctx);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // QLIB_SAMPLE_SECURE_STORAGE_H__
