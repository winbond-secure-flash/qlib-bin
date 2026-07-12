/*---------------------------------------------------------------------------------------------------------*/
/*  Winbond Electronics Corporation                                                                        */
/*                                                                                                         */
/*  Copyright (c) 2019 by Winbond Electronics Corporation                                                  */
/*  All rights reserved                                                                                    */
/*                                                                                                         */
/*<<<------------------------------------------------------------------------------------------------------*/
/* File Contents:                                                                                          */
/*   defs_os_linux_app.h                                                                                   */
/*            This file contains definitions for Linux OS Applications                                     */
/*  Project:                                                                                               */
/*            SWC DEFS                                                                                     */
/*---------------------------------------------------------------------------------------------------------*/

#ifndef DEFS_LINUX_APP_H_
#define DEFS_LINUX_APP_H_

/*---------------------------------------------------------------------------------------------------------*/
/* OS dependent PTR definitions                                                                            */
/*---------------------------------------------------------------------------------------------------------*/
#define PTR8  (volatile uint8_t*)
#define PTR16 (volatile uint16_t*)
#define PTR32 (volatile uint32_t*)

/*---------------------------------------------------------------------------------------------------------*/
/* Variable packing                                                                                        */
/*---------------------------------------------------------------------------------------------------------*/
#ifndef PACK_
#define PACK_(decl) decl __attribute__((__packed__))
#endif

#endif // DEFS_LINUX_APP_H_
