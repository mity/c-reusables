/*
 * C Reusables
 * <http://github.com/mity/c-reusables>
 *
 * Copyright (c) 2016 Martin Mitáš
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

#ifndef CRE_DEFS_H
#define CRE_DEFS_H

#include <stdlib.h>


/* These macros are so trivial and so widespread that, we check whether they
 * are already defined. If they are, we leave them untouched in the believe
 * they should be morally equivalent. */


#ifndef MIN
    #define MIN(a,b)                ((a) < (b) ? (a) : (b))
#endif

#ifndef MAX
    #define MAX(a,b)                ((a) > (b) ? (a) : (b))
#endif

#ifndef MIN3
    #define MIN3(a,b,c)             MIN(MIN((a), (b)), (c))
#endif

#ifndef MAX3
    #define MAX3(a,b,c)             MAX(MAX((a), (b)), (c))
#endif

/* Clamp a value into a range.
 * Note the result is undefined if value_max < value_min. */
#ifndef CLAMP
    #define CLAMP(value,value_min,value_max)                    \
                                    MIN(MAX((value), (value_min)), (value_max))
#endif

#ifndef ABS
    #define ABS(a)                  ((a) >= 0 ? (a) : -(a))
#endif


#ifndef SIZEOF_ARRAY
    #define SIZEOF_ARRAY(array)     (sizeof((array)) / sizeof((array)[0]))
#endif


/* Example:
 *
 * typedef struct {
 *     int x;
 *     int y;
 * } MYSTRUCT;
 *
 * size_t
 * get_offset_of_y_in_MYSTRUCT(void)
 * {
 *     return OFFSETOF(MYSTRUCT, y);
 * }
 */
#ifndef OFFSETOF
    #if defined offsetof
        #define OFFSETOF(type, member)      offsetof(type, member)
    #elif defined __GNUC__ && __GNUC__ >= 4
        #define OFFSETOF(type, member)      __builtin_offsetof(type, member)
    #else
        #define OFFSETOF(type, member)      ((size_t) &((type*)0)->member)
    #endif
#endif

/* Example:
 *
 * typedef struct {
 *     int x;
 *     int y;
 * } MYSTRUCT;
 *
 * typedef struct {
 *     int some_member;
 *     MYSTRUCT s_member;
 * } MYCONTAINER;
 *
 * void
 * foo(MYSTRUCT* ptr_to_s)
 * {
 *     MYCONTAINER* container = CONTAINEROF(ptr_to_s, MYCONTAINER, s_member);
 *
 *     ...  // do something with container.
 * }
 *
 * Of course the example is valid ONLY if we KNOW that PTR_TO_S indeed points
 * to MYSTRUCT inside some MYCONTAINER. The behavior is undefined if it does
 * not.
 */
#ifndef CONTAINEROF
    #define CONTAINEROF(ptr, type, member)  \
                ((type*)((char*)(1 ? (ptr) : &((type *)0)->member) - OFFSETOF(type, member)))
#endif


/* Preprocessor magic for making a string literal from unquoted argument.
 * Example:
 *
 * #define VERSION_MAJOR    5
 * #define VERSION_MINOR    3
 *
 * #define VERSION_STRING   STRINGIZE(VERSION_MAJOR.VERSION_MINOR)  // "5.3"
 */
#ifndef STRINGIZE
    #define STRINGIZE_HELPER__(a)   #a
    #define STRINGIZE(a)            STRINGIZE_HELPER__(a)
#endif


#endif  /* CRE_DEFS_H */
