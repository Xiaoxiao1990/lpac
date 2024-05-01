/*
 * Created by smallfish on 12/21/20.
 */

#include "buffer.h"
#include "security_memory.h"
#include "logs.h"

int buffer_init(buffer_t *buf, uint32_t size)
{
    buf->begin = (uint8_t *)security_zerolloc(size);
    if (buf->begin == NULL) {
        LOGI("No space for buffer allocate now.");
        return -1;
    }

    buf->pos = buf->begin;
    buf->end = buf->begin + size;

    buf->len = 0;

    return 0;
}

void buffer_deinit(buffer_t *buf)
{
    if (buf->begin)
        security_free(buf->begin);

    security_memset(buf, 0, sizeof(buffer_t));

    buf->begin = buf->end = buf->pos = NULL;
    buf->len = 0;
}