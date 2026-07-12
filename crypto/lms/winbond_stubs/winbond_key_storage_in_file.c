/************************************************************************************************************
* @internal
* @remark     Winbond Electronics Corporation - Confidential
* @copyright  Copyright (c) 2025 by Winbond Electronics Corporation . All rights reserved
* @endinternal
*
* @file       winbond_key_storage_in_file.c
* @brief      This file includes specific definitions of key storage, in files
*
* ### project lms
*
************************************************************************************************************/

#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif

/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
/*                                                INCLUDES                                                 */
/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
#include "winbond_key_storage_in_file.h"
#include "sha256.h"
#include <stdio.h>
#include <string.h>
#include "sha256.h"

/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
/*                                        LOCAL FUNCTION PROTOTYPES                                        */
/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
static void hashKeyId_L(const LMS_KEY_ID_T keyId, uint32_t* hash);

/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
/*                                           INTERFACE FUNCTIONS                                           */
/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
LMS_ALG_STATUS LMS_StorePublicKeyInFile(const IN LMS_KEY_ID_T keyId, const IN void* pubKey, IN uint32_t pubKeySize)
{
    uint32_t hash[(uint32_t)SHA256_DIGEST_LENGTH / (uint32_t)sizeof(uint32_t)];
    char     filename[64];
    FILE*    file;
    size_t   written;

    hashKeyId_L(keyId, hash);
    (void)snprintf(filename,
                   sizeof(filename),
                   "pub_key_%02x%02x%02x%02x.bin",
                   (uint8_t)hash[0],
                   (uint8_t)hash[1],
                   (uint8_t)hash[2],
                   (uint8_t)hash[3]);

    file = fopen(filename, "wb");
    if (file == NULL)
    {
        return ALG_ERROR_KID_NOT_FOUND;
    }

    written = fwrite(pubKey, 1, pubKeySize, file);
    (void)fclose(file);

    if (written != pubKeySize)
    {
        return ALG_ERROR_SYSTEM_IN_INCORRECT_STATE;
    }

    return ALG_SUCCESS;
}

LMS_ALG_STATUS LMS_FetchPublicKeyInFile(const IN LMS_KEY_ID_T keyId,
                                        OUT void*             pubKey,
                                        IN uint32_t           pubKeyBufferSize,
                                        OUT uint32_t*         pubKeyBytesReal)
{
    uint32_t hash[(uint32_t)SHA256_DIGEST_LENGTH / (uint32_t)sizeof(uint32_t)];
    char     filename[64];
    FILE*    file;
    long     fileSize;
    size_t   read;

    hashKeyId_L(keyId, hash);
    (void)snprintf(filename,
                   sizeof(filename),
                   "pub_key_%02x%02x%02x%02x.bin",
                   (uint8_t)hash[0],
                   (uint8_t)hash[1],
                   (uint8_t)hash[2],
                   (uint8_t)hash[3]);

    file = fopen(filename, "rb");
    if (file == NULL)
    {
        return ALG_ERROR_KID_NOT_FOUND;
    }

    (void)fseek(file, 0, SEEK_END);
    fileSize = ftell(file);
    (void)fseek(file, 0, SEEK_SET);

    if ((uint32_t)fileSize > pubKeyBufferSize)
    {
        (void)fclose(file);
        return ALG_ERROR_INVALID_PARAMETER;
    }

    read = fread(pubKey, 1, (size_t)fileSize, file);
    (void)fclose(file);

    if (read != (size_t)fileSize)
    {
        return ALG_ERROR_SYSTEM_IN_INCORRECT_STATE;
    }

    *pubKeyBytesReal = (uint32_t)read;
    return ALG_SUCCESS;
}

#ifndef VERIFY_ONLY
LMS_ALG_STATUS LMS_StorePrivateKeyInFile(const IN LMS_KEY_ID_T keyId, const IN void* privKey, IN uint32_t privKeySize)
{
    uint32_t hash[(uint32_t)SHA256_DIGEST_LENGTH / (uint32_t)sizeof(uint32_t)];
    char     filename[64];
    FILE*    file;
    size_t   written;

    hashKeyId_L(keyId, hash);
    (void)snprintf(filename,
                   sizeof(filename),
                   "priv_key_%02x%02x%02x%02x.bin",
                   (uint8_t)hash[0],
                   (uint8_t)hash[1],
                   (uint8_t)hash[2],
                   (uint8_t)hash[3]);

    file = fopen(filename, "wb");
    if (file == NULL)
    {
        return ALG_ERROR_KID_NOT_FOUND;
    }

    written = fwrite(privKey, 1, privKeySize, file);
    (void)fclose(file);

    if (written != privKeySize)
    {
        return ALG_ERROR_SYSTEM_IN_INCORRECT_STATE;
    }

    return ALG_SUCCESS;
}

LMS_ALG_STATUS LMS_FetchPrivateKeyInFile(const IN LMS_KEY_ID_T keyId,
                                         OUT void*             privKey,
                                         IN uint32_t           privKeyBufferSize,
                                         OUT uint32_t*         privKeyBytesReal)
{
    uint32_t hash[(uint32_t)SHA256_DIGEST_LENGTH / (uint32_t)sizeof(uint32_t)];
    char     filename[64];
    FILE*    file;
    long     fileSize;
    size_t   read;

    hashKeyId_L(keyId, hash);
    (void)snprintf(filename,
                   sizeof(filename),
                   "priv_key_%02x%02x%02x%02x.bin",
                   (uint8_t)hash[0],
                   (uint8_t)hash[1],
                   (uint8_t)hash[2],
                   (uint8_t)hash[3]);

    file = fopen(filename, "rb");
    if (file == NULL)
    {
        return ALG_ERROR_KID_NOT_FOUND;
    }

    (void)fseek(file, 0, SEEK_END);
    fileSize = ftell(file);
    (void)fseek(file, 0, SEEK_SET);

    if ((uint32_t)fileSize > privKeyBufferSize)
    {
        (void)fclose(file);
        return ALG_ERROR_INVALID_PARAMETER;
    }

    read = fread(privKey, 1, (size_t)fileSize, file);
    (void)fclose(file);

    if (read != (size_t)fileSize)
    {
        return ALG_ERROR_SYSTEM_IN_INCORRECT_STATE;
    }

    *privKeyBytesReal = (uint32_t)read;
    return ALG_SUCCESS;
}

LMS_ALG_STATUS LMS_StoreCacheInFile(const IN LMS_KEY_ID_T keyId, const IN void* cache, IN uint32_t cacheSize)
{
    uint32_t hash[(uint32_t)SHA256_DIGEST_LENGTH / (uint32_t)sizeof(uint32_t)];
    char     filename[64];
    FILE*    file;
    size_t   written;

    hashKeyId_L(keyId, hash);
    (void)snprintf(filename,
                   sizeof(filename),
                   "cache_%02x%02x%02x%02x.bin",
                   (uint8_t)hash[0],
                   (uint8_t)hash[1],
                   (uint8_t)hash[2],
                   (uint8_t)hash[3]);

    file = fopen(filename, "wb");
    if (file == NULL)
    {
        return ALG_ERROR_KID_NOT_FOUND;
    }

    written = fwrite(cache, 1, cacheSize, file);
    (void)fclose(file);

    if (written != cacheSize)
    {
        return ALG_ERROR_SYSTEM_IN_INCORRECT_STATE;
    }

    return ALG_SUCCESS;
}

LMS_ALG_STATUS LMS_FetchCacheInFile(const IN LMS_KEY_ID_T keyId,
                                    OUT void*             cache,
                                    IN uint32_t           cacheBufferSize,
                                    OUT uint32_t*         cacheBytesReal)
{
    uint32_t hash[(uint32_t)SHA256_DIGEST_LENGTH / (uint32_t)sizeof(uint32_t)];
    char     filename[64];
    FILE*    file;
    long     fileSize;
    size_t   read;

    hashKeyId_L(keyId, hash);
    (void)snprintf(filename,
                   sizeof(filename),
                   "cache_%02x%02x%02x%02x.bin",
                   (uint8_t)hash[0],
                   (uint8_t)hash[1],
                   (uint8_t)hash[2],
                   (uint8_t)hash[3]);

    file = fopen(filename, "rb");
    if (file == NULL)
    {
        return ALG_ERROR_KID_NOT_FOUND;
    }

    (void)fseek(file, 0, SEEK_END);
    fileSize = ftell(file);
    (void)fseek(file, 0, SEEK_SET);

    if ((uint32_t)fileSize > cacheBufferSize)
    {
        (void)fclose(file);
        return ALG_ERROR_INVALID_PARAMETER;
    }

    read = fread(cache, 1, (size_t)fileSize, file);
    (void)fclose(file);

    if (read != (size_t)fileSize)
    {
        return ALG_ERROR_SYSTEM_IN_INCORRECT_STATE;
    }

    *cacheBytesReal = (uint32_t)read;
    return ALG_SUCCESS;
}
#endif // VERIFY_ONLY

LMS_ALG_STATUS LMS_DeleteKeyDataInFile(IN const LMS_KEY_ID_T keyId)
{
    uint32_t hash[(uint32_t)SHA256_DIGEST_LENGTH / (uint32_t)sizeof(uint32_t)];
    char     filename[64];
    FILE*    file;

    hashKeyId_L(keyId, hash);
    (void)snprintf(filename,
                   sizeof(filename),
                   "pub_key_%02x%02x%02x%02x.bin",
                   (uint8_t)hash[0],
                   (uint8_t)hash[1],
                   (uint8_t)hash[2],
                   (uint8_t)hash[3]);

    file = fopen(filename, "r");
    if (file != NULL)
    {
        (void)fclose(file);
        if (remove(filename) != 0)
        {
            return ALG_ERROR_SYSTEM_IN_INCORRECT_STATE;
        }
    }
#ifndef VERIFY_ONLY
    (void)snprintf(filename,
                   sizeof(filename),
                   "priv_key_%02x%02x%02x%02x.bin",
                   (uint8_t)hash[0],
                   (uint8_t)hash[1],
                   (uint8_t)hash[2],
                   (uint8_t)hash[3]);

    file = fopen(filename, "r");
    if (file != NULL)
    {
        (void)fclose(file);
        if (remove(filename) != 0)
        {
            return ALG_ERROR_SYSTEM_IN_INCORRECT_STATE;
        }
    }

    (void)snprintf(filename,
                   sizeof(filename),
                   "cache_%02x%02x%02x%02x.bin",
                   (uint8_t)hash[0],
                   (uint8_t)hash[1],
                   (uint8_t)hash[2],
                   (uint8_t)hash[3]);

    file = fopen(filename, "r");
    if (file != NULL)
    {
        (void)fclose(file);
        if (remove(filename) != 0)
        {
            return ALG_ERROR_SYSTEM_IN_INCORRECT_STATE;
        }
    }
#endif
    return ALG_SUCCESS;
}

/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
/*                                             LOCAL FUNCTIONS                                             */
/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
static void hashKeyId_L(const LMS_KEY_ID_T keyId, uint32_t* hash)
{
    void* ctx;
    (void)LMS_SHA256_Init(&ctx);
    (void)LMS_SHA256_Update(ctx, keyId, (uint32_t)sizeof(LMS_KEY_ID_T));
    (void)LMS_SHA256_Finish(ctx, hash);
}
