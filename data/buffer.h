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

#ifndef CRE_BUFFER_H
#define CRE_BUFFER_H

#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif


#if defined __cplusplus
    #define BUFFER_INLINE__     inline
#elif defined __STDC_VERSION__ && __STDC_VERSION__ >= 199901L
    #define BUFFER_INLINE__     static inline
#elif defined __GNUC__
    #define BUFFER_INLINE__     static __inline__
#elif defined _MSC_VER
    #define BUFFER_INLINE__     static __inline
#else
    #define BUFFER_INLINE__     static
#endif


typedef struct BUFFER {
    char* data;
    size_t size;
    size_t alloc;
} BUFFER;


/* Static initializer. */
#define BUFFER_INITIALIZER          { NULL, 0, 0 }

/* Initialize/deinitialize buffer structure. */
BUFFER_INLINE__ void buffer_init(BUFFER* buf)
        { buf->data = NULL; buf->size = 0; buf->alloc = 0; }
BUFFER_INLINE__ void buffer_fini(BUFFER* buf)
        { free(buf->data); }

/* Allocate/free buffer structure on heap.
 * buffer_alloc() is moral equivalent of malloc() + buffer_init(),
 * buffer_free() is moral equivalent if buffer_fini() + free() */
BUFFER* buffer_alloc(void);
void buffer_free(BUFFER* buf);

/* Change capacity of the buffer. If lower then current size, the data contents
 * beyond the new buffer capacity is lost. */
int buffer_realloc(BUFFER* buf, size_t alloc);
/* Reserve new space for extra_alloc bytes at the end of the buffer. */
int buffer_reserve(BUFFER* buf, size_t extra_alloc);
/* Remove the empty space from the buffer. */
void buffer_shrink(BUFFER* buf);

/* Contents getters. */
BUFFER_INLINE__ char* buffer_data_at(BUFFER* buf, size_t pos)
        { return buf->data + pos; }
BUFFER_INLINE__ char* buffer_data(BUFFER* buf)
        { return buf->data; }
BUFFER_INLINE__ size_t buffer_size(BUFFER* buf)
        { return buf->size; }

/* Contents modifiers. */
int buffer_insert(BUFFER* buf, size_t pos, const char* data, size_t n);
BUFFER_INLINE__ int buffer_append(BUFFER* buf, const char* data, size_t n)
        { return buffer_insert(buf, buf->size, data, n); }
void buffer_remove(BUFFER* buf, size_t pos, size_t n);
BUFFER_INLINE__ void buffer_clear(BUFFER* buf)
        { buffer_remove(buf, 0, buf->size); }


#ifdef __cplusplus
}  /* extern "C" { */
#endif

#endif  /* CRE_BUFFER_H */
