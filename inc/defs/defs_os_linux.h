/*---------------------------------------------------------------------------------------------------------*/
/*  Winbond Electronics Corporation                                                                        */
/*                                                                                                         */
/*  Copyright (c) 2019 by Winbond Electronics Corporation                                                  */
/*  All rights reserved                                                                                    */
/*                                                                                                         */
/*<<<------------------------------------------------------------------------------------------------------*/
/* File Contents:                                                                                          */
/*   defs_os_linux.h                                                                                       */
/*            This file contains definitions for Linux kernel API                                          */
/*  Project:                                                                                               */
/*            SWC DEFS                                                                                     */
/*---------------------------------------------------------------------------------------------------------*/

#ifndef DEFS_LINUX_KERNEL_H_
#define DEFS_LINUX_KERNEL_H_

#include <asm/io.h>

/*---------------------------------------------------------------------------------------------------------*/
/* OS dependent PTR definitions                                                                            */
/*---------------------------------------------------------------------------------------------------------*/
#define PTR8  (volatile uint8_t*)
#define PTR16 (volatile uint16_t*)
#define PTR32 (volatile uint32_t*)

/*---------------------------------------------------------------------------------------------------------*/
/* OS dependent MEM definitions                                                                            */
/*---------------------------------------------------------------------------------------------------------*/
#ifndef MEMR8
#define MEMR8(a) ioread8(a)
#endif
#ifndef MEMR16
#define MEMR16(a) ioread16(a)
#endif
#ifndef MEMR32
#define MEMR32(a) ioread32(a)
#endif

#ifndef MEMW8
#define MEMW8(a, v) iowrite8((v), (a))
#endif
#ifndef MEMW16
#define MEMW16(a, v) iowrite16((v), (a))
#endif
#ifndef MEMW32
#define MEMW32(a, v) iowrite32((v), (a))
#endif

/*---------------------------------------------------------------------------------------------------------*/
/* OS dependent IO definitions                                                                             */
/*---------------------------------------------------------------------------------------------------------*/
#ifndef IOR8
#define IOR8(a) inb(a)
#endif
#ifndef IOR16
#define IOR16(a) inw(a)
#endif
#ifndef IOR32
#define IOR32(a) inl(a)
#endif

#ifndef IOW8
#define IOW8(a, v) outb((v), (a))
#endif
#ifndef IOW16
#define IOW16(a, v) outw((v), (a))
#endif
#ifndef IOW32
#define IOW32(a, v) outl((v), (a))
#endif

/*---------------------------------------------------------------------------------------------------------*/
/* Variable packing                                                                                        */
/*---------------------------------------------------------------------------------------------------------*/
#ifndef PACK_
#define PACK_(decl) decl __attribute__((__packed__))
#endif

/*---------------------------------------------------------------------------------------------------------*/
/* Interrupts macros                                                                                       */
/*---------------------------------------------------------------------------------------------------------*/

/* Not Supported on ALL architectures                                                                      */
#ifndef ENABLE_INTERRUPTS
#define ENABLE_INTERRUPTS() local_irq_enable()
#endif
#ifndef DISABLE_INTERRUPTS
#define DISABLE_INTERRUPTS() local_irq_disable()
#endif

/* Supported on ALL architectures                                                                          */
#ifndef INTERRUPTS_SAVE_DISABLE
#define INTERRUPTS_SAVE_DISABLE(var) local_irq_save(var)
#endif
#ifndef INTERRUPTS_RESTORE
#define INTERRUPTS_RESTORE(var) local_irq_restore(var)
#endif

/*---------------------------------------------------------------------------------------------------------*/
/* Cache macros                                                                                            */
/*---------------------------------------------------------------------------------------------------------*/
#define ICACHE_SAVE_DISABLE(var) var = 0
#define ICACHE_SAVE_ENABLE(var)  var = 0
#define ICACHE_RESTORE(var)

#endif // DEFS_LINUX_KERNEL_H_
