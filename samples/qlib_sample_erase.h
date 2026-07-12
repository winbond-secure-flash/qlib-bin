/************************************************************************************************************
* @internal
* @remark     Winbond Electronics Corporation - Confidential
* @copyright  Copyright (c) 2026 by Winbond Electronics Corporation . All rights reserved
* @endinternal
*
* @file       qlib_sample_erase.h
* @brief      This file contains QLIB erase sample definitions
*
* ### project qlib_sample
*
************************************************************************************************************/

#ifndef QLIB_SAMPLE_ERASE_H__
#define QLIB_SAMPLE_ERASE_H__

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

/************************************************************************************************************
 * @brief       This function demonstrates erase functionality with initialization sequence
 *              It shows erase commands to sections with different access privileges.
 *
 * @param[in]   userData_pv      Pointer to data associated with this qlib instance.
 *
 * @return      QLIB_STATUS__OK if no error occurred, QLIB_STATUS__(ERROR) otherwise
************************************************************************************************************/
QLIB_STATUS_T QLIB_SAMPLE_Erase(void* userData_pv);

/************************************************************************************************************
* @brief      This function configures several sections in the flash, with different access privileges.
*             This function assumes the QLIB library and flash device are already initialized and user is connected
*
* @param[out]  ctx      [QLIB internal state](md_definitions.html#DEF_CONTEXT)
*
* @return      QLIB_STATUS__OK if no error occurred, QLIB_STATUS__(ERROR) otherwise
************************************************************************************************************/
QLIB_STATUS_T QLIB_SAMPLE_Erase_manuf_flow(QLIB_CONTEXT_T* ctx);

/************************************************************************************************************
* @brief      This function shows erase commands to sections with different access privileges.
*             This function assumes the QLIB library and flash device are already initialized and user is connected
*
* @param[out]  ctx      [QLIB internal state](md_definitions.html#DEF_CONTEXT)
*
* @return      QLIB_STATUS__OK if no error occurred, QLIB_STATUS__(ERROR) otherwise
************************************************************************************************************/
QLIB_STATUS_T QLIB_SAMPLE_Erase_field_flow(QLIB_CONTEXT_T* ctx);

/************************************************************************************************************
* @brief      This function shows Erase command to section with plain write access.
*             This function assumes the QLIB library and flash device are already initialized and user is connected
*
* @param[out]  ctx      [QLIB internal state](md_definitions.html#DEF_CONTEXT)
*
* @return      QLIB_STATUS__OK if no error occurred, QLIB_STATUS__(ERROR) otherwise
************************************************************************************************************/
QLIB_STATUS_T QLIB_SAMPLE_Erase_field_flow_plainErase(QLIB_CONTEXT_T* ctx);

/************************************************************************************************************
* @brief      This function shows Erase command to secure section.
*             This function assumes the QLIB library and flash device are already initialized and user is connected
*
* @param[out]  ctx      [QLIB internal state](md_definitions.html#DEF_CONTEXT)
*
* @return      QLIB_STATUS__OK if no error occurred, QLIB_STATUS__(ERROR) otherwise
************************************************************************************************************/
QLIB_STATUS_T QLIB_SAMPLE_Erase_field_flow_secureErase(QLIB_CONTEXT_T* ctx);

/************************************************************************************************************
* @brief      This function shows Erase command to section with authenticated write access.
*             This function assumes the QLIB library and flash device are already initialized and user is connected
*
* @param[out]  ctx      [QLIB internal state](md_definitions.html#DEF_CONTEXT)
*
* @return      QLIB_STATUS__OK if no error occurred, QLIB_STATUS__(ERROR) otherwise
************************************************************************************************************/
QLIB_STATUS_T QLIB_SAMPLE_Erase_field_flow_authPlainErase(QLIB_CONTEXT_T* ctx);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // QLIB_SAMPLE_ERASE_H__
