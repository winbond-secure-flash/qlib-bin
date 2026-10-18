/************************************************************************************************************
* @internal
* @remark     Winbond Electronics Corporation - Confidential
* @copyright  Copyright (c) 2026 by Winbond Electronics Corporation . All rights reserved
* @endinternal
*
* @file       qlib_sample_kd1_config.h
* @brief      This file contains QLIB sample code for provisioning the device using a KD1-derived
*             pre-provisioned master key
*
* ### project qlib_samples
*
************************************************************************************************************/

#ifndef QLIB_SAMPLE_KD1_CONFIG_H__
#define QLIB_SAMPLE_KD1_CONFIG_H__

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

/*-----------------------------------------------------------------------------------------------------------
                                                   INCLUDES
-----------------------------------------------------------------------------------------------------------*/
#include "qlib.h"

/*-----------------------------------------------------------------------------------------------------------
                                                  FUNCTIONS
-----------------------------------------------------------------------------------------------------------*/

/************************************************************************************************************
 * @brief       End-to-end sample flow to calculate the KD1 pre-provisioned master key and use it to configure the device
 *
 *              The function performs the standard QLIB init sequence and delegates the actual KD1 provisioning
 *              flow to @ref QLIB_SAMPLE_KD1Config_provision.
 *
 * @param[in]   userData_pv     Pointer to data associated with this qlib instance
 *
 * @return      QLIB_STATUS__OK on success, QLIB_STATUS__(ERROR) otherwise
************************************************************************************************************/
QLIB_STATUS_T QLIB_SAMPLE_KD1Config(void* userData_pv);

/************************************************************************************************************
 * @brief       Provisions the device using a KD1-derived pre-provisioned master key
 *
 *              Steps:
 *                - Derive KD1 from WID of die 0.
 *                - Configure the device using the calculated KD1:  a new deviceMasterKey, and configure a secure section.
 *                - Open a secure session to the section and verify secure access succeeds.
 *
 * @param[in]   ctx     QLIB context, already initialized and connected to the device
 *
 * @return      QLIB_STATUS__OK on success, QLIB_STATUS__(ERROR) otherwise
************************************************************************************************************/
QLIB_STATUS_T QLIB_SAMPLE_KD1Config_provision(QLIB_CONTEXT_T* ctx);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // QLIB_SAMPLE_KD1_CONFIG_H__
