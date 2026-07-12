/*---------------------------------------------------------------------------------------------------------*/
/*  Winbond Electronics Corporation                                                                        */
/*                                                                                                         */
/*  Copyright (c) 2019 by Winbond Electronics Corporation                                                  */
/*  All rights reserved                                                                                    */
/*                                                                                                         */
/*<<<------------------------------------------------------------------------------------------------------*/
/* File Contents:                                                                                          */
/*    defs_errors.h                                                                                        */
/*            This file contains standard error codes set                                                  */
/* Project:                                                                                                */
/*            SWC DEFS                                                                                     */
/*---------------------------------------------------------------------------------------------------------*/

#ifndef DEFS_ERRORS_H__
#define DEFS_ERRORS_H__

/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
/*                                           ERROR CODE MASKING                                            */
/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
#ifndef DEFS_STATUS_MASK
#ifndef DEFS_STATUS_OK_MASK
#define DEFS_STATUS_OK_MASK 0u
#endif
#ifndef DEFS_STATUS_ERROR_MASK
#define DEFS_STATUS_ERROR_MASK 0u
#endif

#ifndef DEFS_STATUS_SEC_ERROR_MASK
#define DEFS_STATUS_SEC_ERROR_MASK DEFS_STATUS_ERROR_MASK
#endif
#else
#define DEFS_STATUS_OK_MASK        DEFS_STATUS_MASK
#define DEFS_STATUS_ERROR_MASK     DEFS_STATUS_MASK
#define DEFS_STATUS_SEC_ERROR_MASK DEFS_STATUS_MASK
#endif

// clang-format off
/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
/*                                               ERROR CODES                                               */
/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
/**
 * @brief Standard status/error codes.
 *
 * Every API function that can fail returns a value of this type.
 * Check for @c DEFS_STATUS_OK on success; any other value indicates an error.
 */
typedef enum
{
    DEFS_STATUS_OK   = DEFS_STATUS_OK_MASK | 0x00u,
    DEFS_STATUS_FAIL = DEFS_STATUS_ERROR_MASK | 0x01u,

    /*-----------------------------------------------------------------------------------------------------*/
    /* Parameters validity                                                                                 */
    /*-----------------------------------------------------------------------------------------------------*/
    DEFS_STATUS_INVALID_PARAMETER = DEFS_STATUS_ERROR_MASK | 0x10u,

    DEFS_STATUS_INVALID_DATA_SIZE            = DEFS_STATUS_ERROR_MASK | 0x11u,
    DEFS_STATUS_PARAMETER_OUT_OF_RANGE       = DEFS_STATUS_ERROR_MASK | 0x12u,
    DEFS_STATUS_INVALID_DATA_FIELD           = DEFS_STATUS_ERROR_MASK | 0x13u,
    DEFS_STATUS_CORRUPTED_VALUE              = DEFS_STATUS_ERROR_MASK | 0x14u,
    DEFS_STATUS_INVALID_NUMBER_OF_PARAMETERS = DEFS_STATUS_ERROR_MASK | 0x15u,
    DEFS_STATUS_INVALID_DATA_ALIGNMENT       = DEFS_STATUS_ERROR_MASK | 0x16u,

    /*-----------------------------------------------------------------------------------------------------*/
    /* Response                                                                                            */
    /*-----------------------------------------------------------------------------------------------------*/
    DEFS_STATUS_RESPONSE_CANT_BE_PROVIDED = DEFS_STATUS_ERROR_MASK | 0x20u,

    DEFS_STATUS_SYSTEM_BUSY               = DEFS_STATUS_ERROR_MASK | 0x21u,
    DEFS_STATUS_SYSTEM_NOT_INITIALIZED    = DEFS_STATUS_ERROR_MASK | 0x22u,
    DEFS_STATUS_SYSTEM_IN_INCORRECT_STATE = DEFS_STATUS_ERROR_MASK | 0x23u,
    DEFS_STATUS_RESPONSE_TIMEOUT          = DEFS_STATUS_ERROR_MASK | 0x24u,
    DEFS_STATUS_RESPONSE_ABORT            = DEFS_STATUS_ERROR_MASK | 0x25u,

    /*-----------------------------------------------------------------------------------------------------*/
    /* Security                                                                                            */
    /*-----------------------------------------------------------------------------------------------------*/
    DEFS_STATUS_SECURITY_ERROR = DEFS_STATUS_SEC_ERROR_MASK | 0x30u,

    DEFS_STATUS_INSUFFICIENT_PRIVILEGES_LEVEL = DEFS_STATUS_SEC_ERROR_MASK | 0x31u,
    DEFS_STATUS_AUTHENTICATION_FAIL           = DEFS_STATUS_SEC_ERROR_MASK | 0x32u,
    DEFS_STATUS_BAD_SIGNATURE                 = DEFS_STATUS_SEC_ERROR_MASK | 0x33u,
    DEFS_STATUS_LOCKED                        = DEFS_STATUS_SEC_ERROR_MASK | 0x34u,
    DEFS_STATUS_INTEGRITY_FAILED              = DEFS_STATUS_SEC_ERROR_MASK | 0x35u,
    DEFS_STATUS_CHECKSUM_FAILED               = DEFS_STATUS_SEC_ERROR_MASK | 0x36u,
    DEFS_STATUS_ILLEGAL_ACCESS                = DEFS_STATUS_SEC_ERROR_MASK | 0x37u,
    DEFS_STATUS_ILLEGAL_CONFIGURATION         = DEFS_STATUS_SEC_ERROR_MASK | 0x38u,
    DEFS_STATUS_PERMANENT_LOCK                = DEFS_STATUS_SEC_ERROR_MASK | 0x39u,

    /*-----------------------------------------------------------------------------------------------------*/
    /* Communication                                                                                       */
    /*-----------------------------------------------------------------------------------------------------*/
    DEFS_STATUS_COMMUNICATION_ERROR = DEFS_STATUS_ERROR_MASK | 0x40u,

    DEFS_STATUS_NO_CONNECTION           = DEFS_STATUS_ERROR_MASK | 0x41u,
    DEFS_STATUS_CANT_OPEN_CONNECTION    = DEFS_STATUS_ERROR_MASK | 0x42u,
    DEFS_STATUS_CONNECTION_ALREADY_OPEN = DEFS_STATUS_ERROR_MASK | 0x43u,
    DEFS_STATUS_COMMAND_ILLEGAL         = DEFS_STATUS_ERROR_MASK | 0x44u,
    DEFS_STATUS_COMMAND_UNKNOWN         = DEFS_STATUS_ERROR_MASK | 0x45u,
    DEFS_STATUS_COMMAND_IGNORED         = DEFS_STATUS_ERROR_MASK | 0x46u,
    DEFS_STATUS_COMMAND_FAILED          = DEFS_STATUS_ERROR_MASK | 0x47u,

    /*-----------------------------------------------------------------------------------------------------*/
    /* Hardware                                                                                            */
    /*-----------------------------------------------------------------------------------------------------*/
    DEFS_STATUS_HARDWARE_ERROR = DEFS_STATUS_ERROR_MASK | 0x50u,

    DEFS_STATUS_IO_ERROR  = DEFS_STATUS_ERROR_MASK | 0x51u,
    DEFS_STATUS_CLK_ERROR = DEFS_STATUS_ERROR_MASK | 0x52u,

    /*-----------------------------------------------------------------------------------------------------*/
    /* Implementation                                                                                      */
    /*-----------------------------------------------------------------------------------------------------*/
    DEFS_STATUS_NOT_IMPLEMENTED = DEFS_STATUS_ERROR_MASK | 0x60u,

    DEFS_STATUS_IMPLEMENTATION_ERROR = DEFS_STATUS_ERROR_MASK | 0x61u,
    DEFS_STATUS_DEPRECATED_FUNCTION  = DEFS_STATUS_ERROR_MASK | 0x62u,
    DEFS_STATUS_NOT_SUPPORTED        = DEFS_STATUS_ERROR_MASK | 0x63u,

    /*-----------------------------------------------------------------------------------------------------*/
    /* File                                                                                                */
    /*-----------------------------------------------------------------------------------------------------*/
    DEFS_STATUS_FILE_CANT_OPEN = DEFS_STATUS_ERROR_MASK | 0x70u,
    DEFS_STATUS_FILE_NOT_FOUND = DEFS_STATUS_ERROR_MASK | 0x71u,

    /*-----------------------------------------------------------------------------------------------------*/
    /* Memory                                                                                              */
    /*-----------------------------------------------------------------------------------------------------*/
    DEFS_STATUS_OUT_OF_MEMORY   = DEFS_STATUS_ERROR_MASK | 0x80u,
    DEFS_STATUS_BUFFER_IS_FULL  = DEFS_STATUS_ERROR_MASK | 0x81u,
    DEFS_STATUS_BUFFER_IS_EMPTY = DEFS_STATUS_ERROR_MASK | 0x82u,
    DEFS_STATUS_BUFFER_OVERFLOW = DEFS_STATUS_ERROR_MASK | 0x83u,

    /*-----------------------------------------------------------------------------------------------------*/
    /* Custom error codes                                                                                  */
    /*-----------------------------------------------------------------------------------------------------*/
    DEFS_STATUS_CUSTOM_ERROR_00 = DEFS_STATUS_ERROR_MASK | 0xF0u,
    DEFS_STATUS_CUSTOM_ERROR_01 = DEFS_STATUS_ERROR_MASK | 0xF1u,
    DEFS_STATUS_CUSTOM_ERROR_02 = DEFS_STATUS_ERROR_MASK | 0xF2u,
    DEFS_STATUS_CUSTOM_ERROR_03 = DEFS_STATUS_ERROR_MASK | 0xF3u,
    DEFS_STATUS_CUSTOM_ERROR_04 = DEFS_STATUS_ERROR_MASK | 0xF4u,
    DEFS_STATUS_CUSTOM_ERROR_05 = DEFS_STATUS_ERROR_MASK | 0xF5u,
    DEFS_STATUS_CUSTOM_ERROR_06 = DEFS_STATUS_ERROR_MASK | 0xF6u,
    DEFS_STATUS_CUSTOM_ERROR_07 = DEFS_STATUS_ERROR_MASK | 0xF7u,
    DEFS_STATUS_CUSTOM_ERROR_08 = DEFS_STATUS_ERROR_MASK | 0xF8u,
    DEFS_STATUS_CUSTOM_ERROR_09 = DEFS_STATUS_ERROR_MASK | 0xF9u,
    DEFS_STATUS_CUSTOM_ERROR_10 = DEFS_STATUS_ERROR_MASK | 0xFAu,
    DEFS_STATUS_CUSTOM_ERROR_11 = DEFS_STATUS_ERROR_MASK | 0xFBu,
    DEFS_STATUS_CUSTOM_ERROR_12 = DEFS_STATUS_ERROR_MASK | 0xFCu,
    DEFS_STATUS_CUSTOM_ERROR_13 = DEFS_STATUS_ERROR_MASK | 0xFDu,
    DEFS_STATUS_CUSTOM_ERROR_14 = DEFS_STATUS_ERROR_MASK | 0xFEu,
    DEFS_STATUS_CUSTOM_ERROR_15 = DEFS_STATUS_ERROR_MASK | 0xFFu
} DEFS_STATUS;

/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
/*                                           ERROR CODE STRINGS                                            */
/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
#define DEFS_STATUS_ERROR_STRINGS                                                                                                \
    {                                                                                                                            \
        "No error", "General error", "", "", "", "", "", "", "", "", "", "", "", "", "", "",                                     \
                                                                                                                                 \
            "Invalid parameter", "Invalid data size", "Parameter is out of range", "Invalid data field", "Value is corrupted",   \
            "Invalid number of parameters", "Invalid data alignment", "", "", "", "", "", "", "", "", "",                        \
                                                                                                                                 \
            "Response cannot be provided", "System is busy", "System is not initialized", "System is in incorrect state",        \
            "Response timeout occurred", "Response is aborted", "", "", "", "", "", "", "", "", "", "",                          \
                                                                                                                                 \
            "Security error", "Insufficient privileges level", "Authentication failed", "Bad signature", "System is locked",     \
            "Integrity check failed", "Checksum verification failed", "Illegal Access", "Illegal Configuration",                 \
            "System is permanently locked", "", "", "", "", "", "",                                                              \
                                                                                                                                 \
            "Communication error", "No connection", "Can't open connection", "Connection is already open", "Illegal Command",    \
            "Unknown Command", "Command ignored", "Command failed", "", "", "", "", "", "", "", "",                              \
                                                                                                                                 \
            "Hardware error", "IO error", "Clock error", "", "", "", "", "", "", "", "", "", "", "", "", "",                     \
                                                                                                                                 \
            "Not implemented", "Implementation error", "Function is deprecated", "Not supported", "", "", "", "", "", "", "",    \
            "", "", "", "", "",                                                                                                  \
                                                                                                                                 \
            "File cannot be open", "File not found", "", "", "", "", "", "", "", "", "", "", "", "", "", "",                     \
                                                                                                                                 \
            "Out of memory", "Buffer is full", "Buffer is empty", "Buffer overflow", "", "", "", "", "", "", "", "", "", "", "", \
            "",                                                                                                                  \
                                                                                                                                 \
            "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "",                                                      \
                                                                                                                                 \
            "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "",                                                      \
                                                                                                                                 \
            "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "",                                                      \
                                                                                                                                 \
            "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "",                                                      \
                                                                                                                                 \
            "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "",                                                      \
                                                                                                                                 \
            "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "",                                                      \
                                                                                                                                 \
            "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "",                                                      \
    }
/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
/*                                          UTILITY ERROR MACROS                                           */
/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------------------------------------*/
/* Macro:           DEFS_STATUS_RET_CHECK_ACTION                                                           */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  func      - function to check                                                          */
/*                  action    - action to execute on error                                                 */
/*                                                                                                         */
/* Returns:                                                                                                */
/* Side effects:    none                                                                                   */
/* Description:                                                                                            */
/*                  This macro checks if given function returns DEFS_STATUS_(ERROR) and perform specified  */
/*                  action                                                                                 */
/*---------------------------------------------------------------------------------------------------------*/
#define DEFS_STATUS_RET_CHECK_ACTION(func, action)             \
    {                                                          \
        DEFS_STATUS qlib_error_check_ret;                      \
                                                               \
        if ((qlib_error_check_ret = (func)) != DEFS_STATUS_OK) \
        {                                                      \
            DEFS_STATUS_LOG_ERROR;                             \
            action;                                            \
        }                                                      \
        (void)qlib_error_check_ret;                            \
    }

/*---------------------------------------------------------------------------------------------------------*/
/* Macro:           DEFS_STATUS_RET_CHECK_ACTION_NO_RET                                                    */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  func      - function to check                                                          */
/*                  action    - action to execute on error                                                 */
/*                                                                                                         */
/* Returns:                                                                                                */
/* Side effects:    none                                                                                   */
/* Description:                                                                                            */
/*                  This macro checks if given function returns DEFS_STATUS_(ERROR) and perform specified  */
/*                  action. In this macro no __ret variable is available for 'action'                      */
/*---------------------------------------------------------------------------------------------------------*/
#define DEFS_STATUS_RET_CHECK_ACTION_NO_RET(func, action) \
    {                                                     \
        if ((func) != DEFS_STATUS_OK)                     \
        {                                                 \
            DEFS_STATUS_LOG_ERROR;                        \
            action;                                       \
        }                                                 \
    }

/*---------------------------------------------------------------------------------------------------------*/
/* Macro:           DEFS_STATUS_RET_ASSERT                                                                 */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  func    - function to check                                                            */
/*                                                                                                         */
/* Returns:                                                                                                */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This macro checks if given function returns DEFS_STATUS error and asserts FALSE if     */
/*                  error occurs                                                                           */
/*---------------------------------------------------------------------------------------------------------*/
#define DEFS_STATUS_RET_CHECK_ASSERT(func) \
    {                                      \
        if ((func) != DEFS_STATUS_OK)      \
        {                                  \
            DEFS_STATUS_LOG_ERROR;         \
            ASSERT(false)                  \
        }                                  \
    }

/*---------------------------------------------------------------------------------------------------------*/
/* Macro:           DEFS_STATUS_RET_CHECK_GOTO                                                             */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  func         - function to check                                                       */
/*                  err          - variable to hold error value                                            */
/*                  err_label    - error label                                                             */
/*                                                                                                         */
/* Returns:                                                                                                */
/* Side effects:    none                                                                                   */
/* Description:                                                                                            */
/*                  This routine checks if given function returns DEFS_STATUS error, sets error variable   */
/*                  and jumps to given label                                                               */
/*---------------------------------------------------------------------------------------------------------*/
#define DEFS_STATUS_RET_CHECK_GOTO(func, err, err_label) \
    DEFS_STATUS_RET_CHECK_ACTION(func, {                 \
        (err) = qlib_error_check_ret;                    \
        goto err_label;                                  \
    })

/*---------------------------------------------------------------------------------------------------------*/
/* Macro:           DEFS_STATUS_RET_CHECK_VAL                                                              */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  func    - function to check                                                            */
/*                  retVal  - return value                                                                 */
/*                                                                                                         */
/* Returns:                                                                                                */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This macro checks if give function returns DEFS_STATUS error, and returns the error    */
/*                  immediately                                                                            */
/*                                                                                                         */
/* Example:                                                                                                */
/*                                                                                                         */
/*    DEFS_STATUS myFunc(INT p1, INT p2);                                                                  */
/*                                                                                                         */
/*    INT32 otherFunc                                                                                      */
/*    {                                                                                                    */
/*        some code here...                                                                                */
/*        Call myFunc...                                                                                   */
/*        DEFS_STATUS_RET_CHECK(myFunc(p1,p2), -1);                                                        */
/*        ...                                                                                              */
/*    }                                                                                                    */
/*                                                                                                         */
/*                                                                                                         */
/*---------------------------------------------------------------------------------------------------------*/
#define DEFS_STATUS_RET_CHECK_VAL(func, retVal) DEFS_STATUS_RET_CHECK_ACTION_NO_RET(func, DEFS_STATUS_RETURN_ACTION(retVal))

/*---------------------------------------------------------------------------------------------------------*/
/* Macro:           DEFS_STATUS_COND_CHECK_ACTION                                                          */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  cond      - Condition to check                                                         */
/*                  action    - Action to execute if condition is not met                                  */
/*                                                                                                         */
/* Returns:                                                                                                */
/* Side effects:    none                                                                                   */
/* Description:                                                                                            */
/*                  This routine checks if given condition is not met, and performs specific action        */
/*---------------------------------------------------------------------------------------------------------*/
#define DEFS_STATUS_COND_CHECK_ACTION(cond, action) \
    {                                               \
        if (!(cond))                                \
        {                                           \
            DEFS_STATUS_LOG_ERROR;                  \
            action;                                 \
        }                                           \
    }

/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
/*                                       CONTROLLABLE ERROR MACROS                                         */
/*                                                                                                         */
/*  These macros are controlled by following flags:                                                        */
/*  default                         -   Each check macro returns error if FALSE condition met              */
/*  DEFS_STATUS_CHECK_ASSERT        -   If defined, each check generates ASSERT on FALSE condition         */
/*  DEFS_STATUS_CHECK_EXCEPTION     -   If defined, each check generates exception on FALSE condition      */
/*  DEFS_STATUS_CHECK_DISABLE       -   if defined, no checks are performed at all                         */
/*                                                                                                         */
/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
#if defined(DEFS_STATUS_CHECK_DISABLE)
#define DEFS_STATUS_RETURN_ACTION(e)

#elif defined(DEFS_STATUS_CHECK_EXCEPTION)
#define DEFS_STATUS_RETURN_ACTION(e) throw e

#elif defined(DEFS_STATUS_CHECK_ASSERT)
#define DEFS_STATUS_RETURN_ACTION(e) ASSERT(false)

#else
#define DEFS_STATUS_RETURN_ACTION(e) return e
#endif

/*---------------------------------------------------------------------------------------------------------*/
/*                                              ERROR LOGGING                                              */
/*                                                                                                         */
/* User might override the definition of this macro to use custom logging                                  */
/*---------------------------------------------------------------------------------------------------------*/
#if defined DEFS_STATUS_LOG_ERROR_PC
#define DEFS_STATUS_LOG_ERROR                                  \
    {                                                          \
        DEFS_STATUS_LAST_INST = CURRENT_INSTRUCTION_ADDRESS(); \
    }
#elif defined DEFS_STATUS_LOG_FUNCTION
#define DEFS_STATUS_LOG_ERROR                                                                      \
    {                                                                                              \
        DEFS_STATUS_LOG_FUNCTION(__FUNCTION__, __LINE__, __FILE__, CURRENT_INSTRUCTION_ADDRESS()); \
    }
#else
#define DEFS_STATUS_LOG_ERROR
#endif

/*---------------------------------------------------------------------------------------------------------*/
/* Macro:           DEFS_STATUS_COND_CHECK                                                                 */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  cond    - Condition to check                                                           */
/*                  err     - Error to through if condition is not met                                     */
/*                                                                                                         */
/* Returns:                                                                                                */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine checks given condition and returns the given error if condition was not   */
/*                  met                                                                                    */
/*                                                                                                         */
/* Example:                                                                                                */
/*                                                                                                         */
/*    DEFS_STATUS myFunc(void* ptr)                                                                        */
/*    {                                                                                                    */
/*        DEFS_STATUS_COND_CHECK(ptr, DEFS_STATUS_INVALID_PARAMETER);                                      */
/*        ...                                                                                              */
/*    }                                                                                                    */
/*                                                                                                         */
/*---------------------------------------------------------------------------------------------------------*/
#define DEFS_STATUS_COND_CHECK(cond, err) DEFS_STATUS_COND_CHECK_ACTION((cond), DEFS_STATUS_RETURN_ACTION(err))

/*---------------------------------------------------------------------------------------------------------*/
/* Macro:           DEFS_STATUS_COND_CHECK_GOTO                                                            */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  cond      - Condition to check                                                         */
/*                  err       - Error to through if condition is not met                                   */
/*                  var       - variable to hold error value                                               */
/*                  err_label - error label                                                                */
/*                                                                                                         */
/* Returns:                                                                                                */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine checks given condition, sets error variable and jumps to given label      */
/*                  if condition is not met                                                                */
/*                                                                                                         */
/*---------------------------------------------------------------------------------------------------------*/
#define DEFS_STATUS_COND_CHECK_GOTO(cond, err, var, err_label) \
    DEFS_STATUS_COND_CHECK_ACTION(cond, {                      \
        (var) = (err);                                         \
        goto err_label;                                        \
    })

/*---------------------------------------------------------------------------------------------------------*/
/* Macro:           DEFS_STATUS_RET_CHECK                                                                  */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  func    - function to check                                                            */
/*                                                                                                         */
/* Returns:                                                                                                */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This macro checks if give function returns DEFS_STATUS error, and returns the error    */
/*                  immediately                                                                            */
/*                                                                                                         */
/* Example:                                                                                                */
/*                                                                                                         */
/*    DEFS_STATUS myFunc(INT p1, INT p2);                                                                  */
/*                                                                                                         */
/*    DEFS_STATUS otherFunc                                                                                */
/*    {                                                                                                    */
/*        some code here...                                                                                */
/*        Call myFunc...                                                                                   */
/*        DEFS_STATUS_RET_CHECK(myFunc(p1,p2));                                                            */
/*        ...                                                                                              */
/*    }                                                                                                    */
/*                                                                                                         */
/*                                                                                                         */
/*---------------------------------------------------------------------------------------------------------*/
#define DEFS_STATUS_RET_CHECK(func) DEFS_STATUS_RET_CHECK_ACTION(func, DEFS_STATUS_RETURN_ACTION(qlib_error_check_ret))

#endif // __DEFS_ERRORS_H__
