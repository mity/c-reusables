/*
 * C Reusables
 * <http://github.com/mity/c-reusables>
 *
 * Copyright (c) 2018-2023 Martin Mitas
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 */

#include "buffer.h"


/* Mitigate heap fragmentation by rounding buffer allocation sizes to
 * reasonable numbers. */
static size_t
buffer_good_alloc_size(size_t alloc)
{
    size_t good_alloc;

    if(alloc <= 256) {
        good_alloc = 8;
        while(good_alloc < alloc)
            good_alloc *= 2;
    } else if(alloc > SIZE_MAX / 2) {
        /* Prevent overflow in the branch below. */
        good_alloc = SIZE_MAX;
    } else {
        /* For larger buffers, we subtract 32 bytes as the libc heap allocator
         * needs some space for internal bookkeeping, and these would cause
         * that two small blocks cannot fit into a window previously freed
         * from twice as malloc'ed block.
         *
         * (AFAIK, most allocators use 8 or 16 bytes for the purpose, but lets
         * be little bit more conservative.)
         */
        good_alloc = 512;
        while(good_alloc-32 < alloc)
            good_alloc *= 2;
        good_alloc -= 32;
    }

    return good_alloc;
}


int
buffer_realloc(BUFFER* buf, size_t alloc)
{
    void* tmp;

    if(alloc == buf->alloc)
        return 0;

    if(alloc == 0) {
        free(buf->data);
        buffer_init(buf);
        return 0;
    }

    tmp = realloc(buf->data, alloc);
    if(tmp == NULL)
        return -1;

    buf->data = tmp;
    buf->alloc = alloc;
    if(alloc < buf->size)
        buf->size = alloc;
    return 0;
}

int
buffer_reserve(BUFFER* buf, size_t n)
{
    size_t alloc;

    if(buf->size + n < buf->alloc)
        return 0;

    if(n > SIZE_MAX - buf->size)
        return -1;
    alloc = buf->size + n;
    alloc = buffer_good_alloc_size(alloc);

    return buffer_realloc(buf, alloc);
}

void
buffer_shrink(BUFFER* buf)
{
    /* Avoid realloc() if the potential memory gain is negligible. */
    if((0 < buf->alloc && buf->alloc < 8)  ||  buf->alloc / 8 < buf->size / 7)
        return;

    buffer_realloc(buf, buf->size);
}

void*
buffer_insert_raw(BUFFER* buf, size_t pos, size_t n)
{
    if(buf->size + n > buf->alloc) {
        if(buffer_reserve(buf, n) != 0)
            return NULL;
    }

    if(buf->size > pos)
        memmove((uint8_t*)buf->data + pos + n, (uint8_t*)buf->data + pos, buf->size - pos);

    buf->size += n;
    return buffer_data_at(buf, pos);
}

int
buffer_insert(BUFFER* buf, size_t pos, const void* data, size_t n)
{
    void* ptr;

    ptr = buffer_insert_raw(buf, pos, n);
    if(ptr == NULL)
        return -1;

    memcpy(ptr, data, n);
    return 0;
}

void
buffer_remove(BUFFER* buf, size_t pos, size_t n)
{
    if(pos + n < buf->size) {
        memmove((uint8_t*)buf->data + pos, (uint8_t*)buf->data + pos + n, n);
        buf->size -= n;
    } else {
        buf->size = pos;
    }

    if(buf->size == 0) {
        free(buf->data);
        buf->data = NULL;
        buf->alloc = 0;
    } else if(buf->size < buf->alloc / 4) {
        /* No error checking here: If the realloc fails, we still have valid
         * albeit bloated buffer. */
        buffer_realloc(buf, buffer_good_alloc_size(buf->size * 2));
    }
}

