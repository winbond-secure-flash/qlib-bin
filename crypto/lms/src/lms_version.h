/************************************************************************************************************
* @internal
* @remark     Winbond Electronics Corporation - Confidential
* @copyright  Copyright (c) 2024 by Winbond Electronics Corporation . All rights reserved
* @endinternal
*
* @file       lms_version.h
* @brief      This file contains the version number of LMS
*
* ### project LMS
*
************************************************************************************************************/
#ifndef _LMS_VERSION_H_
#define _LMS_VERSION_H_

#ifdef __cplusplus
extern "C" {
#endif

#ifndef LMS_MAKE_32_BIT
/*---------------------------------------------------------------------------------------------------------*/
/* Generate 32bit from bytes                                                                               */
/*---------------------------------------------------------------------------------------------------------*/
#define LMS_MAKE_32_BIT(byte0, byte1, byte2, byte3) \
    ((((uint32_t)(byte0)) << 0u) | (((uint32_t)(byte1)) << 8u) | (((uint32_t)(byte2)) << 16u) | (((uint32_t)(byte3)) << 24u))
#endif

/*---------------------------------------------------------------------------------------------------------*/
/*                                              MAKE STRING                                                */
/*---------------------------------------------------------------------------------------------------------*/
#define _STRINGX(x) #x
#define STRINGX(x)  _STRINGX(x)

// clang-format off
#define LMS_VERSION_MAJOR     0
#define LMS_VERSION_MINOR     2
#define LMS_VERSION_PATCH     0
#define LMS_VERSION_INTERNAL  0

#define LMS_VERSION         LMS_MAKE_32_BIT(LMS_VERSION_INTERNAL, LMS_VERSION_PATCH, LMS_VERSION_MINOR, LMS_VERSION_MAJOR)
#define LMS_VERSION_STR     STRINGX(LMS_VERSION_MAJOR) "." STRINGX(LMS_VERSION_MINOR) "." STRINGX(LMS_VERSION_PATCH) "." STRINGX(LMS_VERSION_INTERNAL)

#define LMS_API_VERSION_MAJOR     0
#define LMS_API_VERSION_MINOR     1
#define LMS_API_VERSION_PATCH     1
#define LMS_API_VERSION_INTERNAL  0

#define LMS_API_VERSION         LMS_MAKE_32_BIT(LMS_API_VERSION_INTERNAL, LMS_API_VERSION_PATCH, LMS_API_VERSION_MINOR, LMS_API_VERSION_MAJOR)
#define LMS_API_VERSION_STR     STRINGX(LMS_API_VERSION_MAJOR) "." STRINGX(LMS_API_VERSION_MINOR) "." STRINGX(LMS_API_VERSION_PATCH) "." STRINGX(LMS_API_VERSION_INTERNAL)
// clang-format on

#ifdef __cplusplus
}
#endif

#endif //_LMS_VERSION_H_
