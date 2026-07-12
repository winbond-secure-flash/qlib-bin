/************************************************************************************************************
* @internal
* @remark     Winbond Electronics Corporation - Confidential
* @copyright  Copyright (c) 2026 by Winbond Electronics Corporation . All rights reserved
* @endinternal
*
* @file       qlib_sample_flash_handle_stubs.h
* @brief      Declarations of QLIB_SAMPLE_GetFlashHandle / QLIB_SAMPLE_ReleaseFlashHandle sample entry-point hooks
*
* ### project qlib_samples
*
************************************************************************************************************/

#ifndef QLIB_SAMPLE_FLASH_HANDLE_STUBS_H__
#define QLIB_SAMPLE_FLASH_HANDLE_STUBS_H__

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

/*-----------------------------------------------------------------------------------------------------------
                                            INTERFACE FUNCTIONS
-----------------------------------------------------------------------------------------------------------*/

/* ACTION NEEDED: Initialize your SPI interface (e.g. open /dev/spidev) and return a pointer to user data */
void* QLIB_SAMPLE_GetFlashHandle(void);

/* ACTION NEEDED: Clean up your SPI interface (e.g. close the spidev file descriptor) */
void QLIB_SAMPLE_ReleaseFlashHandle(void* userData_pv);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // QLIB_SAMPLE_FLASH_HANDLE_STUBS_H__
