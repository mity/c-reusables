/*
 * C Reusables
 * <http://github.com/mity/c-reusables>
 *
 * Copyright (c) 2020-2023 Martin Mitas
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

#ifndef CRE_MEMCHUNK_H
#define CRE_MEMCHUNK_H

#include "stdlib.h"

#ifdef __cplusplus
extern "C" {
#endif


/* Implementation of a simple chunk memory allocator.
 *
 * Often, programs need to incrementally allocate many small pieces of memory
 * which shall eventually all be freed at the same time.
 *
 * In such use case, this allocator may be much more appropriate than general
 * purpose allocator (`malloc()` + `free()`), as it brings multiple benefits:
 *
 * -- Smaller memory overhead per individual allocation. `malloc()` usually
 *    needs extra 8 or 16 bytes per-allocation for its internal bookkeeping.
 *
 * -- The individual allocation is much faster.
 *
 * -- Freeing all the memory is also faster.
 *
 * Limitations:
 *
 * -- Do not use for allocations which need to be released individually.
 * -- Do not use one instance of `MEMCHUNK` concurrently from multiple threads.
 * -- Reallocations are not possible.
 */


#define MEMCHUNK_DEFAULT_BLOCK_SIZE     1024


typedef struct MEMCHUNK_BLOCK MEMCHUNK_BLOCK;


/* The allocator structure. Treat as opaque. */
typedef struct MEMCHUNK {
    MEMCHUNK_BLOCK* head;
    size_t block_size;
    size_t free_off;
} MEMCHUNK;


#define MEMCHUNK_INITIALIZER(block_size)                                \
            { NULL, ((block_size) == 0) ? MEMCHUNK_DEFAULT_BLOCK_SIZE : (block_size), 0 }


/* Initialize the chunk allocator.
 *
 * The block_size specifies the size of the larger blocks allocated under the
 * hood. Using zero means a default block size (currently 1 kB).
 */
void memchunk_init(MEMCHUNK* chunk, size_t block_size);

/* Allocate a (small) memory from the chunk allocator.
 *
 * It will only be released when memchunk_fini() is called (alongside all other
 * memory pieces allocated by the same allocator).
 */
void* memchunk_alloc(MEMCHUNK* chunk, size_t size);

/* Free all the memory used by the given chunk allocator.
 */
void memchunk_fini(MEMCHUNK* chunk);


#ifdef __cplusplus
}
#endif

#endif  /* CRE_MEMCHUNK_H */
