#pragma once

#include "euicc.h"

enum es10b_profile_management_operation
{
    ES10B_PROFILE_MANAGEMENT_OPERATION_NULL = -1,
    ES10B_PROFILE_MANAGEMENT_OPERATION_INSTALL = 0x80,
    ES10B_PROFILE_MANAGEMENT_OPERATION_ENABLE = 0x40,
    ES10B_PROFILE_MANAGEMENT_OPERATION_DISABLE = 0x20,
    ES10B_PROFILE_MANAGEMENT_OPERATION_DELETE = 0x10,
    ES10B_PROFILE_MANAGEMENT_OPERATION_UNDEFINED = 0xFF,
};

enum es10b_bpp_command_id
{
    ES10B_BPP_COMMAND_ID_INITIALISE_SECURE_CHANNEL = 0,
    ES10B_BPP_COMMAND_ID_CONFIGURE_ISDP = 1,
    ES10B_BPP_COMMAND_ID_STORE_METADATA = 2,
    ES10B_BPP_COMMAND_ID_STORE_METADATA2 = 3,
    ES10B_BPP_COMMAND_ID_REPLACE_SESSION_KEYS = 4,
    ES10B_BPP_COMMAND_ID_LOAD_PROFILE_ELEMENTS = 5,
    ES10B_BPP_COMMAND_ID_UNDEFINED = 0xFF,
};

enum es10b_error_reason
{
    ES10B_ERROR_REASON_INCORRECT_INPUT_VALUES = 1,
    ES10B_ERROR_REASON_INVALID_SIGNATURE = 2,
    ES10B_ERROR_REASON_INVALID_TRANSACTION_ID = 3,
    ES10B_ERROR_REASON_UNSUPPORTED_CRT_VALUES = 4,
    ES10B_ERROR_REASON_UNSUPPORTED_REMOTE_OPERATION_TYPE = 5,
    ES10B_ERROR_REASON_UNSUPPORTED_PROFILE_CLASS = 6,
    ES10B_ERROR_REASON_SCP03T_STRUCTURE_ERROR = 7,
    ES10B_ERROR_REASON_SCP03T_SECURITY_ERROR = 8,
    ES10B_ERROR_REASON_INSTALL_FAILED_DUE_TO_ICCID_ALREADY_EXISTS_ON_EUICC = 9,
    ES10B_ERROR_REASON_INSTALL_FAILED_DUE_TO_INSUFFICIENT_MEMORY_FOR_PROFILE = 10,
    ES10B_ERROR_REASON_INSTALL_FAILED_DUE_TO_INTERRUPTION = 11,
    ES10B_ERROR_REASON_INSTALL_FAILED_DUE_TO_PE_PROCESSING_ERROR = 12,
    ES10B_ERROR_REASON_INSTALL_FAILED_DUE_TO_ICCID_MISMATCH = 13,
    ES10B_ERROR_REASON_TEST_PROFILE_INSTALL_FAILED_DUE_TO_INVALID_NAA_KEY = 14,
    ES10B_ERROR_REASON_PPR_NOT_ALLOWED = 15,
    ES10B_ERROR_REASON_INSTALL_FAILED_DUE_TO_UNKNOWN_ERROR = 127,
    ES10B_ERROR_REASON_UNDEFINED = 0xFF,
};

struct es10b_load_bound_profile_package_result
{
    enum es10b_bpp_command_id bppCommandId;
    enum es10b_error_reason errorReason;
};

struct es10b_prepare_download_param
{
    const char *b64_profileMetadata;
    const char *b64_smdpSigned2;
    const char *b64_smdpSignature2;
    const char *b64_smdpCertificate;
    const char *confirmationCode;
};

struct es10b_notification_metadata_list
{
    unsigned long seqNumber;
    enum es10b_profile_management_operation profileManagementOperation;
    char *notificationAddress;
    char *iccid;

    struct es10b_notification_metadata_list *next;
};

struct es10b_pending_notification
{
    char *notificationAddress;
    char *b64_PendingNotification;
};

struct es10b_authenticate_server_param
{
    const char *b64_serverSigned1;
    const char *b64_serverSignature1;
    const char *b64_euiccCiPKIdToBeUsed;
    const char *b64_serverCertificate;
    const char *matchingId;
    const char *imei;
};

struct es10b_cancel_session_param
{
    const uint8_t *transactionId;
    uint8_t transactionIdLen;
    enum es10b_cancel_session_reason
    {
        ES10B_CANCEL_SESSION_REASON_ENDUSERREJECTION = 0,
        ES10B_CANCEL_SESSION_REASON_POSTPONED = 1,
        ES10B_CANCEL_SESSION_REASON_TIMEOUT = 2,
        ES10B_CANCEL_SESSION_REASON_PPRNOTALLOWED = 3,
        ES10B_CANCEL_SESSION_REASON_METADATAMISMATCH = 4,
        ES10B_CANCEL_SESSION_REASON_LOADBPPEXECUTIONERROR = 5,
        ES10B_CANCEL_SESSION_REASON_UNDEFINED = 127
    } reason;
};

int es10b_prepare_download(struct euicc_ctx *ctx, char **b64_PrepareDownloadResponse, struct es10b_prepare_download_param *param);
int es10b_load_bound_profile_package(struct euicc_ctx *ctx, struct es10b_load_bound_profile_package_result *result, const char *b64_BoundProfilePackage);
int es10b_get_euicc_challenge(struct euicc_ctx *ctx, char **b64_euiccChallenge);
int es10b_get_euicc_info(struct euicc_ctx *ctx, char **b64_EUICCInfo1);
int es10b_list_notification(struct euicc_ctx *ctx, struct es10b_notification_metadata_list **notificationMetadataList);
int es10b_retrieve_notifications_list(struct euicc_ctx *ctx, struct es10b_pending_notification *PendingNotification, unsigned long seqNumber);
int es10b_remove_notification_from_list(struct euicc_ctx *ctx, unsigned long seqNumber);
int es10b_authenticate_server(struct euicc_ctx *ctx, char **b64_AuthenticateServerResponse, struct es10b_authenticate_server_param *param);
int es10b_cancel_session(struct euicc_ctx *ctx, char **b64_CancelSessionResponse, struct es10b_cancel_session_param *param);

void es10b_notification_metadata_free_all(struct es10b_notification_metadata_list *notificationMetadataList);
void es10b_notification_free(struct es10b_pending_notification *PendingNotification);
