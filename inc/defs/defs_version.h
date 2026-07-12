/*---------------------------------------------------------------------------------------------------------*/
/*  Winbond Electronics Corporation                                                                        */
/*                                                                                                         */
/*  Copyright (c) 2019 by Winbond Electronics Corporation                                                  */
/*  All rights reserved                                                                                    */
/*                                                                                                         */
/*<<<------------------------------------------------------------------------------------------------------*/
/* File Contents:                                                                                          */
/*   defs_version.h                                                                                        */
/*            This file contains the version number of defs                                                */
/*  Project:                                                                                               */
/*            defs                                                                                         */
/*---------------------------------------------------------------------------------------------------------*/
#ifndef DEFS_VERSION_H__
#define DEFS_VERSION_H__

#define DEFS_VERSION_MAJOR    2
#define DEFS_VERSION_MINOR    4
#define DEFS_VERSION_PATCH    2
#define DEFS_VERSION_INTERNAL 0

#define DEFS_VERSION_NUM MAKE32B(DEFS_VERSION_INTERNAL, DEFS_VERSION_PATCH, DEFS_VERSION_MINOR, DEFS_VERSION_MAJOR)
#define DEFS_VERSION_STR \
    STRINGX(DEFS_VERSION_MAJOR) "." STRINGX(DEFS_VERSION_MINOR) "." STRINGX(DEFS_VERSION_PATCH) "." STRINGX(DEFS_VERSION_INTERNAL)

#endif // DEFS_VERSION_H__
