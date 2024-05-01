/*
 * Created by smallfish on 12/21/20.
 */

#ifndef __LINUX_CPE_BUFFER_H__
#define __LINUX_CPE_BUFFER_H__

#include "types.h"

#ifdef __cplusplus
extern "C"{
#endif

typedef struct buffer_s {
    char *begin;
    char *pos;
    char *end;
    size_t len;
} buffer_t;


int buffer_init(buffer_t *buf, uint32_t size);
void buffer_deinit(buffer_t *buf);

#ifdef __cplusplus
}
#endif

#endif //__LINUX_CPE_BUFFER_H__
