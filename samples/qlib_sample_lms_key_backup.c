/************************************************************************************************************
* @internal
* @remark     Winbond Electronics Corporation - Confidential
* @copyright  Copyright (c) 2025 by Winbond Electronics Corporation . All rights reserved
* @endinternal
*
* @file       qlib_sample_lms_key_backup.c
* @brief      This file contains QLIB sample for replacing LMS root key
*
* @example    qlib_sample_lms_key_backup.c
*
* @page       LMS_Root_key replacement sample code
* This sample code shows how to update the lms root key in a section with LMS command.\n
*
* @include    samples/qlib_sample_lms_key_backup.h
*
************************************************************************************************************/

/*-----------------------------------------------------------------------------------------------------------
                                                   INCLUDES
-----------------------------------------------------------------------------------------------------------*/
#include <stdio.h>

#include "qlib.h"
#include "qlib_sample.h"
#include "qlib_sample_lms_key_backup.h"
#include "qlib_utils_digest.h"
#include "qlib_utils_crc.h"

#if !defined(EXCLUDE_LMS) && (defined(LMS_SAMPLE_PREPARE_IN_ADVANCE) || !defined(VERIFY_ONLY))
#include "lms_api.h"
#include "qlib_utils_lms.h"

#ifndef LMS_SAMPLE_PREPARE_IN_ADVANCE
#include "qlib_sample_lms_common.h"
#endif // LMS_SAMPLE_PREPARE_IN_ADVANCE
/*-----------------------------------------------------------------------------------------------------------
                                                 DEFINITIONS
-----------------------------------------------------------------------------------------------------------*/
#define LMS_SAMPLE_SECTION_ID (1u)
#ifdef LMS_SAMPLE_PREPARE_IN_ADVANCE
#define LMS_KEY_ID_1                                                                                   \
    {                                                                                                  \
        0xfa, 0x16, 0xb4, 0x90, 0xbd, 0xee, 0x56, 0xbd, 0xac, 0x5b, 0xb6, 0x6f, 0xde, 0xbf, 0x7d, 0xfa \
    }
// pre-prepared key pair for seed 2923be84e16cd6ae529049f1f1bbe9ebb3a6db3c870c3e99
#define LMS_PUB_KEY_1                                                                                                           \
    {                                                                                                                           \
        0x00, 0x00, 0x00, 0x0D, 0x00, 0x00, 0x00, 0x07, 0xfa, 0x16, 0xb4, 0x90, 0xbd, 0xee, 0x56, 0xbd, 0xAc, 0x5b, 0xB6, 0x6F, \
            0xDE, 0xBF, 0x7D, 0xFA, 0x6b, 0x66, 0x51, 0x03, 0xc2, 0xaa, 0xe4, 0xbc, 0x49, 0x14, 0x0c, 0xdb, 0x19, 0xe1, 0x7c,   \
            0x69, 0x30, 0x15, 0xa3, 0xfb, 0x22, 0xcd, 0xf9, 0x16                                                                \
    }

#define LMS_KEY_ID_2                                                                                   \
    {                                                                                                  \
        0x7f, 0x2f, 0x23, 0xa8, 0xa7, 0xf0, 0x74, 0x3f, 0x8b, 0x8f, 0xf7, 0xd8, 0x9c, 0x09, 0xc5, 0xf7 \
    }
// pre-prepared key pair for seed e1984145ebd8aad90a28ecb7c153bc967870e3e77106da12
#define LMS_PUB_KEY_2                                                                                                           \
    {                                                                                                                           \
        0x00, 0x00, 0x00, 0x0d, 0x00, 0x00, 0x00, 0x07, 0x7f, 0x2f, 0x23, 0xa8, 0xa7, 0xf0, 0x74, 0x3f, 0x8b, 0x8f, 0xf7, 0xd8, \
            0x9c, 0x09, 0xc5, 0xf7, 0x78, 0x9f, 0xb2, 0xd7, 0xe1, 0x6f, 0xe8, 0x3a, 0x45, 0xc5, 0x12, 0xe6, 0xaa, 0x03, 0xab,   \
            0x07, 0x81, 0xb4, 0x69, 0x38, 0xa3, 0x26, 0x8e, 0x67                                                                \
    }
#endif // LMS_SAMPLE_PREPARE_IN_ADVANCE
/*-----------------------------------------------------------------------------------------------------------
                                         LOCAL FUNCTION PROTOTYPES
-----------------------------------------------------------------------------------------------------------*/
static void          QLIB_SAMPLE_LmsKeyBackup_storeKid_L(const QLIB_LMS_KEY_ID_T keyId);
static void          QLIB_SAMPLE_LmsKeyBackup_retrieveKid_L(QLIB_LMS_KEY_ID_T keyId);
static void          QLIB_SAMPLE_LmsKeyBackup_storeBackupCmd_L(const QLIB_LMS_KEY_ID_T          keyId,
                                                               const QLIB_LMS_MSG_DATA_STRUCT_T lmsMsg_pu8,
                                                               const QLIB_LMS_SIG_BUFFER_T      signature_pu8);
static void          QLIB_SAMPLE_LmsKeyBackup_retrieveBackupCmd_L(QLIB_LMS_KEY_ID_T          keyId,
                                                                  QLIB_LMS_MSG_DATA_STRUCT_T lmsMsg_pu8,
                                                                  QLIB_LMS_SIG_BUFFER_T      signature_pu8);
static QLIB_STATUS_T QLIB_SAMPLE_LmsKeyBackup_createBackupKey_L(void);

/*-----------------------------------------------------------------------------------------------------------
                                                   LOCAL VARIABLES
-----------------------------------------------------------------------------------------------------------*/

static QLIB_LMS_KEY_ID_T lms_kid_L;

/*-----------------------------------------------------------------------------------------------------------
                                                   TYPES
-----------------------------------------------------------------------------------------------------------*/

typedef struct
{
    QLIB_LMS_KEY_ID_T          keyId;
    QLIB_LMS_MSG_DATA_STRUCT_T lmsMsg_pu8;
    QLIB_LMS_SIG_BUFFER_T      sig;
} LMS_SAMPLE_BACKUP_INFO_T;

LMS_SAMPLE_BACKUP_INFO_T backupCmd_g;
/*-----------------------------------------------------------------------------------------------------------
                                             INTERFACE FUNCTIONS
-----------------------------------------------------------------------------------------------------------*/

QLIB_STATUS_T QLIB_SAMPLE_LmsKeyBackup(void* userData_pv)
{
    QLIB_CONTEXT_T    qlibContext;
    QLIB_STATUS_T     status    = QLIB_STATUS__OK;
    QLIB_BUS_FORMAT_T busFormat = QLIB_BUS_FORMAT(QLIB_BUS_MODE_1_1_1, (bool)false);

    /*-------------------------------------------------------------------------------------------------------
     Init QLIB  - needed when running QLIB either on a device or on a remote server.
    -------------------------------------------------------------------------------------------------------*/
    QLIB_STATUS_RET_CHECK(QLIB_InitLib(&qlibContext));

    /*-------------------------------------------------------------------------------------------------------
     Set the user data into Qlib's context
    -------------------------------------------------------------------------------------------------------*/
    QLIB_SetUserData(&qlibContext, userData_pv);

    /*-------------------------------------------------------------------------------------------------------
     Take the ownership of flash communication channel (it belongs to local MCU or to remote server, exclusively)
    -------------------------------------------------------------------------------------------------------*/
    QLIB_STATUS_RET_CHECK(QLIB_Connect(&qlibContext));

    /*-------------------------------------------------------------------------------------------------------
     Init Flash Device - not needed when using QLIB on a remote server
    -------------------------------------------------------------------------------------------------------*/
    QLIB_STATUS_RET_CHECK_GOTO(QLIB_InitDevice(&qlibContext, busFormat), status, disconnect);

    /*-------------------------------------------------------------------------------------------------------
     configure flash for sample flow
    -------------------------------------------------------------------------------------------------------*/
    QLIB_STATUS_RET_CHECK_GOTO(QLIB_SAMPLE_LmsKeyBackup_manuf_flow(&qlibContext), status, disconnect);

    /*-------------------------------------------------------------------------------------------------------
     Run sample code
    -------------------------------------------------------------------------------------------------------*/
    QLIB_STATUS_RET_CHECK_GOTO(QLIB_SAMPLE_LmsKeyBackup_field_flow(&qlibContext), status, disconnect);

#ifdef LMS_SAMPLE_PREPARE_IN_ADVANCE
    // we can change the keys multiple times if we prepared a backup key when the last backup is used
    QLIB_STATUS_RET_CHECK_GOTO(QLIB_SAMPLE_LmsKeyBackup_field_flow(&qlibContext), status, disconnect);
#endif // LMS_SAMPLE_PREPARE_IN_ADVANCE

disconnect:
    /*-------------------------------------------------------------------------------------------------------
     Release the ownership of flash communication channel (it belongs to local MCU or to remote server, exclusively)
    -------------------------------------------------------------------------------------------------------*/
    (void)QLIB_Disconnect(&qlibContext);

    return status;
}

QLIB_STATUS_T QLIB_SAMPLE_LmsKeyBackup_manuf_flow(QLIB_CONTEXT_T* qlibContext)
{
    QLIB_HW_VER_T                 hwVer;
    QLIB_ID_T                     id;
    uint8_t                       die_u8 = 0u;
    uint32_t                      sect_u32;
    QLIB_FLASH_CONFIG_T           flashCfg = {0u};
    QLIB_DIE_CONFIG_T             dieCfg   = {0u};
    QLIB_SECTIONS_CONF_TABLE_T    cfgLine[QLIB_MAX_DIES_SUPPORTED];
    QLIB_EXTENDED_SECTION_CONF_T* sectCfg;
    uint8_t                       diesNum_u8 = QLIB_GET_NUM_OF_DIES(qlibContext);
    bool                          isProvisioned_b;

    // See what chip is connected and its unique ids. We might need this info to obtain keys for this
    // chips according to formula or via DB see QLIB_SAMPLE_GetDeviceKey() function
    QLIB_STATUS_RET_CHECK(QLIB_GetHWVersion(qlibContext, &hwVer));
    QLIB_STATUS_RET_CHECK(QLIB_GetId(qlibContext, &id));

    // We do read / modify /write configuration since we want to change only what is needed
    // read configuration
    QLIB_STATUS_RET_CHECK(QLIB_GetDeviceConfigMultiDie(qlibContext, &flashCfg, &dieCfg, cfgLine, diesNum_u8));

    // modify configuration

    // set pre provision master key if needed
    QLIB_STATUS_RET_CHECK(
        QLIB_SAMPLE_GetDeviceKey(qlibContext, id.sec.wid, QLIB_KID__DEVICE_KEY_PRE_PROVISIONING, dieCfg.preProvisionedMasterKey));

    // set new master key
    QLIB_STATUS_RET_CHECK(QLIB_SAMPLE_GetDeviceKey(qlibContext, id.sec.wid, QLIB_KID__DEVICE_MASTER, dieCfg.deviceMasterKey));

#if QLIB_MAX_DIES_SUPPORTED > 1u
    QLIB_STATUS_RET_CHECK(QLIB_GetActiveDie(qlibContext, &die_u8));
#endif // QLIB_MAX_DIES_SUPPORTED > 1u
    sectCfg = cfgLine[die_u8].sectionConfigTable;

    sect_u32 = LMS_SAMPLE_SECTION_ID;

    sectCfg[sect_u32].baseAddr = QLIB_SAMPLE_RESERVED_BOOT_SECTION_SIZE;
    sectCfg[sect_u32].size     = SIZE_512KB_;

    // section configured usually will be set with rollback protected for FW update
    sectCfg[sect_u32].sectionConfig.policy.plainAccessReadEnable   = 1u;
    sectCfg[sect_u32].sectionConfig.policy.plainAccessWriteEnable  = 0u;
    sectCfg[sect_u32].sectionConfig.policy.authPlainAccess         = 0u;
    sectCfg[sect_u32].sectionConfig.policy.checksumIntegrity       = 0u;
    sectCfg[sect_u32].sectionConfig.policy.digestIntegrity         = 0u;
    sectCfg[sect_u32].sectionConfig.policy.digestIntegrityOnAccess = 0u;
    sectCfg[sect_u32].sectionConfig.policy.rollbackProt            = 1u;
    sectCfg[sect_u32].sectionConfig.policy.slog                    = 0u;
    sectCfg[sect_u32].sectionConfig.policy.writeProt               = 0u;

    // Full access key configuration.This is the key to use later for the section configuration
    // changes or write access
    QLIB_STATUS_RET_CHECK(QLIB_SAMPLE_GetSectionAccessKey(qlibContext,
                                                          id.sec.wid,
                                                          QLIB_KID__FULL_ACCESS_SECTION,
                                                          sect_u32,
                                                          die_u8,
                                                          sectCfg[sect_u32].fullAccessKey));

    // configure LMS public key.
    QLIB_STATUS_RET_CHECK(QLIB_IsKeyProvisioned(qlibContext, QLIB_KID__SECTION_LMS, sect_u32, &isProvisioned_b));

    if ((bool)false == isProvisioned_b)
    {
#ifndef LMS_SAMPLE_PREPARE_IN_ADVANCE
        // lms public key is obtained from hsm
        LMS_PUB_KEY_T     publicKey_u8;
        QLIB_LMS_KEY_ID_T keyId;

        QLIB_STATUS_RET_CHECK(QLIB_LMS_HsmGenerateKeyPair(keyId));
        QLIB_STATUS_RET_CHECK(QLIB_LMS_HsmGetKey(keyId, publicKey_u8));
#else
        const QLIB_LMS_KEY_ID_T keyId          = LMS_KEY_ID_1;
        const uint8_t           publicKey_u8[] = LMS_PUB_KEY_1;
#endif // LMS_SAMPLE_PREPARE_IN_ADVANCE
        QLIB_SAMPLE_LmsKeyBackup_storeKid_L(keyId);
        // copy to chip configuration data structure
        (void)memcpy(sectCfg[sect_u32].lmsKey, publicKey_u8, sizeof(LMS_PUB_KEY_T));
    }
    else
    {
#ifndef LMS_SAMPLE_PREPARE_IN_ADVANCE
        // Can not run the sample as LMS key is already set on section
        return QLIB_STATUS__SYSTEM_IN_INCORRECT_STATE;
#else
        // lms public key can be already inside, we assume that this sample was already ran before
        const QLIB_LMS_KEY_ID_T keyId = LMS_KEY_ID_1;
        QLIB_SAMPLE_LmsKeyBackup_storeKid_L(keyId);
#endif // LMS_SAMPLE_PREPARE_IN_ADVANCE
    }

    // write configuration
    QLIB_STATUS_RET_CHECK(QLIB_ConfigDeviceMultiDie(qlibContext, &flashCfg, &dieCfg, cfgLine, diesNum_u8));

#if QLIB_MAX_DIES_SUPPORTED > 1u
    // write configuration sets die to 0. return to orig die
    QLIB_STATUS_RET_CHECK(QLIB_SetActiveDie(qlibContext, die_u8));
#endif // QLIB_MAX_DIES_SUPPORTED > 1u

    // Create backup key for redundancy and sign the LMS key replacement command using the current LMS key
    QLIB_STATUS_RET_CHECK(QLIB_SAMPLE_LmsKeyBackup_createBackupKey_L());

    return QLIB_STATUS__OK;
}

QLIB_STATUS_T QLIB_SAMPLE_LmsKeyBackup_field_flow(QLIB_CONTEXT_T* qlibContext)
{
    QLIB_LMS_KEY_ID_T          keyId;
    QLIB_LMS_KEY_ID_T          backupKeyId_pu8;
    QLIB_LMS_MSG_DATA_STRUCT_T lmsMsg_pu8;
    QLIB_LMS_SIG_BUFFER_T      signature_pu8;
    /*-------------------------------------------------------------------------------------------------------
     Initialize KeyId to the stored key id that match the configured public key on flash
    -------------------------------------------------------------------------------------------------------*/
    QLIB_SAMPLE_LmsKeyBackup_retrieveKid_L(keyId);

    QLIB_SAMPLE_LmsKeyBackup_retrieveBackupCmd_L(backupKeyId_pu8, lmsMsg_pu8, signature_pu8);

    /*---------------------------------------------------------------------------------------------------
        Send the LMS command to the flash
    ---------------------------------------------------------------------------------------------------*/
    QLIB_STATUS_RET_CHECK(QLIB_SendLMSCommand(qlibContext, lmsMsg_pu8, keyId, (uint8_t*)signature_pu8, LMS_SAMPLE_SECTION_ID));

    /*---------------------------------------------------------------------------------------------------
        Now the active key became the new one we just provisioned to flash
    ---------------------------------------------------------------------------------------------------*/
    QLIB_SAMPLE_LmsKeyBackup_storeKid_L(backupKeyId_pu8);

    /*---------------------------------------------------------------------------------------------------
        Optionally, user can create new backup for future use.
        In this example we avoid it when the key pair generation is needed since this is time consuming
    ---------------------------------------------------------------------------------------------------*/
#ifdef LMS_SAMPLE_PREPARE_IN_ADVANCE
    QLIB_STATUS_RET_CHECK(QLIB_SAMPLE_LmsKeyBackup_createBackupKey_L());
#endif // LMS_SAMPLE_PREPARE_IN_ADVANCE

    return QLIB_STATUS__OK;
}

/*-----------------------------------------------------------------------------------------------------------
                                               LOCAL FUNCTIONS
-----------------------------------------------------------------------------------------------------------*/

QLIB_STATUS_T QLIB_SAMPLE_LmsKeyBackup_createBackupKey_L(void)
{
    QLIB_LMS_KEY_ID_T keyId = {0u};
#ifndef LMS_SAMPLE_PREPARE_IN_ADVANCE
    uint32_t                   sectionID_u32 = LMS_SAMPLE_SECTION_ID;
    QLIB_LMS_MSG_DATA_STRUCT_T lmsMsg_pu8;
    QLIB_LMS_SIG_BUFFER_T      signature_pu8;
    LMS_PUB_KEY_T              backupPublicKey;
    QLIB_LMS_KEY_ID_T          backupKeyId_pu8;
#ifdef __cplusplus
    LMS_SIGN_MESSAGE_OP_T signParams((uint8_t*)keyId,
                                     lmsMsg_pu8,
                                     sizeof(QLIB_LMS_MSG_DATA_STRUCT_T),
                                     signature_pu8,
                                     sizeof(signature_pu8),
                                     0u);
#else
    LMS_SIGN_MESSAGE_OP_T signParams =
        {keyId, lmsMsg_pu8, sizeof(QLIB_LMS_MSG_DATA_STRUCT_T), signature_pu8, sizeof(signature_pu8), 0u};
#endif // __cplusplus
#else
    // constant pre-calculated values to replace public key 1 with new public key 2
    const QLIB_LMS_KEY_ID_T backupKeyId2 = LMS_KEY_ID_2;
    const QLIB_LMS_MSG_DATA_STRUCT_T lmsMsg2 = {0x00, 0x00, 0x31, 0x2A, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                                0x00, 0x00, 0x00, 0x7F, 0x2F, 0x23, 0xA8, 0xA7, 0xF0, 0x74, 0x3F, 0x8B, 0x8F,
                                                0xF7, 0xD8, 0x9C, 0x09, 0xC5, 0xF7, 0x78, 0x9F, 0xB2, 0xD7, 0xE1, 0x6F, 0xE8,
                                                0x3A, 0x45, 0xC5, 0x12, 0xE6, 0xAA, 0x03, 0xAB, 0x07, 0x81, 0xB4, 0x69, 0x38,
                                                0xA3, 0x26, 0x8E, 0x67, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    const uint32_t signature2_u32[sizeof(QLIB_LMS_SIG_BUFFER_T) / sizeof(uint32_t)] =
        {0x01000000, 0x07000000, 0xABABABAB, 0xABABABAB, 0xABABABAB, 0xABABABAB, 0xABABABAB, 0xABABABAB, 0x83395B6B, 0x33B3E3CA,
         0x4D36BDBA, 0xCFD1F22B, 0xC5344BEF, 0x55A8463C, 0x74F887F2, 0x63F01B6B, 0x644E06B3, 0x4B6C722B, 0x0359A574, 0x343893FB,
         0x03DA6D2A, 0x4592A754, 0xB40A56D8, 0x1CEEAE84, 0x3663274D, 0xC917DF61, 0x6ED63645, 0x0D08FC41, 0xA539153D, 0xE0925C7F,
         0x4DA2AA32, 0xD67C5A05, 0x5432B5DC, 0x7E68C4F4, 0x278A4DA2, 0x658149DA, 0x88736BD3, 0x26FF4064, 0xDCD67103, 0x33AE8323,
         0x6B6E7C04, 0xC94021CF, 0x908422EE, 0x2CF16D95, 0xDC7A76ED, 0xABF955F0, 0xD164EF9D, 0x49CF9B71, 0x4994C60C, 0x5F30D2F0,
         0x5B46EDDF, 0x40604E60, 0x1CA0E497, 0x03559914, 0xE772EFA5, 0x77745C89, 0x87045133, 0x09F37169, 0xADC507DA, 0x74F95EDC,
         0x0076F2BB, 0xEC8E9BA6, 0x2E8AD0C2, 0x3D900FEF, 0x7DDA3AE0, 0xE6F6C2B9, 0x13E96541, 0x4C82EECD, 0xDB386E45, 0x4BB7EE38,
         0x957845B8, 0x9B2A8E4B, 0x62573980, 0x6C5CAE7C, 0xC7A4AA69, 0xC30818A3, 0x08E05558, 0x063CA912, 0x85632491, 0x85227035,
         0x7107D713, 0x01A78746, 0xE41DD49C, 0x68344FF6, 0xE38E7322, 0xFCD57CDE, 0x356DCBC0, 0xDA12820E, 0x7B7A86C2, 0x8E258D56,
         0x00C0346A, 0xF1F55D8C, 0x35D63EA9, 0xB9C40B77, 0x72A713C4, 0x7EEE9058, 0x9C0DBA39, 0x50D3455E, 0x0E5D28E9, 0xB9C81C34,
         0x4FE5176F, 0x0954CA46, 0x18C5526E, 0x04C62BDC, 0xF17BF8F5, 0x1126A573, 0xFDBEEE4F, 0xA238BA5B, 0xAF7FE51A, 0x705C0F9D,
         0x52ACF950, 0x07059EC8, 0x645F0295, 0x2DB2EA63, 0x949168CC, 0xD2D52667, 0x9C4AD339, 0x0D402A23, 0xB318B118, 0x2783DB32,
         0xA09BA83B, 0x813F21E4, 0x6469619E, 0xF3A9035F, 0xC44AC9F8, 0x0D762960, 0xFC837FC0, 0x2AA18D73, 0x3BD5FF89, 0x5B4EBFEC,
         0x6F618284, 0x73360556, 0x3422FF36, 0x745E30FD, 0xD72843F6, 0xB6D17FC5, 0xFEC779F6, 0x3F669DB0, 0xF2A176B6, 0xE64E4E64,
         0x53045C6C, 0x1B1F9EAE, 0x4D10C710, 0x8A335AF6, 0xFE26DBE2, 0xEAF7A6A7, 0x08294A27, 0x01DD589B, 0xB3F932B2, 0x82365365,
         0xD48A4671, 0x35B25F69, 0x6A2176F0, 0xC6435B65, 0x13B6DDAE, 0x7C19CE19, 0x17FB75C2, 0x5AED0E29, 0x23C10A9A, 0x4DD4B024,
         0x68113B5A, 0xAED47C4F, 0x1BC671C5, 0x58D7320D, 0xCAF8B9BD, 0xE8C7933F, 0x50559BDD, 0x25F4AB7B, 0x7BEFD4F5, 0xA7C1F79D,
         0x8657AB33, 0x3C9B14BF, 0x524466B1, 0x00F51E94, 0x8A8ED690, 0xCFA8F302, 0xBDD7DD59, 0x356A12D0, 0xE4BF1C95, 0xD20869BF,
         0x7C5305D5, 0xBB39E14B, 0x0C83EA32, 0x4A626970, 0xDF7B107F, 0xA7D67D7C, 0x0F4B8929, 0xAFC6E50D, 0xF62D4562, 0x9C92EDD3,
         0xB9856754, 0x78BA6133, 0x3A098E78, 0x4CFE330E, 0x012C3D30, 0x82D32037, 0x6DCAFEFB, 0x2E934EC3, 0x7C23F673, 0x7BA169E9,
         0xC05959DB, 0x1E36932C, 0xE75CCC9D, 0x08BDA06B, 0x34EB3345, 0xB854BA9B, 0xCAC94E69, 0xB4E26E15, 0x3B099D5C, 0x2F64A5F2,
         0x5452E0F9, 0x5F779325, 0x7E8D883C, 0x1DCDBC98, 0x4508F0A4, 0xDC732CF0, 0xE1A38B03, 0x4DD86399, 0x15B70A9D, 0x3CA3640D,
         0x215AA0F0, 0x532EBD57, 0x03BAAD63, 0x83A16875, 0x5C3F849B, 0x3D452F4F, 0x49CC40AC, 0x2B772B92, 0x5C93FF91, 0x2F75351A,
         0xDFA25237, 0x0492E6FE, 0x2892E5CE, 0x293927ED, 0xC5599F35, 0xC57C3F3E, 0x7B0822F1, 0x1D536547, 0xDCA00983, 0xE2EC009F,
         0xEF3C5D7A, 0xAEF743DE, 0x373863EF, 0x70B67512, 0x3239CCA2, 0x8C1EF69A, 0x02965336, 0x00B1F0E6, 0x04D07774, 0x048F4145,
         0x543F5B97, 0x13D7F860, 0x3DF90285, 0x729C6DF6, 0xEE11B26A, 0xAD80177B, 0x389BF0A0, 0xDBF195C3, 0xC6DFA2C7, 0xA9D2D1ED,
         0xB6D1C708, 0x89E207D3, 0xF3A66D33, 0x6B81D14D, 0xDAD71656, 0x865270E7, 0xF5984337, 0x15124C55, 0x5B064222, 0xAC86102F,
         0x0B45DAB1, 0xCDE2BECE, 0xEFAFC172, 0xFBC5C0E1, 0xDE1E01C0, 0x117519A5, 0x46079BF0, 0x777C06BA, 0x2334173C, 0x432BC5E2,
         0xDA74FDCD, 0xB2EF8291, 0x350FDBC3, 0x0ADF8653, 0xDC0A1540, 0xEE6468B0, 0x2A3A4BF5, 0xA73EC95B, 0xE2AEF395, 0xD6F7B5BD,
         0xE475A95E, 0x017A094D, 0xF3C058C7, 0xE67D2012, 0xFAF5B178, 0xBC170F12, 0x64119A7E, 0xDAC48FED, 0xEE4C294A, 0x4AD64C9A,
         0x6AFA5E3B, 0x72614203, 0x6154BA3E, 0xEE0ED8B6, 0xC98A28FE, 0x12F0EEC9, 0x7F4BF0BC, 0x1B6EDF13, 0x4EB34E85, 0xFAC03AB5,
         0x4DC0A9B7, 0x148F38EE, 0x9C87661D, 0x09C8F8AA, 0x0D000000, 0xFB8492A4, 0x362D39BF, 0x2DA52F40, 0x7C0EDC87, 0x89DF527C,
         0x76F96225, 0x0A79DEEC, 0xFED8F306, 0xCA54967A, 0xABBBC4B8, 0x8CDF7F46, 0x98F1ADF8, 0xCDC3F263, 0xBB489794, 0x6E20FE28,
         0x278A782B, 0x33EDBC6B, 0xB1742266, 0x3B6D6548, 0x4EEF42F3, 0x57683C51, 0x93BA0A88, 0xBD088998, 0xBB9241A7, 0x829A8BD3,
         0x8A65C80E, 0x0BFFB484, 0xEC7ADCFF, 0xEE01FE0E, 0x34241185, 0xD0269F60, 0x5ECF562C, 0xD6B5D1B7, 0xC44AD1DF, 0x841E2F15,
         0x3D8D6399, 0x1A229448, 0x7323203E, 0xE6008D7C, 0x957E2B3D, 0xF5C7BAD1, 0x7CF36209, 0x6BE1B3E9, 0xEEDAF322, 0x9B02D5FB,
         0xAB7BF8D1, 0x65DDC498, 0x927EEAB0, 0xA6D65A7A, 0xFB1D86EF, 0xEC7D63FE, 0x00F575B1, 0xF318CFD4, 0x120C9686, 0x087CDCA8,
         0xCDCC9BB0, 0x25513F45, 0x9445536F, 0x915206AC, 0x132E4183, 0xB6957C07, 0x35A6EDBF, 0x6ED9B081, 0xD927272B, 0xADDEDEFB,
         0x0A759C29, 0xB2A652C3, 0xB6251FE7, 0xC92F0926, 0x36488D00, 0x2AA0B544, 0x63698FB3, 0xB5B4081A, 0xA023E523, 0xD2A26B7E,
         0x5BC399AE, 0xC8CA4662, 0xA80441A6, 0xC2A1AA0B, 0x8374DCD8, 0xA5FF008E, 0x0C283859, 0xE07B3D10, 0x84B59D87, 0x523B1C7C,
         0x74A4643D, 0x6EE1B490, 0x59E48138, 0x539CFCCF, 0xF6669876, 0x615E9181, 0x5A82D44D, 0xFEE9F22F, 0x596AAAE5, 0x6905A63B,
         0x0248324E, 0x6E242D65, 0x4675125E, 0x0AFEFDF8, 0xB356FCFB, 0x29A59C18, 0xB70F7EF5, 0xA7457E0C, 0xC5D91FBC, 0x9BCF202D,
         0x07996B40, 0x958D6D29, 0x854BC89F, 0x692C6992, 0xC15F0345, 0xC4E11BC6, 0xBB38C3EC, 0xFF7D7FEA, 0x8201F04E, 0x1A6BE944,
         0x1F933374, 0x0D5F73C3, 0x33215423, 0x2A54DBE6, 0x94E15532};

    // constant pre-calculated values to replace public key 2 with new public key 1
    const QLIB_LMS_KEY_ID_T backupKeyId1 = LMS_KEY_ID_1;
    const QLIB_LMS_MSG_DATA_STRUCT_T lmsMsg1 = {0x00, 0x00, 0x31, 0x2A, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                                0x00, 0x00, 0x00, 0xFA, 0x16, 0xB4, 0x90, 0xBD, 0xEE, 0x56, 0xBD, 0xAC, 0x5B,
                                                0xB6, 0x6F, 0xDE, 0xBF, 0x7D, 0xFA, 0x6B, 0x66, 0x51, 0x03, 0xC2, 0xAA, 0xE4,
                                                0xBC, 0x49, 0x14, 0x0C, 0xDB, 0x19, 0xE1, 0x7C, 0x69, 0x30, 0x15, 0xA3, 0xFB,
                                                0x22, 0xCD, 0xF9, 0x16, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

    const uint32_t signature1_u32[sizeof(QLIB_LMS_SIG_BUFFER_T) / sizeof(uint32_t)] =
        {0x01000000, 0x07000000, 0xABABABAB, 0xABABABAB, 0xABABABAB, 0xABABABAB, 0xABABABAB, 0xABABABAB, 0x962A829A, 0x6DD25D03,
         0x69EACB1D, 0xC3DD12D8, 0x836C3E4B, 0x0BA60EB0, 0x28690235, 0x2A3650FC, 0xB2029FED, 0xBC38F4A1, 0x4C5BB8C7, 0x74F5F909,
         0x1963404B, 0xA7A429CB, 0xA10C963E, 0xFEF45E13, 0x19578006, 0xBDCC9720, 0x2A8B467F, 0x3EB3C454, 0xF75A8BD4, 0xD67982C6,
         0xD3F1D928, 0x0A8A647F, 0xAD7A6EA7, 0x45A7B9F6, 0x7D50E275, 0x9F8600D9, 0xA3249F42, 0x487CDF7E, 0xF87B560C, 0x53E4DA0A,
         0x042F36C7, 0x44BEC754, 0xB05B67C2, 0x849094D6, 0xA60E983A, 0x8EA3E37F, 0xAB78D59E, 0xEB82868A, 0x893027BA, 0xE757FC39,
         0xD53E98A4, 0x2FA6EF2D, 0x48B79F2F, 0xBA54FF3B, 0x37947335, 0x95DFDF31, 0xE0AC3C58, 0x1E82B6E9, 0x8A5335B6, 0x910EE75F,
         0x5BCFA680, 0x96E91963, 0x13F25C0B, 0xC3D11B64, 0xEC878208, 0x84557CFF, 0xCB67B57B, 0x66FE41B9, 0x74927866, 0xFE310049,
         0x0E63FB23, 0xC2B70BE3, 0xDDF5757F, 0xC2349926, 0xA5C23614, 0x4D3885F4, 0x03D8BBBC, 0x4B224649, 0xC0BFBCA7, 0x8347938A,
         0xB7D750B0, 0xF6FB6D5C, 0xC2C4F191, 0x69D9FF9A, 0x2FF853D5, 0xA9BCE09D, 0x4458E9CD, 0xE817B6F8, 0xAA129C37, 0x04AB42E9,
         0x1D5BAF8D, 0x14481E55, 0xDF582A98, 0x81E7DFC2, 0xF1A916FF, 0x9DCA51AE, 0xEC64EFD9, 0x5D947F31, 0xC929DCB0, 0x5DA91E65,
         0xC656AAD9, 0x7095BAD4, 0xDB9A1FA6, 0x3AC09A2D, 0x26BCF9D2, 0x85507CC8, 0x6375DB91, 0x68E52F4C, 0x1E37D395, 0xC0530DB3,
         0x74255827, 0x94880251, 0x673754BB, 0x1CABFEBD, 0x383AD44B, 0x165A189A, 0xF75E0397, 0xB5F7A291, 0x6482BD4D, 0x9E216C49,
         0x2F559300, 0xAB3CB941, 0xF3BE5B41, 0xDB070658, 0xD82C1007, 0xB3B06DF7, 0x4D6F37FB, 0xA2CA760D, 0xDF22F4B2, 0x4D61F085,
         0xE6758EA5, 0xA84A6076, 0xA3026FA7, 0x389B4968, 0x7C5919B7, 0x2425E5B6, 0x5C7AE5F7, 0xC0C52212, 0x83474230, 0x92B7F537,
         0x0BB8D979, 0x78C9DA7C, 0x4C0E72A5, 0x2D1D51C7, 0xACACE9D8, 0x3C167EF1, 0x9EFEAD7D, 0x55F4D865, 0x0B3D8A2D, 0xDAC1F558,
         0xAD0219B6, 0x2725E958, 0x70C5EDFA, 0x18BA0027, 0xB8384479, 0x34486236, 0x713533FA, 0xB72401DD, 0xEC22BDE6, 0x235AC93F,
         0xFF4A622F, 0x2DDCE9AE, 0xF841046D, 0x86316C58, 0x7F14B437, 0xB8BABDB9, 0x8266041C, 0xD00A7664, 0xA688AD25, 0x20DC634B,
         0x8CD4BACC, 0x589B70EE, 0xC5C6F0C5, 0x4F45721B, 0x43606C04, 0xBC967669, 0x3860B1B8, 0x22A8BE6A, 0x017571A9, 0x1ABEC8C5,
         0xD7C0022D, 0xB4CB39F1, 0x9ECE0E33, 0x50C1515E, 0xDE08A228, 0xEC17D1E3, 0x419BBC98, 0xC12C8772, 0x62D27C81, 0xE5339B3D,
         0xA3EECC60, 0xE0473230, 0x1CB7BD82, 0x9C51DC5E, 0x9261E418, 0xE4596253, 0xB343C8E3, 0xE8F21E06, 0x9AFAB2BC, 0x738940CB,
         0x48038752, 0x133AA80D, 0x020764FB, 0xC60EBA5A, 0xC49A89A5, 0x3D3DE7EA, 0xB01A8923, 0x8F48DB32, 0xEDCE2B56, 0x3764D2F1,
         0xC35DF880, 0x7CE76D00, 0x31C58FB3, 0xE7BEDF9F, 0x98F83792, 0x511BBB05, 0x15F4F643, 0x00ADC096, 0xCEAC6180, 0xD8391CC1,
         0xB0DD2AD5, 0xBA2EEE6C, 0xA692904F, 0x18BBF3CB, 0x6306AF64, 0x993EFF31, 0x91B5F477, 0x852A997A, 0xA5DFB161, 0x21D67846,
         0x2B4224D6, 0x24CA1917, 0x17ADF439, 0x78418CA5, 0x03D0B022, 0x3693F21D, 0xD61B2B63, 0x196C5D5D, 0xE6D385C6, 0xE72E3A11,
         0xD25186CD, 0x9C6A3EC8, 0x69529A29, 0xB73F0101, 0xCAAAFAFF, 0x6554000A, 0x2C34F09B, 0x53941A6A, 0xF725198A, 0x327BC03F,
         0xE235948F, 0x6C06B56F, 0xF494F2CD, 0x08E058CA, 0xA4C05EE2, 0x8A8CF8B5, 0x232CAC64, 0xB67C8BBF, 0x7B7B135E, 0x5AD471A9,
         0x1CCC1B7C, 0x31EB0D8F, 0x3F808538, 0x4C5F8ECD, 0x376F21D5, 0xB827148D, 0xFF41D9DE, 0xCBDC1DD2, 0xF34CA674, 0x65E14FA2,
         0x30B66C49, 0xF436ECCF, 0xC29C9F60, 0x1915B91E, 0xC9850CCA, 0x14F7A914, 0x4F717389, 0xCAC66432, 0xC46F8F0B, 0xBE2F0A8A,
         0x3AE4F1D5, 0x2CB71073, 0x500BB014, 0xA636E43B, 0x3C7DD8E1, 0xD9C0F3CB, 0x29389729, 0xADE77969, 0x30C345D1, 0xA6530A91,
         0x34B65172, 0xED4E0D1C, 0xC34E6988, 0x4D1DD5AE, 0x9628BD4C, 0x1BDD4D2A, 0x03040EE8, 0x317C4987, 0x7B2415D9, 0x49F6C16B,
         0x227BEA07, 0x8370ACCC, 0x728A1713, 0x12E9BE7A, 0x307CF3A6, 0x62ED03E5, 0x0BFAA1FC, 0x04CB7E45, 0xB76AA8AF, 0x06CAA2E2,
         0x33AC97BB, 0xDDC4A6F3, 0x1B4A0472, 0x22C80010, 0x0D000000, 0x06D709D3, 0xE7AEDDD4, 0x5CE37982, 0x089CC057, 0xE99B526A,
         0x3DB67983, 0x9058895D, 0x44F4DDC0, 0xD2C5F2A7, 0x7E9AF5DA, 0x6AE89107, 0x951AA187, 0x1D45F058, 0x47EBB6DE, 0x2800FF34,
         0x805377BC, 0x2A67823D, 0x38DE9322, 0x94F8BBA3, 0x62ED0F98, 0x125787A7, 0xB08CF555, 0x536BC246, 0x236F09E8, 0x71B9F5EC,
         0x71DF1A15, 0xEC7F3705, 0xAA9B299B, 0x03ED228B, 0x409D426C, 0x4CE8BBEF, 0x1E5F70F9, 0x4B997531, 0x90847AE0, 0xFC9F05C1,
         0x64DA3442, 0x008E0F4F, 0xAC11ACAE, 0x0C991B7A, 0x00E12334, 0x3768439C, 0x5E6F49FB, 0x78AF1AD1, 0x83A838A6, 0xCB887ECD,
         0x7B0FF1CB, 0x7CFC6DB1, 0xB5699444, 0x9D8F0276, 0xCF2F2343, 0x3EA14E5F, 0x87605162, 0x81215FC0, 0x54B2308E, 0x9D4C7763,
         0xB9825CDA, 0x963A3258, 0x8E08BD77, 0xCFA7BC42, 0x5EE2F113, 0x397BF309, 0xBCD64FF0, 0x926C71AF, 0x77B10559, 0xB5DB46F3,
         0x0C8B6FE6, 0x7681475A, 0x0DA5D12A, 0x72715CD5, 0xA4A994BD, 0x1BC4E910, 0x002CEB93, 0x2D048BBB, 0x410CD831, 0x6F8B5A73,
         0x368886F3, 0xEA71C0BA, 0x16A615C4, 0x9D70BB02, 0x05D63DF9, 0x039B09C4, 0x44F5048E, 0x05FA5175, 0x1DEF44C0, 0x8EAA8434,
         0x1ACD89E3, 0xC3AECAA1, 0xEA1E2DFF, 0xD61B5E3E, 0x7C3F69E8, 0xB5BAA6AD, 0x8BF8FB62, 0xBB5482D8, 0x0B008529, 0xDDC87BAA,
         0x95F6B248, 0x8454184D, 0x84742FBA, 0x082D9846, 0x6A937439, 0xECC674EE, 0x3C650181, 0x7261FA46, 0x30E043F3, 0xF206794A,
         0x8AF6835D, 0x9F14A5D0, 0xC620F765, 0xB5FF2646, 0x4F1C0935, 0x7563AC12, 0x217CA59D, 0xC5723908, 0x3BE4F764, 0xAEF51C32,
         0x1931F65C, 0xD67E7BAE, 0xD001A2F1, 0xD5DB290F, 0xF5E1B470};

    const uint8_t* lmsMsg_pu8 = NULL;
    const uint8_t* signature_pu8 = NULL;
    const uint8_t* backupKeyId_pu8 = NULL;
#endif // LMS_SAMPLE_PREPARE_IN_ADVANCE

    /*-------------------------------------------------------------------------------------------------------
     Initialize KeyId to the stored key id that match the configured public key on flash
    -------------------------------------------------------------------------------------------------------*/
    QLIB_SAMPLE_LmsKeyBackup_retrieveKid_L(keyId);

#ifdef LMS_SAMPLE_PREPARE_IN_ADVANCE
    if (memcmp(keyId, backupKeyId1, sizeof(QLIB_LMS_KEY_ID_T)) == 0u)
    {
        backupKeyId_pu8 = backupKeyId2;
        lmsMsg_pu8      = lmsMsg2;
        signature_pu8   = (const uint8_t*)signature2_u32;
    }
    else if (memcmp(keyId, backupKeyId2, sizeof(QLIB_LMS_KEY_ID_T)) == 0u)
    {
        backupKeyId_pu8 = backupKeyId1;
        lmsMsg_pu8      = lmsMsg1;
        signature_pu8   = (const uint8_t*)signature1_u32;
    }
    else
    {
        return QLIB_STATUS__SYSTEM_IN_INCORRECT_STATE;
    }
#else
    // lms new public key is obtained from hsm
    QLIB_STATUS_RET_CHECK(QLIB_LMS_HsmGenerateKeyPair(backupKeyId_pu8));
    QLIB_STATUS_RET_CHECK(QLIB_LMS_HsmGetKey(backupKeyId_pu8, backupPublicKey));

    /*-------------------------------------------------------------------------------------------------------
     Create the LMS_SET_KEY command
    -------------------------------------------------------------------------------------------------------*/
    QLIB_STATUS_RET_CHECK(QLIB_UTILS_LMS_CreateSetKeyMsg(sectionID_u32, backupPublicKey, lmsMsg_pu8));

    /*-------------------------------------------------------------------------------------------------------
     sign the LMS_SET_KEY command using the current key
    -------------------------------------------------------------------------------------------------------*/
    QLIB_STATUS_RET_CHECK(QLIB_LMS_HsmSign(&signParams));
#endif // LMS_SAMPLE_PREPARE_IN_ADVANCE
    /*-------------------------------------------------------------------------------------------------------
     store the LMS BACKUP command for future key replacement
    -------------------------------------------------------------------------------------------------------*/
    QLIB_SAMPLE_LmsKeyBackup_storeBackupCmd_L(backupKeyId_pu8, lmsMsg_pu8, signature_pu8);

    return QLIB_STATUS__OK;
}

/************************************************************************************************************
 * @brief       This routine stores the key id for future usage
 *
 * @param[in]  keyId
 *************************************************************************************************************/
static void QLIB_SAMPLE_LmsKeyBackup_storeKid_L(const QLIB_LMS_KEY_ID_T keyId)
{
    (void)memcpy(lms_kid_L, keyId, sizeof(QLIB_LMS_KEY_ID_T));
}
/************************************************************************************************************
 * @brief       This routine retrieves the stored key Id
 *
 * @param[out]  keyId
************************************************************************************************************/
static void QLIB_SAMPLE_LmsKeyBackup_retrieveKid_L(QLIB_LMS_KEY_ID_T keyId)
{
    (void)memcpy(keyId, lms_kid_L, sizeof(QLIB_LMS_KEY_ID_T));
}

/************************************************************************************************************
 * @brief       This routine stores the backup command parameters
 *
 * @param[in]  keyId
 * @param[in]  lmsMsg_pu8
 * @param[in]  signature_pu8
 *************************************************************************************************************/
static void QLIB_SAMPLE_LmsKeyBackup_storeBackupCmd_L(const QLIB_LMS_KEY_ID_T          keyId,
                                                      const QLIB_LMS_MSG_DATA_STRUCT_T lmsMsg_pu8,
                                                      const QLIB_LMS_SIG_BUFFER_T      signature_pu8)
{
    (void)memcpy(backupCmd_g.keyId, keyId, sizeof(QLIB_LMS_KEY_ID_T));
    (void)memcpy(backupCmd_g.lmsMsg_pu8, lmsMsg_pu8, sizeof(QLIB_LMS_MSG_DATA_STRUCT_T));
    (void)memcpy(backupCmd_g.sig, signature_pu8, sizeof(QLIB_LMS_SIG_BUFFER_T));
}

/************************************************************************************************************
 * @brief       This routine retrieves the backup command parameters
 *
 * @param[out]  keyId
 * @param[out]  lmsMsg_pu8
 * @param[out]  signature_pu8
************************************************************************************************************/
static void QLIB_SAMPLE_LmsKeyBackup_retrieveBackupCmd_L(QLIB_LMS_KEY_ID_T          keyId,
                                                         QLIB_LMS_MSG_DATA_STRUCT_T lmsMsg_pu8,
                                                         QLIB_LMS_SIG_BUFFER_T      signature_pu8)
{
    (void)memcpy(keyId, backupCmd_g.keyId, sizeof(QLIB_LMS_KEY_ID_T));
    (void)memcpy(lmsMsg_pu8, backupCmd_g.lmsMsg_pu8, sizeof(QLIB_LMS_MSG_DATA_STRUCT_T));
    (void)memcpy(signature_pu8, backupCmd_g.sig, sizeof(QLIB_LMS_SIG_BUFFER_T));
}
#endif // !defined(EXCLUDE_LMS) && (defined(LMS_SAMPLE_PREPARE_IN_ADVANCE) || !defined(VERIFY_ONLY))
