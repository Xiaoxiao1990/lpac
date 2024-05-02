/**
 * Copyright (c) 2018 TGT, Inc. All Rights Reserved.
 * Date   : 8/29/18 2:59 PM
 * Author : Zhou Linlin
 * e-mail : 461146760@qq.com
 * */

#include "security_memory.h"

#include <stdlib.h>
#include <string.h>

void *security_malloc(size_t size)
{
	return malloc(size);
}

void *security_zerolloc(size_t size)
{
	void *p = security_malloc(size);

	memset(p, 0, size);

	return p;
}

void security_free(void *ptr)
{
	if (ptr == NULL)
		return;

	free(ptr);

	ptr = NULL;
}

void *security_memset(void *ptr, int n, size_t size)
{
	if (ptr == NULL)
		return NULL;
	memset(ptr, n, size);
}