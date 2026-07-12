/************************************************************************************************************
* @internal
* @remark     Winbond Electronics Corporation - Confidential
* @copyright  Copyright (c) 2019 by Winbond Electronics Corporation . All rights reserved
* @endinternal
*
* @file       qlib_sample.h
* @brief      This file contains sample types definitions and checks
*
* ### project qlib
*
************************************************************************************************************/

#ifndef QLIB_SAMPLE_H__
#define QLIB_SAMPLE_H__

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

/*-----------------------------------------------------------------------------------------------------------
                                                   INCLUDES
-----------------------------------------------------------------------------------------------------------*/
#include "defs.h"
#include "qlib_types.h"
#include "qlib.h"

/*-----------------------------------------------------------------------------------------------------------
                                                 DEFINITIONS
-----------------------------------------------------------------------------------------------------------*/

#ifndef QLIB_SAMPLE_PRINTF
#define QLIB_SAMPLE_PRINTF(...) (void)0
#endif // QLIB_SAMPLE_PRINTF


#define QLIB_SAMPLE_ALLOW_TO_FAIL__START()
#define QLIB_SAMPLE_ALLOW_TO_FAIL__END()
#define QLIB_SAMPLE_ALLOW_TO_FAIL__VAL() (0)

#ifndef QLIB_SAMPLE_RESERVED_BOOT_SECTION_SIZE
#define QLIB_SAMPLE_RESERVED_BOOT_SECTION_SIZE \
    (SIZE_1MB_) // first 1MB are not compromised by samples, reserved for boot section_u32

#endif // QLIB_SAMPLE_RESERVED_BOOT_SECTION_SIZE

// Device Master key
#define QLIB_SAMPLE_KD                                 \
    {                                                  \
        0x11111111, 0x12121212, 0x13131313, 0x14141414 \
    }

// Device Secret Key
#define QLIB_SAMPLE_KDS                                \
    {                                                  \
        0x15151515, 0x16161616, 0x17171717, 0x18181818 \
    }

// Device Pre-Provisioned Master Key
// Should be updated according to the actual key in the flash
#ifndef QLIB_FPGA_TARGET
#define QLIB_SAMPLE_KD1                                \
    {                                                  \
        0x01010101, 0x02020202, 0x03030303, 0x04040404 \
    }
#else
#define QLIB_SAMPLE_KD1                                \
    {                                                  \
        0x04040404, 0x03030303, 0x45321433, 0x37636677 \
    }
#endif // QLIB_FPGA_TARGET

// Secure User ID
#define QLIB_SAMPLE_SUID                                   \
    {                                                      \
        0x19191919u, 0x10101010u, 0x21212121u, 0x22222222u \
    }

// Section 0 Full access key
#define QLIB_SAMPLE_FULL_ACCESS_K_0                        \
    {                                                      \
        0x23232323u, 0x24242424u, 0x25252525u, 0x26262626u \
    }

// Section 1 Full access key
#define QLIB_SAMPLE_FULL_ACCESS_K_1                        \
    {                                                      \
        0x27272727u, 0x28282828u, 0x29292929u, 0x20202020u \
    }

// Section 2 Full access key
#define QLIB_SAMPLE_FULL_ACCESS_K_2                        \
    {                                                      \
        0x31313131u, 0x32323232u, 0x33333333u, 0x34343434u \
    }

// Section 3 Full access key
#define QLIB_SAMPLE_FULL_ACCESS_K_3                        \
    {                                                      \
        0x35353535u, 0x36363636u, 0x37373737u, 0x38383838u \
    }

// Section 4 Full access key
#define QLIB_SAMPLE_FULL_ACCESS_K_4                        \
    {                                                      \
        0x39393939u, 0x30303030u, 0x41414141u, 0x42424242u \
    }

// Section 5 Full access key
#define QLIB_SAMPLE_FULL_ACCESS_K_5                        \
    {                                                      \
        0x43434343u, 0x44444444u, 0x45454545u, 0x46464646u \
    }

// Section 6 Full access key
#define QLIB_SAMPLE_FULL_ACCESS_K_6                        \
    {                                                      \
        0x47474747u, 0x48484848u, 0x49494949u, 0x40404040u \
    }

// Section 7 Full access key
#define QLIB_SAMPLE_FULL_ACCESS_K_7                        \
    {                                                      \
        0x51515151u, 0x52525252u, 0x53535353u, 0x54545454u \
    }

// Section 0 restricted access key
#define QLIB_SAMPLE_RESTRICTED_K_0                         \
    {                                                      \
        0x55555555u, 0x56565656u, 0x57575757u, 0x58585858u \
    }

// Section 1 restricted access key
#define QLIB_SAMPLE_RESTRICTED_K_1                         \
    {                                                      \
        0x59595959u, 0x50505050u, 0x61616161u, 0x62626262u \
    }

// Section 2 restricted access key
#define QLIB_SAMPLE_RESTRICTED_K_2                         \
    {                                                      \
        0x63636363u, 0x64646464u, 0x65656565u, 0x66666666u \
    }

// Section 3 restricted access key
#define QLIB_SAMPLE_RESTRICTED_K_3                         \
    {                                                      \
        0x67676767u, 0x68686868u, 0x69696969u, 0x60606060u \
    }

// Section 4 restricted access key
#define QLIB_SAMPLE_RESTRICTED_K_4                         \
    {                                                      \
        0x71717171u, 0x72727272u, 0x73737373u, 0x74747474u \
    }

// Section 5 restricted access key
#define QLIB_SAMPLE_RESTRICTED_K_5                         \
    {                                                      \
        0x75757575u, 0x76767676u, 0x77777777u, 0x78787878u \
    }

// Section 6 restricted access key
#define QLIB_SAMPLE_RESTRICTED_K_6                         \
    {                                                      \
        0x79797979u, 0x70707070u, 0x81818181u, 0x82828282u \
    }

// Section 7 restricted access key
#define QLIB_SAMPLE_RESTRICTED_K_7                         \
    {                                                      \
        0x83838383u, 0x84848484u, 0x85858585u, 0x86868686u \
    }

// Vault Full access key
#define QLIB_SAMPLE_VAULT_FULL_ACCESS                      \
    {                                                      \
        0x87878787u, 0x88888888u, 0x89898989u, 0x90909090u \
    }

// Vault restricted access key
#define QLIB_SAMPLE_VAULT_RESTRICTED                       \
    {                                                      \
        0x91919191u, 0x92929292u, 0x93939393u, 0x94949494u \
    }

// LMS keys
#define QLIB_SAMPLE_LMS_K_0                                                                                                    \
    {                                                                                                                          \
        0x00u, 0x00u, 0x00u, 0x0Du, 0x00u, 0x00u, 0x00u, 0x07u, 0x07u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, \
            0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x29u, 0xAAu, 0x2Bu, 0x31u, 0x4Fu, 0x16u, 0xD3u, 0x1Eu, 0x4Cu,    \
            0xDBu, 0x5Du, 0x36u, 0x63u, 0x50u, 0xE0u, 0x63u, 0x2Cu, 0xE7u, 0x11u, 0x2Au, 0x30u, 0x8Au, 0xB2u, 0x64u            \
    }
#define QLIB_SAMPLE_LMS_K_1 \
    {                       \
        0u                  \
    }
#define QLIB_SAMPLE_LMS_K_2 \
    {                       \
        0u                  \
    }
#define QLIB_SAMPLE_LMS_K_3 \
    {                       \
        0u                  \
    }
#define QLIB_SAMPLE_LMS_K_4 \
    {                       \
        0u                  \
    }
#define QLIB_SAMPLE_LMS_K_5 \
    {                       \
        0u                  \
    }
#define QLIB_SAMPLE_LMS_K_6 \
    {                       \
        0u                  \
    }
#define QLIB_SAMPLE_LMS_K_7 \
    {                       \
        0u                  \
    }
#define QLIB_SAMPLE_LMS_K_8 \
    {                       \
        0u                  \
    }

typedef TYPE_128BIT UNIQUE_ID_T;

/*---------------------------------------------------------------------------------------------------------*/
/* Flash characteristics                                                                                   */
/*---------------------------------------------------------------------------------------------------------*/
#ifndef QLIB_FLASH_BLOCK_SIZE
#define QLIB_FLASH_BLOCK_SIZE SIZE_64KB_
#endif // QLIB_FLASH_BLOCK_SIZE
#ifndef QLIB_FLASH_SECTOR_SIZE
#define QLIB_FLASH_SECTOR_SIZE SIZE_4KB_ // minimum erase size

#endif // QLIB_FLASH_SECTOR_SIZE
#ifndef QLIB_FLASH_PAGE_SIZE
#define QLIB_FLASH_PAGE_SIZE SIZE_256B_ // maximum write size

#endif // QLIB_FLASH_PAGE_SIZE
/*-----------------------------------------------------------------------------------------------------------
                                                  FUNCTIONS
-----------------------------------------------------------------------------------------------------------*/

QLIB_STATUS_T QLIB_SAMPLE_GetSectionAccessKey(QLIB_CONTEXT_T* ctx,
                                              UNIQUE_ID_T     uid,
                                              QLIB_KID_TYPE_T keyType,
                                              uint32_t        section_u32,
                                              uint32_t        die_u32,
                                              KEY_T           key);
#ifndef QLIB_ONLY_SECURE_STORAGE
QLIB_STATUS_T QLIB_SAMPLE_GetSectionLmsKey(QLIB_CONTEXT_T* ctx,
                                           UNIQUE_ID_T     uid,
                                           uint32_t        section_u32,
                                           uint32_t        die_u32,
                                           LMS_PUB_KEY_T   key);
QLIB_STATUS_T QLIB_SAMPLE_GetDeviceKey(QLIB_CONTEXT_T* ctx, UNIQUE_ID_T uid, QLIB_KID_TYPE_T keyType, KEY_T deviceKey);
#endif // QLIB_ONLY_SECURE_STORAGE

QLIB_STATUS_T QLIB_SAMPLE_GetSectionKeyPtr(QLIB_CONTEXT_T* ctx,
                                           UNIQUE_ID_T     uid,
                                           QLIB_KID_TYPE_T keyType,
                                           uint32_t        section_u32,
                                           uint32_t        die_u32,
                                           CONST_KEY_P_T*  key);

#ifndef QLIB_ONLY_SECURE_STORAGE
QLIB_STATUS_T QLIB_SAMPLE_GetSecureUID(QLIB_CONTEXT_T* ctx, UNIQUE_ID_T uid, TYPE_128BIT suid);
#endif // QLIB_ONLY_SECURE_STORAGE
#ifdef __cplusplus
}
#endif // __cplusplus

#endif // QLIB_SAMPLE_H__
