/*
 * C Reusables
 * <http://github.com/mity/c-reusables>
 *
 * Copyright (c) 2018 Martin Mitáš
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

#ifndef CRE_VALUE_H
#define CRE_VALUE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdlib.h>


/* The value structure.
 * Use as opaque.
 */
typedef struct VALUE {
    /* We need at least 2 * sizeof(void*). Sixteen bytes covers that on 64-bit
     * platforms and it seems as a good compromise allowing to "inline" all
     * numeric types as well as short strings; which is good idea: most dict
     * keys as well as many string values are in practice quite short. */
    uint8_t data[16];
} VALUE;


/* Value types.
 */
typedef enum VALUE_TYPE {
    VALUE_NULL = 0,
    VALUE_BOOL,
    VALUE_INT32,
    VALUE_UINT32,
    VALUE_INT64,
    VALUE_UINT64,
    VALUE_FLOAT,
    VALUE_DOUBLE,
    VALUE_STRING,
    VALUE_ARRAY,
    VALUE_DICT
} VALUE_TYPE;


/* Free any resources the value holds.
 * For ARRAY and DICT it is recursive.
 */
void value_fini(VALUE* v);

/* Get value type.
 */
VALUE_TYPE value_type(const VALUE* v);

/* Check whether the value is "compatible" with the given type.
 *
 * This is especially useful for determining whether a numeric value can be
 * "casted" to other numeric type. The function does some basic checking
 * whether such conversion looses substantial information.
 *
 * For example, value initialized with init_float(&v, 1.0f) is considered
 * compatible with INT32, because 1.0f has zero fraction and 1 fits between
 * INT32_MIN and INT32_MAX. Therefore calling int32_value(&v) gets sensible
 * result.
 */
int value_is_compatible(const VALUE* v, VALUE_TYPE type);

/* Values newly added into array or dictionary are of type VALUE_NULL.
 *
 * Additionally, for such newly created values, an internal flag is used to
 * mark that the value was never explicitly initialized by the application.
 *
 * This function checks value of the flag, and allows thus the caller to
 * distinguish whether the value was just added; or whether the value was
 * explicitly initialized as VALUE_NULL with value_init_null().
 *
 * Caller is supposed to initialize all such newly added value with any of the
 * value_init_XXX() functions, and hence reset the flag.
 */
int value_is_new(const VALUE* v);

/* Simple recursive getter, capable to get a value dwelling deep in the
 * hierarchy formed by nested arrays and dictionaries.
 *
 * Limitations: The function is not capable to deal with object keys which
 * contain zero byte '\0', slash '/' or brackets '[' ']' because those are
 * interpreted by the function as special characters:
 *
 *  -- '/' delimits dictionary keys (and optionally also array indexes;
 *     paths "foo/[4]" and "foo[4]" are treated as equivalent.)
 *  -- '[' ']' enclose array indexes (for distinguishing from numbered
 *     dictionary keys). Note that negative indexes are supported here;
 *     '[-1]' refers to the last element in the array, '[-2]' to the element
*       before the last element etc.
 *  -- '\0' terminates the whole path (as is normal with C strings).
 *
 * Examples:
 *
 *  (1) value_path(root, "") gets directly the root.
 *
 *  (2) value_path(root, "foo") gets value keyed with 'foo' if root is a
 *      dictionary having such value, or NULL otherwise.
 *
 *  (3) value_path(root, "[4]") gets value with index 4 if root is an array
 *      having so many members, or NULL otherwise.
 *
 *  (4) value_path(root, "foo[2]/bar/baz[3]") walks deeper and deeper and
 *      returns a value stored there assuming these all conditions are true:
 *       -- root is dictionary having the key "foo";
 *       -- that value is a nested list having the index [2];
 *       -- that value is a nested dictionary having the key "bar";
 *       -- that value is a nested dictionary having the key "baz";
 *       -- and finally, that is a list having the index [3].
 *      If any of those is not fulfilled, then NULL is returned.
 */
VALUE* value_path(VALUE* root, const char* path);

/* value_build_path() is similar to value_path(); but allows easy populating
 * of value hierarchies.
 *
 * If all values along the path already exist, the behavior is exactly the same
 * as value_path().
 *
 * But when a value corresponding to any component of the path does not exist
 * then, instead of returning NULL, new value is added into the parent
 * container (assuming the parent existing container has correct type as
 * assumed by the path.)
 *
 * Caller may use empty "[]" to always enforce appending a new value into an
 * array. E.g. value_build_path(root, "multiple_values/[]/name") makes sure the
 * root contains an array under the key "multiple_values", and a new dictionary
 * is appended at the end of the array. This new dictionary gets a new value
 * under the key "name". Assuming the function succeeds, the caller can now be
 * sure the "name" is initialized as VALUE_NULL because the new dictionary has
 * been just created and added as the last element if the list.
 *
 * If such new value does not correspond to the last path component, the new
 * value gets initialized as the right type so subsequent path component can
 * be treated the same way.
 *
 * If the function creates the value corresponding to the last component of the
 * path, it is initialized as VALUE_NULL and the "new flag" is set for it, so
 * caller can test this condition with value_is_new().
 *
 * Returns NULL if the path cannot be resolved because any existing value
 * has a type incompatible with the path; if creation of any value along the
 * path fails; or if an array index is out of bounds.
 */
VALUE* value_build_path(VALUE* root, const char* path);


/******************
 *** VALUE_NULL ***
 ******************/

/* Note it is guaranteed that VALUE_NULL does not need any explicit clean-up;
 * i.e. application may avoid calling value_fini().
 *
 * But it is allowed to. value_fini() for VALUE_NULL is a noop.
 */


/* Static initializer.
 */
#define VALUE_NULL_INITIALIZER    { { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 } }

void value_init_null(VALUE* v);


/******************
 *** VALUE_BOOL ***
 ******************/

int value_init_bool(VALUE* v, int b);

int value_bool(const VALUE* v);


/*********************
 *** Numeric types ***
 *********************/


/* Initializers.
 */
int value_init_int32(VALUE* v, int32_t i32);
int value_init_uint32(VALUE* v, uint32_t u32);
int value_init_int64(VALUE* v, int64_t i64);
int value_init_uint64(VALUE* v, uint64_t u64);
int value_init_float(VALUE* v, float f);
int value_init_double(VALUE* v, double d);

/* Getters.
 *
 * Note you may use any of the getter functions for any numeric value type.
 * All these functions are able to perform any required conversion under the
 * hood.
 *
 * Naturally, the conversion may have similar side effects as C casting, such
 * as an overflow, an underflow or, when converting float/double to any integer
 * type, a rounding.
 *
 * However application may use value_is_compatible() to verify the getter
 * will provide a reasonable result for the particular value in question.
 */
int32_t value_int32(const VALUE* v);
uint32_t value_uint32(const VALUE* v);
int64_t value_int64(const VALUE* v);
uint64_t value_uint64(const VALUE* v);
float value_float(const VALUE* v);
double value_double(const VALUE* v);


/********************
 *** VALUE_STRING ***
 ********************/

/* Note VALUE_STRING allows to store any sequences of any bytes, even a binary
 * data. No particular encoding of the string is assumed. Even zero bytes are
 * allowed (but then the caller has to use value_init_string_() and specify
 * the string length explicitly).
 */

/* The function value_init_string_() initializes the VALUE_STRING with any
 * sequence of bytes, of any length. It also adds automatically one zero byte
 * (not counted in the length of the string).
 *
 * The function value_init_string() is equivalent to calling directly
 * value_init_string_(str, strlen(str)).
 *
 * The parameter str is allowed to be NULL (then the functions behave the same
 * way as if it is points to an empty string).
 */
int value_init_string_(VALUE* v, const char* str, size_t len);
int value_init_string(VALUE* v, const char* str);

/* Get pointer to the internal buffer holding the string. The caller may assume
 * the returned string is always zero-terminated.
 */
const char* value_string(const VALUE* v);

/* Get length of the string. (The implicit zero terminator does not count.)
 */
size_t value_string_length(const VALUE* v);


/*******************
 *** VALUE_ARRAY ***
 *******************/

/* Array of values.
 *
 * Note that any new value added into the array with value_array_append() or
 * value_array_insert() is initially of the type VALUE_NULL and that it has
 * an internal flag marking the value as new (so that value_is_new() returns
 * non-zero for it). Application is supposed to initialize the newly added
 * value by any of the value initialization functions.
 *
 * WARNING: Modifying contents of an array (i.e. inserting, appending and also
 * removing a value)  can lead to reallocation of internal array buffer.
 * Hence, consider all VALUE* pointers invalid after modifying the array.
 * That includes the return values of value_array_get(), value_array_get_all(),
 * but also preceding calls of value_array_append() and value_array_insert().
 */
int value_init_array(VALUE* v);

/* Get count of items in the array.
 */
size_t value_array_size(const VALUE* v);

/* Get the specified item.
 */
VALUE* value_array_get(const VALUE* v, size_t index);

/* Get pointer to internal C array of all items.
 */
VALUE* value_array_get_all(const VALUE* v);

/* Append/insert new item.
 */
VALUE* value_array_append(VALUE* v);
VALUE* value_array_insert(VALUE* v, size_t index);

/* Remove an item (or range of items).
 */
int value_array_remove(VALUE* v, size_t index);
int value_array_remove_range(VALUE* v, size_t index, size_t count);

/* Remove and destroy all members (recursively).
 */
void value_array_clean(VALUE* v);


/******************
 *** VALUE_DICT ***
 ******************/

/* Dictionary of values. (Internally implemented as red-black tree.)
 *
 * Note that any new value added into the dictionary is initially of the type
 * VALUE_NULL and that it has  an internal flag marking the value as new
 * (so that value_is_new() returns non-zero for it). Application is supposed
 * to initialize the newly added value by any of the value initialization
 * functions.
 *
 * Note that all the functions adding/removing any items may invalidate all
 * pointers into the dictionary.
 */


/* Flag for init_dict_ex() asking to maintain the order in which the dictionary
 * is populated and enabling dict_walk_ordered().
 *
 * If used, the dictionary consumes more memory.
 */
#define VALUE_DICT_MAINTAINORDER      0x0001

/* Initialize the value as a (empty) dictionary.
 *
 * value_init_dict_ex() allows to specify custom comparer function (may be NULL)
 * or flags changing the default behavior of the dictionary.
 */
int value_init_dict(VALUE* v);
int value_init_dict_ex(VALUE* v,
                       int (*custom_cmp_func)(const char* /*key1*/, size_t /*len1*/,
                                              const char* /*key2*/, size_t /*len2*/),
                       unsigned flags);

/* Get flags of the dictionary.
 */
unsigned value_dict_flags(const VALUE* v);

/* Get count of items in the dictionary.
 */
size_t value_dict_size(const VALUE* v);

/* Get all keys.
 *
 * If the buffer provided by the caller is too small, only subset of keys shall
 * be retrieved.
 *
 * Returns count of retrieved keys.
 */
size_t value_dict_keys_sorted(const VALUE* v, const VALUE** buffer, size_t buffer_size);
size_t value_dict_keys_ordered(const VALUE* v, const VALUE** buffer, size_t buffer_size);

/* Find an item with the given key, or return NULL of no such item exists.
 */
VALUE* value_dict_get_(const VALUE* v, const char* key, size_t key_len);
VALUE* value_dict_get(const VALUE* v, const char* key);

/* Add new item with the given key of type VALUE_NULL.
 *
 * Returns NULL if the key is already used.
 */
VALUE* value_dict_add_(VALUE* v, const char* key, size_t key_len);
VALUE* value_dict_add(VALUE* v, const char* key);

/* This is combined operation of value_dict_get() and value_dict_add().
 *
 * Get value of the given key. If no such value exists, new one is added.
 * Application can check for such situation with value_is_new().
 *
 * NULL is returned only in an out-of-memory situation.
 */
VALUE* value_dict_get_or_add_(VALUE* v, const char* key, size_t key_len);
VALUE* value_dict_get_or_add(VALUE* v, const char* key);

/* Remove and destroy (recursively) the given item from the dictionary.
 */
int value_dict_remove_(VALUE* v, const char* key, size_t key_len);
int value_dict_remove(VALUE* v, const char* key);

/* Walking over all items in the dictionary. The callback function is called
 * for every item in the dictionary, providing key and value and propagating
 * the user data into it. If the callback returns non-zero, the function
 * aborts immediately.
 *
 * Note dict_walk_ordered() is supported only if DICT_MAINTAINORDER
 * flag was used in init_dict().
 */
int value_dict_walk_ordered(const VALUE* v,
            int (*visit_func)(const VALUE*, VALUE*, void*), void* ctx);
int value_dict_walk_sorted(const VALUE* v,
            int (*visit_func)(const VALUE*, VALUE*, void*), void* ctx);

/* Remove and destroy all members (recursively).
 */
void value_dict_clean(VALUE* v);


#ifdef __cplusplus
}
#endif

#endif  /* CRE_VALUE_H */
