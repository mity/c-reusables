/*
 * C Reusables
 * <http://github.com/mity/c-reusables>
 *
 * Copyright (c) 2018-2020 Martin Mitas
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

#ifndef CRE_LIST_H
#define CRE_LIST_H

#ifdef __cplusplus
extern "C" {
#endif


#if defined __cplusplus
    #define LIST_INLINE__       inline
#elif defined __STDC_VERSION__ && __STDC_VERSION__ >= 199901L
    #define LIST_INLINE__       static inline
#elif defined __GNUC__
    #define LIST_INLINE__       static __inline__
#elif defined _MSC_VER
    #define LIST_INLINE__       static __inline
#else
    #define LIST_INLINE__       static
#endif

#if defined offsetof
    #define LIST_OFFSETOF__(type, member)   offsetof(type, member)
#elif defined __GNUC__ && __GNUC__ >= 4
    #define LIST_OFFSETOF__(type, member)   __builtin_offsetof(type, member)
#else
    #define LIST_OFFSETOF__(type, member)   ((size_t) &((type*)0)->member)
#endif


/* This header implements simple intrusive doubly linked list.
 *
 * The word intrusive means our LIST_NODE structure does not hold any data
 * on its own. Instead, you are supposed to embed LIST_NODE in your structure.
 *
 * Given the simplicity of all operations, all functions are inline.
 *
 * To manipulate or query the list, the nodes are represented by pointer to
 * the LIST_NODE structure. To get a pointer to the enclosing application
 * specific data payload structure, use the macro LIST_DATA (which is just the
 * the typical "container_of()" macro implementation).
 *
 * Note that the list uses the common little trick and does not use NULL as
 * an end-of-list sentinel. Instead the first and last nodes point to the
 * structure representing the list as a whole. This simplifies many operations
 * because it mitigates the need for branches.
 *
 * The cost of that trick is the caller may not test an end condition with
 * NULL. For the sake of simplicity we provide the function list_end(), which
 * can be used similarly as std::vector::end() in C++ (both in forward as well
 * as backward iteration).
 *
 * Hence, the typical iteration over the list may look like this:
 *
 * ````
 * typedef struct MyStruct {
 *    ...   // Some data
 *
 *    // The embedded list node structure:
 *    LIST_NODE list_node;
 *
 *    ...   // Some more data
 * } MyStruct;
 *
 *
 * static void
 * walk_my_list(LIST* list)
 * {
 *     LIST_NODE* node;
 *     MyStruct* data_payload;
 *
 *     for(node = list_head(list); node != list_end(list); node = list_next(node)) {
 *         // Retrieve the data pay load from the node:
 *         data_payload = LIST_DATA(node, MyStruct, list_node);
 *
 *         ...   // Process the data payload as desired.
 *     }
 * }
 * ````
 */


/* List node structure. Treat as opaque.
 */
typedef struct LIST_NODE {
    struct LIST_NODE* p;
    struct LIST_NODE* n;
} LIST_NODE;


/* List structure. Treat as opaque.
 */
typedef struct LIST {
    struct LIST_NODE main;
} LIST;


/* Macro for getting pointer to the structure holding list node data.
 */
#define LIST_DATA(node_ptr, type, member)  \
                ((type*)((char*)(node_ptr) - LIST_OFFSETOF__(type, member)))


/* The list has to be initialized before it is used by any other function.
 */
LIST_INLINE__ void list_init(LIST* list)
        { list->main.p = list->main.n = &list->main; }


/* Check whether the list is empty or not.
 */
LIST_INLINE__ int list_is_empty(const LIST* list)
        { return (list->main.p == &list->main); }


/* Iterating the list.
 */
LIST_INLINE__ LIST_NODE* list_head(const LIST* list)        { return list->main.n; }
LIST_INLINE__ LIST_NODE* list_tail(const LIST* list)        { return list->main.p; }
LIST_INLINE__ LIST_NODE* list_prev(const LIST_NODE* node)   { return node->p; }
LIST_INLINE__ LIST_NODE* list_next(const LIST_NODE* node)   { return node->n; }
LIST_INLINE__ const LIST_NODE* list_end(const LIST* list)   { return &list->main; }

/* Add the given node into the list.
 *
 * Note that any node can be only in one list at any given time. If you
 * attempt to add the node into multiple lists (or multiple times into the
 * same list), the result is undefined.
 */
LIST_INLINE__ void list_append(LIST* list, LIST_NODE* node)
        { node->p = list->main.p; node->n = &list->main; node->p->n = node; node->n->p = node; }
LIST_INLINE__ void list_prepend(LIST* list, LIST_NODE* node)
        { node->p = &list->main; node->n = list->main.n; node->p->n = node; node->n->p = node; }
LIST_INLINE__ void list_insert_after(LIST_NODE* node_where, LIST_NODE* node)
        { node->p = node_where; node->n = node_where->n; node->p->n = node; node->n->p = node; }
LIST_INLINE__ void list_insert_before(LIST_NODE* node_where, LIST_NODE* node)
        { node->p = node_where->p; node->n = node_where; node->p->n = node; node->n->p = node; }

/* Disconnect the given node from its list.
 */
LIST_INLINE__ void list_remove(LIST_NODE* node)
        { node->p->n = node->n; node->n->p = node->p; }


#ifdef __cplusplus
}  /* extern "C" { */
#endif

#endif  /* CRE_LIST_H */
