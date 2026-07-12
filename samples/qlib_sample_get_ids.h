/************************************************************************************************************
* @internal
* @remark     Winbond Electronics Corporation - Confidential
* @copyright  Copyright (c) 2022 by Winbond Electronics Corporation . All rights reserved
* @endinternal
*
* @file       qlib_sample_get_ids.h
* @brief      This file contains QLIB sample code definitions for getting Flash IDs
*
* ### project qlib_sample
*
************************************************************************************************************/

#ifndef QLIB_SAMPLE_GET_IDS_H__
#define QLIB_SAMPLE_GET_IDS_H__

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

/************************************************************************************************************
 * @brief       This routine shows how to fetch flash IDs including initialization sequence
 *              The Flash IDs are:
 *                 - UID : Flash Unique ID is a factory programmed ID that is unique to each W77Q device
 *                 - SUID : Secure Unique ID is programmed by the Device Controller in manufacturing stage.
 *                 - WID : Winbond ID is a factory programmed ID containing a device identifier.
 *
 *
 * @param[in]   userData_pv      Pointer to data associated with this qlib instance.
 *
 * @return      0 if no error occurred, QLIB_STATUS__(ERROR) otherwise
************************************************************************************************************/
QLIB_STATUS_T QLIB_SAMPLE_GetIDs(void* userData_pv);

/************************************************************************************************************
 * @brief       This function demonstrates how to fetch the flash IDs.
 *              The function assumes the QLIB library and flash device are already initialized and user is connected
 *
 * @param[out]  qlibContext      [QLIB internal state](md_definitions.html#DEF_CONTEXT)
 *
 * @return      QLIB_STATUS__OK if no error occurred, QLIB_STATUS__(ERROR) otherwise
************************************************************************************************************/
QLIB_STATUS_T QLIB_SAMPLE_GetIDs_flow(QLIB_CONTEXT_T* qlibContext);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // QLIB_SAMPLE_GET_IDS_H__
