/* Copyright (C) 2018 - Hyogi Sim <simh@ornl.gov>
 * 
 * Please refer to COPYING for the license.
 * ---------------------------------------------------------------------------
 * 
 */
#ifndef __AUTOBUF_H
#define __AUTOBUF_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>

struct _autobuf {
    char *buf;
    char *pos;
    uint64_t buflen;

    uint64_t realloc_calls;
};

typedef struct _autobuf autobuf_t;

#define AUTOBUF_DEFAULT_SIZE            (512*(1<<10))

int autobuf_init(autobuf_t *abuf, size_t size);

static inline void autobuf_fini(autobuf_t *abuf)
{
    if (abuf && abuf->buf)
        free(abuf->buf);
}

int autobuf_append_data(autobuf_t *abuf, const char *data, size_t datalen);

static inline int autobuf_append_string(autobuf_t *abuf, const char *str)
{
    return autobuf_append_data(abuf, str, strlen(str));
}

static inline const char *autobuf_get_buf(autobuf_t *abuf)
{
    const char *ret = NULL;

    if (abuf)
        ret = abuf->buf;
    else
        errno = EINVAL;

    return ret;
}

static inline ssize_t autobuf_get_buflen(autobuf_t *abuf)
{
    return abuf ? (unsigned long) abuf->pos - (unsigned long) abuf->buf
                : -EINVAL;
}

static inline void autobuf_dump(autobuf_t *abuf, FILE *fp)
{
    if (abuf) {
        fprintf(fp, "\n## autobuf status (%p)"
                    "\n## . buf           = %p"
                    "\n## . pos           = %p"
                    "\n## . buflen        = %lu bytes"
                    "\n## . datasize      = %lu bytes"
                    "\n## . realloc calls = %lu\n\n",
                    abuf,
                    abuf->buf,
                    abuf->pos,
                    abuf->buflen,
                    autobuf_get_buflen(abuf),
                    abuf->realloc_calls);
    }
}

#endif /* __AUTOBUF_H */
