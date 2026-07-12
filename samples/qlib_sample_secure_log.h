/************************************************************************************************************
* @internal
* @remark     Winbond Electronics Corporation - Confidential
* @copyright  Copyright (c) 2023 by Winbond Electronics Corporation . All rights reserved
* @endinternal
*
* @file       qlib_sample_secure_log.h
* @brief      This file contains QLIB secure log sample definitions
*
* ### project qlib_sample
*
************************************************************************************************************/

#ifndef QLIB_SAMPLE_SECURE_LOG_H__
#define QLIB_SAMPLE_SECURE_LOG_H__

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

/************************************************************************************************************
 * @brief       This function demonstrates secure log functionality with initialization sequence
 *              It shows read/write/erase commands to log section with different access privileges.
 *
 * @param[in]   userData_pv      Pointer to data associated with this qlib instance.
 *
 * @return      QLIB_STATUS__OK if no error occurred, QLIB_STATUS__(ERROR) otherwise
************************************************************************************************************/
QLIB_STATUS_T QLIB_SAMPLE_SecureLog(void* userData_pv);

/************************************************************************************************************
* @brief      This function configures several log sections in the flash, with different access privileges.
*             This function assumes the QLIB library and flash device are already initialized and user is connected
 *
 * @param[in]   ctx      [QLIB internal state](md_definitions.html#DEF_CONTEXT)
 *
 * @return      QLIB_STATUS__OK if no error occurred, QLIB_STATUS__(ERROR) otherwise
************************************************************************************************************/
QLIB_STATUS_T QLIB_SAMPLE_SecureLog_manuf_flow(QLIB_CONTEXT_T* ctx);

/************************************************************************************************************
* @brief      This function shows secure log commands to sections with different access privileges.
*             This function assumes the QLIB library and flash device are already initialized and user is connected
*
* @param[out]  qlibContext      [QLIB internal state](md_definitions.html#DEF_CONTEXT)
*
* @return      QLIB_STATUS__OK if no error occurred, QLIB_STATUS__(ERROR) otherwise
************************************************************************************************************/
QLIB_STATUS_T QLIB_SAMPLE_SecureLog_field_flow(QLIB_CONTEXT_T* ctx);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // QLIB_SAMPLE_SECURE_LOG_H__
