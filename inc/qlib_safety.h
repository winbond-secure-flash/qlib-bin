/************************************************************************************************************
* @internal
* @remark     Winbond Electronics Corporation
* @copyright  Copyright (c) 2025 by Winbond Electronics Corporation . All rights reserved
* @endinternal
*
* @file       qlib_safety.h
* @brief      This file contains the Safe QLIB interface
*
* ### project qlib
*
************************************************************************************************************/
#ifndef QLIB_SAFETY_H__
#define QLIB_SAFETY_H__

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

/*-----------------------------------------------------------------------------------------------------------
                                                   INCLUDES
-----------------------------------------------------------------------------------------------------------*/
#include "qlib.h"

/*-----------------------------------------------------------------------------------------------------------
                                             INTERFACE FUNCTIONS
-----------------------------------------------------------------------------------------------------------*/

/************************************************************************************************************
 * @brief       This function performs QLIB library initialization and initializes the communication with the W77Q/T.

 * The initialized state is contained in @p qlibContext.
 * This initialization must be performed before usage of other safety QLIB functions.
 *
 * @param[out]  qlibContext            [QLIB internal state](md_definitions.html#DEF_CONTEXT)
 * @param[in]   busFormat              Flash interface format, consists of bus mode and dtr.
 * @param[in]   key                    Value of full access key for section 0, array of 4 uint32_t (total of 128 bits).\n
 *                                     NULL or zero value key is not supported.
 * @param[in]   qlibSafetyVersion_u32  QLIB safety version to verify. Must be the same as the current QLIB safety version. 0x00010000 -> 0.1.0.0
 *
 * @return
 * QLIB_STATUS__OK = 0                  - no error occurred\n
 * QLIB_STATUS__INVALID_PARAMETER       - @p qlibContext is NULL\n
 * QLIB_STATUS__INVALID_PARAMETER       - @p key is NULL or zero value key\n
 * QLIB_STATUS__INVALID_PARAMETER       - @p qlibSafetyVersion_u32 does not match the current QLIB safety version\n
 * QLIB_STATUS__DEVICE_SYSTEM_ERR       - One of the sections has wrong configuration (e.g rollback and size is not correct)\n
 * QLIB_STATUS__PARAMETER_OUT_OF_RANGE  - @p busFormat is @ref QLIB_BUS_MODE_4_4_4 and QLIB_SUPPORT_QPI is not defined\n
 * QLIB_STATUS__(ERROR)                 - Other error
************************************************************************************************************/
QLIB_API QLIB_STATUS_T QLIB_SAFETY_Init(QLIB_CONTEXT_T*   qlibContext,
                               QLIB_BUS_FORMAT_T          busFormat,
                               const KEY_T                key,
                               uint32_t                   qlibSafetyVersion_u32);

/************************************************************************************************************
 * @brief       This function deploys new security configuration to the flash, and  verifies the configuration.
 *
 * This function should be used on a clean formatted flash in case it configures the keys.
 * The flash is configured with a single section (section 0), with the full flash size.
 * The section is configured with full access key only, and without any plain access.
 * This function requires running QLIB_SAFETY_Init before usage.
 *
 * @param[out]  qlibContext                [QLIB internal state](md_definitions.html#DEF_CONTEXT)
 * @param[in]   masterKey                  Master key value, array of 4 uint32_t (total of 128 bits). NULL or zero value key is not supported.
 * @param[in]   sectionKey                 Section 0 key value, array of 4 uint32_t (total of 128 bits). NULL or zero value key is not supported.
 *
 * @return
 * QLIB_STATUS__OK = 0                    - no error occurred \n
 * QLIB_STATUS__INVALID_PARAMETER         - @p qlibContext is NULL \n
 * QLIB_STATUS__INVALID_PARAMETER         - masterKey is NULL or zero value \n
 * QLIB_STATUS__INVALID_PARAMETER         - sectionKey is NULL or zero value \n
 * QLIB_STATUS__SYSTEM_IN_INCORRECT_STATE - Flash device was not initialized. use @ref QLIB_InitDevice \n
 * QLIB_STATUS__(ERROR)                   - Other error
************************************************************************************************************/
QLIB_API QLIB_STATUS_T QLIB_SAFETY_ConfigDevice(QLIB_CONTEXT_T* qlibContext, KEY_T masterKey, KEY_T sectionKey);

/************************************************************************************************************
 * @brief       This function safely reads data from the flash
 *
 * The data is read into @p buf_pu8 from offset.\n
 * This function requires running QLIB_SAFETY_Init before usage.
 *
 * @param[out]  qlibContext   [QLIB internal state](md_definitions.html#DEF_CONTEXT)
 * @param[out]  buf_pu8       The data read
 * @param[in]   offset_u32    [Section offset](md_definitions.html#DEF_OFFSET)
 * @param[in]   size_u32      [Size](md_definitions.html#DEF_SIZE)
 *
 * @return
 * QLIB_STATUS__OK = 0                    - no error occurred\n
 * QLIB_STATUS__INVALID_PARAMETER         - @p qlibContext is NULL\n
 * QLIB_STATUS__INVALID_PARAMETER         - @p buf_pu8 is NULL\n
 * QLIB_STATUS__PARAMETER_OUT_OF_RANGE    - @p size_u32 == 0 in case of standard read only\n
 * QLIB_STATUS__PARAMETER_OUT_OF_RANGE    - @p offset_u32 + @p size_u32 > section size\n
 * QLIB_STATUS__SYSTEM_IN_INCORRECT_STATE - Flash device was not initialized. use @ref QLIB_InitDevice or @ref QLIB_ImportState \n
 * QLIB_STATUS__SYSTEM_IN_INCORRECT_STATE - Chip is not in working state\n
 * QLIB_STATUS__SYSTEM_IN_INCORRECT_STATE - Section is not enabled\n
 * QLIB_STATUS__(ERROR)                   - Other error
************************************************************************************************************/
QLIB_API QLIB_STATUS_T QLIB_SAFETY_Read(QLIB_CONTEXT_T* qlibContext, uint8_t* buf_pu8, uint32_t offset_u32, uint32_t size_u32);

/************************************************************************************************************
 * @brief       This function safely writes data to the flash
 *
 * The data is written from @p buf to offset.\n
 * This function requires running QLIB_SAFETY_Init before usage.
 *
 * @param[out]  qlibContext   [QLIB internal state](md_definitions.html#DEF_CONTEXT)
 * @param[in]   buf_pu8       The data to write
 * @param[in]   offset_u32    [Section offset](md_definitions.html#DEF_OFFSET)
 * @param[in]   size_u32      [Size](md_definitions.html#DEF_SIZE)
 *
 * @return
 * QLIB_STATUS__OK = 0                                              - no error occurred\n
 * QLIB_STATUS__INVALID_PARAMETER                                   - @p qlibContext is NULL\n
 * QLIB_STATUS__INVALID_PARAMETER                                   - @p buf_pu8 is NULL\n
 * QLIB_STATUS__PARAMETER_OUT_OF_RANGE                              - @p offset_u32 + @p size_u32 > section size\n
 * QLIB_STATUS__SYSTEM_IN_INCORRECT_STATE                           - Flash device was not initialized. use @ref QLIB_InitDevice or\n
 *                                                                    @ref QLIB_ImportState \n
 * QLIB_STATUS__SYSTEM_IN_INCORRECT_STATE                           - Chip is not in working state\n
 * QLIB_STATUS__SYSTEM_IN_INCORRECT_STATE                           - Section is not enabled\n
 * QLIB_STATUS__(ERROR)                                             - Other error
************************************************************************************************************/
QLIB_API QLIB_STATUS_T QLIB_SAFETY_Write(QLIB_CONTEXT_T* qlibContext, const uint8_t* buf_pu8, uint32_t offset_u32, uint32_t size_u32);

/************************************************************************************************************
 * @brief       This function safely erases the given memory range.
 *
 * The minimal memory range size is defined by QLIB_FLASH_SECTOR_SIZE.
 * This function requires running QLIB_SAFETY_Init before usage.
 *
 * @param[out]  qlibContext   [QLIB internal state](md_definitions.html#DEF_CONTEXT)
 * @param[in]   offset_u32    [Section offset](md_definitions.html#DEF_OFFSET). The offset must be aligned to QLIB_FLASH_SECTOR_SIZE.
 * @param[in]   size_u32      [Size](md_definitions.html#DEF_SIZE). The size must be aligned to QLIB_FLASH_SECTOR_SIZE.
 *
 * @return
 * QLIB_STATUS__OK = 0                    - no error occurred\n
 * QLIB_STATUS__INVALID_PARAMETER         - @p qlibContext is NULL\n
 * QLIB_STATUS__INVALID_DATA_ALIGNMENT    - @p offset_u32 is not aligned to sector size\n
 * QLIB_STATUS__INVALID_DATA_SIZE         - @p size_u32 is not aligned to sector size\n
 * QLIB_STATUS__SYSTEM_IN_INCORRECT_STATE - Flash device was not initialized. use @ref QLIB_InitDevice or @ref QLIB_ImportState \n
 * QLIB_STATUS__SYSTEM_IN_INCORRECT_STATE - Chip is not in working state\n
 * QLIB_STATUS__PARAMETER_OUT_OF_RANGE    - @p offset_u32 + @p size_u32 > section size\n
 * QLIB_STATUS__(ERROR)                   - Other error
************************************************************************************************************/
QLIB_API QLIB_STATUS_T QLIB_SAFETY_Erase(QLIB_CONTEXT_T* qlibContext, uint32_t offset_u32, uint32_t size_u32);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // QLIB_SAFETY_H__
