/************************************************************************************************************
* @internal
* @remark     Winbond Electronics Corporation - Confidential
* @copyright  Copyright (c) 2026 by Winbond Electronics Corporation . All rights reserved
* @endinternal
*
* @file       qlib_sample_oem_section_provisioning.h
* @brief      This file contains QLIB sample code definitions for SoC to OEM section provisioning sample
*
* ### project qlib_sample
*
************************************************************************************************************/

#ifndef QLIB_SAMPLE_OEM_SECTION_PROVISIONING_H__
#define QLIB_SAMPLE_OEM_SECTION_PROVISIONING_H__

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#ifndef QLIB_ONLY_SECURE_STORAGE
#ifdef OEM_SECTION_PROVISIONING

/************************************************************************************************************
 * @brief       This routine demonstrates the full SoC to OEM section key provisioning flow.
 * Note: this sample locks the device configuration. After running this sample and before any re-configuration,
 * will require format
 *
 * @param[in]   userData_pv     Pointer to data associated with this qlib instance.
 *
 * @return
 * QLIB_STATUS__OK                    - no error\n
 * QLIB_STATUS__INVALID_PARAMETER     - @p userData_pv context is invalid\n
 * QLIB_STATUS__(ERROR)               - Other error
************************************************************************************************************/
QLIB_STATUS_T QLIB_SAMPLE_SoC_OEM_SectionProvision(void* userData_pv);

/************************************************************************************************************
 * @brief       SoC manufacture flow: configures sections sizes, master key, and derives the
 *              provisioning key for secure transfer to the OEM.
 * Note: this flow locks the device configuration. After running it, a re-configuration will require formatting the device.
 *
 * @param[in]   qlibContext              [QLIB internal state](md_definitions.html#DEF_CONTEXT)
 * @param[out]  oemSectionId_pu32        OEM section ID for which the provisioning key was derived
 * @param[out]  sectionProvisionKey      Derived OEM section provisioning key to be handed off to the OEM
 *
 * @return
 * QLIB_STATUS__OK                    - no error\n
 * QLIB_STATUS__INVALID_PARAMETER     - @p qlibContext, @p sectionProvisionKey, or @p oemSectionId_pu32 is NULL\n
 * QLIB_STATUS__(ERROR)               - Other error
************************************************************************************************************/
QLIB_STATUS_T QLIB_SAMPLE_SOC_flow(QLIB_CONTEXT_T* qlibContext, uint32_t* oemSectionId_pu32, KEY_T sectionProvisionKey);

/************************************************************************************************************
 * @brief       OEM provisioning flow: uses the section provisioning key received from the SoC to set
 *              section keys and policy, then opens a full session and verifies DEVCFG.CFG_LOCK is locked.
 *
 * @param[in]   qlibContext          [QLIB internal state](md_definitions.html#DEF_CONTEXT)
 * @param[in]   sectionId_u32        Section ID to provision
 * @param[in]   sectionProvisionKey  Section provisioning key received from the SoC
 *
 * @return
 * QLIB_STATUS__OK                          - no error\n
 * QLIB_STATUS__INVALID_PARAMETER           - @p qlibContext or @p sectionProvisionKey is NULL\n
 * QLIB_STATUS__SYSTEM_IN_INCORRECT_STATE   - DEVCFG.CFG_LOCK is not locked\n
 * QLIB_STATUS__(ERROR)                     - Other error
************************************************************************************************************/
QLIB_STATUS_T QLIB_SAMPLE_OEM_flow(QLIB_CONTEXT_T* qlibContext, uint32_t sectionId_u32, const KEY_T sectionProvisionKey);

#endif // OEM_SECTION_PROVISIONING
#endif // QLIB_ONLY_SECURE_STORAGE

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // QLIB_SAMPLE_OEM_SECTION_PROVISIONING_H__
