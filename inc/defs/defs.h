/*---------------------------------------------------------------------------------------------------------*/
/*  Winbond Electronics Corporation                                                                        */
/*                                                                                                         */
/*  Copyright (c) 2019 by Winbond Electronics Corporation                                                  */
/*  All rights reserved                                                                                    */
/*                                                                                                         */
/*<<<------------------------------------------------------------------------------------------------------*/
/* File Contents:                                                                                          */
/*    defs.h                                                                                               */
/*            This file contains Software Definitions Macros                                               */
/* Project:                                                                                                */
/*            SWC DEFS                                                                                     */
/*---------------------------------------------------------------------------------------------------------*/

#ifndef DEFS_H__
#define DEFS_H__

// platform specific optional include file
#ifdef DEFS_PLATFORM_HEADER
#include DEFS_PLATFORM_HEADER
#endif

#ifndef PRJ_UNIQUE_ERRORS
#include "defs_errors.h"
#endif // PRJ_UNIQUE_ERRORS
#include "defs_types.h"
#include "defs_access.h"
#include "defs_utils.h"
#include "defs_secure.h"
#include "defs_os_general.h"

#endif // DEFS_H__
