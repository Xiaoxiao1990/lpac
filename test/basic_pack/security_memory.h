/**
 * Copyright (c) 2018 TGT, Inc. All Rights Reserved.
 * Date   : 8/29/18 2:59 PM
 * Author : Zhou Linlin
 * e-mail : 461146760@qq.com
 * */

#ifndef SECURITY_IOT_SECURITY_MEMORY_H
#define SECURITY_IOT_SECURITY_MEMORY_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

void *security_malloc(size_t size);
void *security_zerolloc(size_t size);
void security_free(void *ptr);
void *security_memset(void *ptr, int n, size_t c);

#ifdef __cplusplus
}
#endif
#endif // SECURITY_IOT_SECURITY_MEMORY_H
