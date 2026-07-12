/************************************************************************************************************
* @internal
* @remark     Winbond Electronics Corporation - Confidential
* @copyright  Copyright (c) 2020 by Winbond Electronics Corporation . All rights reserved
* @endinternal
*
* @file       qlib_sample_direct_attestation.h
* @brief      This file contains QLIB direct attestation sample definitions
*
* ### project qlib_sample
*
************************************************************************************************************/

#ifndef QLIB_SAMPLE_DIRECT_ATTESTATION_H__
#define QLIB_SAMPLE_DIRECT_ATTESTATION_H__

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#define QLIB_SAMPLE_DIRECT_ATTEST_SECTION 1u

/************************************************************************************************************
 * @brief       This function demonstrates direct attestation flow on W77Q/T
 *              The sample demonstrates the "Generating Direct Root Of Trust (Direct Attestation)" secure flow
 *              described in W77Q AS v0.93 revA section from January 17,2021 (section 8.6.4)
 *              This function assumes the QLIB library and flash device are already initialized and connected
 *
 * @param[in]   qlibContext    [QLIB internal state](md_definitions.html#DEF_CONTEXT)
 *
 * @return      QLIB_STATUS__OK if no error occurred, QLIB_STATUS__(ERROR) otherwise
************************************************************************************************************/
QLIB_STATUS_T QLIB_SAMPLE_DirectAttestation_field_flow(QLIB_CONTEXT_T* qlibContext);

/************************************************************************************************************
* @brief      This function configures a section with digest integrity for direct attestation flow
*             This function assumes the QLIB library and flash device are already initialized and user is connected
*
* @param[out]  qlibContext      [QLIB internal state](md_definitions.html#DEF_CONTEXT)
*
* @return      QLIB_STATUS__OK if no error occurred, QLIB_STATUS__(ERROR) otherwise
************************************************************************************************************/
QLIB_STATUS_T QLIB_SAMPLE_DirectAttestation_manuf_flow(QLIB_CONTEXT_T* qlibContext);

/************************************************************************************************************
 * @brief       This function demonstrates direct attestation flow on W77Q/T with initialization and configuration flows
 *
 * @param[in]   userData_pv      Pointer to data associated with this qlib instance.
 *
 * @return      QLIB_STATUS__OK if no error occurred, QLIB_STATUS__(ERROR) otherwise
************************************************************************************************************/
QLIB_STATUS_T QLIB_SAMPLE_DirectAttestation(void* userData_pv);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // QLIB_SAMPLE_DIRECT_ATTESTATION_H__
