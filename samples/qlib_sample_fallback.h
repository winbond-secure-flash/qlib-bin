/************************************************************************************************************
* @internal
* @remark     Winbond Electronics Corporation - Confidential
* @copyright  Copyright (c) 2019 by Winbond Electronics Corporation . All rights reserved
* @endinternal
*
* @file       qlib_sample_fallback.h
* @brief      This file contains QLIB fallback related headers
*
* ### project qlib_sample
*
************************************************************************************************************/

#ifndef QLIB_SAMPLE_FALLBACK_H__
#define QLIB_SAMPLE_FALLBACK_H__

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

/************************************************************************************************************
 * @brief       This routine shows secure fallback flow with initialization sequence
 *
 * @param[in]   userData_pv     Pointer to data associated with this qlib instance.
 *
 * @return      0 if no error occurred, QLIB_STATUS__(ERROR) otherwise
************************************************************************************************************/
QLIB_STATUS_T QLIB_SAMPLE_Fallback(void* userData_pv);

/************************************************************************************************************
 * @brief       This routine shows section configuration for secure fallback flow.
 *
 * @param[out]      qlibContext              [QLIB internal state](md_definitions.html#DEF_CONTEXT)
 * @param[in]       id               [QLIB device ID](md_definitions.html#DEF_ID)
 * @param[in]       die_u32          Die index
 * @param[in]       sect_u32         Section index
 * @param[in,out]   crc_pu32         If not NULL, section_u32 will be updated with CRC, and copied here
 * @param[in,out]   digest_pu64      If not NULL, section_u32 will be updated with digest, and copied here
 * @param[in]       isSymm_b         If true, section key is symmetric, otherwise asymmetric. This is needed to obtain
 *                                   the key from DB or calculate it according to formula
 *
 * @return      0 if no error occurred, QLIB_STATUS__(ERROR) otherwise
************************************************************************************************************/
QLIB_STATUS_T QLIB_SAMPLE_Fallback_config_section(QLIB_CONTEXT_T* qlibContext,
                                                  QLIB_ID_T*      id,
                                                  uint32_t        die_u32,
                                                  uint32_t        sect_u32,
                                                  uint32_t*       crc_pu32,
                                                  uint64_t*       digest_pu64);

/************************************************************************************************************
* @brief      This function configures a section for secure fallback flow.
*             This function assumes the QLIB library and flash device are already initialized and user is connected
*
* @param[out]  qlibContext      [QLIB internal state](md_definitions.html#DEF_CONTEXT)
*
* @return      QLIB_STATUS__OK if no error occurred, QLIB_STATUS__(ERROR) otherwise
************************************************************************************************************/
QLIB_STATUS_T QLIB_SAMPLE_Fallback_manuf_flow(QLIB_CONTEXT_T* qlibContext);

/************************************************************************************************************
 * @brief       This routine shows secure fallback flow. Assumes that device is initialized and Connected
 *
 * @param[out]  qlibContext             [QLIB internal state](md_definitions.html#DEF_CONTEXT)
 *
 * @return      0 if no error occurred, QLIB_STATUS__(ERROR) otherwise
************************************************************************************************************/
QLIB_STATUS_T QLIB_SAMPLE_Fallback_field_flow(QLIB_CONTEXT_T* qlibContext);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // QLIB_SAMPLE_FALLBACK_H__
