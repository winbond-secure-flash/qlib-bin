/************************************************************************************************************
* @internal
* @remark     Winbond Electronics Corporation - Confidential
* @copyright  Copyright (c) 2019 by Winbond Electronics Corporation . All rights reserved
* @endinternal
*
* @file       qlib_sample_wd.h
* @brief      This file contains QLIB WD related headers
*
* ### project qlib_samples
*
************************************************************************************************************/

#ifndef QLIB_SAMPLE_WD_H__
#define QLIB_SAMPLE_WD_H__

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

/************************************************************************************************************
 * @brief   This macro holds execution for @p mSec milliseconds.
 *          This is required for WD calibration, if not defined by the user WD calibration will fail.
 * @param   mSec   time to hold
************************************************************************************************************/
#ifndef QLIB_SAMPLE_DELAY_MSEC
#define QLIB_SAMPLE_DELAY_MSEC(mSec)
#endif // QLIB_SAMPLE_DELAY_MSEC

/************************************************************************************************************
 * @brief       This routine shows WD flow with initialization sequence
 *
 * @param[in]   userData_pv     Pointer to data associated with this qlib instance (NULL if no data required).
 *
 * @return      0 if no error occurred, QLIB_STATUS__(ERROR) otherwise
************************************************************************************************************/
QLIB_STATUS_T QLIB_SAMPLE_WatchDog(void* userData_pv);

/************************************************************************************************************
 * @brief       This routine shows example of using QLIB_Watchdog_ConfigSet function
 *
 * @param[out]  qlibContext     [QLIB internal state](md_definitions.html#DEF_CONTEXT)
 * @param[in]   enable_b        TRUE to enable WD, FALSE to disable
 * @param[in]   secure_b        TRUE if WD is secured, FALSE if non-secured
 * @param[in]   threshold       WD counter limit enumeration
 * @param[in]   section_u32     Number of section, its key is used for secured WD
 *
 * @return      0 if no error occurred, QLIB_STATUS__(ERROR) otherwise
************************************************************************************************************/
QLIB_STATUS_T QLIB_SAMPLE_WatchDogConfig(QLIB_CONTEXT_T* qlibContext,
                                         bool            enable_b,
                                         bool            secure_b,
                                         QLIB_AWDT_TH_T  threshold,
                                         uint32_t        section_u32);

/************************************************************************************************************
 * @brief       This routine shows an example of how to use QLIB_Watchdog_Touch function
 *
 * @param[out]  qlibContext     [QLIB internal state](md_definitions.html#DEF_CONTEXT)
 * @param[in]   secure_b        TRUE if WD is secured, FALSE if non-secured
 * @param[in]   section_u32     [Section index](md_definitions.html#DEF_SECTION). The section key is used for secured WD
 *
 * @return      0 if no error occurred, QLIB_STATUS__(ERROR) otherwise
************************************************************************************************************/
QLIB_STATUS_T QLIB_SAMPLE_WatchDogTouch(QLIB_CONTEXT_T* qlibContext, bool secure_b, uint32_t section_u32);

/************************************************************************************************************
 * @brief       This routine shows example of using QLIB_Watchdog_Get function
 *
 * @param[out]  qlibContext     [QLIB internal state](md_definitions.html#DEF_CONTEXT)
 * @param[in]   secure_b        TRUE if WD is secured, FALSE if non-secured
 * @param[in]   section_u32     [Section index](md_definitions.html#DEF_SECTION). The section key is used for secured WD
 * @param[out]  time_pu32       WD counter value in milliseconds
 * @param[out]  expired_pb      if TRUE WD is expired (in this case time value irrelevant)
 *
 * @return      0 if no error occurred, QLIB_STATUS__(ERROR) otherwise
************************************************************************************************************/
QLIB_STATUS_T QLIB_SAMPLE_WatchDogGet(QLIB_CONTEXT_T* qlibContext,
                                      bool            secure_b,
                                      uint32_t        section_u32,
                                      uint32_t*       time_pu32,
                                      bool*           expired_pb);

/************************************************************************************************************
* @brief      This function configures the default watchdog on flash
*             This function assumes the QLIB library and flash device are already initialized and connected
*
* @param[out]  ctx      [QLIB internal state](md_definitions.html#DEF_CONTEXT)
*
* @return      QLIB_STATUS__OK if no error occurred, QLIB_STATUS__(ERROR) otherwise
************************************************************************************************************/
QLIB_STATUS_T QLIB_SAMPLE_WatchDog_manuf_flow(QLIB_CONTEXT_T* qlibContext);

/************************************************************************************************************
* @brief      This function shows how to enable and use the watchdog.
*             This function assumes the QLIB library and flash device are already initialized and connected
*
* @param[out]  ctx      [QLIB internal state](md_definitions.html#DEF_CONTEXT)
*
* @return      QLIB_STATUS__OK if no error occurred, QLIB_STATUS__(ERROR) otherwise
************************************************************************************************************/
QLIB_STATUS_T QLIB_SAMPLE_WatchDog_field_flow(QLIB_CONTEXT_T* qlibContext);


#ifdef __cplusplus
}
#endif // __cplusplus

#endif // QLIB_SAMPLE_WD_H
