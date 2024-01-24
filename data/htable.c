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

#include "htable.h"

#include <string.h>


#define HTABLE_PLANE_SIZE(plane_index)  (59 << (plane_index))
#define HTABLE_SLOT_COUNT(htable)       (HTABLE_PLANE_SIZE(0) * ((1 << (htable)->n_planes) - 1))
#define HTABLE_TOO_FULL(htable)         ((htable)->n >= HTABLE_SLOT_COUNT(htable))
#define HTABLE_TOO_EMPTY(htable)        ((htable)->n < (HTABLE_SLOT_COUNT(htable) / 4))


static int
htable_grow(HTABLE* htable)
{
    HTABLE_NODE** new_planes;

    /* Grow by appending one more plane, twice the size the previous one. */
    new_planes = (HTABLE_NODE**) realloc(htable->planes, (htable->n_planes+1) * sizeof(HTABLE_NODE*));
    if(new_planes == NULL) {
        if(htable->n_planes > 0) {
            /* It may be suboptimal, but we can still add the new stuff into
             * the current planes. */
            return 0;
        }
        return -1;
    }
    htable->planes = (void**) new_planes;

    new_planes[htable->n_planes] = (HTABLE_NODE*) malloc(HTABLE_PLANE_SIZE(htable->n_planes) * sizeof(HTABLE_NODE*));
    if(new_planes[htable->n_planes] == NULL)
        return -1;
    memset(new_planes[htable->n_planes], 0, HTABLE_PLANE_SIZE(htable->n_planes) * sizeof(HTABLE_NODE*));
    htable->n_planes++;

    return 0;
}

static void
htable_free_all_planes(HTABLE* htable)
{
    int i;

    for(i = 0; i < htable->n_planes; i++)
        free(htable->planes[i]);
    free(htable->planes);

    htable->planes = NULL;
    htable->n_planes = 0;
}

static void
htable_shrink(HTABLE* htable)
{
    HTABLE_NODE** prev_plane;
    size_t prev_plane_size;
    HTABLE_NODE** plane;
    size_t plane_size;
    size_t i;

    if(htable->n == 0) {
        htable_free_all_planes(htable);
        return;
    }

    if(htable->n_planes <= 1)
        return;

    /* We shrink by merging the largest plane into the previous one. */
    prev_plane = (HTABLE_NODE**) htable->planes[htable->n_planes-2];
    prev_plane_size = HTABLE_PLANE_SIZE(htable->n_planes-2);
    plane = (HTABLE_NODE**) htable->planes[htable->n_planes-1];
    plane_size = HTABLE_PLANE_SIZE(htable->n_planes-1);
    for(i = 0; i < plane_size; i++) {
        if(plane[i] != NULL) {
            if(prev_plane[i % prev_plane_size] != NULL) {
                /* Join the slot in the previous plane to our tail. */
                HTABLE_NODE* tail = plane[i];
                while(tail->next != NULL)
                    tail = tail->next;
                tail->next = prev_plane[i % prev_plane_size];
            }

            /* Move it to the previous plane. */
            prev_plane[i % prev_plane_size] = plane[i];
        }
    }

    free(plane);
    htable->n_planes--;
}

static HTABLE_NODE*
htable_lookup_internal(HTABLE* htable, uint32_t hash, const HTABLE_NODE* key,
                       HTABLE_NODE*** p_ref, HTABLE_CMP_FUNC cmp_func)
{
    HTABLE_NODE* node;
    HTABLE_NODE** ref;
    int i;

    /* It's better to lookup the biggest planes first, as much more stuff is
     * stored there, and also because it's moire recently inserted stuff which
     * is arguably more likely to accessed soon. */
    for(i = htable->n_planes-1; i >= 0; i--) {
        HTABLE_NODE** plane;
        size_t index;

        plane = (HTABLE_NODE**) htable->planes[i];
        index = hash % HTABLE_PLANE_SIZE(i);

        ref = &plane[index];
        node = plane[index];

        while(node != NULL) {
            if(cmp_func(key, node) == 0) {
                if(p_ref != NULL)
                    *p_ref = ref;
                return node;
            }

            ref = &node->next;
            node = node->next;
        }
    }

    return NULL;
}

void
htable_fini(HTABLE* htable, void (*dtor_func)(HTABLE_NODE*))
{
    if(dtor_func != NULL  &&  htable->n > 0) {
        unsigned i;
        size_t index;

        for(i = 0; i < htable->n_planes; i++) {
            HTABLE_NODE** plane = (HTABLE_NODE**) htable->planes[i];
            for(index = 0; index < HTABLE_PLANE_SIZE(i); index++) {
                while(plane[index] != NULL) {
                    HTABLE_NODE* node;

                    node = plane[index];
                    plane[index] = node->next;
                    dtor_func(node);
                }
            }
        }
    }
    htable->n = 0;

    htable_free_all_planes(htable);
}

static int
htable_insert_internal(HTABLE* htable, HTABLE_NODE* node,
                       HTABLE_CMP_FUNC cmp_func, HTABLE_HASH_FUNC hash_func,
                       int skip_lookup)
{
    uint32_t hash;
    HTABLE_NODE** plane;

    hash = hash_func(node);
    if(!skip_lookup) {
        if(htable_lookup_internal(htable, hash, node, NULL, cmp_func) != NULL)
            return -1;
    }

    /* When we are too populated, grow by adding a new plane. */
    if(HTABLE_TOO_FULL(htable)) {
        if(htable_grow(htable) != 0)
            return -1;
    }

    plane = (HTABLE_NODE**) htable->planes[htable->n_planes - 1];

    node->next = plane[hash % HTABLE_PLANE_SIZE(htable->n_planes - 1)];
    plane[hash % HTABLE_PLANE_SIZE(htable->n_planes - 1)] = node;

    htable->n++;
    return 0;
}

int
htable_insert(HTABLE* htable, HTABLE_NODE* node,
              HTABLE_CMP_FUNC cmp_func, HTABLE_HASH_FUNC hash_func)
{
    return htable_insert_internal(htable, node, cmp_func, hash_func, 0);
}

int
htable_insert_unsafe(HTABLE* htable, HTABLE_NODE* node,
                     HTABLE_CMP_FUNC cmp_func, HTABLE_HASH_FUNC hash_func)
{
    return htable_insert_internal(htable, node, cmp_func, hash_func, 1);
}

HTABLE_NODE*
htable_remove(HTABLE* htable, const HTABLE_NODE* key,
              HTABLE_CMP_FUNC cmp_func, HTABLE_HASH_FUNC hash_func)
{
    uint32_t hash;
    HTABLE_NODE* node;
    HTABLE_NODE** p_ref;

    hash = hash_func(key);
    node = htable_lookup_internal(htable, hash, key, &p_ref, cmp_func);
    if(node == NULL)
        return NULL;

    *p_ref = node->next;
    htable->n--;

    if(HTABLE_TOO_EMPTY(htable))
        htable_shrink(htable);

    return node;
}

HTABLE_NODE*
htable_lookup(HTABLE* htable, const HTABLE_NODE* key,
              HTABLE_CMP_FUNC cmp_func, HTABLE_HASH_FUNC hash_func)
{
    uint32_t hash;

    hash = hash_func(key);
    return htable_lookup_internal(htable, hash, key, NULL, cmp_func);
}
