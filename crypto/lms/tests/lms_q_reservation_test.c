#include <pthread.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "lms.h"
#include "winbond_key_storage.h"

#define H10_SIGNATURE_SIZE ((uint32_t)LMS_SIGNATURE_SIZE(LMS_PARAM_M, LMS_PARAM_N, LMS_PARAM_P, 10u))

typedef struct
{
    LMS_KEY_ID_T keyId;
    uint8_t expected[sizeof(LMS_SEED_T) + (2u * sizeof(uint32_t))];
    uint8_t reserved[sizeof(LMS_SEED_T) + (2u * sizeof(uint32_t))];
    LMS_ALG_STATUS status;
} ReservationArgs;

static void* reserve_thread(void* argument)
{
    ReservationArgs* args = (ReservationArgs*)argument;
    args->status = LMS_ReservePrivateKey(args->keyId, args->expected, args->reserved, (uint32_t)sizeof(args->expected));
    return NULL;
}

static uint32_t load_u32_be(const uint8_t* input)
{
    return ((uint32_t)input[0] << 24u) |
           ((uint32_t)input[1] << 16u) |
           ((uint32_t)input[2] << 8u) |
           (uint32_t)input[3];
}

int main(void)
{
    static const uint8_t message[] = "concurrent LMS signing";
    ALG_LMS_T algorithm = {0};
    LMS_KEY_ID_T keyId = {0};
    uint8_t privateKey[sizeof(LMS_SEED_T) + (2u * sizeof(uint32_t))] = {0};
    uint32_t privateKeySize = 0u;
    uint8_t sigA[H10_SIGNATURE_SIZE] = {0};
    uint8_t sigB[H10_SIGNATURE_SIZE] = {0};
    LMS_GEN_KEY_PAIR_OP_T generate = {
        keyId,
        LMOTS_SHA256_N24_W4_PARAM_SET_ID,
        LMS_SHA256_M24_H10_PARAM_SET_ID,
    };
    ReservationArgs first = {0};
    ReservationArgs second = {0};
    LMS_SIGN_MESSAGE_OP_T signA = {keyId, message, (uint32_t)(sizeof(message) - 1u), sigA, sizeof(sigA), 0u};
    LMS_SIGN_MESSAGE_OP_T signB = {keyId, message, (uint32_t)(sizeof(message) - 1u), sigB, sizeof(sigB), 0u};
    pthread_t firstThread;
    pthread_t secondThread;

    if (LMS_DeleteAllData() != ALG_SUCCESS || lmsInit(&algorithm) != ALG_SUCCESS ||
        lmsGenerateKeyPair(&algorithm, &generate) != ALG_SUCCESS ||
        LMS_FetchPrivateKey(keyId, privateKey, (uint32_t)sizeof(privateKey), &privateKeySize) != ALG_SUCCESS ||
        privateKeySize != sizeof(privateKey))
    {
        return 1;
    }

    (void)memcpy(first.keyId, keyId, sizeof(keyId));
    (void)memcpy(second.keyId, keyId, sizeof(keyId));
    (void)memcpy(first.expected, privateKey, sizeof(privateKey));
    (void)memcpy(second.expected, privateKey, sizeof(privateKey));
    (void)memcpy(first.reserved, privateKey, sizeof(privateKey));
    (void)memcpy(second.reserved, privateKey, sizeof(privateKey));
    /* Reserve q=0 from the generated UINT32_MAX sentinel. */
    first.reserved[sizeof(LMS_SEED_T) + 0u] = 0u;
    first.reserved[sizeof(LMS_SEED_T) + 1u] = 0u;
    first.reserved[sizeof(LMS_SEED_T) + 2u] = 0u;
    first.reserved[sizeof(LMS_SEED_T) + 3u] = 0u;
    (void)memcpy(second.reserved, first.reserved, sizeof(first.reserved));

    if (pthread_create(&firstThread, NULL, reserve_thread, &first) != 0 ||
        pthread_create(&secondThread, NULL, reserve_thread, &second) != 0)
    {
        return 1;
    }
    (void)pthread_join(firstThread, NULL);
    (void)pthread_join(secondThread, NULL);
    if (!((first.status == ALG_SUCCESS && second.status == ALG_ERROR_SYSTEM_IN_INCORRECT_STATE) ||
          (second.status == ALG_SUCCESS && first.status == ALG_ERROR_SYSTEM_IN_INCORRECT_STATE)))
    {
        (void)fprintf(stderr, "reservation race statuses: %u, %u\n", first.status, second.status);
        return 1;
    }

    if (lmsSignMessage(&algorithm, &signA) != ALG_SUCCESS || lmsSignMessage(&algorithm, &signB) != ALG_SUCCESS ||
        load_u32_be(sigA) == load_u32_be(sigB))
    {
        return 1;
    }

    (void)printf("PASS: private-key reservation is atomic; concurrent signatures use distinct q values\n");
    return 0;
}
