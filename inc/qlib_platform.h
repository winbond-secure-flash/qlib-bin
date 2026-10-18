/************************************************************************************************************
* @internal
* @remark     Winbond Electronics Corporation
* @copyright  Copyright (c) 2019 by Winbond Electronics Corporation . All rights reserved
* @endinternal
*
* @file       qlib_platform.h
* @brief      This file includes platform specific definitions
*
* ### project qlib
*
************************************************************************************************************/
#ifndef QLIB_PLATFORM_H__
#define QLIB_PLATFORM_H__

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

/*-----------------------------------------------------------------------------------------------------------
                                           DEFINITIONS
-----------------------------------------------------------------------------------------------------------*/
#ifndef PLAT_API
#define PLAT_API
#endif // PLAT_API

/*-----------------------------------------------------------------------------------------------------------
                                                   INCLUDES
-----------------------------------------------------------------------------------------------------------*/
#include <stdint.h>
#include <stdbool.h>
/*-----------------------------------------------------------------------------------------------------------
                                             QLIB CONFIGURATIONS
-----------------------------------------------------------------------------------------------------------*/

/************************************************************************************************************
 * define QLIB_MAX_SPI_INPUT_SIZE to SPI input buffer limitation. If defined PLAT_SPI_WriteReadTransaction
 * will be invoked in chunks of that limit.
************************************************************************************************************/
//example for 1024 bytes limit
//#define QLIB_MAX_SPI_INPUT_SIZE 1024
#ifndef QLIB_MAX_SPI_INPUT_SIZE
#define QLIB_MAX_SPI_INPUT_SIZE 256
#endif // QLIB_MAX_SPI_INPUT_SIZE

/************************************************************************************************************
 * define QLIB_MAX_SPI_OUTPUT_SIZE to SPI output_pu32 buffer limitation. The buffer includes the CTAG and DATA bytes.
 * If defined, PLAT_SPI_WriteReadTransaction for secure commands will be invoked in chunks of that limit.
 * In w77q64jw, w77q128jw, w77q64jv and w77q128jv this value can not be less then 4 bytes
 * In other w77q/t this value should be at least 8 bytes, since the CTAG is 4 bytes long.
 * This define is relevant only for flash that supports SPLIT_IBUF_FEATURE
 *
 * example (w77q64jw, w77q128jw, w77q64jv and w77q128jv):
 * The SET_GMT command format is [OP1, CTAG (32 bits), GMT (160 bits), SIG (64 bits)]
 * If QLIB_MAX_SPI_OUTPUT_SIZE is 16 bytes,
 *
 * [OP1, CTAG (4 bytes), GMT (first 12 bytes)]
 * [OP1, GMT (next 8 bytes), SIG (8 bytes)]
 *
 * example in other flash (e.g W77t) for same conditions:
 * [OP1, CTAG (4 bytes), GMT (first 12 bytes)]
 * [OP1, DUMMY (4 bytes), GMT (next 8 bytes), SIG (first 4 bytes)]
 * [OP1, DUMMY (4 bytes), SIG (next 4 bytes)]
 ************************************************************************************************************/
//example for 16 bytes limit
//#define QLIB_MAX_SPI_OUTPUT_SIZE 16

/************************************************************************************************************
 * Enable async HASH implementation if available
************************************************************************************************************/
//#define QLIB_HASH_OPTIMIZATION_ENABLED
//#define QLIB_SPI_OPTIMIZATION_ENABLED

/************************************************************************************************************
 * define SPI_INIT_ADDRESS_MODE_4_BYTES if the core operates in 4 bytes address mode on its initialization.
 * by default the flash powers up in 3 bytes address mode. If the user wants the flash to power up
 * in 4 bytes address mode, this definition is required.
 * This configuration is relevant only for flash that supports 4 bytes address mode
 ************************************************************************************************************/
//#define SPI_INIT_ADDRESS_MODE_4_BYTES

/*-----------------------------------------------------------------------------------------------------------
                                                 QLIB DEFINES
-----------------------------------------------------------------------------------------------------------*/

/************************************************************************************************************
 * SPI bus mode
************************************************************************************************************/
typedef enum QLIB_BUS_MODE_T
{
    QLIB_BUS_MODE_INVALID = 0u,
    QLIB_BUS_MODE_1_1_1   = 1u,
    QLIB_BUS_MODE_1_1_2   = 2u,
    QLIB_BUS_MODE_1_2_2   = 3u,
    QLIB_BUS_MODE_1_1_4   = 4u,
    QLIB_BUS_MODE_1_4_4   = 5u,
    QLIB_BUS_MODE_4_4_4   = 6u,
    QLIB_BUS_MODE_1_8_8   = 7u,
    QLIB_BUS_MODE_8_8_8   = 8u,
    QLIB_BUS_MODE_MAX     = QLIB_BUS_MODE_8_8_8
} QLIB_BUS_MODE_T;

/************************************************************************************************************
 * SPI commands Flags
************************************************************************************************************/
#define QLIB_SPI_FLAGS__DATA_PHASE_DTR (1u << 0u)
#define QLIB_SPI_FLAGS__ADDR_PHASE_DTR (1u << 1u)
#define QLIB_SPI_FLAGS__CMD_PHASE_DTR  (1u << 2u)

/************************************************************************************************************
 * This type contains dual transfer rate for SPI commands
************************************************************************************************************/
#define QLIB_DTR__NO_DTR    0u
#define QLIB_DTR__ADDR_DATA (QLIB_SPI_FLAGS__DATA_PHASE_DTR | QLIB_SPI_FLAGS__ADDR_PHASE_DTR)
#define QLIB_DTR__ALL       (QLIB_SPI_FLAGS__DATA_PHASE_DTR | QLIB_SPI_FLAGS__ADDR_PHASE_DTR | QLIB_SPI_FLAGS__CMD_PHASE_DTR)
#define QLIB_DTR_MASK       QLIB_DTR__ALL

/************************************************************************************************************
 * Hash optimization options
 * QLIB can send optimization request for hash operation. this is optional, user may ignore it
************************************************************************************************************/
typedef enum QLIB_HASH_OPT_T
{
    QLIB_HASH_OPT_NONE,            ///< No optimization
    QLIB_HASH_OPT_FIXED_55_ALIGNED ///< following hash operation includes exactly 55u bytes in one update operation \n
                                   ///< and data pointer is 4u bytes aligned (optimize SRD/SARD)
} QLIB_HASH_OPT_T;

/*-----------------------------------------------------------------------------------------------------------
                                            QLIB DEFINE OVERRIDES
-----------------------------------------------------------------------------------------------------------*/

/************************************************************************************************************
 * @brief Atomic compare and exchange operation
 *
 * This macro performs an atomic compare-and-exchange operation on a boolean variable_b.
 * If the current value equals the expected value, it updates to the desired value and returns true.
 * Otherwise, it returns false without modifying the variable_b.
 *
 * The implementation should be atomic and thread/interrupt safe.
 * The default implementation is not atomic - suitable when QLIB is used as single user of the SPI bus.
 * User should override this macro with proper atomic operation if needed.
 *
 * @param[in,out] ptr       Pointer to a bool variable_b to operate on
 * @param[in]     expected  Expected current value
 * @param[in]     desired   Desired new value
 *
 * @return true if exchange was successful (value was expected and updated to desired), false otherwise
 *
 ************************************************************************************************************/
#ifndef QLIB_SINGLE_SPI_BUS_USER
#ifndef PLATFORM_ATOMIC_COMPARE_EXCHANGE
// user should override this macro if QLIB is not the single user of the platform SPI bus
#define PLATFORM_ATOMIC_COMPARE_EXCHANGE(ptr, expected, desired) \
    ((*(ptr)) == (expected) ? ((*(ptr)) = (desired), (bool)true) : (bool)false)
#endif // PLATFORM_ATOMIC_COMPARE_EXCHANGE
#endif // QLIB_SINGLE_SPI_BUS_USER

/************************************************************************************************************
 * @brief Declare a variable_b for SPI bus lock context
 *
 * This macro declares a variable_b that will be used by PLATFORM_SPI_BUS_LOCK and PLATFORM_SPI_BUS_UNLOCK.
 * The implementation depends on the locking mechanism (e.g., interrupt state, semaphore handle).
 * Default implementation is empty (no-op) - suitable when QLIB is used as single user of the SPI bus.
 *
 * @param[in] var_u32   Variable name to declare
 *
 * Example implementations:
 * - Interrupt-based: \#define PLATFORM_SPI_BUS_LOCK_DECLARE(var_u32) uint32_t var_u32 = 0u
 ************************************************************************************************************/
#ifndef PLATFORM_SPI_BUS_LOCK_DECLARE
#define PLATFORM_SPI_BUS_LOCK_DECLARE(var_u32) \
    {                                          \
    }
#endif // PLATFORM_SPI_BUS_LOCK_DECLARE

/************************************************************************************************************
 * @brief Acquire SPI bus lock
 *
 * This macro should acquire exclusive access to the SPI bus. It should prevent:
 * - Concurrent access from other places, including other threads, direct flash access,
 *   Interruption by ISRs that may use the same SPI bus
 * Default implementation is empty (no-op) - suitable when QLIB is used as single user of the SPI bus.
 *
 * @param[in,out] var_u32   Lock context variable_b (declared by PLATFORM_SPI_BUS_LOCK_DECLARE)
 *
 * Example implementations:
 * - Interrupt disable: \#define PLATFORM_SPI_BUS_LOCK(var_u32) { var_u32 = DisableGlobalIRQ(); }
 ************************************************************************************************************/
#ifndef PLATFORM_SPI_BUS_LOCK
#define PLATFORM_SPI_BUS_LOCK(var_u32) \
    {                                  \
    }
#endif // PLATFORM_SPI_BUS_LOCK

/************************************************************************************************************
 * @brief Release SPI bus lock
 *
 * This macro should release the SPI bus lock acquired by PLATFORM_SPI_BUS_LOCK.
 * It should restore the previous interrupt/lock state.
 * Default implementation is empty (no-op) - suitable when QLIB is used as single user of the SPI bus.
 *
 * @param[in,out] var_u32   Lock context variable_b (declared by PLATFORM_SPI_BUS_LOCK_DECLARE)
 *
 * Example implementations:
 * - Interrupt restore: \#define PLATFORM_SPI_BUS_UNLOCK(var_u32) { EnableGlobalIRQ(var_u32); }
 ************************************************************************************************************/
#ifndef PLATFORM_SPI_BUS_UNLOCK
#define PLATFORM_SPI_BUS_UNLOCK(var_u32) \
    {                                    \
    }
#endif // PLATFORM_SPI_BUS_UNLOCK

/************************************************************************************************************
 * @brief   Define the LMS attestation Merkle Tree height, which determines the maximal number of signatures.
 *          The OST number is limited to (2^h - 1)
 *          Relevant only for flash that supports LMS attestation feature (W77Q/T 256Mb to 1Gb)
 *          QLIB supported values are 10 or 20
************************************************************************************************************/
#ifndef QLIB_LMS_ATTEST_TREE_HEIGHT
#define QLIB_LMS_ATTEST_TREE_HEIGHT 10u
#endif // QLIB_LMS_ATTEST_TREE_HEIGHT

/*-----------------------------------------------------------------------------------------------------------
                                         PLATFORM SPECIFIC FUNCTIONS
-----------------------------------------------------------------------------------------------------------*/
/************************************************************************************************************
 * @brief       This function resets the core CPU
************************************************************************************************************/
PLAT_API void CORE_RESET(void);

/************************************************************************************************************
 * @brief       This function initialize HASH context\n
 *
 * @param[out]  ctx_pv     Hash context
 * @param[in]   opt     Hash optimization option
 *
 * @return
 * 0                      - no error occurred\n
 * non-zero               - error occurred
 ************************************************************************************************************/
PLAT_API int PLAT_HASH_Init(void** ctx_pv, QLIB_HASH_OPT_T opt);

/************************************************************************************************************
 * @brief       This function adds data to current HASH calculation.\n
 * This function can be called repeatedly with an arbitrary amount of data to be hashed.\n
 * This function is an implementation of the hash function supported by the W77Q defined in the spec.\n
 * For performance reasons, it is recommended to use HW implementation of this function.\n
 * Test vectors can be found in the spec TBD
 *
 * @param[in,out]  ctx_pv        Hash context
 * @param[in]      data_pv       Input data
 * @param[in]      dataSize_u32   Input data_pv size in bytes
 *
 * @return
 * 0                      - no error occurred\n
 * non-zero               - error occurred
 ************************************************************************************************************/
PLAT_API int PLAT_HASH_Update(void* ctx_pv, const void* data_pv, uint32_t dataSize_u32);

/************************************************************************************************************
 * @brief       Finalize hashing and erases the context.
 *
 * @param[in,out]  ctx_pv        Hash context
 * @param[out]     output_pu32     digest
 *
* @return
 * 0                      - no error occurred\n
 * non-zero               - error occurred
 ************************************************************************************************************/
PLAT_API int PLAT_HASH_Finish(void* ctx_pv, uint32_t* output_pu32);


#ifdef USE_CONSTANT_NONCE
/************************************************************************************************************
 * @brief       This function sets a constant 'nonce' number.
 *
* @return
 * 0                      - no error occurred\n
 * non-zero               - error occurred
************************************************************************************************************/
PLAT_API int PLAT_SetNONCE(uint64_t constNonce_u64);
#endif // USE_CONSTANT_NONCE

/************************************************************************************************************
 * @brief       This function returns non-repeating 'nonce' number.
 * A 'nonce' is a 64bit number that is used in session establishment.\n
 * To prevent replay attacks such nonce must be 'non-repeating' - appear different each function execution.\n
 * Typically implemented as a HW TRNG.
 *
 * @return      64 bit random number
************************************************************************************************************/
PLAT_API uint64_t PLAT_GetNONCE(void);

/************************************************************************************************************
 * @brief       This routine performs SPI write-read transaction.
 * This function should be linked to RAM memory.\n
 * In order to verify that the function works properly, please use the following wave form examples:\n\n
 * PLAT_SPI_WriteReadTransaction(format=1_1_1, flags_u32=0, dataOutStream_pu8=[0x9F], \n
 * cmdSize_u32=1, addressSize_u32=0, dataOutSize_u32=0, dummy=0, dataIn_pu8=Ptr, dataInSize_u32=3)\n
 * ![wave form example 1](spi_wave_form_1.png)\n\n
 * PLAT_SPI_WriteReadTransaction(format=1_1_1, flags_u32=0, dataOutStream_pu8=[0x0B, 0x00, 0x00, 0x00], \n
 * cmdSize_u32=1, addressSize_u32=3, dataOutSize_u32=0, dummy=8, dataIn_pu8=Ptr, dataInSize_u32=32)\n
 * ![wave form example 2](spi_wave_form_2.png)
 * The system integrator needs to allocate sufficient space in the SPI controller on the MCU side to drive the command, address, \n
 * and dataOut for the SPI transaction.
 * Sufficient space should be allocated in the SPI controller to sample dataIn_pu8.
 * The maximal sizes for the fields are specified in the routine's interface below @p dataOutSize_u32 and @p dataInSize_u32 parameters.
 *
 * @param[in,out]   userData_pv        User data which is set using @ref QLIB_SetUserData
 * @param[in]       format          SPI format
 * @param[in]       flags_u32           SPI flags, including DTR flags. For supported flags refer to QLIB_SPI_FLAGS definitions above.
 * @param[in]       dataOutStream_pu8   pointer to a buffer with SPI output information: SPI command followed by address and  dataOut
 * @param[in]       cmdSize_u32         Number of SPI command bytes in dataOutStream_pu8 buffer
 * @param[in]       addressSize_u32     Number of address bytes in dataOutStream_pu8 buffer
 * @param[in]       dataOutSize_u32     Number of dataOut bytes in dataOutStream_pu8 buffer.
 * @param[in]       dummyCycles_u32     Dummy cycles between write and read phases
 * @param[out]      dataIn_pu8          pointer to a buffer which holds the data received
 * @param[in]       dataInSize_u32      data received size in bytes.
 *
 * @return
 * QLIB_STATUS__OK = 0                      - no error occurred\n
 * QLIB_STATUS__(ERROR)                     - Other error
************************************************************************************************************/
PLAT_API int PLAT_SPI_WriteReadTransaction(const void*     userData_pv,
                                           QLIB_BUS_MODE_T format,
                                           uint32_t        flags_u32,
                                           const uint8_t*  dataOutStream_pu8,
                                           uint32_t        cmdSize_u32,
                                           uint32_t        addressSize_u32,
                                           uint32_t        dataOutSize_u32,
                                           uint32_t        dummyCycles_u32,
                                           uint8_t*        dataIn_pu8,
                                           uint32_t        dataInSize_u32);

#ifdef QLIB_SPI_OPTIMIZATION_ENABLED

/************************************************************************************************************
 * @brief       This routine performs SPI multi transaction start.
 * When using the same SPI command, `PLAT_SPI_MultiTransactionStart` saves the command in dedicated SPI cache.\n
 * Next time the command will be called it will be taken directly from the SPI cache thus saving calculation time
************************************************************************************************************/
PLAT_API void PLAT_SPI_MultiTransactionStart(void);

/************************************************************************************************************
 * @brief       This routine performs SPI multi transaction stop.
 * This function stops the usage of SPI cache saved while calling `PLAT_SPI_MultiTransactionStart`
************************************************************************************************************/
PLAT_API void PLAT_SPI_MultiTransactionStop(void);

#endif //QLIB_SPI_OPTIMIZATION_ENABLED


#ifdef __cplusplus
}
#endif // __cplusplus

#endif // QLIB_PLATFORM_H__
