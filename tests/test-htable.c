/*
 * C Reusables
 * <http://github.com/mity/c-reusables>
 *
 * Copyright (c) 2023 Martin Mitas
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
#include "htable.h"

#include <stdio.h>


typedef struct VAL {
    HTABLE_NODE the_node;
    char* key;
    int payload;
} VAL;


static HTABLE_NODE*
make_val(const char* key, int payload)
{
    VAL* v;

    v = (VAL*) malloc(sizeof(VAL));
    TEST_ASSERT(v != NULL);

    v->key = strdup(key);
    v->payload = payload;

    return &v->the_node;
}

static uint32_t
hash_func(const HTABLE_NODE* node)
{
    /* FNV-1a */
    static const uint32_t FNV1A_PRIME_32 = 16777619;
    VAL* val = (VAL*) HTABLE_DATA(node, VAL, the_node);
    const uint8_t* ptr = (const uint8_t*) val->key;
    uint32_t fnv1a = 0;

    while(*ptr) {
        fnv1a ^= *ptr;
        fnv1a *= FNV1A_PRIME_32;
        ptr++;
    }

    return fnv1a;
}

static int
cmp_func(const HTABLE_NODE* node1, const HTABLE_NODE* node2)
{
    VAL* data1 = (VAL*) HTABLE_DATA(node1, VAL, the_node);
    VAL* data2 = (VAL*) HTABLE_DATA(node2, VAL, the_node);

    return strcmp(data1->key, data2->key);
}

static void
dtor_func(HTABLE_NODE* node)
{
    VAL* val;

    TEST_ASSERT(node != NULL);

    val = (VAL*) HTABLE_DATA(node, VAL, the_node);
    free(val->key);
    free(val);
}


/*****************************
 ***   The test routines   ***
 *****************************/

static void
test_empty(void)
{
    HTABLE htable = HTABLE_INITIALIZER;

    TEST_CHECK(htable_is_empty(&htable));
    TEST_CHECK(htable_insert(&htable, make_val("key", 42), cmp_func, hash_func) == 0);
    TEST_CHECK(!htable_is_empty(&htable));
    htable_fini(&htable, dtor_func);
    TEST_CHECK(htable_is_empty(&htable));
}

static void
test_insert(void)
{
    HTABLE htable = HTABLE_INITIALIZER;

    TEST_CHECK(htable_is_empty(&htable));
    TEST_CHECK(htable_insert(&htable, make_val("key", 42), cmp_func, hash_func) == 0);
    TEST_CHECK(!htable_is_empty(&htable));

    /* Check we cannot insert value with the same key. */
    TEST_CHECK(htable_insert(&htable, make_val("key", 42), cmp_func, hash_func) != 0);
    htable_fini(&htable, dtor_func);
    TEST_CHECK(htable_is_empty(&htable));
}

static void
test_lookup(void)
{
    HTABLE htable = HTABLE_INITIALIZER;
    VAL val_key;
    char key[8];
    int i;

    for(i = 0; i < 100000; i++) {
        snprintf(key, 8, "%d", i);
        TEST_CHECK(htable_insert(&htable, make_val(key, i), cmp_func, hash_func) == 0);
    }

    for(i = 0; i < 100000; i++) {
        HTABLE_NODE* node;

        val_key.key = key;
        snprintf(val_key.key, 8, "%d", i);

        node = htable_lookup(&htable, &val_key.the_node, cmp_func, hash_func);
        TEST_CHECK(node != NULL);
        TEST_CHECK(HTABLE_DATA(node, VAL, the_node)->payload == i);
    }

    val_key.key = key;
    snprintf(val_key.key, 8, "n/a");
    TEST_CHECK(htable_lookup(&htable, &val_key.the_node, cmp_func, hash_func) == NULL);

    htable_fini(&htable, dtor_func);
}

static void
test_remove(void)
{
    HTABLE htable = HTABLE_INITIALIZER;
    VAL val_key;
    char key[8];
    int i;

    for(i = 0; i < 100000; i++) {
        snprintf(key, 8, "%d", i);
        TEST_CHECK(htable_insert(&htable, make_val(key, i), cmp_func, hash_func) == 0);
    }

    for(i = 0; i < 100000; i++) {
        HTABLE_NODE* node;

        val_key.key = key;
        snprintf(val_key.key, 8, "%d", i);

        node = htable_remove(&htable, &val_key.the_node, cmp_func, hash_func);
        TEST_CHECK(node != NULL);
        TEST_MSG("Broken element: %d", i);
        dtor_func(node);
    }

    htable_fini(&htable, dtor_func);
}


/*************************
 ***   List of tests   ***
 *************************/

TEST_LIST = {
    { "empty",      test_empty },
    { "insert",     test_insert },
    { "lookup",     test_lookup },
    { "remove",     test_remove },
    { NULL, NULL }
};
