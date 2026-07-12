/************************************************************************************************************
* @internal
* @remark     Winbond Electronics Corporation - Confidential
* @copyright  Copyright (c) 2023 by Winbond Electronics Corporation . All rights reserved
* @endinternal
*
* @file       qlib_sample_vault.h
* @brief      This file contains QLIB vault sample definitions
*
* ### project qlib_sample
*
************************************************************************************************************/

#ifndef QLIB_SAMPLE_VAULT_H__
#define QLIB_SAMPLE_VAULT_H__

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

/************************************************************************************************************
 * @brief       This function demonstrates vault functionality with initialization sequence
 *              It shows read/write/erase commands to vault section with different access privileges.
 *
 * @param[in]   userData_pv      Pointer to data associated with this qlib instance.
 *
 * @return      QLIB_STATUS__OK if no error occurred, QLIB_STATUS__(ERROR) otherwise
************************************************************************************************************/
QLIB_STATUS_T QLIB_SAMPLE_Vault(void* userData_pv);

/************************************************************************************************************
* @brief      This function configures the vault section in the flash.
*             This function assumes the QLIB library and flash device are already initialized and connected
*
* @param[out]  ctx      [QLIB internal state](md_definitions.html#DEF_CONTEXT)
*
* @return      QLIB_STATUS__OK if no error occurred, QLIB_STATUS__(ERROR) otherwise
************************************************************************************************************/
QLIB_STATUS_T QLIB_SAMPLE_Vault_manuf_flow(QLIB_CONTEXT_T* qlibContext);

/************************************************************************************************************
 * @brief       This function demonstrates vault functionality
 *              It shows read/write/erase commands to vault section with different access privileges.
 *              This function assumes the QLIB library and flash device are already initialized and connected
 *
 * @param[in]   qlibContext      [QLIB internal state](md_definitions.html#DEF_CONTEXT)
 *
 * @return      QLIB_STATUS__OK if no error occurred, QLIB_STATUS__(ERROR) otherwise
************************************************************************************************************/
QLIB_STATUS_T QLIB_SAMPLE_Vault_field_flow(QLIB_CONTEXT_T* qlibContext);

/************************************************************************************************************
* @brief      This function shows vault commands - secure commands allowed with full key, plain commands are not allowed.
*             This function assumes the QLIB library and flash device are already initialized and connected.
*
* @param[out]  qlibContext      [QLIB internal state](md_definitions.html#DEF_CONTEXT)
*
* @return      QLIB_STATUS__OK if no error occurred, QLIB_STATUS__(ERROR) otherwise
************************************************************************************************************/
QLIB_STATUS_T QLIB_SAMPLE_VaultFullKey(QLIB_CONTEXT_T* qlibContext);

/************************************************************************************************************
* @brief      This function shows vault commands - only read is allowed when using restricted key.
*             This function assumes the QLIB library and flash device are already initialized and connected.
*
* @param[out]  qlibContext      [QLIB internal state](md_definitions.html#DEF_CONTEXT)
*
* @return      QLIB_STATUS__OK if no error occurred, QLIB_STATUS__(ERROR) otherwise
************************************************************************************************************/
QLIB_STATUS_T QLIB_SAMPLE_VaultRestrictedKey(QLIB_CONTEXT_T* qlibContext);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // QLIB_SAMPLE_VAULT_H__
