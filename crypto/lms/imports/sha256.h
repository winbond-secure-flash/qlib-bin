/************************************************************************************************************
* @internal
* @remark     Winbond Electronics Corporation - Confidential
* @copyright  Copyright (c) 2025 by Winbond Electronics Corporation . All rights reserved
* @endinternal
*
* @file       sha256.h
* @brief      This file includes platform specific sha256
*
* ### project lms
*
************************************************************************************************************/
#ifndef __LMS_SHA256_H__
#define __LMS_SHA256_H__

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

/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
/*                                       PLATFORM SPECIFIC FUNCTIONS                                       */
/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
/************************************************************************************************************
 * @brief       This function initialize HASH context\n
 *
 * @param[out]  ctx     Hash context
 * 
 * @return
 * 0                      - no error occurred\n
 * non-zero               - error occurred
 ************************************************************************************************************/
int LMS_SHA256_Init(void** ctx);

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
int LMS_SHA256_Update(void* ctx, const void* data, uint32_t dataSize);

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
int LMS_SHA256_Finish(void* ctx, uint32_t* output);

#ifdef __cplusplus
}
#endif

#endif // __LMS_SHA256_H__
