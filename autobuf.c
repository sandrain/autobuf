/* Copyright (C) 2018 - Hyogi Sim <simh@ornl.gov>
 * 
 * Please refer to COPYING for the license.
 * ---------------------------------------------------------------------------
 * 
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <errno.h>

#include "autobuf.h"

int autobuf_init(autobuf_t *abuf, size_t size)
{
    uint64_t alloc_size = AUTOBUF_DEFAULT_SIZE;
    char *mem = NULL;

    if (!abuf)
        return -EINVAL;

    if (size > 0)
        alloc_size = size;

    mem = malloc(alloc_size);
    if (!mem)
        return -ENOMEM;

    abuf->buf = mem;
    abuf->pos = mem;
    abuf->buflen = alloc_size;

    return 0;
}

static int autobuf_increase(autobuf_t *abuf, size_t datalen)
{
    char *new_buf = NULL;
    uint64_t new_buflen = abuf->buflen*2;
    uint64_t buf_used = autobuf_get_buflen(abuf);

    while ((new_buflen - buf_used) <= datalen)
        new_buflen *= 2;

    new_buf = realloc(abuf->buf, new_buflen);
    if (!new_buf)
        return -ENOMEM;

    abuf->buf = new_buf;
    abuf->pos = &new_buf[buf_used];
    abuf->buflen = new_buflen;
    abuf->realloc_calls++;

    return 0;
}

static inline
void __autobuf_append(autobuf_t *abuf, const char *data, size_t datalen)
{
    char *pos = abuf->pos;

    memcpy((void *) pos, (const void *) data, datalen);
    abuf->pos = &pos[datalen];
    *abuf->pos = '\0';
}

int autobuf_append_data(autobuf_t *abuf, const char *data, size_t datalen)
{
    uint64_t buf_used = 0;

    if (!abuf)
        return -EINVAL;

    buf_used = autobuf_get_buflen(abuf);

    if ((abuf->buflen - buf_used) <= datalen) {
        int ret = autobuf_increase(abuf, datalen);

        if (ret < 0)
            return ret;
    }

    __autobuf_append(abuf, data, datalen);

    return 0;
}

