/**
 * Copyright (c) 2018 TGT, Inc. All Rights Reserved.
 * Date   : 12/3/18 1:31 PM
 * Author : Zhou Linlin
 * e-mail : 461146760@qq.com
 * */

#ifndef SECURITY_IOT_SYMMETRIC_ENCRYPTION_H
#define SECURITY_IOT_SYMMETRIC_ENCRYPTION_H

#include "types.h"

#ifdef __cplusplus
extern "C"{
#endif

int encrypt_string_to_file(const char *message, const char *filepath);

int decrypt_string_from_file(const char *filepath, char *message, size_t *message_len);

int symmetric_encryption_test1(void);

#ifdef __cplusplus
}
#endif
#endif // SECURITY_IOT_SYMMETRIC_ENCRYPTION_H
