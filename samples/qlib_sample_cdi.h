/************************************************************************************************************
* @internal
* @remark     Winbond Electronics Corporation - Confidential
* @copyright  Copyright (c) 2020 by Winbond Electronics Corporation . All rights reserved
* @endinternal
*
* @file       qlib_sample_cdi.h
* @brief      This file contains QLIB cdi sample definitions
*
* ### project qlib_sample
*
************************************************************************************************************/

#ifndef QLIB_SAMPLE_CDI_H__
#define QLIB_SAMPLE_CDI_H__

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

/*-----------------------------------------------------------------------------------------------------------
                                                   DEFINES
-----------------------------------------------------------------------------------------------------------*/
#define QLIB_SAMPLE_CDI_BOOT_SECTION 0u
#define QLIB_SAMPLE_CDI_NUM_SECTIONS 3u

/************************************************************************************************************
 * @brief       This function demonstrates cdi value retrieval through the boot chain consisted of 3 boot stages.
 *              The function run also the initialization sequence.
 *              The sample demonstrates the "Extending Chain Of Trust" secure flow described in W77Q AS v0.93
 *              revA section from January 17,2021 (section 8.6.3)
 *
 * @param[in]   userData_pv     Pointer to data associated with this qlib instance.
 * @param[in]   runFieldFlow_b  if true, the field flow is run (assuming flash is configured with sections 0,1,2).
                                if false, the manufacture (configuration) flow is run to configure the flash
                                Note, that after flash configuration a HW reset to flash must occur before running the CDI field flow
 *
 * @return      QLIB_STATUS__OK if no error occurred, QLIB_STATUS__(ERROR) otherwise
************************************************************************************************************/
QLIB_STATUS_T QLIB_SAMPLE_DiceAttestation(void* userData_pv, bool runFieldFlow_b);

/************************************************************************************************************
* @brief      This function configures 3 sections on flash with digest for the demonstration of cdi value retrieval
              through the boot chain consisted of 3 boot stages.
*             This function assumes the QLIB library and flash device are already initialized and connected
*
* @param[out]  ctx      [QLIB internal state](md_definitions.html#DEF_CONTEXT)
*
* @return      QLIB_STATUS__OK if no error occurred, QLIB_STATUS__(ERROR) otherwise
************************************************************************************************************/
QLIB_STATUS_T QLIB_SAMPLE_DiceAttestation_manuf_flow(QLIB_CONTEXT_T* ctx);

/************************************************************************************************************
* @brief      This function demonstrates cdi value retrieval through the boot chain consisted of 3 boot stages.
*             This function assumes the QLIB library and flash device are already initialized and connected
*
* @param[out]  ctx      [QLIB internal state](md_definitions.html#DEF_CONTEXT)
*
* @return      QLIB_STATUS__OK if no error occurred, QLIB_STATUS__(ERROR) otherwise
************************************************************************************************************/
QLIB_STATUS_T QLIB_SAMPLE_DiceAttestation_field_flow(QLIB_CONTEXT_T* ctx);

/************************************************************************************************************
* @brief      This function shows how CDI value is retrieved.
*             This function assumes the QLIB library and flash device are already initialized.
*             This sample demonstrates the "Generating Root Of Trust" secure flow described in W77Q AS v0.93
*             revA section from January 17,2021 (section 8.6.2)
*
* @param[out]  qlibContext          [QLIB internal state](md_definitions.html#DEF_CONTEXT)
* @param[out]  nextCdi              CDI value produced by this module
* @param[in]   prevCdi              CDI value obtained from previous module
* @param[in]   sectionId_u32        section number CDI associated with
* @param[in]   sectionFullAccessKey section full access key
*
* @return      QLIB_STATUS__OK if no error occurred, QLIB_STATUS__(ERROR) otherwise
************************************************************************************************************/
QLIB_STATUS_T QLIB_SAMPLE_GetCdi(QLIB_CONTEXT_T* qlibContext,
                                 TYPE_256BIT     nextCdi,
                                 TYPE_256BIT     prevCdi,
                                 uint32_t        sectionId_u32,
                                 const KEY_T     sectionFullAccessKey);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // QLIB_SAMPLE_CDI_H__
