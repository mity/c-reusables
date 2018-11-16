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

#include "acutest.h"
#include "value.h"

int value_dict_verify(VALUE* v);

#include <stdint.h>
#include <string.h>


static void
test_null(void)
{
    VALUE v = VALUE_NULL_INITIALIZER;

    TEST_CHECK(value_is_compatible(&v, VALUE_NULL));
    TEST_CHECK(!value_is_compatible(&v, VALUE_BOOL));
    TEST_CHECK(!value_is_compatible(&v, VALUE_INT32));
    TEST_CHECK(!value_is_compatible(&v, VALUE_UINT32));
    TEST_CHECK(!value_is_compatible(&v, VALUE_INT64));
    TEST_CHECK(!value_is_compatible(&v, VALUE_UINT64));
    TEST_CHECK(!value_is_compatible(&v, VALUE_FLOAT));
    TEST_CHECK(!value_is_compatible(&v, VALUE_DOUBLE));
    TEST_CHECK(!value_is_compatible(&v, VALUE_ARRAY));
    TEST_CHECK(!value_is_compatible(&v, VALUE_DICT));

    TEST_CHECK(value_type(&v) == VALUE_NULL);
    TEST_CHECK(!value_is_new(&v));

    memset(&v, 0xff, sizeof(VALUE));

    value_init_null(&v);
    TEST_CHECK(value_type(&v) == VALUE_NULL);
    TEST_CHECK(!value_is_new(&v));
    value_fini(&v);
}

static void
test_bool(void)
{
    VALUE v;

    value_init_bool(&v, 1 == 1);
    TEST_CHECK(value_type(&v) == VALUE_BOOL);
    TEST_CHECK(!value_is_compatible(&v, VALUE_NULL));
    TEST_CHECK(value_is_compatible(&v, VALUE_BOOL));
    TEST_CHECK(!value_is_compatible(&v, VALUE_INT32));
    TEST_CHECK(!value_is_compatible(&v, VALUE_UINT32));
    TEST_CHECK(!value_is_compatible(&v, VALUE_INT64));
    TEST_CHECK(!value_is_compatible(&v, VALUE_UINT64));
    TEST_CHECK(!value_is_compatible(&v, VALUE_FLOAT));
    TEST_CHECK(!value_is_compatible(&v, VALUE_DOUBLE));
    TEST_CHECK(!value_is_compatible(&v, VALUE_ARRAY));
    TEST_CHECK(!value_is_compatible(&v, VALUE_DICT));
    TEST_CHECK(value_bool(&v));
    value_fini(&v);

    value_init_bool(&v, 1 == 0);
    TEST_CHECK(value_type(&v) == VALUE_BOOL);
    TEST_CHECK(!value_is_compatible(&v, VALUE_NULL));
    TEST_CHECK(value_is_compatible(&v, VALUE_BOOL));
    TEST_CHECK(!value_is_compatible(&v, VALUE_INT32));
    TEST_CHECK(!value_is_compatible(&v, VALUE_UINT32));
    TEST_CHECK(!value_is_compatible(&v, VALUE_INT64));
    TEST_CHECK(!value_is_compatible(&v, VALUE_UINT64));
    TEST_CHECK(!value_is_compatible(&v, VALUE_FLOAT));
    TEST_CHECK(!value_is_compatible(&v, VALUE_DOUBLE));
    TEST_CHECK(!value_is_compatible(&v, VALUE_ARRAY));
    TEST_CHECK(!value_is_compatible(&v, VALUE_DICT));
    TEST_CHECK(!value_bool(&v));
    value_fini(&v);

    value_init_bool(&v, 0xaabbccdd);
    TEST_CHECK(value_bool(&v) == 1);
    value_fini(&v);
}

static void
test_int32(void)
{
    VALUE v;

    value_init_int32(&v, 0);
    TEST_CHECK(value_type(&v) == VALUE_INT32);
    TEST_CHECK(!value_is_compatible(&v, VALUE_NULL));
    TEST_CHECK(!value_is_compatible(&v, VALUE_BOOL));
    TEST_CHECK(value_is_compatible(&v, VALUE_INT32));
    TEST_CHECK(value_is_compatible(&v, VALUE_UINT32));
    TEST_CHECK(value_is_compatible(&v, VALUE_INT64));
    TEST_CHECK(value_is_compatible(&v, VALUE_UINT64));
    TEST_CHECK(value_is_compatible(&v, VALUE_FLOAT));
    TEST_CHECK(value_is_compatible(&v, VALUE_DOUBLE));
    TEST_CHECK(!value_is_compatible(&v, VALUE_ARRAY));
    TEST_CHECK(!value_is_compatible(&v, VALUE_DICT));
    TEST_CHECK(value_int32(&v) == 0);
    TEST_CHECK(value_uint32(&v) == 0);
    TEST_CHECK(value_int64(&v) == 0);
    TEST_CHECK(value_uint64(&v) == 0);
    TEST_CHECK(value_float(&v) == 0.0f);
    TEST_CHECK(value_double(&v) == 0.0);
    value_fini(&v);

    value_init_int32(&v, -1);
    TEST_CHECK(value_type(&v) == VALUE_INT32);
    TEST_CHECK(!value_is_compatible(&v, VALUE_NULL));
    TEST_CHECK(!value_is_compatible(&v, VALUE_BOOL));
    TEST_CHECK(value_is_compatible(&v, VALUE_INT32));
    TEST_CHECK(!value_is_compatible(&v, VALUE_UINT32));
    TEST_CHECK(value_is_compatible(&v, VALUE_INT64));
    TEST_CHECK(!value_is_compatible(&v, VALUE_UINT64));
    TEST_CHECK(value_is_compatible(&v, VALUE_FLOAT));
    TEST_CHECK(value_is_compatible(&v, VALUE_DOUBLE));
    TEST_CHECK(!value_is_compatible(&v, VALUE_ARRAY));
    TEST_CHECK(!value_is_compatible(&v, VALUE_DICT));
    TEST_CHECK(value_int32(&v) == -1);
    TEST_CHECK(value_int64(&v) == -1);
    TEST_CHECK(value_float(&v) == -1.0f);
    TEST_CHECK(value_double(&v) == -1.0);
    value_fini(&v);

    value_init_int32(&v, INT32_MIN);
    TEST_CHECK(value_type(&v) == VALUE_INT32);
    TEST_CHECK(!value_is_compatible(&v, VALUE_NULL));
    TEST_CHECK(!value_is_compatible(&v, VALUE_BOOL));
    TEST_CHECK(value_is_compatible(&v, VALUE_INT32));
    TEST_CHECK(!value_is_compatible(&v, VALUE_UINT32));
    TEST_CHECK(value_is_compatible(&v, VALUE_INT64));
    TEST_CHECK(!value_is_compatible(&v, VALUE_UINT64));
    TEST_CHECK(value_is_compatible(&v, VALUE_FLOAT));
    TEST_CHECK(value_is_compatible(&v, VALUE_DOUBLE));
    TEST_CHECK(!value_is_compatible(&v, VALUE_ARRAY));
    TEST_CHECK(!value_is_compatible(&v, VALUE_DICT));
    TEST_CHECK(value_int32(&v) == INT32_MIN);
    value_fini(&v);

    value_init_int32(&v, INT32_MAX);
    TEST_CHECK(value_type(&v) == VALUE_INT32);
    TEST_CHECK(!value_is_compatible(&v, VALUE_NULL));
    TEST_CHECK(!value_is_compatible(&v, VALUE_BOOL));
    TEST_CHECK(value_is_compatible(&v, VALUE_INT32));
    TEST_CHECK(value_is_compatible(&v, VALUE_UINT32));
    TEST_CHECK(value_is_compatible(&v, VALUE_INT64));
    TEST_CHECK(value_is_compatible(&v, VALUE_UINT64));
    TEST_CHECK(value_is_compatible(&v, VALUE_FLOAT));
    TEST_CHECK(value_is_compatible(&v, VALUE_DOUBLE));
    TEST_CHECK(!value_is_compatible(&v, VALUE_ARRAY));
    TEST_CHECK(!value_is_compatible(&v, VALUE_DICT));
    TEST_CHECK(value_int32(&v) == INT32_MAX);
    value_fini(&v);
}

static void
test_uint32(void)
{
    VALUE v;

    value_init_uint32(&v, 0U);
    TEST_CHECK(value_type(&v) == VALUE_UINT32);
    TEST_CHECK(!value_is_compatible(&v, VALUE_NULL));
    TEST_CHECK(!value_is_compatible(&v, VALUE_BOOL));
    TEST_CHECK(value_is_compatible(&v, VALUE_INT32));
    TEST_CHECK(value_is_compatible(&v, VALUE_UINT32));
    TEST_CHECK(value_is_compatible(&v, VALUE_INT64));
    TEST_CHECK(value_is_compatible(&v, VALUE_UINT64));
    TEST_CHECK(value_is_compatible(&v, VALUE_FLOAT));
    TEST_CHECK(value_is_compatible(&v, VALUE_DOUBLE));
    TEST_CHECK(!value_is_compatible(&v, VALUE_ARRAY));
    TEST_CHECK(!value_is_compatible(&v, VALUE_DICT));
    TEST_CHECK(value_int32(&v) == 0);
    TEST_CHECK(value_uint32(&v) == 0);
    TEST_CHECK(value_int64(&v) == 0);
    TEST_CHECK(value_uint64(&v) == 0);
    TEST_CHECK(value_float(&v) == 0.0f);
    TEST_CHECK(value_double(&v) == 0.0);
    value_fini(&v);

    value_init_uint32(&v, UINT32_MAX);
    TEST_CHECK(value_type(&v) == VALUE_UINT32);
    TEST_CHECK(!value_is_compatible(&v, VALUE_NULL));
    TEST_CHECK(!value_is_compatible(&v, VALUE_BOOL));
    TEST_CHECK(!value_is_compatible(&v, VALUE_INT32));
    TEST_CHECK(value_is_compatible(&v, VALUE_UINT32));
    TEST_CHECK(value_is_compatible(&v, VALUE_INT64));
    TEST_CHECK(value_is_compatible(&v, VALUE_UINT64));
    TEST_CHECK(value_is_compatible(&v, VALUE_FLOAT));
    TEST_CHECK(value_is_compatible(&v, VALUE_DOUBLE));
    TEST_CHECK(!value_is_compatible(&v, VALUE_ARRAY));
    TEST_CHECK(!value_is_compatible(&v, VALUE_DICT));
    TEST_CHECK(value_int32(&v) == UINT32_MAX);
    TEST_CHECK(value_uint32(&v) == UINT32_MAX);
    TEST_CHECK(value_int64(&v) == UINT32_MAX);
    TEST_CHECK(value_uint64(&v) == UINT32_MAX);
    TEST_CHECK(value_float(&v) == (float) UINT32_MAX);
    TEST_CHECK(value_double(&v) == (double) UINT32_MAX);
    value_fini(&v);
}

static void
test_int64(void)
{
    VALUE v;

    value_init_int64(&v, 0);
    TEST_CHECK(value_type(&v) == VALUE_INT64);
    TEST_CHECK(!value_is_compatible(&v, VALUE_NULL));
    TEST_CHECK(!value_is_compatible(&v, VALUE_BOOL));
    TEST_CHECK(value_is_compatible(&v, VALUE_INT32));
    TEST_CHECK(value_is_compatible(&v, VALUE_UINT32));
    TEST_CHECK(value_is_compatible(&v, VALUE_INT64));
    TEST_CHECK(value_is_compatible(&v, VALUE_UINT64));
    TEST_CHECK(value_is_compatible(&v, VALUE_FLOAT));
    TEST_CHECK(value_is_compatible(&v, VALUE_DOUBLE));
    TEST_CHECK(!value_is_compatible(&v, VALUE_ARRAY));
    TEST_CHECK(!value_is_compatible(&v, VALUE_DICT));
    TEST_CHECK(value_int32(&v) == 0);
    TEST_CHECK(value_uint32(&v) == 0);
    TEST_CHECK(value_int64(&v) == 0);
    TEST_CHECK(value_uint64(&v) == 0);
    TEST_CHECK(value_float(&v) == 0.0f);
    TEST_CHECK(value_double(&v) == 0.0);
    value_fini(&v);

    value_init_int64(&v, -1);
    TEST_CHECK(value_type(&v) == VALUE_INT64);
    TEST_CHECK(!value_is_compatible(&v, VALUE_NULL));
    TEST_CHECK(!value_is_compatible(&v, VALUE_BOOL));
    TEST_CHECK(value_is_compatible(&v, VALUE_INT32));
    TEST_CHECK(!value_is_compatible(&v, VALUE_UINT32));
    TEST_CHECK(value_is_compatible(&v, VALUE_INT64));
    TEST_CHECK(!value_is_compatible(&v, VALUE_UINT64));
    TEST_CHECK(value_is_compatible(&v, VALUE_FLOAT));
    TEST_CHECK(value_is_compatible(&v, VALUE_DOUBLE));
    TEST_CHECK(!value_is_compatible(&v, VALUE_ARRAY));
    TEST_CHECK(!value_is_compatible(&v, VALUE_DICT));
    TEST_CHECK(value_int32(&v) == -1);
    TEST_CHECK(value_int64(&v) == -1);
    TEST_CHECK(value_float(&v) == -1.0f);
    TEST_CHECK(value_double(&v) == -1.0);
    value_fini(&v);

    value_init_int64(&v, INT64_MIN);
    TEST_CHECK(value_type(&v) == VALUE_INT64);
    TEST_CHECK(!value_is_compatible(&v, VALUE_NULL));
    TEST_CHECK(!value_is_compatible(&v, VALUE_BOOL));
    TEST_CHECK(!value_is_compatible(&v, VALUE_INT32));
    TEST_CHECK(!value_is_compatible(&v, VALUE_UINT32));
    TEST_CHECK(value_is_compatible(&v, VALUE_INT64));
    TEST_CHECK(!value_is_compatible(&v, VALUE_UINT64));
    TEST_CHECK(value_is_compatible(&v, VALUE_FLOAT));
    TEST_CHECK(value_is_compatible(&v, VALUE_DOUBLE));
    TEST_CHECK(!value_is_compatible(&v, VALUE_ARRAY));
    TEST_CHECK(!value_is_compatible(&v, VALUE_DICT));
    TEST_CHECK(value_int64(&v) == INT64_MIN);
    TEST_CHECK(value_float(&v) == (float) INT64_MIN);
    TEST_CHECK(value_double(&v) == (double) INT64_MIN);
    value_fini(&v);

    value_init_int64(&v, INT64_MAX);
    TEST_CHECK(value_type(&v) == VALUE_INT64);
    TEST_CHECK(!value_is_compatible(&v, VALUE_NULL));
    TEST_CHECK(!value_is_compatible(&v, VALUE_BOOL));
    TEST_CHECK(!value_is_compatible(&v, VALUE_INT32));
    TEST_CHECK(!value_is_compatible(&v, VALUE_UINT32));
    TEST_CHECK(value_is_compatible(&v, VALUE_INT64));
    TEST_CHECK(value_is_compatible(&v, VALUE_UINT64));
    TEST_CHECK(value_is_compatible(&v, VALUE_FLOAT));
    TEST_CHECK(value_is_compatible(&v, VALUE_DOUBLE));
    TEST_CHECK(!value_is_compatible(&v, VALUE_ARRAY));
    TEST_CHECK(!value_is_compatible(&v, VALUE_DICT));
    TEST_CHECK(value_int64(&v) == INT64_MAX);
    TEST_CHECK(value_uint64(&v) == INT64_MAX);
    TEST_CHECK(value_float(&v) == (float) INT64_MAX);
    TEST_CHECK(value_double(&v) == (double) INT64_MAX);
    value_fini(&v);
}

static void
test_uint64(void)
{
    VALUE v;

    value_init_uint64(&v, 0U);
    TEST_CHECK(value_type(&v) == VALUE_UINT64);
    TEST_CHECK(!value_is_compatible(&v, VALUE_NULL));
    TEST_CHECK(!value_is_compatible(&v, VALUE_BOOL));
    TEST_CHECK(value_is_compatible(&v, VALUE_INT32));
    TEST_CHECK(value_is_compatible(&v, VALUE_UINT32));
    TEST_CHECK(value_is_compatible(&v, VALUE_INT64));
    TEST_CHECK(value_is_compatible(&v, VALUE_UINT64));
    TEST_CHECK(value_is_compatible(&v, VALUE_FLOAT));
    TEST_CHECK(value_is_compatible(&v, VALUE_DOUBLE));
    TEST_CHECK(!value_is_compatible(&v, VALUE_ARRAY));
    TEST_CHECK(!value_is_compatible(&v, VALUE_DICT));
    TEST_CHECK(value_int32(&v) == 0);
    TEST_CHECK(value_uint32(&v) == 0);
    TEST_CHECK(value_int64(&v) == 0);
    TEST_CHECK(value_uint64(&v) == 0);
    TEST_CHECK(value_float(&v) == 0.0f);
    TEST_CHECK(value_double(&v) == 0.0);
    value_fini(&v);

    value_init_uint64(&v, UINT64_MAX);
    TEST_CHECK(value_type(&v) == VALUE_UINT64);
    TEST_CHECK(!value_is_compatible(&v, VALUE_NULL));
    TEST_CHECK(!value_is_compatible(&v, VALUE_BOOL));
    TEST_CHECK(!value_is_compatible(&v, VALUE_INT32));
    TEST_CHECK(!value_is_compatible(&v, VALUE_UINT32));
    TEST_CHECK(!value_is_compatible(&v, VALUE_INT64));
    TEST_CHECK(value_is_compatible(&v, VALUE_UINT64));
    TEST_CHECK(value_is_compatible(&v, VALUE_FLOAT));
    TEST_CHECK(value_is_compatible(&v, VALUE_DOUBLE));
    TEST_CHECK(!value_is_compatible(&v, VALUE_ARRAY));
    TEST_CHECK(!value_is_compatible(&v, VALUE_DICT));
    TEST_CHECK(value_uint64(&v) == UINT64_MAX);
    TEST_CHECK(value_float(&v) == (float) UINT64_MAX);
    TEST_CHECK(value_double(&v) == (double) UINT64_MAX);
    value_fini(&v);
}

static void
test_float(void)
{
    VALUE v;

    value_init_float(&v, 0.0f);
    TEST_CHECK(value_type(&v) == VALUE_FLOAT);
    TEST_CHECK(!value_is_compatible(&v, VALUE_NULL));
    TEST_CHECK(!value_is_compatible(&v, VALUE_BOOL));
    TEST_CHECK(value_is_compatible(&v, VALUE_INT32));
    TEST_CHECK(value_is_compatible(&v, VALUE_UINT32));
    TEST_CHECK(value_is_compatible(&v, VALUE_INT64));
    TEST_CHECK(value_is_compatible(&v, VALUE_UINT64));
    TEST_CHECK(value_is_compatible(&v, VALUE_FLOAT));
    TEST_CHECK(value_is_compatible(&v, VALUE_DOUBLE));
    TEST_CHECK(!value_is_compatible(&v, VALUE_ARRAY));
    TEST_CHECK(!value_is_compatible(&v, VALUE_DICT));
    TEST_CHECK(value_int32(&v) == 0);
    TEST_CHECK(value_uint32(&v) == 0);
    TEST_CHECK(value_int64(&v) == 0);
    TEST_CHECK(value_uint64(&v) == 0);
    TEST_CHECK(value_float(&v) == 0.0f);
    TEST_CHECK(value_double(&v) == 0.0);
    value_fini(&v);

    value_init_float(&v, -1.0f);
    TEST_CHECK(value_type(&v) == VALUE_FLOAT);
    TEST_CHECK(!value_is_compatible(&v, VALUE_NULL));
    TEST_CHECK(!value_is_compatible(&v, VALUE_BOOL));
    TEST_CHECK(value_is_compatible(&v, VALUE_INT32));
    TEST_CHECK(!value_is_compatible(&v, VALUE_UINT32));
    TEST_CHECK(value_is_compatible(&v, VALUE_INT64));
    TEST_CHECK(!value_is_compatible(&v, VALUE_UINT64));
    TEST_CHECK(value_is_compatible(&v, VALUE_FLOAT));
    TEST_CHECK(value_is_compatible(&v, VALUE_DOUBLE));
    TEST_CHECK(!value_is_compatible(&v, VALUE_ARRAY));
    TEST_CHECK(!value_is_compatible(&v, VALUE_DICT));
    TEST_CHECK(value_int32(&v) == -1);
    TEST_CHECK(value_int64(&v) == -1);
    TEST_CHECK(value_float(&v) == -1.0f);
    TEST_CHECK(value_double(&v) == (double) -1.0f);
    value_fini(&v);

    value_init_float(&v, 0.5f);
    TEST_CHECK(value_type(&v) == VALUE_FLOAT);
    TEST_CHECK(!value_is_compatible(&v, VALUE_NULL));
    TEST_CHECK(!value_is_compatible(&v, VALUE_BOOL));
    TEST_CHECK(!value_is_compatible(&v, VALUE_INT32));
    TEST_CHECK(!value_is_compatible(&v, VALUE_UINT32));
    TEST_CHECK(!value_is_compatible(&v, VALUE_INT64));
    TEST_CHECK(!value_is_compatible(&v, VALUE_UINT64));
    TEST_CHECK(value_is_compatible(&v, VALUE_FLOAT));
    TEST_CHECK(value_is_compatible(&v, VALUE_DOUBLE));
    TEST_CHECK(!value_is_compatible(&v, VALUE_ARRAY));
    TEST_CHECK(!value_is_compatible(&v, VALUE_DICT));
    TEST_CHECK(value_int32(&v) == 1);
    TEST_CHECK(value_int64(&v) == 1);
    TEST_CHECK(value_float(&v) == 0.5f);
    TEST_CHECK(value_double(&v) == (double) 0.5f);
    value_fini(&v);

    value_init_float(&v, 0.4f);
    TEST_CHECK(value_int32(&v) == 0);
    TEST_CHECK(value_int64(&v) == 0);
    TEST_CHECK(value_float(&v) == 0.4f);
    TEST_CHECK(value_double(&v) == (double) 0.4f);
    value_fini(&v);

    value_init_float(&v, -0.4f);
    TEST_CHECK(value_int32(&v) == 0);
    TEST_CHECK(value_int64(&v) == 0);
    TEST_CHECK(value_float(&v) == -0.4f);
    TEST_CHECK(value_double(&v) == (double) -0.4f);
    value_fini(&v);

    value_init_float(&v, -0.5f);
    TEST_CHECK(value_int32(&v) == -1);
    TEST_CHECK(value_int64(&v) == -1);
    TEST_CHECK(value_float(&v) == -0.5f);
    TEST_CHECK(value_double(&v) == (double) -0.5f);
    value_fini(&v);
}

static void
test_double(void)
{
    VALUE v;

    value_init_double(&v, 0.0);
    TEST_CHECK(value_type(&v) == VALUE_DOUBLE);
    TEST_CHECK(!value_is_compatible(&v, VALUE_NULL));
    TEST_CHECK(!value_is_compatible(&v, VALUE_BOOL));
    TEST_CHECK(value_is_compatible(&v, VALUE_INT32));
    TEST_CHECK(value_is_compatible(&v, VALUE_UINT32));
    TEST_CHECK(value_is_compatible(&v, VALUE_INT64));
    TEST_CHECK(value_is_compatible(&v, VALUE_UINT64));
    TEST_CHECK(value_is_compatible(&v, VALUE_FLOAT));
    TEST_CHECK(value_is_compatible(&v, VALUE_DOUBLE));
    TEST_CHECK(!value_is_compatible(&v, VALUE_ARRAY));
    TEST_CHECK(!value_is_compatible(&v, VALUE_DICT));
    TEST_CHECK(value_int32(&v) == 0);
    TEST_CHECK(value_uint32(&v) == 0);
    TEST_CHECK(value_int64(&v) == 0);
    TEST_CHECK(value_uint64(&v) == 0);
    TEST_CHECK(value_float(&v) == 0.0f);
    TEST_CHECK(value_double(&v) == 0.0);
    value_fini(&v);

    value_init_double(&v, -1.0);
    TEST_CHECK(value_type(&v) == VALUE_DOUBLE);
    TEST_CHECK(!value_is_compatible(&v, VALUE_NULL));
    TEST_CHECK(!value_is_compatible(&v, VALUE_BOOL));
    TEST_CHECK(value_is_compatible(&v, VALUE_INT32));
    TEST_CHECK(!value_is_compatible(&v, VALUE_UINT32));
    TEST_CHECK(value_is_compatible(&v, VALUE_INT64));
    TEST_CHECK(!value_is_compatible(&v, VALUE_UINT64));
    TEST_CHECK(value_is_compatible(&v, VALUE_FLOAT));
    TEST_CHECK(value_is_compatible(&v, VALUE_DOUBLE));
    TEST_CHECK(!value_is_compatible(&v, VALUE_ARRAY));
    TEST_CHECK(!value_is_compatible(&v, VALUE_DICT));
    TEST_CHECK(value_int32(&v) == -1);
    TEST_CHECK(value_int64(&v) == -1);
    TEST_CHECK(value_float(&v) == (float) -1.0);
    TEST_CHECK(value_double(&v) == -1.0);
    value_fini(&v);

    value_init_double(&v, 0.5);
    TEST_CHECK(value_type(&v) == VALUE_DOUBLE);
    TEST_CHECK(!value_is_compatible(&v, VALUE_NULL));
    TEST_CHECK(!value_is_compatible(&v, VALUE_BOOL));
    TEST_CHECK(!value_is_compatible(&v, VALUE_INT32));
    TEST_CHECK(!value_is_compatible(&v, VALUE_UINT32));
    TEST_CHECK(!value_is_compatible(&v, VALUE_INT64));
    TEST_CHECK(!value_is_compatible(&v, VALUE_UINT64));
    TEST_CHECK(value_is_compatible(&v, VALUE_FLOAT));
    TEST_CHECK(value_is_compatible(&v, VALUE_DOUBLE));
    TEST_CHECK(!value_is_compatible(&v, VALUE_ARRAY));
    TEST_CHECK(!value_is_compatible(&v, VALUE_DICT));
    TEST_CHECK(value_int32(&v) == 1);
    TEST_CHECK(value_int64(&v) == 1);
    TEST_CHECK(value_float(&v) == (float) 0.5);
    TEST_CHECK(value_double(&v) == 0.5);
    value_fini(&v);

    value_init_double(&v, 0.4);
    TEST_CHECK(value_int32(&v) == 0);
    TEST_CHECK(value_int64(&v) == 0);
    TEST_CHECK(value_float(&v) == (float) 0.4);
    TEST_CHECK(value_double(&v) == 0.4);
    value_fini(&v);

    value_init_double(&v, -0.4);
    TEST_CHECK(value_int32(&v) == 0);
    TEST_CHECK(value_int64(&v) == 0);
    TEST_CHECK(value_float(&v) == (float) -0.4);
    TEST_CHECK(value_double(&v) == -0.4);
    value_fini(&v);

    value_init_double(&v, -0.5);
    TEST_CHECK(value_int32(&v) == -1);
    TEST_CHECK(value_int64(&v) == -1);
    TEST_CHECK(value_float(&v) == (float) -0.5);
    TEST_CHECK(value_double(&v) == -0.5);
    value_fini(&v);
}

static void
test_string(void)
{
    /* String long enough to not inline the string inside the value structure. */
    static const char longstr[] =
            "Lorem ipsum dolor sit amet, consectetuer adipiscing elit. "
            "Pellentesque sapien. Maecenas ipsum velit, consectetuer eu "
            "lobortis ut, dictum at dui. Nulla pulvinar eleifend sem.";

    VALUE v;

    value_init_string(&v, NULL);                /* Same as "". */
    TEST_CHECK(value_type(&v) == VALUE_STRING);
    TEST_CHECK(value_string_length(&v) == 0);
    TEST_CHECK(strcmp(value_string(&v), "") == 0);
    value_fini(&v);

    value_init_string(&v, "");
    TEST_CHECK(value_type(&v) == VALUE_STRING);
    TEST_CHECK(value_string_length(&v) == 0);
    TEST_CHECK(strcmp(value_string(&v), "") == 0);
    value_fini(&v);

    value_init_string(&v, "foo");
    TEST_CHECK(value_type(&v) == VALUE_STRING);
    TEST_CHECK(value_string_length(&v) == 3);
    TEST_CHECK(strcmp(value_string(&v), "foo") == 0);
    value_fini(&v);

    value_init_string_(&v, "foo bar", 3);       /* Explicit count of bytes. */
    TEST_CHECK(value_type(&v) == VALUE_STRING);
    TEST_CHECK(value_string_length(&v) == 3);
    TEST_CHECK(strcmp(value_string(&v), "foo") == 0);
    value_fini(&v);

    value_init_string_(&v, "foo\0bar", 7);      /* Zero byte in the middle. */
    TEST_CHECK(value_type(&v) == VALUE_STRING);
    TEST_CHECK(value_string_length(&v) == 7);
    TEST_CHECK(memcmp(value_string(&v), "foo\0bar\0", 8) == 0);
    value_fini(&v);

    value_init_string(&v, longstr);
    TEST_CHECK(value_type(&v) == VALUE_STRING);
    TEST_CHECK(value_string_length(&v) == strlen(longstr));
    TEST_CHECK(strcmp(value_string(&v), longstr) == 0);
    value_fini(&v);

    value_init_string_(&v, longstr, strlen(longstr));
    TEST_CHECK(value_type(&v) == VALUE_STRING);
    TEST_CHECK(value_string_length(&v) == strlen(longstr));
    TEST_CHECK(strcmp(value_string(&v), longstr) == 0);
    value_fini(&v);
}

static void
test_array_basic(void)
{
    VALUE a;
    VALUE* v;

    value_init_array(&a);
    TEST_CHECK(value_type(&a) == VALUE_ARRAY);
    TEST_CHECK(!value_is_compatible(&a, VALUE_NULL));
    TEST_CHECK(!value_is_compatible(&a, VALUE_BOOL));
    TEST_CHECK(!value_is_compatible(&a, VALUE_INT32));
    TEST_CHECK(!value_is_compatible(&a, VALUE_UINT32));
    TEST_CHECK(!value_is_compatible(&a, VALUE_INT64));
    TEST_CHECK(!value_is_compatible(&a, VALUE_UINT64));
    TEST_CHECK(!value_is_compatible(&a, VALUE_FLOAT));
    TEST_CHECK(!value_is_compatible(&a, VALUE_DOUBLE));
    TEST_CHECK(value_is_compatible(&a, VALUE_ARRAY));
    TEST_CHECK(!value_is_compatible(&a, VALUE_DICT));
    TEST_CHECK(value_array_size(&a) == 0);
    TEST_CHECK(value_array_get(&a, 0) == NULL);
    value_fini(&a);

    /* Check basic new value properties. */
    value_init_array(&a);
    v = value_array_append(&a);
    TEST_CHECK(v != NULL);
    TEST_CHECK(value_type(v) == VALUE_NULL);
    TEST_CHECK(value_is_new(v));
    value_init_null(v);
    v = value_array_get(&a, 0);
    TEST_CHECK(!value_is_new(v));
    TEST_CHECK(value_array_get(&a, 1) == NULL);
    TEST_CHECK(value_array_size(&a) == 1);
    value_fini(&a);

    /* Simple append/insert test. */
    value_init_array(&a);
    v = value_array_append(&a);
    value_init_int32(v, 1);
    v = value_array_append(&a);
    value_init_int32(v, 2);
    v = value_array_append(&a);
    value_init_int32(v, 3);
    v = value_array_insert(&a, 0);
    value_init_int32(v, 0);
    TEST_CHECK(value_array_size(&a) == 4);
    v = value_array_get(&a, 0);
    TEST_CHECK(v != NULL && value_int32(v) == 0);
    v = value_array_get(&a, 1);
    TEST_CHECK(v != NULL && value_int32(v) == 1);
    v = value_array_get(&a, 2);
    TEST_CHECK(v != NULL && value_int32(v) == 2);
    v = value_array_get(&a, 3);
    TEST_CHECK(v != NULL && value_int32(v) == 3);
    v = value_array_get(&a, 4);
    TEST_CHECK(v == NULL);
    value_array_clean(&a);
    TEST_CHECK(value_array_size(&a) == 0);
    value_fini(&a);
}

static void
test_array_append(void)
{
    const int N = 500000;

    VALUE a;
    VALUE* v;
    int i;

    value_init_array(&a);
    for(i = 0; i < N; i++) {
        v = value_array_append(&a);
        value_init_int32(v, i);
    }
    TEST_CHECK(value_array_size(&a) == N);
    for(i = 0; i < N; i++) {
        v = value_array_get(&a, i);
        TEST_CHECK(value_int32(v) == i);
    }
    value_fini(&a);
}

static void
test_array_insert(void)
{
    const int N = 500000;

    VALUE a;
    VALUE* v;
    int i;

    value_init_array(&a);
    v = value_array_append(&a);
    value_init_int32(v, -1);
    v = value_array_append(&a);
    value_init_int32(v, N);
    for(i = 0; i < N; i++) {
        v = value_array_insert(&a, i + 1);
        value_init_int32(v, i);
    }
    TEST_CHECK(value_array_size(&a) == N + 2);
    for(i = 0; i < N + 2; i++) {
        v = value_array_get(&a, i);
        TEST_CHECK(value_int32(v) == i - 1);
    }
    value_fini(&a);
}

static int
dict_size_callback(const VALUE* key, VALUE* value, void* ctx)
{
    size_t* size = (size_t*) ctx;

    TEST_CHECK(key != NULL);
    TEST_CHECK(value != NULL);

    *size = *size + 1;
    return 0;
}

static size_t
dict_size(VALUE* d)
{
    size_t size = 0;
    value_dict_walk_sorted(d, dict_size_callback, &size);
    return size;
}

static void
test_dict_basic(void)
{
    VALUE d;
    VALUE* foo;
    VALUE* bar;
    VALUE* baz;

    value_init_dict(&d, 0);
    TEST_CHECK(value_type(&d) == VALUE_DICT);
    TEST_CHECK(!value_is_compatible(&d, VALUE_NULL));
    TEST_CHECK(!value_is_compatible(&d, VALUE_BOOL));
    TEST_CHECK(!value_is_compatible(&d, VALUE_INT32));
    TEST_CHECK(!value_is_compatible(&d, VALUE_UINT32));
    TEST_CHECK(!value_is_compatible(&d, VALUE_INT64));
    TEST_CHECK(!value_is_compatible(&d, VALUE_UINT64));
    TEST_CHECK(!value_is_compatible(&d, VALUE_FLOAT));
    TEST_CHECK(!value_is_compatible(&d, VALUE_DOUBLE));
    TEST_CHECK(!value_is_compatible(&d, VALUE_ARRAY));
    TEST_CHECK(value_is_compatible(&d, VALUE_DICT));
    TEST_CHECK(value_dict_find(&d, "n/a") == NULL);
    TEST_CHECK(dict_size(&d) == 0);
    TEST_CHECK(value_dict_get(&d, "new") != NULL);
    TEST_CHECK(dict_size(&d) == 1);
    value_dict_clean(&d);
    TEST_CHECK(dict_size(&d) == 0);
    value_fini(&d);

    value_init_dict(&d, 0);
    foo = value_dict_get(&d, "foo");
    value_init_string(foo, "foo value");
    bar = value_dict_get(&d, "bar");
    value_init_string(bar, "bar value");
    baz = value_dict_get(&d, "baz");
    value_init_string(baz, "baz value");
    TEST_CHECK(dict_size(&d) == 3);
    TEST_CHECK(value_dict_find(&d, "foo") == foo);
    TEST_CHECK(value_dict_find(&d, "bar") == bar);
    TEST_CHECK(value_dict_find(&d, "baz") == baz);
    TEST_CHECK(value_dict_find(&d, "n/a") == NULL);
    TEST_CHECK(value_dict_get(&d, "foo") == foo);
    TEST_CHECK(value_dict_get(&d, "bar") == bar);
    TEST_CHECK(value_dict_get(&d, "baz") == baz);
    value_fini(&d);
}

static void
test_dict_big(void)
{
    const int N = 500000;

    VALUE d;
    VALUE* v;
    int i;
    char key[32];

    value_init_dict(&d, 0);
    for(i = 0; i < N; i++) {
        sprintf(key, "%d", i);
        v = value_dict_get(&d, key);
        TEST_CHECK(v != NULL  &&  value_init_int32(v, i) == 0);
    }
    TEST_CHECK(dict_size(&d) == N);
    for(i = 0; i < N; i++) {
        sprintf(key, "%d", i);
        v = value_dict_find(&d, key);
        TEST_CHECK(v != NULL  &&  value_int32(v) == i);
    }
    TEST_CHECK(value_dict_verify(&d) == 0);
    value_dict_clean(&d);
    TEST_CHECK(dict_size(&d) == 0);
    value_fini(&d);
}

static void
test_dict_remove(void)
{
    const int N = 500000;

    VALUE d;
    VALUE* v;
    int i;
    int n_removed = 0;
    char key[32];

    value_init_dict(&d, 0);
    for(i = 0; i < N; i++) {
        sprintf(key, "%d", i);
        v = value_dict_get(&d, key);
        TEST_CHECK(v != NULL);
        value_init_int32(v, i);
    }
    TEST_CHECK(value_dict_verify(&d) == 0);

    for(i = 0; i < N; i += 17) {
        sprintf(key, "%d", i);
        TEST_CHECK(value_dict_remove(&d, key) == 0);
        n_removed++;
    }
    TEST_CHECK(value_dict_remove(&d, "n/a") != 0);

    TEST_CHECK(dict_size(&d) == N - n_removed);

    for(i = 0; i < N; i++) {
        sprintf(key, "%d", i);
        v = value_dict_find(&d, key);
        if(i % 17 == 0)
            TEST_CHECK(v == NULL);
        else
            TEST_CHECK(v != NULL);
    }

    value_dict_clean(&d);
    TEST_CHECK(dict_size(&d) == 0);
    value_fini(&d);
}


TEST_LIST = {
    { "null",           test_null },
    { "bool",           test_bool },
    { "int32",          test_int32 },
    { "uint32",         test_uint32 },
    { "int64",          test_int64 },
    { "uint64",         test_uint64 },
    { "float",          test_float },
    { "double",         test_double },
    { "string",         test_string },
    { "array-basic",    test_array_basic },
    { "array-append",   test_array_append },
    { "array-insert",   test_array_insert },
    { "dict-basic",     test_dict_basic },
    { "dict-big",       test_dict_big },
    { "dict-remove",    test_dict_remove },
    { 0 }
};

