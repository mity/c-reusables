/*
 * C Reusables
 * <http://github.com/mity/c-reusables>
 *
 * Copyright (c) 2020 Martin Mitas
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

#ifndef CRE_RBTREE_H
#define CRE_RBTREE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdlib.h>


#if defined __cplusplus
    #define RBTREE_INLINE__     inline
#elif defined __STDC_VERSION__ && __STDC_VERSION__ >= 199901L
    #define RBTREE_INLINE__     static inline
#elif defined __GNUC__
    #define RBTREE_INLINE__     static __inline__
#elif defined _MSC_VER
    #define RBTREE_INLINE__     static __inline
#else
    #define RBTREE_INLINE__     static
#endif

#if defined offsetof
    #define RBTREE_OFFSETOF__(type, member)     offsetof(type, member)
#elif defined __GNUC__ && __GNUC__ >= 4
    #define RBTREE_OFFSETOF__(type, member)     __builtin_offsetof(type, member)
#else
    #define RBTREE_OFFSETOF__(type, member)     ((size_t) &((type*)0)->member)
#endif


/* This header implements (an intrusive) red-black tree.
 *
 * See e.g. https://en.wikipedia.org/wiki/Red–black_tree if you are unfamiliar
 * with the concept of red-black tree.
 *
 * To manipulate or query the tree, the tree as a whole is represented by the
 * RBTREE structure and its nodes are represented by the RBTREE_NODE structure.
 *
 * The word "intrusive" in the title means our RBTREE_NODE structure does not
 * hold any payload data on its own. Instead, you are supposed to embed the
 * node in your own data structure.
 *
 * Also note we do not distinguish any "key" from "data". Caller has to
 * provide his own comparator function for defining the order of the data in
 * the tree and it's up to application to decide which data the node holds
 * serve as the "key", i.e. are used for the ordering.
 *
 * The design decision has some consequences:
 *
 * - To get a pointer to the enclosing application specific data structure, 
 *   use the macro RBTREE_DATA (which is just the the typical "container_of()"
 *   macro implementation.
 *
 * - For some operations like e.g. lookup, the caller has to provide a pointer
 *   to the same dummy structure, initialized enough to serve as the "key" so
 *   the comparator function may do its job.
 *
 * - It also means our implementation actually never allocates/frees any memory
 *   on the heap. All the tree operations like e.g. "insert" or "remove" just
 *   update the pointers in the node structure (and those in other nodes of
 *   the tree).
 *
 *   Caller has to allocate and initialize the payload node structure (which
 *   embeds the RBTREE_NODE) _before_ he inserts it onto the tree (so that the
 *   node can be compared to other nodes in the tree). Similarly, caller is
 *   responsible for freeing any resources the payload structure holds _after_
 *   it is removed from the tree.
 *
 * Note of warning: The RBTREE_NODE stores its color in the least significant
 * bit of the RBTREE_NODE::left pointer. This means that all the RBTREE_NODE
 * instances must be reasonably aligned.
 */


/* Tree node structure. Treat as opaque.
 */
typedef struct RBTREE_NODE {
    struct RBTREE_NODE* lc; /* combination of left ptr and color bit */
    struct RBTREE_NODE* r;  /* right ptr */
} RBTREE_NODE;


/* Tree structure. Treat as opaque.
 */
typedef struct RBTREE {
    RBTREE_NODE* root;
} RBTREE;


/* Comparator function type.
 *
 * The comparator function defines the order of the data stored in the tree.
 * As such, we suppose the same comparator function is used throughout the life
 * time of the tree.
 *
 * WARNING: When different comparator functions are used during the life time
 * of the tree, the behavior is undefined.
 *
 * It has to return:
 *   - negative value if the 1st argument is lower then the 2nd one;
 *   - positive value if the 1st argument is greater then the 2nd one;
 *   - zero if they are equal.
 */
typedef int (*RBTREE_CMP_FUNC)(const RBTREE_NODE*, const RBTREE_NODE*);


/* Macro for getting pointer to the structure holding the rbtree node data.
 */
#define RBTREE_DATA(node_ptr, type, member)     \
                ((type*)((char*)(node_ptr) - RBTREE_OFFSETOF__(type, member)))


/* The tree has to be initialized before it is used by any other function.
 *
 * Note there is no explicit cleanup counterpart: The caller is responsible
 * to eventually remove all nodes manually with rbtree_remove().
 *
 * Alternatively, the final clean-up can be done by walking over them via
 * rbtree_first_node() and rbtree_next_node() functions, destructing all the
 * nodes and then by re-initializing the tree (i.e. setting its root to NULL)
 * for a potential re-use.
 *
 * (For the final clean-up the later is generally more effective then calling
 * rbtree_remove() for every node because it avoids any re-balancing of the
 * tree.)
 */
RBTREE_INLINE__ void rbtree_init(RBTREE* tree)
        { tree->root = NULL; }

#define RBTREE_INITIALIZER      { NULL }


/* The tree has to be initialized before it is used by any other function.
 */
RBTREE_INLINE__ int rbtree_is_empty(const RBTREE* tree)
        { return (tree->root == NULL); }


/* Insert a new node into the tree.
 *
 * Returns 0 on success or -1 on failure (which may happen only if an equal
 * node is already present in the tree).
 */
int rbtree_insert(RBTREE* tree, RBTREE_NODE* node, RBTREE_CMP_FUNC cmp_func);


/* Remove a node equal to the key (as defined by the comparator function).
 *
 * Returns pointer to the node disconnected from the tree (so that caller can
 * e.g. to destroy it), or NULL if no such item has been found in the tree.
 */
RBTREE_NODE* rbtree_remove(RBTREE* tree, RBTREE_NODE* key, RBTREE_CMP_FUNC cmp_func);


/* Find a node equal to the key (as defined by the comparator function).
 *
 * Returns pointer to the found node or NULL if no such node has been found in
 * the tree.
 */
RBTREE_NODE* rbtree_lookup(RBTREE* tree, RBTREE_NODE* key, RBTREE_CMP_FUNC cmp_func);


/* Functions for walking over all nodes in the tree. After the last node in the
 * tree has been visited, the functions return NULL.
 *
 * Typical walking over the complete tree is implemented as follows:
 *
 * ```
 * static void walk_over_my_tree(RBTREE* tree)
 * {
 *     RBTREE_WALK walk;
 *     RBTREE* node;
 *
 *     for(node = rbtree_first_node(tree, &walk);
 *         node != NULL;
 *         node = rbtree_next_node(tree, &walk))
 *     {
 *         ...
 *     }
 * }
 * ```
 *
 * The nodes are visited in the order as defined by the comparator function
 * used during construction of the tree.
 *
 * Note the walking is safe in respect to a final tree cleanup. I.e. you may
 * destruct the node which is currently being visited and then still safely
 * call rbtree_next_node().
 *
 * However you must not modify the tree by any other means during the walk.
 */
typedef struct RBTREE_WALK {
    /* (2 * 8 * sizeof(void*)) is good enough to handle RB-trees of _any_ size.
     * Consider there cannot be more then 2^(8*sizeof(void*)) nodes in the
     * process memory and the longest root<-->leaf path in any RB-tree cannot
     * be longer then twice the shortest one. */
    RBTREE_NODE* stack[2 * 8 * sizeof(void*)];
    unsigned n;
} RBTREE_WALK;

RBTREE_NODE* rbtree_first_node(RBTREE* tree, RBTREE_WALK* walk);
RBTREE_NODE* rbtree_next_node(RBTREE* tree, RBTREE_WALK* walk);


#ifdef __cplusplus
}
#endif

#endif  /* CRE_RBTREE_H */
