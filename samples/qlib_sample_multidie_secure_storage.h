/************************************************************************************************************
* @internal
* @remark     Winbond Electronics Corporation - Confidential
* @copyright  Copyright (c) 2020 by Winbond Electronics Corporation . All rights reserved
* @endinternal
*
* @file       qlib_sample_multidie_secure_storage.h
* @brief      This file contains QLIB secure storage sample definitions
*
* ### project qlib_sample
*
************************************************************************************************************/

#ifndef QLIB_SAMPLE_MULTIDIE_SECURE_STORAGE_H__
#define QLIB_SAMPLE_MULTIDIE_SECURE_STORAGE_H__

#include "qlib_sample_spi_interface.h"

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

/************************************************************************************************************
 * @brief       This function demonstrates secure storage usage with initialization sequence.
 *              It shows Secure storage with multiDie flash.
 *
 * @param[in]   userData_pv      Pointer to data associated with this qlib instance.
 * @param[in]   setFetchCmd   Pointer to platform specific implementation of callback to set SPI fetch command
 *
 * @return      QLIB_STATUS__OK if no error occurred, QLIB_STATUS__(ERROR) otherwise
************************************************************************************************************/
QLIB_STATUS_T QLIB_SAMPLE_MultiDieSecureStorage(void* userData_pv, QLIB_SAMPLE_SetFetchCmd_CB setFetchCmd);

#ifndef QLIB_ONLY_SECURE_STORAGE
/************************************************************************************************************
* @brief      This function configures the flash for multidie secure storage sample flow.
*             This function assumes the QLIB library and flash device are already initialized and user is connected
*
* @param[out]  qlibContext      [QLIB internal state](md_definitions.html#DEF_CONTEXT)
*
* @return      QLIB_STATUS__OK if no error occurred, QLIB_STATUS__(ERROR) otherwise
************************************************************************************************************/
QLIB_STATUS_T QLIB_SAMPLE_MultiDieSecureStorage_manuf_flow(QLIB_CONTEXT_T* qlibContext);
#endif // QLIB_ONLY_SECURE_STORAGE

/************************************************************************************************************
 * @brief       This function demonstrates secure storage usage in Qlib.
 *              It shows Secure storage with multiDie flash.
 *              This function assumes the QLIB library and flash device are already initialized and user is connected
 *
 * @param[in]   qlibContext   [QLIB internal state](md_definitions.html#DEF_CONTEXT)
 * @param[in]   setFetchCmd   Pointer to platform specific implementation of callback to set SPI fetch command
 *
 * @return      QLIB_STATUS__OK if no error occurred, QLIB_STATUS__(ERROR) otherwise
************************************************************************************************************/
QLIB_STATUS_T QLIB_SAMPLE_MultiDieSecureStorage_field_flow(QLIB_CONTEXT_T* qlibContext, QLIB_SAMPLE_SetFetchCmd_CB setFetchCmd);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // QLIB_SAMPLE_MULTIDIE_SECURE_STORAGE_H__
