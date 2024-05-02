/*********************************************************************
 * Author: Zhou Linlin
 * E-mail: 461146760@qq.com
 * Date  : 2022-08-12 19:42:39.
 * Copyright (c) 2022 all rights reserved.
*********************************************************************/

#ifndef __HAL_UART_H__
#define __HAL_UART_H__

#include "types.h"

#define UART_BUFFER_SIZE			1024

#ifdef __cplusplus
extern "C" {
#endif

struct Buffer{
	char *bgn;
	char *pos;
	char *end;
	u32 bytes;
};

typedef struct{
	const char *dev_path;
	int baud_rate;
	int data_bits;
	int stop_bits;
	char parity;
	int fd;
	struct Buffer *buffer;
//    int (*read_callback)(void *arg, u32 count);
//    int (*write_callback)(void *arg, u32 count);
} uart_t;

#ifdef __cplusplus
}
#endif

int uart_init(uart_t *uart, int has_buffer);
void uart_deinit(uart_t *uart, int has_buffer);

#endif /* __HAL_UART_H__ */