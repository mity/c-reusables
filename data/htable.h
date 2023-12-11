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

#ifndef CRE_HTABLE_H
#define CRE_HTABLE_H

#include <stdint.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif


#if defined __cplusplus
    #define HTABLE_INLINE__     inline
#elif defined __STDC_VERSION__ && __STDC_VERSION__ >= 199901L
    #define HTABLE_INLINE__     static inline
#elif defined __GNUC__
    #define HTABLE_INLINE__     static __inline__
#elif defined _MSC_VER
    #define HTABLE_INLINE__     static __inline
#else
    #define HTABLE_INLINE__     static
#endif


#if defined offsetof
    #define HTABLE_OFFSETOF__(type, member)     offsetof(type, member)
#elif defined __GNUC__ && __GNUC__ >= 4
    #define HTABLE_OFFSETOF__(type, member)     __builtin_offsetof(type, member)
#else
    #define HTABLE_OFFSETOF__(type, member)     ((size_t) &((type*)0)->member)
#endif


typedef struct HTABLE_NODE {
    struct HTABLE_NODE* next;
} HTABLE_NODE;


typedef struct HTABLE {
    void** planes;
    int n_planes;
    size_t n;
} HTABLE;


/* Comparator function type.
 *
 * The comparator function determines whether the nodes have the identical key.
 * By 'key' we understand any internal data which uniquely identifies the node
 * within the hashtable and which during its presense in the hash table never
 * changes.
 *
 * Note we assume the same comparator function is used throughout the life
 * time of the hashtable.
 *
 * WARNING: When different comparator functions are used during the life time
 * of the hash table, the behavior is undefined.
 *
 * It has to return:
 *   - zero if the two nodes are equal;
 *   - non-zero otherwise.
 */
typedef int (*HTABLE_CMP_FUNC)(const HTABLE_NODE*, const HTABLE_NODE*);

/* Hash function.
 *
 * The function implementation must compute the hash only from some part of the
 * node data, which serves as the unique key of the node.
 *
 * The function should be implemented so that for (very large number of) nodes,
 * the returned values are about as uniformly distributed as possible in the
 * whole range between 0 and `UITN32_MAX`.
 *
 * WARNING: When different hash functions are used during the life time
 * of the hash table, the behavior is undefined.
 */
typedef uint32_t (*HTABLE_HASH_FUNC)(const HTABLE_NODE*);


/* Macro for getting pointer to the structure holding the hashtable node data.
 *
 * (If you use the HTABLE_NODE as the first member of your structure, you
 * can use a simple casting instead.)
 */
#define HTABLE_DATA(node_ptr, type, member)     \
                ((type*)((char*)(node_ptr) - HTABLE_OFFSETOF__(type, member)))


#define HTABLE_INITIALIZER              { NULL, 0, 0 }

HTABLE_INLINE__ void htable_init(HTABLE* htable)
        { htable->planes = NULL; htable->n_planes = 0; htable->n = 0; }

/* Cleaner of the hashtable. Calls the provided destructor for every node
 * and releases all itnernal buffers.:x
 */
void htable_fini(HTABLE* htable, void (*dtor_func)(HTABLE_NODE*));

/* Returns non-zero if the hash table is empty.
 */
HTABLE_INLINE__ int htable_is_empty(HTABLE* htable)
        { return (htable->n == 0); }

/* Insert a new node into the hash table.
 *
 * Returns 0 on success or -1 on failure. The function fails if an internal
 * memory allocation fails, or if a node with the same key is already present.
 */
int htable_insert(HTABLE* htable, HTABLE_NODE* node,
                  HTABLE_CMP_FUNC cmp_func, HTABLE_HASH_FUNC hash_func);

/* Faster version of htable_insert() which does not check whether there is not
 * yet any node of the same key present. We generally assume, application uses
 * this function only if it _knows_ there is no such node yet present.
 *
 * WARNING: If multiple nodes of the same key are inserted into the hash table,
 * it's then undefined which of those nodes are matched whenever lookup for the
 * given key is performed.
 *
 * Returns 0 on success or -1 on failure.
 */
int htable_insert_unsafe(HTABLE* htable, HTABLE_NODE* node,
                         HTABLE_CMP_FUNC cmp_func, HTABLE_HASH_FUNC hash_func);

/* Remove a node from the hash table equal to the provided key.
 *
 * Returns pointer to the removed node (so caller may e.g. free any resources
 * associated with it), or NULL if no such node is present in the table.
 */
HTABLE_NODE* htable_remove(HTABLE* htable, const HTABLE_NODE* key,
                           HTABLE_CMP_FUNC cmp_func, HTABLE_HASH_FUNC hash_func);

/* Looks for a node from the hash table equal to the provided key.
 *
 * Returns pointer to the found nod, or NULL if no such node is present in the
 * table.
 */
HTABLE_NODE* htable_lookup(HTABLE* htable, const HTABLE_NODE* key,
                           HTABLE_CMP_FUNC cmp_func, HTABLE_HASH_FUNC hash_func);


#ifdef __cplusplus
}  /* extern "C" { */
#endif

#endif  /* #ifndef CRE_HTABLE_H */
