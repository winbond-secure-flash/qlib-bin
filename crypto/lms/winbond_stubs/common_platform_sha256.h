/************************************************************************************************************
*
* @remark     Winbond Electronics Corporation
* @copyright  Copyright (c) 2026 by Winbond Electronics Corporation . All rights reserved
*
*
* @file       common_platform_sha256.h
* @brief      This file includes SHA256 definitions
*
* ### project common_board_api
*
************************************************************************************************************/
#ifndef COMMON_PLATFORM_SHA256_H__
#define COMMON_PLATFORM_SHA256_H__

#ifdef __cplusplus
extern "C" {
#endif

/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
/*                                                INCLUDES                                                 */
/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
#include <stdint.h>
#include <stdbool.h>

/************************************************************************************************************
 * @brief       This function initialize HASH context\n
 *
 * @param[out]  ctx     Hash context
 * 
 * @return
 * 0                      - no error occurred\n
 * non-zero               - error occurred
 ************************************************************************************************************/
int PLAT_SHA256_Init(void** ctx);

/************************************************************************************************************
 * @brief       This function adds data to current HASH calculation.\n
 * This function can be called repeatedly with an arbitrary amount of data to be hashed.\n
 *
 * @param[in,out]  ctx        Hash context
 * @param[in]      data       Input data
 * @param[in]      dataSize   Input data size in bytes
 * 
 * @return
 * 0                      - no error occurred\n
 * non-zero               - error occurred
 ************************************************************************************************************/
int PLAT_SHA256_Update(void* ctx, const void* data, uint32_t dataSize);

/************************************************************************************************************
 * @brief       Finalize hashing and erases the context. 
 *
 * @param[in,out]  ctx        Hash context
 * @param[out]     output     digest
 * 
* @return
 * 0                      - no error occurred\n
 * non-zero               - error occurred
 ************************************************************************************************************/
int PLAT_SHA256_Finish(void* ctx, uint32_t* output);

/************************************************************************************************************
 * @brief Hash with SHA256
 *
 * @param[out]  output       Hash data
 * @param[in]   data         data buffer
 * @param[in]   dataSize     data buffer size
 * @param[in]   padValue     4 bytes padding after data value
 * @param[in]   padSize      padding after data size
 *
 * @return      0 if no error occurred
 ************************************************************************************************************/
int PLAT_SHA256WithPadding(uint32_t* output, const void* data, uint32_t dataSize, uint32_t padValue, uint32_t padSize);

#ifdef __cplusplus
}
#endif

#endif // COMMON_PLATFORM_SHA256_H__
