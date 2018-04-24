/*
 * C Reusables
 * <http://github.com/mity/c-reusables>
 *
 * Copyright (c) 2018 Martin Mitas
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

#include <string.h>


BUFFER*
buffer_alloc(void)
{
    BUFFER* buf;

    buf = (BUFFER*) malloc(sizeof(BUFFER));
    if(buf != NULL)
        buffer_init(buf);
    return buf;
}

void
buffer_free(BUFFER* buf)
{
    buffer_fini(buf);
    free(buf);
}

int
buffer_realloc(BUFFER* buf, size_t alloc)
{
    char* tmp;

    tmp = realloc(buf->data, alloc);
    if(tmp == NULL  &&  alloc > 0)
        return -1;

    buf->data = tmp;
    buf->alloc = alloc;
    if(buf->size > alloc)
        buf->size = alloc;
    return 0;
}

int
buffer_reserve(BUFFER* buf, size_t extra_alloc)
{
    if(buf->size + extra_alloc > buf->alloc)
        return buffer_realloc(buf, buf->size + extra_alloc);
    else
        return 0;
}

void
buffer_shrink(BUFFER* buf)
{
    /* Avoid realloc() if the potential memory gain is negligible. */
    if(buf->alloc / 4 > buf->size / 3)
        buffer_realloc(buf, buf->size);
}

int
buffer_insert(BUFFER* buf, size_t pos, const char* data, size_t n)
{
    if(buf->size + n > buf->alloc) {
        if(buffer_realloc(buf, (buf->size + n) * 2) != 0)
            return -1;
    }

    if(buf->size > pos)
        memmove(buf->data + pos + n, buf->data + pos, buf->size - pos);
    memcpy(buf->data + pos, data, n);
    buf->size += n;
    return 0;
}

void
buffer_remove(BUFFER* buf, size_t pos, size_t n)
{
    if(pos + n < buf->size) {
        memmove(buf->data + pos, buf->data + pos + n, n);
        buf->size -= n;
    } else {
        buf->size = pos;
    }
}

