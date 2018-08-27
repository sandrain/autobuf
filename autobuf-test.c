#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>
#include <sys/time.h>

#include "autobuf.h"

static autobuf_t _abuf;

static uint64_t initsize = 16;
static uint64_t testsize = 128*(1<<10);
static uint64_t chunksize = 64;

static char *chunk;

static struct timeval before, after;

static inline double timediff(struct timeval *t1, struct timeval *t2)
{
    double sec = 1.0F*(t2->tv_sec - t1->tv_sec);
    double usec = 1.0F*(t2->tv_usec - t1->tv_usec);

    return sec + usec*(1e-6);
}

static void buffer_populate(uint64_t iter)
{
    uint64_t i = 0;
    uint64_t start = iter*sizeof(uint64_t);
    uint64_t iterations = chunksize/sizeof(uint64_t);
    uint64_t *ibuf = (uint64_t *) chunk;

    for (i = 0; i < iterations; i++)
        ibuf[i] = start + i;
}

static int buffer_write(void)
{
    int ret = 0;
    uint64_t i = 0;
    uint64_t iterations = testsize/chunksize;
    autobuf_t *abuf = &_abuf;

    for (i = 0; i < iterations; i++) {
        buffer_populate(i);

        ret = autobuf_append_data(abuf, chunk, chunksize);
        assert(ret == 0);
    }

    return 0;
}

static int buffer_verify(void)
{
    uint64_t i = 0;
    uint64_t iterations = testsize/chunksize;
    const char *pos = NULL;

    autobuf_t *abuf = &_abuf;
    uint64_t len = autobuf_get_buflen(abuf);

    assert(len == testsize);

    for (i = 0; i < iterations; i++) {
        buffer_populate(i);

        pos = autobuf_get_buf(abuf);
        pos = &pos[chunksize*i];

        assert(0 == memcmp((const void *) chunk, (const void *) pos,
                           chunksize));
    }

    return 0;
}

int main(int argc, char **argv)
{
    int ret = 0;
    autobuf_t *abuf = &_abuf;

    if (argc == 4) {
        initsize = strtoull(argv[1], NULL, 0);
        testsize = strtoull(argv[2], NULL, 0);
        chunksize = strtoull(argv[3], NULL, 0);
    }

    assert(chunksize%sizeof(uint64_t) == 0);
    assert(testsize%chunksize == 0);

    printf("initsize  = %lu\n", initsize);
    printf("testsize  = %lu\n", testsize);
    printf("chunksize = %lu\n", chunksize);

    ret = autobuf_init(abuf, initsize);
    assert(ret == 0);

    chunk = calloc(1, chunksize);
    assert(chunk);

    printf("\nwriting to buffer... ");
    fflush(stdout);

    gettimeofday(&before, NULL);

    buffer_write();

    gettimeofday(&after, NULL);

    printf("done, %lf seconds.\n", timediff(&before, &after));

    printf("\nverifying the content... ");
    fflush(stdout);

    gettimeofday(&before, NULL);

    buffer_verify();

    gettimeofday(&after, NULL);

    printf("done, %lf seconds.\n", timediff(&before, &after));

    autobuf_dump(abuf, stdout);

    printf("test successfully finished.\n\n");

    autobuf_fini(abuf);

    return 0;
}
