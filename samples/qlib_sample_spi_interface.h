/************************************************************************************************************
* @internal
* @remark     Winbond Electronics Corporation - Confidential
* @copyright  Copyright (c) 2024 by Winbond Electronics Corporation . All rights reserved
* @endinternal
*
* @file       qlib_sample_spi_interface.h
* @brief      This file contains QLIB sample code definitions for setting SPI interface
*
* ### project qlib_sample
*
************************************************************************************************************/

#ifndef QLIB_SAMPLE_SPI_INTERFACE_H__
#define QLIB_SAMPLE_SPI_INTERFACE_H__

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

/************************************************************************************************************
 * Callback to be implemented according to user platform
 * This callback is needed if user uses direct flash access (e.g when running with XIP)
 * Note that this function must be mapped to RAM section
************************************************************************************************************/
typedef int (*QLIB_SAMPLE_SetFetchCmd_CB)(QLIB_BUS_FORMAT_T busFormat, uint32_t dummyCycles_u32, bool addrMode3Bytes_b);
/************************************************************************************************************
 * @brief       This routine shows how to Set the SPI interface including initialization sequence
 *
 * @param[in]   userData_pv      Pointer to data associated with this qlib instance.
 * @param[in]   setFetchCmd   Pointer to platform specific implementation of callback to set SPI fetch command.
 *                            This function should be mapped to RAM section
 *
 * @return      0 if no error occurred, QLIB_STATUS__(ERROR) otherwise
************************************************************************************************************/
QLIB_STATUS_T QLIB_SAMPLE_SetSpiInterface(void* userData_pv, QLIB_SAMPLE_SetFetchCmd_CB setFetchCmd);

/************************************************************************************************************
 * @brief       This function demonstrates how to Set the SPI interface.
 *              This function assumes the QLIB library and flash device are already initialized and connected
 *
 * @param[out]  qlibContext      [QLIB internal state](md_definitions.html#DEF_CONTEXT)
 * @param[in]   setFetchCmd      Pointer to platform specific implementation of callback to set SPI fetch command
 *                               This function should be mapped to RAM section
 *
 * @return      QLIB_STATUS__OK if no error occurred, QLIB_STATUS__(ERROR) otherwise
************************************************************************************************************/
QLIB_STATUS_T QLIB_SAMPLE_SetSpiInterface_flow(QLIB_CONTEXT_T* qlibContext, QLIB_SAMPLE_SetFetchCmd_CB setFetchCmd);

/************************************************************************************************************
 * @brief       This function sets the SPI interface parameters in core, qlib and flash.
 *
 * @param[out]  qlibContext      [QLIB internal state](md_definitions.html#DEF_CONTEXT)
 * @param[in]   setFetchCmd      Pointer to platform specific implementation of callback to set SPI fetch command
 *                               This function should be mapped to RAM section
 *
 * @return      QLIB_STATUS__OK if no error occurred, QLIB_STATUS__(ERROR) otherwise
************************************************************************************************************/
QLIB_STATUS_T QLIB_SAMPLE_SetSPI(QLIB_CONTEXT_T*            qlibContext,
                                 QLIB_BUS_FORMAT_T          busFormat,
                                 uint32_t                   dummyCycles_u32,
                                 QLIB_STD_ADDR_MODE_T       addrMode,
                                 QLIB_SAMPLE_SetFetchCmd_CB setFetchCmd) RAM_SECTION;

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // QLIB_SAMPLE_SPI_INTERFACE_H__
