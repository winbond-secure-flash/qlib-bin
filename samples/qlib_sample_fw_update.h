/************************************************************************************************************
* @internal
* @remark     Winbond Electronics Corporation - Confidential
* @copyright  Copyright (c) 2019 by Winbond Electronics Corporation . All rights reserved
* @endinternal
*
* @file       qlib_sample_fw_update.h
* @brief      This file contains QLIB fw update related headers
*
* ### project qlib_sample
*
************************************************************************************************************/

#ifndef QLIB_SAMPLE_FW_UPDATE_H__
#define QLIB_SAMPLE_FW_UPDATE_H__

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

/*-----------------------------------------------------------------------------------------------------------
 should be aligned to the demo configuration in sample
-----------------------------------------------------------------------------------------------------------*/
#define QLIB_SAMPLE_SECTION_SIZE MIN(SIZE_1MB_, UINTPTR_MAX) // section to be updated is assumed to be _1M_

#define QLIB_SAMPLE_MAX_FW_SIZE (QLIB_SAMPLE_SECTION_SIZE / 2u) // two halves ( active and not active )

/************************************************************************************************************
 * @brief       This routine shows secure update section flow with initialization sequence
 *
 * @param[in]   userData_pv     Pointer to data associated with this qlib instance.
 *
 * @return      0 if no error occurred, QLIB_STATUS__(ERROR) otherwise
************************************************************************************************************/
QLIB_STATUS_T QLIB_SAMPLE_FwUpdate(void* userData_pv);

/************************************************************************************************************
 * @brief       This routine shows secure update section flow.  Assumes that device is Connected and
 *              proper key to corresponding section is loaded
 *
 * @param[out]      qlibContext              [QLIB internal state](md_definitions.html#DEF_CONTEXT)
 * @param[in]       buff_pu32                data buffer to be set in section_u32
 * @param[in]       buffSize_u32             data buffer size in bytes
 * @param[in]       section_u32              [Section index](md_definitions.html#DEF_SECTION)
 * @param[in,out]   digestIntegrity_pu64     If not NULL, section_u32 will be updated with digest, and copied here
 * @param[in,out]   checksumIntegrity_pu32   If not NULL, section_u32 will be updated with crc, and copied here
 * @param[in]       newVersion_pu32          Pointer to new version, if NULL no change will be set
 * @param[in]       swap_b                   Defines if need to swap_b partitions or swap_b and reset
 *
 * @return      0 if no error occurred, QLIB_STATUS__(ERROR) otherwise
************************************************************************************************************/
QLIB_STATUS_T QLIB_SAMPLE_SectionUpdate(QLIB_CONTEXT_T*            qlibContext,
                                        const uint8_t*             buff_pu8,
                                        uint32_t                   buffSize_u32,
                                        uint32_t                   section_u32,
                                        uint64_t*                  digestIntegrity_pu64,
                                        uint32_t*                  checksumIntegrity_pu32,
                                        uint32_t*                  newVersion_pu32,
                                        bool                       swap_b,
                                        QLIB_SECTION_CONF_ACTION_T action);

/************************************************************************************************************
* @brief      This function configures a section for secure update flow.
*             This function assumes the QLIB library and flash device are already initialized and user is connected
*
* @param[out]  qlibContext      [QLIB internal state](md_definitions.html#DEF_CONTEXT)
*
* @return      QLIB_STATUS__OK if no error occurred, QLIB_STATUS__(ERROR) otherwise
************************************************************************************************************/
QLIB_STATUS_T QLIB_SAMPLE_FirmwareUpdate_manuf_flow(QLIB_CONTEXT_T* qlibContext);

/************************************************************************************************************
 * @brief       This routine shows secure update firmware flow. Assumes that device is initialized and Connected
 *
 * @param[out]  qlibContext             [QLIB internal state](md_definitions.html#DEF_CONTEXT)
 *
 * @return      0 if no error occurred, QLIB_STATUS__(ERROR) otherwise
************************************************************************************************************/
QLIB_STATUS_T QLIB_SAMPLE_FirmwareUpdate_field_flow(QLIB_CONTEXT_T* qlibContext);

#ifndef QLIB_SUPPORT_XIP
/************************************************************************************************************
 * @brief       This routine gets secure update firmware flow parameters. It will calculate digest/crc if needed.
 *              Assumed that proper key to corresponding section_u32 is loaded. The user that knows the crc/digest
 *              for his fw will not use it
 *
 * @param[out]  qlibContext             [QLIB internal state](md_definitions.html#DEF_CONTEXT)
 * @param[in]   section_u32             [Section index](md_definitions.html#DEF_SECTION)
 * @param[in]   buff_pu32               FW buffer
 * @param[in]   buffSize_u32            FW buffer size in bytes
 * @param[out]  digestIntegrity_pu64    If not NULL, fw will be updated with digest, and copied here
 * @param[out]  checksumIntegrity_pu32  If not NULL, fw will be updated with crc, and copied here
 * @param[out]  oldVersion_pu32         If not NULL, current section version will be set here
 *
 * @return      0 if no error occurred, QLIB_STATUS__(ERROR) otherwise
************************************************************************************************************/
QLIB_STATUS_T QLIB_SAMPLE_GetUpdateSectionParams(QLIB_CONTEXT_T* qlibContext,
                                                 uint32_t        section_u32,
                                                 const uint32_t* buff_pu32,
                                                 uint32_t        buffSize_u32,
                                                 uint64_t*       digestIntegrity_pu64,
                                                 uint32_t*       checksumIntegrity_pu32,
                                                 uint32_t*       oldVersion_pu32);

#endif // QLIB_SUPPORT_XIP

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // QLIB_SAMPLE_FW_UPDATE_H__
