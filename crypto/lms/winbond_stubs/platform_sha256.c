/************************************************************************************************************
* @internal
* @remark     Winbond Electronics Corporation - Confidential
* @copyright  Copyright (c) 2019 by Winbond Electronics Corporation . All rights reserved
* @endinternal
*
* @file       platform_sha256.c
* @brief      This file contains software specific implementations of SHA256.
*
*
* ### project win_sal
*
************************************************************************************************************/

/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
/*                                                INCLUDES                                                 */
/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
#include <string.h>
#include <stdlib.h>
#include "common_platform_sha256.h"

/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
/*                                                 DEFINE                                                  */
/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/

#define ROTRIGHT(a, b) (((a) >> (b)) | ((a) << (32 - (b))))
#define CH(x, y, z)    (((x) & (y)) ^ (~(x) & (z)))
#define MAJ(x, y, z)   (((x) & (y)) ^ ((x) & (z)) ^ ((y) & (z)))
#define EP0(x)         (ROTRIGHT(x, 2) ^ ROTRIGHT(x, 13) ^ ROTRIGHT(x, 22))
#define EP1(x)         (ROTRIGHT(x, 6) ^ ROTRIGHT(x, 11) ^ ROTRIGHT(x, 25))
#define SIG0(x)        (ROTRIGHT(x, 7) ^ ROTRIGHT(x, 18) ^ ((x) >> 3))
#define SIG1(x)        (ROTRIGHT(x, 17) ^ ROTRIGHT(x, 19) ^ ((x) >> 10))

/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
/*                                                  TYPES                                                  */
/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/

typedef struct
{
    uint8_t  data[64];
    uint32_t dataLen;
    uint64_t bitLen;
    uint32_t state[8];
} SHA256_CTX;

/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
/*                                        LOCAL FUNCTION PROTOTYPES                                        */
/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
static void PLAT_SHA256_Init_L(SHA256_CTX* ctx);
static void PLAT_SHA256_Update_L(SHA256_CTX* ctx, const uint8_t data[], uint32_t len);
static void PLAT_SHA256_Final_L(SHA256_CTX* ctx, uint8_t hash[]);
static void PLAT_SHA256_Transform_L(SHA256_CTX* ctx, const uint8_t data[]);
/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
/*                                           INTERFACE FUNCTIONS                                           */
/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/

int PLAT_SHA256_Init(void** ctx)
{
    /*lint -e586 */
    SHA256_CTX* sha256_ctx = (SHA256_CTX*)malloc(sizeof(SHA256_CTX));
    /*lint +e586 */
    if (sha256_ctx == NULL)
    {
        return -1; // Memory allocation failed
    }

    PLAT_SHA256_Init_L(sha256_ctx);
    *ctx = (void*)(sha256_ctx);
    return 0;
}

int PLAT_SHA256_Update(void* ctx, const void* data, uint32_t dataSize)
{
    PLAT_SHA256_Update_L((SHA256_CTX*)ctx, (const uint8_t*)data, dataSize);
    return 0;
}

int PLAT_SHA256_Finish(void* ctx, uint32_t* output)
{
    PLAT_SHA256_Final_L((SHA256_CTX*)ctx, (uint8_t*)output);

    /*lint -e586 */
    free(ctx);
    /*lint +e586 */

    return 0;
}

/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
/*                                        LOCAL FUNCTIONS                                                  */
/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/

static void PLAT_SHA256_Init_L(SHA256_CTX* ctx)
{
    ctx->dataLen  = 0;
    ctx->bitLen   = 0;
    ctx->state[0] = 0x6a09e667u;
    ctx->state[1] = 0xbb67ae85u;
    ctx->state[2] = 0x3c6ef372u;
    ctx->state[3] = 0xa54ff53au;
    ctx->state[4] = 0x510e527fu;
    ctx->state[5] = 0x9b05688cu;
    ctx->state[6] = 0x1f83d9abu;
    ctx->state[7] = 0x5be0cd19u;
}

static void PLAT_SHA256_Update_L(SHA256_CTX* ctx, const uint8_t data[], uint32_t len)
{
    uint32_t i;

    for (i = 0; i < len; ++i)
    {
        ctx->data[ctx->dataLen] = data[i];
        ctx->dataLen++;
        if (ctx->dataLen == 64u)
        {
            PLAT_SHA256_Transform_L(ctx, ctx->data);
            ctx->bitLen += 512u;
            ctx->dataLen = 0;
        }
    }
}

static void PLAT_SHA256_Final_L(SHA256_CTX* ctx, uint8_t hash[])
{
    uint32_t i, j, temp;

    i = ctx->dataLen;

    // Pad whatever data is left in the buffer.
    if (ctx->dataLen < 56u)
    {
        ctx->data[i++] = 0x80;
        while (i < 56u)
        {
            ctx->data[i++] = 0x00;
        }
    }
    else
    {
        ctx->data[i++] = 0x80;
        while (i < 64u)
        {
            ctx->data[i++] = 0x00;
        }
        PLAT_SHA256_Transform_L(ctx, ctx->data);

        for (j = 0; j < 56u; j++)
        {
            ctx->data[j] = 0;
        }
    }

    // Append to the padding the total message's length in bits and transform.
    temp = ctx->dataLen * 8u;
    ctx->bitLen += (uint64_t)temp;
    ctx->data[63] = (uint8_t)(ctx->bitLen & 0xFFu);
    ctx->data[62] = (uint8_t)((ctx->bitLen >> 8) & 0xFFu);
    ctx->data[61] = (uint8_t)((ctx->bitLen >> 16) & 0xFFu);
    ctx->data[60] = (uint8_t)((ctx->bitLen >> 24) & 0xFFu);
    ctx->data[59] = (uint8_t)((ctx->bitLen >> 32) & 0xFFu);
    ctx->data[58] = (uint8_t)((ctx->bitLen >> 40) & 0xFFu);
    ctx->data[57] = (uint8_t)((ctx->bitLen >> 48) & 0xFFu);
    ctx->data[56] = (uint8_t)((ctx->bitLen >> 56) & 0xFFu);
    PLAT_SHA256_Transform_L(ctx, ctx->data);

    // Since this implementation uses little endian byte ordering and SHA uses big endian,
    // reverse all the bytes when copying the final state to the output hash.
    for (i = 0; i < 4u; ++i)
    {
        hash[i]       = (uint8_t)((ctx->state[0] >> (24u - i * 8u)) & 0x000000ffu);
        hash[i + 4u]  = (uint8_t)((ctx->state[1] >> (24u - i * 8u)) & 0x000000ffu);
        hash[i + 8u]  = (uint8_t)((ctx->state[2] >> (24u - i * 8u)) & 0x000000ffu);
        hash[i + 12u] = (uint8_t)((ctx->state[3] >> (24u - i * 8u)) & 0x000000ffu);
        hash[i + 16u] = (uint8_t)((ctx->state[4] >> (24u - i * 8u)) & 0x000000ffu);
        hash[i + 20u] = (uint8_t)((ctx->state[5] >> (24u - i * 8u)) & 0x000000ffu);
        hash[i + 24u] = (uint8_t)((ctx->state[6] >> (24u - i * 8u)) & 0x000000ffu);
        hash[i + 28u] = (uint8_t)((ctx->state[7] >> (24u - i * 8u)) & 0x000000ffu);
    }
}

static void PLAT_SHA256_Transform_L(SHA256_CTX* ctx, const uint8_t data[])
{
    uint32_t a, b, c, d, e, f, g, h, i, t1, t2, m[64]; ///<NOLINT
    uint32_t j = 0;
    /* clang-format off */
    static const uint32_t k[64] = {0x428a2f98u, 0x71374491u, 0xb5c0fbcfu, 0xe9b5dba5u, 0x3956c25bu, 0x59f111f1u, 0x923f82a4u, 0xab1c5ed5u,
                                   0xd807aa98u, 0x12835b01u, 0x243185beu, 0x550c7dc3u, 0x72be5d74u, 0x80deb1feu, 0x9bdc06a7u, 0xc19bf174u,
                                   0xe49b69c1u, 0xefbe4786u, 0x0fc19dc6u, 0x240ca1ccu, 0x2de92c6fu, 0x4a7484aau, 0x5cb0a9dcu, 0x76f988dau,
                                   0x983e5152u, 0xa831c66du, 0xb00327c8u, 0xbf597fc7u, 0xc6e00bf3u, 0xd5a79147u, 0x06ca6351u, 0x14292967u,
                                   0x27b70a85u, 0x2e1b2138u, 0x4d2c6dfcu, 0x53380d13u, 0x650a7354u, 0x766a0abbu, 0x81c2c92eu, 0x92722c85u,
                                   0xa2bfe8a1u, 0xa81a664bu, 0xc24b8b70u, 0xc76c51a3u, 0xd192e819u, 0xd6990624u, 0xf40e3585u, 0x106aa070u,
                                   0x19a4c116u, 0x1e376c08u, 0x2748774cu, 0x34b0bcb5u, 0x391c0cb3u, 0x4ed8aa4au, 0x5b9cca4fu, 0x682e6ff3u,
                                   0x748f82eeu, 0x78a5636fu, 0x84c87814u, 0x8cc70208u, 0x90befffau, 0xa4506cebu, 0xbef9a3f7u, 0xc67178f2u};
    /* clang-format on */

    for (i = 0; i < 16u; ++i)
    {
        j = i * 4u;

        m[i] = (((uint32_t)data[j]) << 24) | (((uint32_t)data[j + 1u]) << 16) | ((uint32_t)(data[j + 2u]) << 8) |
               ((uint32_t)(data[j + 3u]));
    }

    for (; i < 64u; ++i)
    {
        m[i] = SIG1(m[i - 2u]) + m[i - 7u] + SIG0(m[i - 15u]) + m[i - 16u];
    }

    a = ctx->state[0];
    b = ctx->state[1];
    c = ctx->state[2];
    d = ctx->state[3];
    e = ctx->state[4];
    f = ctx->state[5];
    g = ctx->state[6];
    h = ctx->state[7];

    for (i = 0; i < 64u; ++i)
    {
        t1 = h + EP1(e) + CH(e, f, g) + k[i] + m[i];
        t2 = EP0(a) + MAJ(a, b, c);
        h  = g;
        g  = f;
        f  = e;
        e  = d + t1;
        d  = c;
        c  = b;
        b  = a;
        a  = t1 + t2;
    }

    ctx->state[0] += a;
    ctx->state[1] += b;
    ctx->state[2] += c;
    ctx->state[3] += d;
    ctx->state[4] += e;
    ctx->state[5] += f;
    ctx->state[6] += g;
    ctx->state[7] += h;
}
