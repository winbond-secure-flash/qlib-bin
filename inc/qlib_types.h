/************************************************************************************************************
* @internal
* @remark     Winbond Electronics Corporation
* @copyright  Copyright (c) 2025 by Winbond Electronics Corporation . All rights reserved
* @endinternal
*
* @file       qlib_types.h
* @brief      This file contains QLIB interface types
*
* ### project qlib
*
************************************************************************************************************/
#ifndef QLIB_TYPES_H__
#define QLIB_TYPES_H__

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus
/*-----------------------------------------------------------------------------------------------------------
                                                   INCLUDES
-----------------------------------------------------------------------------------------------------------*/
#include <stdint.h>
#include "qlib_targets.h"
#include "defs_types.h"
#include "defs_utils.h"
#include "defs_os_general.h"
#include "qlib_ext_size.h"

/*-----------------------------------------------------------------------------------------------------------
                                                 DEFINITIONS
-----------------------------------------------------------------------------------------------------------*/
#define QLIB_NUM_OF_SECTIONS       9UL // all sections (including the vault)
#define QLIB_NUM_OF_MAIN_SECTIONS  8UL // main array sections 0..7
#define QLIB_SECTION_ID_VAULT      8UL
#define QLIB_MIN_SECTION_SIZE      SIZE_512KB_
#define QLIB_MAX_SECTION_SIZE      SIZE_16MB_
#define QLIB_MIN_SECTION_SIZE_LOG2 LOG2_512KB_

/********************************************************************************************************
 * Default Definitions
********************************************************************************************************/
#ifndef QLIB_MAX_DIES_SUPPORTED
#define QLIB_MAX_DIES_SUPPORTED 4u
#endif // QLIB_MAX_DIES_SUPPORTED

#if ((QLIB_MAX_DIES_SUPPORTED != 1UL) && (QLIB_MAX_DIES_SUPPORTED != 2UL) && (QLIB_MAX_DIES_SUPPORTED != 4UL) && \
     (QLIB_MAX_DIES_SUPPORTED != 8UL))
#error bad QLIB_MAX_DIES_SUPPORTED definition
#endif // ((QLIB_MAX_DIES_SUPPORTED != 1UL) && (QLIB_MAX_DIES_SUPPORTED != 2UL) && (QLIB_MAX_DIES_SUPPORTED != 4UL) && ...

#define FLASH_CR_DUMMY_DEFAULT                           (8u)   // default (reset) value of dummy field

/*---------------------------------------------------------------------------------------------------------*/
/* Flash characteristics                                                                                   */
/*---------------------------------------------------------------------------------------------------------*/
#define QLIB_FLASH_BLOCK_SIZE  SIZE_64KB_
#define QLIB_FLASH_SECTOR_SIZE SIZE_4KB_  // minimum erase size
#define QLIB_FLASH_PAGE_SIZE   SIZE_256B_ // maximum write size

/************************************************************************************************************
 *  Maximum data size
************************************************************************************************************/
#define QLIB_SIGNED_DATA_MAX_SIZE (uint8_t) BITS_TO_BYTES(160u)

/*-----------------------------------------------------------------------------------------------------------
                                                    TYPES
-----------------------------------------------------------------------------------------------------------*/
/**
 * @brief Opaque QLIB context. Allocate one per flash device and pass to all QLIB API calls.
 *        Contents are managed by the library and can't be accessed directly.
 */
typedef union QLIB_CONTEXT_T
{
    uint8_t  buf[QLIB_CONTEXT_SIZE];
    uint64_t align_u64;
    void*    align_ptr_pv;
} QLIB_CONTEXT_T;

/** @brief Opaque synchronization object used internally by QLIB. */
typedef union QLIB_SYNC_OBJ_T
{
    uint8_t  buf[QLIB_SYNC_OBJ_SIZE];
    uint64_t align_u64;
    void*    align_ptr_pv;
} QLIB_SYNC_OBJ_T;

typedef uint8_t QLIB_LMS_KEY_ID_T[16]; // key ID
typedef uint32_t SSPRn_T;  ///< 32b

/************************************************************************************************************
 * LMS Attestation Types
************************************************************************************************************/
#define LMS_ATTEST_PARAM_P 51u // Maximum number of chains (including checksum)
#define LMS_ATTEST_PARAM_N 24u // LMS OTS Hash output size in bytes
#define LMS_ATTEST_PARAM_M 24u // LMS Hash output size in bytes

typedef uint8_t LMS_OTS_ATTEST_CHUNK_T[LMS_ATTEST_PARAM_N]; // output of the OTS HASH function
typedef LMS_OTS_ATTEST_CHUNK_T
    LMS_ATTEST_OTS_SIG_T[LMS_ATTEST_PARAM_P]; // The OTS signature holds the single signature of OTS leaf


/************************************************************************************************************
*                                      Register data types
************************************************************************************************************/
typedef enum
{
    QLIB_SIGNED_DATA_ID_SECTION_DIGEST = 0x00u,
    QLIB_SIGNED_DATA_ID_WID            = 0x10u,
    QLIB_SIGNED_DATA_ID_SUID           = 0x14u,
    QLIB_SIGNED_DATA_ID_HW_VER         = 0x18u,
    QLIB_SIGNED_DATA_ID_MEMORY_RANGE   = 0X1Cu,
    QLIB_SIGNED_DATA_ID_SSR            = 0x20u,
    QLIB_SIGNED_DATA_ID_ESSR           = 0x21u,
    QLIB_SIGNED_DATA_ID_AWDTCFG        = 0x24u,
    QLIB_SIGNED_DATA_ID_AWDTSR         = 0x25u,
    QLIB_SIGNED_DATA_ID_MC             = 0x28u,
    QLIB_SIGNED_DATA_ID_GMC            = 0x30u,
    QLIB_SIGNED_DATA_ID_GMT            = 0x32u,
    QLIB_SIGNED_DATA_ID_SECTION_CONFIG = 0x40u

} QLIB_SIGNED_DATA_ID_T;
typedef enum

{
/************************************************************************************************************
*                                            ID size
************************************************************************************************************/
    QLIB_SIGNED_DATA_TYPE_SECTION_DIGEST = MAKE16(QLIB_SIGNED_DATA_ID_SECTION_DIGEST, BITS_TO_BYTES(64u)),
    QLIB_SIGNED_DATA_TYPE_WID            = MAKE16(QLIB_SIGNED_DATA_ID_WID, BITS_TO_BYTES(64u)),
    QLIB_SIGNED_DATA_TYPE_SUID           = MAKE16(QLIB_SIGNED_DATA_ID_SUID, BITS_TO_BYTES(128u)),
    QLIB_SIGNED_DATA_TYPE_HW_VER         = MAKE16(QLIB_SIGNED_DATA_ID_HW_VER, BITS_TO_BYTES(32u)),
    QLIB_SIGNED_DATA_TYPE_MEMORY_RANGE   = MAKE16(QLIB_SIGNED_DATA_ID_MEMORY_RANGE, BITS_TO_BYTES(64u)),
    QLIB_SIGNED_DATA_TYPE_SSR            = MAKE16(QLIB_SIGNED_DATA_ID_SSR, BITS_TO_BYTES(32u)),
    QLIB_SIGNED_DATA_TYPE_ESSR           = MAKE16(QLIB_SIGNED_DATA_ID_ESSR, BITS_TO_BYTES(64u)),
    QLIB_SIGNED_DATA_TYPE_AWDTCFG        = MAKE16(QLIB_SIGNED_DATA_ID_AWDTCFG, BITS_TO_BYTES(32u)),
    QLIB_SIGNED_DATA_TYPE_AWDTSR         = MAKE16(QLIB_SIGNED_DATA_ID_AWDTSR, BITS_TO_BYTES(32u)),
    QLIB_SIGNED_DATA_TYPE_MC             = MAKE16(QLIB_SIGNED_DATA_ID_MC, BITS_TO_BYTES(64u)),
    QLIB_SIGNED_DATA_TYPE_GMC            = MAKE16(QLIB_SIGNED_DATA_ID_GMC, BITS_TO_BYTES(160u)),
    QLIB_SIGNED_DATA_TYPE_GMT            = MAKE16(QLIB_SIGNED_DATA_ID_GMT, BITS_TO_BYTES(160u)),
    QLIB_SIGNED_DATA_TYPE_SECTION_CONFIG = MAKE16(QLIB_SIGNED_DATA_ID_SECTION_CONFIG, BITS_TO_BYTES(160u))
} QLIB_SIGNED_DATA_TYPE_T;

/*---------------------------------------------------------------------------------------------------------*/
/* Get data size                                                                                           */
/*---------------------------------------------------------------------------------------------------------*/
#define QLIB_SIGNED_DATA_TYPE_GET_SIZE(data_type) BYTE(data_type, 1)

/*---------------------------------------------------------------------------------------------------------*/
/* Get data ID                                                                                             */
/*---------------------------------------------------------------------------------------------------------*/
#define QLIB_SIGNED_DATA_TYPE_GET_ID(data_type, section)                                                                         \
    ((uint8_t)(BYTE(data_type, 0) +                                                                                              \
               ((((data_type) == QLIB_SIGNED_DATA_TYPE_SECTION_DIGEST) || ((data_type) == QLIB_SIGNED_DATA_TYPE_SECTION_CONFIG)) \
                    ? ((uint8_t)(0x0fu & (section)))                                                                             \
                    : 0u)))

/************************************************************************************************************
 * XIP support
************************************************************************************************************/
#if defined(QLIB_SUPPORT_XIP)
    #if !defined(RAM_SECTION)
        #if !defined(QLIB_RAM_SECTION)
            #error "QLIB_RAM_SECTION must be defined in order to support XIP"
        #endif //QLIB_RAM_SECTION
        // Use compiler-specific macros to place code in RAM section
        #define RAM_SECTION                     COMPILER_NOINLINE COMPILER_SECTION(QLIB_RAM_SECTION)
    #endif //RAM_SECTION
#else
    #define RAM_SECTION
#endif // defined(QLIB_SUPPORT_XIP)

/************************************************************************************************************
 * Defines the possible chip specific features
************************************************************************************************************/
#define W77Q_FEATURE_START                  0u
#define W77Q_FEATURE_4_BYTES_ADDRESS_MODE   1u  // Flash supports 4 bytes address mode
#define W77Q_FEATURE_DEVCFG_CTAG_MODE       2u  // Flash DEVCFG register CTAG_MODE controls the SPI format of the CTAG field of OP1
#define W77Q_FEATURE_AWDTCFG_FALLBACK       3u  // Flash supports watchdog fallback
#define W77Q_FEATURE_ESSR                   4u  // Flash supports Extended Secure Status Register
#define W77Q_FEATURE_RNG                    5u  // Flash has Random-Number Generator function
#define W77Q_FEATURE_AUTH_AC                6u  // Flash can verify section digest when a section is first accessed
#define W77Q_FEATURE_DUAL_SPI               8u  // Flash supports dual SPI bus mode
#define W77Q_FEATURE_1_1_4_SPI              9u  // FLash supports quad SPI mode 1_1_4
#define W77Q_FEATURE_OCTAL_SPI              10u // FLash supports octal and OPI SPI modes (It can be linmited though by flash package)
#define W77Q_FEATURE_HW_RPMC                11u // Flash device supports RPMC
#define W77Q_FEATURE_PRE_PROV_MASTER_KEY    12u // Flash supports pre-provision device master key
#define W77Q_FEATURE_FAST_READ_DUMMY_CONFIG 13u // Flash supports configuration of dummy cycles for Fast-Read instructions
#define W77Q_FEATURE_RST_PA                 15u // Flash can grant plain access on reset to configured sections
#define W77Q_FEATURE_VAULT                  16u // Flash supports Vault memory
#define W77Q_FEATURE_FORMAT_MODE            17u // Flash supports mode bits (RESET/DEFAUL) in Format command
#define W77Q_FEATURE_PA_GRANT_REVOKE        18u // Flash supports PA_GRANT, PA_GRANT_PLAIN and PA_REVOKE commands
#define W77Q_FEATURE_LMS                    19u // Flash supports Hash-Based LMS Asymmetric Signature scheme for signing Firmware Updates.
#define W77Q_FEATURE_LMS_ATTESTATION        20u // Flash supports asymmetric-key One-Time Signatures for user messages to be used
                                                 // for Supply-Chain Protection
#define W77Q_FEATURE_SECURE_LOG             21u // Flash supports Secure Log
#define W77Q_FEATURE_MEM_CRC                22u // Flash supports Memory CRC calculation with MEM_CRC command
#define W77Q_FEATURE_MEM_COPY               23u // Flash supports Memory Copy with MEM_COPY command
#define W77Q_FEATURE_SIG_MEM_RANGE          24u // Flash supports signature verification of memory ranges with SIG_MEM_RANGE command
#define W77Q_FEATURE_MULTI_DIE              25u // Flash supports multiple dies
#define W77Q_FEATURE_INTERRUPTS             26u // Flash supports interrupt mechanism
#define W77Q_FEATURE_FALLBACK_MAPPING       27u // Flash supports fallback mapping configuration
#define W77Q_FEATURE_HW_VER_FLASH_SIZE      28u // HW_VER register has FLASH_SIZE field
#define W77Q_LMS_KEY_BACKUP                 29u // Flash supports LMS Key Backup feature
#define W77Q_FEATURE_DIGEST_EXT             30u // Flash LMS FW Update supports digest extension verification
#define W77Q_FEATURE_VAULT_128KB            31u // Flash supports up to 128KB Vault memory
#define W77Q_FEATURE_END                    W77Q_FEATURE_VAULT_128KB

/*-----------------------------------------------------------------------------------------------------------
                                                 DEFINITIONS
-----------------------------------------------------------------------------------------------------------*/

/********************************************************************************************************
 * Define features support by die type (target). target parameter is assumed to be one specific target
********************************************************************************************************/
// clang-format off
#define W77Q_FEATURE_SUPPORTED_BY_TARGET(feature, target)                                                                                           \
    ((feature) == W77Q_FEATURE_4_BYTES_ADDRESS_MODE      ? ((((target) & q2_mcd_targets) != 0u) ? 0u : 1u)                                          \
    : ((feature) == W77Q_FEATURE_DEVCFG_CTAG_MODE        ? ((((target) & q2_hcd_targets) != 0u) ? 1u : 0u)                                          \
    : ((feature) == W77Q_FEATURE_AWDTCFG_FALLBACK        ? ((((target) & q2_mcd_targets) != 0u) ? 0u : 1u)                                          \
    : ((feature) == W77Q_FEATURE_ESSR                    ? ((((target) & q2_mcd_targets) != 0u) ? 0u : 1u)                                          \
    : ((feature) == W77Q_FEATURE_RNG                     ? ((((target) & q2_mcd_targets) != 0u) ? 0u : 1u)                                          \
    : ((feature) == W77Q_FEATURE_AUTH_AC                 ? ((((target) & all_q3_targets) != 0u) || (((target) & q2_hcd33_targets) != 0u) ? 1u : 0u) \
    : ((feature) == W77Q_FEATURE_DUAL_SPI                ? ((((target) & all_q2_targets) != 0u) ? 1u : 0u)                                          \
    : ((feature) == W77Q_FEATURE_1_1_4_SPI               ? ((((target) & all_q2_targets) != 0u) ? 1u : 0u)                                          \
    : ((feature) == W77Q_FEATURE_OCTAL_SPI               ? ((((target) & q3_t_targets) != 0u) ? 1u : 0u)                                            \
    : ((feature) == W77Q_FEATURE_HW_RPMC                 ? ((((target) & all_q3_0_targets) != 0u) ||                                                \
                                                            (((target) & all_q3_1_targets) != 0u) ? 1u : 0u)                                        \
    : ((feature) == W77Q_FEATURE_PRE_PROV_MASTER_KEY     ? ((((target) & all_q3_targets) != 0u) ? 1u : 0u)                                          \
    : ((feature) == W77Q_FEATURE_FAST_READ_DUMMY_CONFIG  ? ((((target) & all_q3_targets) != 0u) ? 1u : 0u)                                          \
    : ((feature) == W77Q_FEATURE_RST_PA                  ? ((((target) & all_q3_targets) != 0u) ? 1u : 0u)                                          \
    : ((feature) == W77Q_FEATURE_VAULT                   ? ((((target) & all_q3_targets) != 0u) ? 1u : 0u)                                          \
    : ((feature) == W77Q_FEATURE_FORMAT_MODE             ? ((((target) & all_q3_targets) != 0u) ? 1u : 0u)                                          \
    : ((feature) == W77Q_FEATURE_PA_GRANT_REVOKE         ? ((((target) & all_q3_targets) != 0u) ? 1u : 0u)                                          \
    : ((feature) == W77Q_FEATURE_LMS                     ? ((((target) & all_q3_targets) != 0u) ? 1u : 0u)                                          \
    : ((feature) == W77Q_FEATURE_LMS_ATTESTATION         ? ((((target) & all_q3_targets) != 0u) ? 1u : 0u)                                          \
    : ((feature) == W77Q_FEATURE_SECURE_LOG              ? ((((target) & all_q3_targets) != 0u) ? 1u : 0u)                                          \
    : ((feature) == W77Q_FEATURE_MEM_CRC                 ? ((((target) & all_q3_targets) != 0u) ? 1u : 0u)                                          \
    : ((feature) == W77Q_FEATURE_MEM_COPY                ? ((((target) & all_q3_targets) != 0u) ? 1u : 0u)                                          \
    : ((feature) == W77Q_FEATURE_SIG_MEM_RANGE           ? ((((target) & all_q3_targets) != 0u) ? 1u : 0u)                                          \
    : ((feature) == W77Q_FEATURE_MULTI_DIE               ? ((((target) & all_q3_targets) != 0u) ? 1u : 0u)                                          \
    : ((feature) == W77Q_FEATURE_INTERRUPTS              ? ((((target) & all_q3_targets) != 0u) ? 1u : 0u)                                          \
    : ((feature) == W77Q_FEATURE_FALLBACK_MAPPING        ? ((((target) & all_q3_1_targets) != 0u) ||                                                \
                                                            (((target) & all_q3_2_targets) != 0u) ? 1u : 0u)                                        \
    : ((feature) == W77Q_FEATURE_HW_VER_FLASH_SIZE       ? ((((target) & all_q2_targets) != 0u) ? 0u : 1u)                                          \
    : ((feature) == W77Q_LMS_KEY_BACKUP                  ? ((((target) & all_q3_1_targets) != 0u) ||                                                \
                                                            (((target) & all_q3_2_targets) != 0u) ? 1u : 0u)                                        \
    : ((feature) == W77Q_FEATURE_DIGEST_EXT              ? ((((target) & all_q3_1_targets) != 0u) ||                                                \
                                                            (((target) & all_q3_2_targets) != 0u) ? 1u : 0u)                                        \
    : ((feature) == W77Q_FEATURE_VAULT_128KB             ? ((((target) & all_q3_0_targets) != 0u) ||                                                \
                                                            (((target) & all_q3_1_targets) != 0u) ? 1u : 0u)                                        \
    : 0u)))))))))))))))))))))))))))))
// clang-format on

#define QLIB_LMS_PUBLIC_KEY_SIZE(m)                        \
    (sizeof(uint32_t) +          /* lmsParameterSetId */   \
     sizeof(uint32_t) +          /* lmotsParameterSetId */ \
     sizeof(QLIB_LMS_KEY_ID_T) + /* key Id */              \
     (m))                        /* tree root */

#define QLIB_LMS_SIGNATURE_SIZE(m, n, p, h)       \
    (sizeof(uint32_t) + /* otsIdxQ */             \
     sizeof(uint32_t) + /* lmotsParameterSetId */ \
     (n) +              /* randomString */        \
     ((p) * (n)) +      /* otsSignature */        \
     sizeof(uint32_t) + /*lmsParameterSetId */    \
     ((h) * (m)))       /* path */

#if (W77Q_FEATURE_SUPPORTED_BY_TARGET(W77Q_FEATURE_LMS, QLIB_TARGET) == 0u)
#ifndef EXCLUDE_LMS
#define EXCLUDE_LMS
#endif // EXCLUDE_LMS
#endif // W77Q_FEATURE_SUPPORTED_BY_TARGET(W77Q_FEATURE_LMS, QLIB_TARGET) == 0u

#if (W77Q_FEATURE_SUPPORTED_BY_TARGET(W77Q_FEATURE_LMS_ATTESTATION, QLIB_TARGET) == 0u)
#ifndef EXCLUDE_LMS_ATTESTATION
#define EXCLUDE_LMS_ATTESTATION
#endif // EXCLUDE_LMS_ATTESTATION
#endif // W77Q_FEATURE_SUPPORTED_BY_TARGET(W77Q_FEATURE_LMS_ATTESTATION, QLIB_TARGET) == 0u

/************************************************************************************************************
 * SPI bus format including dtr
************************************************************************************************************/
typedef uint32_t QLIB_BUS_FORMAT_T;

#define QLIB_BUS_FORMAT_GET_MODE(busFormat) ((QLIB_BUS_MODE_T)(uint32_t)BYTE_OF((busFormat), 0u))

#define QLIB_BUS_FORMAT_GET_DTR(busFormat) (INT_TO_BOOLEAN(BYTE_OF((busFormat), 1u)))

#ifndef QLIB_BUS_FORMAT
#define QLIB_BUS_FORMAT(mode, dtr) ((uint32_t)(mode) | (((uint32_t)BOOLEAN_TO_INT((bool)(dtr))) << 8u))
#endif // QLIB_BUS_FORMAT

/************************************************************************************************************
 * Keys
************************************************************************************************************/
typedef TYPE_128BIT KEY_T;

/************************************************************************************************************
 * Flash power state selector
************************************************************************************************************/
typedef enum
{
    QLIB_POWER_FIRST = 0x1145,

    QLIB_POWER_UP,
    QLIB_POWER_DOWN,

    QLIB_POWER_LAST
} QLIB_POWER_T;

/************************************************************************************************************
 * QLIB notifications
************************************************************************************************************/
typedef struct QLIB_NOTIFICATIONS_T
{
    uint8_t mcMaintenance : 1; ///< Monotonic Counter maintenance is needed
    uint8_t resetDevice : 1;   ///< Device reset is needed since Transaction Counter is close to its max value
    uint8_t replaceDevice : 1; ///< Device replacement is needed since DMC is close to its max value
} QLIB_NOTIFICATIONS_T;

/************************************************************************************************************
 * This enumeration defines the session access type to open
************************************************************************************************************/
typedef enum QLIB_SESSION_ACCESS_T
{
    QLIB_SESSION_ACCESS_RESTRICTED,  ///< Open session using restricted key
    QLIB_SESSION_ACCESS_CONFIG_ONLY, ///< Open session just for configuration - ignoring the validity of SCRi
    QLIB_SESSION_ACCESS_FULL         ///< Open session using full key
} QLIB_SESSION_ACCESS_T;

/************************************************************************************************************
 * Revoke plain access types
************************************************************************************************************/
typedef enum
{
    QLIB_PA_REVOKE_WRITE_ACCESS = 2, ///< revoke plain write access
    QLIB_PA_REVOKE_ALL_ACCESS   = 3, ///< revoke both plain read and plain write access
} QLIB_PA_REVOKE_TYPE_T;

/************************************************************************************************************
 * This enumeration defines the Authenticated watchdog threshold
************************************************************************************************************/
typedef enum QLIB_AWDT_TH_T
{
    QLIB_AWDT_TH_1_SEC      = 0,
    QLIB_AWDT_TH_2_SEC      = 1,
    QLIB_AWDT_TH_4_SEC      = 2,
    QLIB_AWDT_TH_8_SEC      = 3,
    QLIB_AWDT_TH_16_SEC     = 4,
    QLIB_AWDT_TH_32_SEC     = 5,
    QLIB_AWDT_TH_1_MINUTES  = 6,
    QLIB_AWDT_TH_2_MINUTES  = 7,
    QLIB_AWDT_TH_4_MINUTES  = 8,
    QLIB_AWDT_TH_8_MINUTES  = 9,
    QLIB_AWDT_TH_17_MINUTES = 10,
    QLIB_AWDT_TH_34_MINUTES = 11,
    QLIB_AWDT_TH_1_HOURS    = 12,
    QLIB_AWDT_TH_2_HOURS    = 13,
    QLIB_AWDT_TH_4_HOURS    = 14,
    QLIB_AWDT_TH_9_HOURS    = 15,
    QLIB_AWDT_TH_18_HOURS   = 16,
    QLIB_AWDT_TH_36_HOURS   = 17,
    QLIB_AWDT_TH_72_HOURS   = 18,
    QLIB_AWDT_TH_6_DAYS     = 19,
    QLIB_AWDT_TH_12_DAYS    = 20
} QLIB_AWDT_TH_T;

/************************************************************************************************************
 * This type contains Authenticated watchdog configuration
************************************************************************************************************/
typedef struct QLIB_WATCHDOG_CONF_T
{
    bool           enable;        ///< Watchdog enable
    bool           lfOscEn;       ///< Low frequency oscillator enable
    bool           swResetEn;     ///< Software reset enable
    bool           authenticated; ///< Requires authentication (signature) for its configurations and reset
    uint32_t       sectionID;     ///< Section ID of the key associated with the AWDT function
    QLIB_AWDT_TH_T threshold;     ///< Watchdog expires when the timer reaches this value
    bool           lock;          ///< AWDTCFG register is locked
    uint32_t       oscRateHz;     ///< Frequency of the internal oscillator, used for calibration. Set to 0 if non-applicable
    bool fallbackEn; ///< AWDT Fallback function is triggered after the watchdog expires (only AWDTCFG Fallback is supported))
} QLIB_WATCHDOG_CONF_T;

/************************************************************************************************************
 * This type contains IO2 muxing options
************************************************************************************************************/
typedef enum QLIB_IO23_MODE_T
{
    QLIB_IO23_MODE__LEGACY_WP_HOLD,
    QLIB_IO23_MODE__RESET_IN_OUT,
    QLIB_IO23_MODE__QUAD,
    QLIB_IO23_MODE__NONE ///< QLIB does not configure IO23 pins.
} QLIB_IO23_MODE_T;

/************************************************************************************************************
 * This type contains W77Q pin muxing options
************************************************************************************************************/
typedef struct QLIB_PIN_MUX_T
{
    QLIB_IO23_MODE_T io23Mux;            // Set IO2 and IO3 mux type (LEGACY/RESET/QUAD)
    bool             dedicatedResetInEn; // Set TRUE to enable dedicated RSTIN#
} QLIB_PIN_MUX_T;

/************************************************************************************************************
 * Standard commands address mode
************************************************************************************************************/
typedef enum
{
    QLIB_STD_ADDR_MODE__3_BYTE, ///< 3 bytes address mode
    QLIB_STD_ADDR_MODE__4_BYTE  ///< 4 bytes address mode
} QLIB_STD_ADDR_MODE_T;

#ifdef DEPRECATED_STD_ADDR_LEN
/************************************************************************************************************
* Standard logical address length
************************************************************************************************************/
typedef enum QLIB_STD_ADDR_LEN_T
{
    QLIB_STD_ADDR_LEN__22_BIT = 19, ///< address is 3 bits die, 3 bits of SID and offset of 19 bits
    QLIB_STD_ADDR_LEN__23_BIT = 20, ///< address is 3 bits die, 3 bits of SID and offset of 20 bits
    QLIB_STD_ADDR_LEN__24_BIT = 21, ///< address is 3 bits die, 3 bits of SID and offset of 21 bits
    QLIB_STD_ADDR_LEN__25_BIT = 22, ///< address is 3 bits die, 3 bits of SID and offset of 22 bits
    QLIB_STD_ADDR_LEN__26_BIT = 23, ///< address is 3 bits die, 0 or 3 bits of SID and offset of 23 bits
    QLIB_STD_ADDR_LEN__27_BIT = 24, ///< address is 3 bits die, 1 or 3 bits of SID and offset of 24 bits
    QLIB_STD_ADDR_LEN__LAST
} QLIB_STD_ADDR_LEN_T;

typedef enum QLIB_STD_SID_LEN_T
{
    QLIB_STD_SID_LEN__0_BIT = 0, ///< 0 bits of SID
    QLIB_STD_SID_LEN__1_BIT = 1, ///< 1 bit of SID
    QLIB_STD_SID_LEN__3_BIT = 3, ///< 3 bits of SID
    QLIB_STD_SID_LEN__LAST
} QLIB_STD_SID_LEN_T;

#endif // DEPRECATED_STD_ADDR_LEN

#define QLIB_STD_ADDR_LEN_MIN     19u
#define QLIB_STD_ADDR_LEN_MAX     25u
#define QLIB_STD_ADDR_LEN_DEFAULT 22u

/************************************************************************************************************
* This type contains the standard commands address size
************************************************************************************************************/
typedef struct QLIB_STD_ADDR_SIZE_T
{
    uint8_t addrLen_u8;
    uint8_t sidLen_u8; ///< Number of bits for section ID
    QLIB_STD_ADDR_MODE_T addrMode; ///< Power up address mode set in the non-volatile Status Register bit ADP
} QLIB_STD_ADDR_SIZE_T;

typedef struct QLIB_FLASH_CONFIG_T
{
    QLIB_WATCHDOG_CONF_T watchdogDefault; ///< Watchdog and rest configurations. Relevant only for die 0
    QLIB_PIN_MUX_T       pinMux;          ///< Pin muxing configuration. Relevant only for die 0
    QLIB_STD_ADDR_SIZE_T stdAddrSize;     ///< Address size configuration.
    uint8_t fastReadDummyCycles; ///< Number of dummy cycles for all Fast-Read instructions (only W77Q/T 256Mb to 1Gb flash). Relevant only for die 0
    bool dqsDisable; ///< Disable the DQS pin. Relevant only for die 0
} QLIB_FLASH_CONFIG_T;

/************************************************************************************************************
 * Section configuration actions
************************************************************************************************************/
typedef enum
{
    QLIB_SECTION_CONF_ACTION__NO,     ///< New configuration is not loaded after command execution
    QLIB_SECTION_CONF_ACTION__RELOAD, ///< Automatically load new configuration after command execution
    QLIB_SECTION_CONF_ACTION__RESET   ///< Trigger host and flash hardware reset after command execution
} QLIB_SECTION_CONF_ACTION_T;

typedef struct QLIB_DIE_CONFIG_T
{
    KEY_T                 deviceMasterKey; ///< Device Master Key value.
    KEY_T                 deviceSecretKey; ///< Device Secret Key value. The device secret key is used to configure the Root Of Trust feature.
    KEY_T                 preProvisionedMasterKey; ///< Pre-provisioned master key value.
    TYPE_128BIT           suid;      ///< Secure User ID
    bool                  safeFB;    ///< Safe fall back Enabled
    bool                  FBFail;    ///< Fallback fail
    bool                  FBSelect;  ///< Fallback select
    uint8_t               FBMapping; ///< Fallback mapping
    bool                  speculCK;  ///< Speculative Cypher Key Generation
    bool                  nonSecureFormatEn; ///< Non-secure FORMAT enable
    bool                  rngPAEn;           ///< RNG Plain Access Enabled (for devices supporting W77Q_RNG_FEATURE)
    bool                  ctagModeMulti; ///< OP1 instruction is sent with all available SPI IO pins. Relevant only if Q2_SUPPORT_DEVCFG_CTAG_MODE
    bool                  devcfgLock;    ///< DEVCFG register is locked
#if QLIB_MAX_DIES_SUPPORTED > 1
    bool                  FBIsPerDieConfig; ///< whether fallback configuration is per-die specific. if true, it is set by the QLIB_FALLBACK_CONFIG_T
#endif // QLIB_MAX_DIES_SUPPORTED > 1
} QLIB_DIE_CONFIG_T;

typedef struct QLIB_SECTION_CONF_POST_ACTIONS_T
{
    uint32_t swap : 1;
    uint32_t reload : 1;
    uint32_t reset : 1;
} QLIB_SECTION_CONF_POST_ACTIONS_T;

/************************************************************************************************************
 * This type contains section policy configuration
************************************************************************************************************/
typedef struct QLIB_POLICY_T
{
    uint32_t digestIntegrity : 1;         ///< perform digest integrity check when Section Configuration Register is set
    uint32_t checksumIntegrity : 1;       ///< perform CRC integrity check
    uint32_t writeProt : 1;               ///< write protected section
    uint32_t rollbackProt : 1;            ///< rollback protected section
    uint32_t plainAccessWriteEnable : 1;  ///< allow plain write access
    uint32_t plainAccessReadEnable : 1;   ///< allow plain read access
    uint32_t authPlainAccess : 1;         ///< plain access require @ref QLIB_OpenSession
    uint32_t digestIntegrityOnAccess : 1; ///< perform digest integrity check when section is accessed
    uint32_t slog : 1;                    ///< use the section as secure log
} QLIB_POLICY_T;

typedef struct QLIB_SECTION_CONF_T
{
    QLIB_POLICY_T                    policy;      ///< Section policy
    uint32_t                         crc;         ///< Section CRC
    uint64_t                         digest;      ///< Section digest
    uint32_t                         version;     ///< Section version
    QLIB_SECTION_CONF_POST_ACTIONS_T postActions; ///< Flags for defining actions after configuration happens
} QLIB_SECTION_CONF_T;

/************************************************************************************************************
 * This type contains runtime status of a section, reflecting the volatile ACL state after reset.
************************************************************************************************************/
typedef struct QLIB_SECTION_RUNTIME_STATUS_T
{
    bool rollbackEnabled;    ///< TRUE if section has rollback protection enabled
    bool swapped;            ///< TRUE if partitions are swapped (physical partition B is active)
    bool plainReadEnabled;   ///< TRUE if plain read access is currently enabled
    bool plainWriteEnabled;  ///< TRUE if plain write access is currently enabled
    bool integrityCheckPass; ///< TRUE if section passed integrity check after reset
} QLIB_SECTION_RUNTIME_STATUS_T;

/************************************************************************************************************
 * LMS Types
************************************************************************************************************/
#define QLIB_LMS_PARAM_P     51u // Maximum number of chains (including checksum)
#define QLIB_LMS_PARAM_N     24u // LMS OTS Hash output size in bytes
#define QLIB_LMS_TREE_HEIGHT 20u
#define QLIB_LMS_PARAM_M     24u // LMS Hash output size in bytes

typedef uint8_t LMS_PUB_KEY_T[QLIB_LMS_PUBLIC_KEY_SIZE(QLIB_LMS_PARAM_M)];
typedef uint8_t QLIB_LMS_MSG_DATA_STRUCT_T[64];
typedef uint8_t
    QLIB_LMS_SIG_BUFFER_T[QLIB_LMS_SIGNATURE_SIZE(QLIB_LMS_PARAM_M, QLIB_LMS_PARAM_N, QLIB_LMS_PARAM_P, QLIB_LMS_TREE_HEIGHT)];

#ifndef QLIB_LMS_ATTEST_TREE_HEIGHT
#define QLIB_LMS_ATTEST_TREE_HEIGHT 10u
#endif // QLIB_LMS_ATTEST_TREE_HEIGHT

#define QLIB_LMS_ATTEST_ALL_NODES              ((uint32_t)1 << ((QLIB_LMS_ATTEST_TREE_HEIGHT) + 1u))

typedef uint8_t LMS_ATTEST_PRIVATE_SEED_T[LMS_ATTEST_PARAM_M]; // Private key seed
typedef uint8_t LMS_ATTEST_CHUNK_T[LMS_ATTEST_PARAM_M];        // output of the LMS HASH function
typedef uint8_t LMS_ATTEST_NONCE_T[LMS_ATTEST_PARAM_N];   // Nonce value used for the signature

#ifndef EXCLUDE_LMS_ATTESTATION

typedef uint8_t LMS_ATTEST_SIG_T
    [QLIB_LMS_SIGNATURE_SIZE(LMS_ATTEST_PARAM_M, LMS_ATTEST_PARAM_N, LMS_ATTEST_PARAM_P, QLIB_LMS_ATTEST_TREE_HEIGHT)];

typedef uint8_t LMS_ATTEST_PUB_KEY_T[QLIB_LMS_PUBLIC_KEY_SIZE(LMS_ATTEST_PARAM_M)];
#endif // EXCLUDE_LMS_ATTESTATION

typedef struct QLIB_EXTENDED_SECTION_CONF_T
{
    uint32_t            baseAddr;      ///< Flash physical address
    uint32_t            size;          ///< Section size. When the size equals to 0 the section is disabled.
    QLIB_SECTION_CONF_T sectionConfig; ///< Section fields that can be updated with QLIB_ConfigDeviceSection function
    bool                resetPA;       ///< Plain Access enabled for section after reset
    KEY_T               restrictedKey; ///< Restricted key for the section
    KEY_T               fullAccessKey; ///< Full access key for the section
    LMS_PUB_KEY_T       lmsKey;        ///< LMS Root key for the section in format of RFC 8554
} QLIB_EXTENDED_SECTION_CONF_T;

#if QLIB_MAX_DIES_SUPPORTED > 1
/************************************************************************************************************
* This type contains specific DIE fallback mapping configuration.
* It allows user to set different safe fallback configuration to each of the DIEs by setting
* FBIsPerDieConfig field in QLIB_DIE_CONFIG_T to true and setting specific DIE FB values in QLIB_FALLBACK_CONFIG_T struct.
************************************************************************************************************/
#endif // QLIB_MAX_DIES_SUPPORTED > 1
typedef struct QLIB_FALLBACK_CONFIG_T
{
    bool    safeFB;    ///< Safe fall back Enabled
    bool    FBSelect;  ///< Fallback select
    uint8_t FBMapping; ///< Fallback mapping
} QLIB_FALLBACK_CONFIG_T;

typedef struct QLIB_SECTIONS_CONF_TABLE_T
{
    QLIB_EXTENDED_SECTION_CONF_T sectionConfigTable[QLIB_NUM_OF_SECTIONS];
#if QLIB_MAX_DIES_SUPPORTED > 1
    QLIB_FALLBACK_CONFIG_T       fallbackConfig; ///< Fallback configuration for the DIE, relevant only if FBIsPerDieConfig is true
#endif // QLIB_MAX_DIES_SUPPORTED > 1
} QLIB_SECTIONS_CONF_TABLE_T;

/************************************************************************************************************
 * Interrupt types
************************************************************************************************************/
typedef struct QLIB_INTERRUPT
{
    uint32_t securityViolationInt : 1; ///< Security Violation Interrupt
    uint32_t watchdogTimerInt : 1;     ///< Watchdog Timer Interrupt
    uint32_t spiIntegrityInt : 1;      ///< SPI Integrity Interrupt
    uint32_t eccSingleErrInt : 1;      ///< ECC Single Error Correction Interrupt
    uint32_t eccDoubleErrInt : 1;      ///< ECC Double Error Detection Interrupt
    uint32_t eccMultipleProgInt : 1;   ///< ECC Multiple-programming Interrupt
} QLIB_INTERRUPT_T;

typedef struct
{
    bool singleErr;
    bool doubleErr;
    bool multipleProg;
    bool erasedLine;
    bool eccEnabled;
    bool eccOff;
} QLIB_ECC_STATUS_T;

typedef struct
{
    bool     sacvf; // TRUE if sra is valid
    uint32_t sra;   // Physical Address (28 lsb) of the first SEC event from previous memory read operations
    uint8_t  sc;    // SEC Counter
    bool     dacvf; // TRUE if dra is valid
    uint32_t dra;   // Physical Address (28 lsb) of the first DED event from previous memory read operations
    uint8_t  dc;    // DED counter
} QLIB_ADVANCED_ECC_T;

/************************************************************************************************************
 * Signed data parameters, by data type
************************************************************************************************************/
typedef union QLIB_SIGNED_DATA_PARAMS_T
{
    uint32_t asArray[2];
    struct
    {
        uint32_t startAddrOffset;
        uint32_t memoryLength;
    } asStructMemoryRange;

} QLIB_SIGNED_DATA_PARAMS_T;

/************************************************************************************************************
 * This enumeration defines the session integrity check type
************************************************************************************************************/
typedef enum QLIB_INTEGRITY_T
{
    QLIB_INTEGRITY_CRC,   ///< CRC type integrity
    QLIB_INTEGRITY_DIGEST ///< digest type integrity
} QLIB_INTEGRITY_T;

/************************************************************************************************************
 * Flash ID - Standard part
************************************************************************************************************/
typedef struct
{
    TYPE_128BIT uniqueID; ///< unique ID
} QLIB_STD_ID_T;

/************************************************************************************************************
 * This type contains the value of W77Q/T WID
************************************************************************************************************/
typedef TYPE_64BIT QLIB_WID_T;

/************************************************************************************************************
 * Flash ID - secure part
************************************************************************************************************/
typedef struct
{
    TYPE_128BIT suid; ///< secure user ID
    QLIB_WID_T  wid;  ///< Winbond ID
} QLIB_SEC_ID_T;

/************************************************************************************************************
 * Flash ID
************************************************************************************************************/
typedef struct
{
    QLIB_STD_ID_T std; ///< standard ID
    QLIB_SEC_ID_T sec; ///< secure ID
} QLIB_ID_T;

/************************************************************************************************************
 * Hardware Version - Standard part
************************************************************************************************************/
typedef struct
{
    uint8_t manufacturerID; ///< Manufacture ID
    uint8_t memoryType;     ///< Memory type
    uint8_t capacity;       ///< Capacity
    uint8_t deviceID;       ///< Device ID
} QLIB_STD_HW_VER_T;

/************************************************************************************************************
 * Hardware Version - Secure part
************************************************************************************************************/
typedef struct
{
    uint8_t flashVersion;    ///< Flash version
    uint8_t securityVersion; ///< Security Protocol Version
    uint8_t revision;        ///< Hardware revision
    uint8_t flashSize;       ///< Flash size
    uint8_t hashVersion;     ///< Hash function version
} QLIB_SEC_HW_VER_T;

/************************************************************************************************************
 * Flash Size
************************************************************************************************************/
typedef enum
{
    QLIB_FLASH_SIZE_UNKNOWN = 0,
    QLIB_FLASH_SIZE_32Mb    = 0x15,
    QLIB_FLASH_SIZE_64Mb,
    QLIB_FLASH_SIZE_128Mb,
    QLIB_FLASH_SIZE_256Mb,
    QLIB_FLASH_SIZE_512Mb,
    QLIB_FLASH_SIZE_1Gb
} QLIB_FLASH_SIZE_T;

/************************************************************************************************************
 * Flash Revision
************************************************************************************************************/
typedef enum
{
    // Note that the numeric values of QLIB_FLASH_REVISION_* here should match the values
    // of HW_VER register's revision field
    QLIB_FLASH_REVISION_UNKNOWN = 0xFF,
    QLIB_FLASH_REVISION_A       = 0,
    QLIB_FLASH_REVISION_B       = 1,
    QLIB_FLASH_REVISION_C       = 2,
    QLIB_FLASH_REVISION_D       = 3,
    QLIB_2_FLASH_REVISION_A  = QLIB_FLASH_REVISION_A,
    QLIB_2_FLASH_REVISION_B  = QLIB_FLASH_REVISION_B,
    QLIB_2_FLASH_REVISION_C  = QLIB_FLASH_REVISION_C,
    QLIB_3_FLASH_REVISION_A  = QLIB_FLASH_REVISION_A,
    QLIB_3_FLASH_REVISION_B  = QLIB_FLASH_REVISION_B,
    QLIB_3_FLASH_REVISION_D  = QLIB_FLASH_REVISION_C,
    QLIB_31_FLASH_REVISION_A = QLIB_FLASH_REVISION_A
} QLIB_FLASH_REVISION_T;

/************************************************************************************************************
 * Flash Supply Voltage
************************************************************************************************************/
typedef enum
{
    QLIB_FLASH_VOLTAGE_UNKNOWN,
    QLIB_FLASH_VOLTAGE_1_8V,
    QLIB_FLASH_VOLTAGE_3_3V
} QLIB_FLASH_VOLTAGE_T;

/************************************************************************************************************
 * Hardware Version - General Information
************************************************************************************************************/
typedef struct
{
    QLIB_FLASH_SIZE_T     flashSize;   ///< Flash size
    bool                  isSingleDie; ///< Single/Multi die
    QLIB_FLASH_SIZE_T     dieSize;     ///< Single Die size
    QLIB_FLASH_VOLTAGE_T  voltage;     ///< Flash voltage
    QLIB_FLASH_REVISION_T revision;    ///< Target revision
    uint32_t              dieType;     ///< QLIB DIE type as defined in @ref qlib_targets.h e.g w77q25nw_revD
} QLIB_HW_INFO_T;

/************************************************************************************************************
 * Hardware Version
************************************************************************************************************/
typedef struct QLIB_HW_VER_T
{
    QLIB_STD_HW_VER_T std;  ///< standard HW information
    QLIB_SEC_HW_VER_T sec;  ///< secure HW version
    QLIB_HW_INFO_T    info; ///< HW information
} QLIB_HW_VER_T;

/************************************************************************************************************
 * QLIB software version information
************************************************************************************************************/

typedef struct
{
    uint32_t qlibVersion; ///< SW version
    uint32_t qlibTarget; ///< Target information for which QLIB was compiled, as defined in @ref qlib_targets.h
} QLIB_SW_VERSION_T;


/************************************************************************************************************
 * HW log data
************************************************************************************************************/
typedef TYPE_128BIT LOG_ENTRY_T;

/************************************************************************************************************
 * QLIB Reset Status
************************************************************************************************************/
#define QLIB_MAX_DIES 8u
PACKED_START
typedef struct QLIB_RESET_STATUS_T
{
    uint32_t powerOnReset : 1;                  // 1==HW reset, 0==SW reset (by software command)
    uint32_t fallbackRemapping : QLIB_MAX_DIES; // A bit per die, 1==Remapping occurred in die
    uint32_t watchdogReset : 1;                 // 1==Watchdog reset occurred
    uint32_t reserved : (30u - QLIB_MAX_DIES);
} PACKED QLIB_RESET_STATUS_T;
PACKED_END

/************************************************************************************************************
 * Key type
************************************************************************************************************/
typedef enum
{
    QLIB_KID__RESTRICTED_ACCESS_SECTION = 0x00,
    QLIB_KID__FULL_ACCESS_SECTION       = 0x10,
    QLIB_KID__SECTION_PROVISIONING      = 0x20,
    QLIB_KID__SECTION_LMS               = 0x30,
    QLIB_KID__DEVICE_SECRET = 0x8F,
    QLIB_KID__DEVICE_MASTER = 0x9F,
    QLIB_KID__DEVICE_KEY_PRE_PROVISIONING = 0xAF,
    QLIB_KID__INVALID                     = 0xFF
} QLIB_KID_TYPE_T;

/*---------------------------------------------------------------------------------------------------------*/
/* SSR bit-field structure                                                                                 */
/*---------------------------------------------------------------------------------------------------------*/
typedef struct
{
    uint32_t BUSY : 1U;
    uint32_t RESERVED_0 : 1U;
    uint32_t ERR : 1U;
    uint32_t RESERVED_1 : 1U;

    uint32_t SES_READY : 1U;
    uint32_t RESP_READY : 1U;
    uint32_t POR : 1U;
    uint32_t FB_REMAP : 1U;

    uint32_t AWDT_EXP : 1U;
    uint32_t RESERVED_2 : 1U;
    uint32_t SES_ERR_S : 1U;
    uint32_t SPIF_ERR : 1U;

    uint32_t INTG_ERR_S : 1U;
    uint32_t AUTH_ERR_S : 1U;
    uint32_t PRIV_ERR_S : 1U;
    uint32_t IGNORE_ERR_S : 1U;

    uint32_t SYS_ERR_S : 1U;
    uint32_t FLASH_ERR_S : 1U;
    uint32_t RESERVED_3 : 1U;
    uint32_t MC_ERR : 1U;

    uint32_t MC_MAINT : 2U;
    uint32_t SUSPEND_E : 1U;
    uint32_t SUSPEND_W : 1U;

    uint32_t STATE : 3U;
    uint32_t FULL_PRIV : 1U;

    uint32_t KID : 4U;
} QLIB_REG_SSR_STRUCT_T;

/*---------------------------------------------------------------------------------------------------------*/
/* SSR bitfield-uint32_t union                                                                             */
/*---------------------------------------------------------------------------------------------------------*/
typedef union
{
    uint32_t              asUint;
    QLIB_REG_SSR_STRUCT_T asStruct;
} QLIB_REG_SSR_T;

/*---------------------------------------------------------------------------------------------------------*/
/* ESSR bit-field structure                                                                                */
/*---------------------------------------------------------------------------------------------------------*/
typedef struct
{
    QLIB_REG_SSR_STRUCT_T SSR; // note that SSR busy bit is always set for GET_ESSR command
    uint32_t              DIE_ID_u32 : 3;
    uint32_t              RESERVED_1_u32 : 1;
    uint32_t              RNG_RDY_u32 : 1;
    uint32_t              AWDT_75_u32 : 1;
    uint32_t              CMVP_DONE_u32 : 1;
    uint32_t              CMVP_PASS_u32 : 1;
    uint32_t              RESERVED_2_u32 : 20;
    uint32_t              KID_MSB_u32 : 4;
} QLIB_REG_ESSR_STRUCT_T;

/*---------------------------------------------------------------------------------------------------------*/
/* ESSR bitfield-uint64_t union                                                                                 */
/*---------------------------------------------------------------------------------------------------------*/
typedef union
{
    uint32_t               asUint32;
    uint64_t               asUint64;
    QLIB_REG_ESSR_STRUCT_T asStruct;
} QLIB_REG_ESSR_T;

/************************************************************************************************************
 * This type determines the memory size allocated to The Vault and to the Replay Protection Monotonic Counter
************************************************************************************************************/
typedef enum
{
    QLIB_VAULT_DISABLED_RPMC_8_COUNTERS = 0, ///< Vault is disabled. RPMC (if supported) implements 8 counters.
    QLIB_VAULT_64KB_RPMC_4_COUNTERS     = 2, ///< Vault uses 64kB. RPMC (if supported) implements 4 counters.
    QLIB_VAULT_128KB_RPMC_DISABLED      = 3  ///< Vault uses 128kB. RPMC is disabled.
} QLIB_VAULT_RPMC_CONFIG_T;

/************************************************************************************************************
 * Flash monotonic counter
************************************************************************************************************/
typedef uint32_t QLIB_MC_T[2];

/************************************************************************************************************
 * Flash monotonic counter offsets
************************************************************************************************************/
typedef enum
{
    TC  = 0, ///< Offset to TC part in @ref QLIB_MC_T
    DMC = 1  ///< Offset to DMC part in @ref QLIB_MC_T
} QLIB_MC_OFFSET_T;

/*---------------------------------------------------------------------------------------------------------*/
/* Keys                                                                                                    */
/*---------------------------------------------------------------------------------------------------------*/
typedef KEY_T           KEY_ARRAY_T[QLIB_NUM_OF_SECTIONS];
typedef const uint32_t* CONST_KEY_P_T;
typedef CONST_KEY_P_T   KEY_P_ARRAY_T[QLIB_NUM_OF_SECTIONS];
typedef LMS_PUB_KEY_T   QLIB_LMS_KEY_ARRAY_T[QLIB_NUM_OF_SECTIONS];
typedef TYPE_320BIT     QLIB_LMS_KEY_T;
/************************************************************************************************************
 * Key ID
************************************************************************************************************/
typedef uint8_t QLIB_KID_T;

/*---------------------------------------------------------------------------------------------------------*/
/* Dummy cycles                                                                                            */
/*---------------------------------------------------------------------------------------------------------*/
// for W77Q (16Mb to 128Mb) flash that supports Set Read Parameters command
#ifndef QLIB_QPI_READ_DUMMY_CYCLES
#define QLIB_QPI_READ_DUMMY_CYCLES (8u) // set to max value to support all frequencies
#else
#if (QLIB_QPI_READ_DUMMY_CYCLES != 4u) && (QLIB_QPI_READ_DUMMY_CYCLES != 6u) && (QLIB_QPI_READ_DUMMY_CYCLES != 8u)
#error Value of QLIB_QPI_READ_DUMMY_CYCLES is not supported
#endif // check supported values
#endif // QLIB_QPI_READ_DUMMY_CYCLES

/************************************************************************************************************
 * SPI interface configuration parameters
************************************************************************************************************/
typedef struct QLIB_INTERFACE_CONFIG_T
{
    QLIB_BUS_FORMAT_T    busFormat;
    QLIB_STD_ADDR_MODE_T addrMode;
    uint8_t              dummyCycles;
} QLIB_INTERFACE_CONFIG_T;

/************************************************************************************************************
* SET_SCR command mode field
************************************************************************************************************/
#define QLIB_SEC_CMD_SET_SCR_RESET_MODE_FIELD  0u, 1u
#define QLIB_SEC_CMD_SET_SCR_RELOAD_MODE_FIELD 1u, 1u
#define QLIB_SEC_CMD_SET_SCR_DIGEST_MODE_FIELD 2u, 1u

#define QLIB_SEC_CMD_SET_SCR_MODE_SET(mode, reset, reload, digestExt)                                                    \
    {                                                                                                                    \
        SET_VAR_FIELD((mode), QLIB_SEC_CMD_SET_SCR_RESET_MODE_FIELD, ((reset) == (bool)true) ? 0x1u : 0u, uint8_t);      \
        SET_VAR_FIELD((mode), QLIB_SEC_CMD_SET_SCR_RELOAD_MODE_FIELD, ((reload) == (bool)true) ? 0x1u : 0u, uint8_t);    \
        SET_VAR_FIELD((mode), QLIB_SEC_CMD_SET_SCR_DIGEST_MODE_FIELD, ((digestExt) == (bool)true) ? 0x1u : 0u, uint8_t); \
    }

#ifdef __cplusplus
}
#endif // __cplusplus


#endif // QLIB_TYPES_H
