#include <stdint.h>
#include <stdio.h>

#include "lms.h"
#include "winbond_key_storage.h"

#define H10_LEAF_COUNT     (1u << 10u)
#define H10_SIGNATURE_SIZE ((uint32_t)LMS_SIGNATURE_SIZE(LMS_PARAM_M, LMS_PARAM_N, LMS_PARAM_P, 10u))

static uint32_t load_u32_be(const uint8_t* input)
{
    return ((uint32_t)input[0] << 24u) |
           ((uint32_t)input[1] << 16u) |
           ((uint32_t)input[2] << 8u) |
           (uint32_t)input[3];
}

static int expect_status(const char* operation, LMS_ALG_STATUS actual, LMS_ALG_STATUS expected)
{
    if (actual == expected)
    {
        return 0;
    }

    (void)fprintf(stderr,
                  "%s returned %u, expected %u\n",
                  operation,
                  (unsigned int)actual,
                  (unsigned int)expected);
    return 1;
}

int main(void)
{
    static const uint8_t message[] = "exercise every H=10 LMS leaf";
    ALG_LMS_T algorithm = {0};
    LMS_KEY_ID_T keyId = {0};
    uint8_t signature[H10_SIGNATURE_SIZE] = {0};
    LMS_GEN_KEY_PAIR_OP_T generate = {
        keyId,
        LMOTS_SHA256_N24_W4_PARAM_SET_ID,
        LMS_SHA256_M24_H10_PARAM_SET_ID,
    };
    LMS_SIGN_MESSAGE_OP_T sign = {
        keyId,
        message,
        (uint32_t)(sizeof(message) - 1u),
        signature,
        sizeof(signature),
        0u,
    };
    uint32_t expectedQ;
    int failures = 0;

    failures += expect_status("clear key storage", LMS_DeleteAllData(), ALG_SUCCESS);
    failures += expect_status("initialize LMS", lmsInit(&algorithm), ALG_SUCCESS);
    failures += expect_status("generate H=10 key", lmsGenerateKeyPair(&algorithm, &generate), ALG_SUCCESS);

    for (expectedQ = 0u; expectedQ < H10_LEAF_COUNT; ++expectedQ)
    {
        LMS_ALG_STATUS status = lmsSignMessage(&algorithm, &sign);

        if (status != ALG_SUCCESS)
        {
            failures += expect_status("sign message", status, ALG_SUCCESS);
            break;
        }
        if (load_u32_be(signature) != expectedQ)
        {
            (void)fprintf(stderr,
                          "signature encoded q=%u, expected q=%u\n",
                          load_u32_be(signature),
                          expectedQ);
            failures++;
            break;
        }
    }

    failures += expect_status("reject exhausted key",
                              lmsSignMessage(&algorithm, &sign),
                              ALG_ERROR_ALL_OTS_USED);
    failures += expect_status("keep exhausted key rejected",
                              lmsSignMessage(&algorithm, &sign),
                              ALG_ERROR_ALL_OTS_USED);
    failures += expect_status("finalize LMS", lmsFinit(&algorithm), ALG_SUCCESS);
    failures += expect_status("clear generated key", LMS_DeleteAllData(), ALG_SUCCESS);

    if (failures != 0)
    {
        return 1;
    }

    (void)printf("PASS: H=10 signatures use q=0 through q=1023\n");
    return 0;
}
