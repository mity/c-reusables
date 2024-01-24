/*
 * C Reusables
 * <http://github.com/mity/c-reusables>
 *
 * Copyright (c) 2023 Martin Mitáš
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

#include "acutest.h"
#include "memchunk.h"

#include <stdint.h>
#include <string.h>


static void
test_alloc_small(void)
{
    MEMCHUNK chunk = MEMCHUNK_INITIALIZER(0);
    void* prev_ptr = NULL;
    void* ptr;
    int i;

    for(i = 0; i < 10; i++) {
        ptr = memchunk_alloc(&chunk, 12);
        TEST_ASSERT(ptr != NULL);
        if(prev_ptr != NULL)
            TEST_CHECK((uintptr_t) ptr - (uintptr_t) prev_ptr == 12);
        memset(ptr, i, 12);
        prev_ptr = ptr;
    }

    memchunk_fini(&chunk);
}

static void
test_alloc_big(void)
{
    MEMCHUNK chunk = MEMCHUNK_INITIALIZER(0);
    void* prev_ptr = NULL;
    void* ptr;
    int i;

    for(i = 0; i < 3; i++) {
        ptr = memchunk_alloc(&chunk, 12);
        TEST_ASSERT(ptr != NULL);
        memset(ptr, i, 12);
        prev_ptr = ptr;
    }

    /* Allocate a bug block. */
    ptr = memchunk_alloc(&chunk, 2048);
    TEST_ASSERT(ptr != NULL);
    memset(ptr, 0xaa, 2048);

    /* And see that smaller allocation are still done from the smaller initial block. */
    for(i = 0; i < 3; i++) {
        ptr = memchunk_alloc(&chunk, 12);
        TEST_ASSERT(ptr != NULL);
        if(prev_ptr != NULL)
            TEST_CHECK((uintptr_t) ptr - (uintptr_t) prev_ptr == 12);
        memset(ptr, i, 12);
        prev_ptr = ptr;
    }

    memchunk_fini(&chunk);
}


TEST_LIST = {
    { "alloc_small",    test_alloc_small },
    { "alloc_big",      test_alloc_big },
    { 0 }
};

