/*
 * C Reusables
 * <http://github.com/mity/c-reusables>
 *
 * Copyright (c) 2018-2023 Martin Mitáš
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

#ifndef CRE_BUFFER_H
#define CRE_BUFFER_H

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif


#if defined __cplusplus
    #define BUFFER_INLINE__     inline
#elif defined __STDC_VERSION__ && __STDC_VERSION__ >= 199901L
    #define BUFFER_INLINE__     static inline
#elif defined __GNUC__
    #define BUFFER_INLINE__     static __inline__
#elif defined _MSC_VER
    #define BUFFER_INLINE__     static __inline
#else
    #define BUFFER_INLINE__     static
#endif


typedef struct BUFFER {
    void* data;
    size_t size;
    size_t alloc;
} BUFFER;


/* Static initializer. */
#define BUFFER_INITIALIZER          { NULL, 0, 0 }

/* Initialize/deinitialize buffer structure. */
BUFFER_INLINE__ void buffer_init(BUFFER* buf)
        { buf->data = NULL; buf->size = 0; buf->alloc = 0; }
BUFFER_INLINE__ void buffer_fini(BUFFER* buf)
        { free(buf->data); }

/* Change capacity of the buffer. If lower than current size, the buffer is
 * truncated. */
int buffer_realloc(BUFFER* buf, size_t alloc);

/* Reserve new space for at least n bytes at the end of the buffer. */
int buffer_reserve(BUFFER* buf, size_t n);

/* Remove any empty space from the buffer. */
void buffer_shrink(BUFFER* buf);

BUFFER_INLINE__ size_t buffer_size(const BUFFER* buf)
        { return buf->size; }
BUFFER_INLINE__ int buffer_is_empty(const BUFFER* buf)
        { return (buf->size == 0); }

/* Const accessors. */
BUFFER_INLINE__ const void* buffer_const_data(const BUFFER* buf)
        { return buf->data; }
BUFFER_INLINE__ const void* buffer_const_data_at(const BUFFER* buf, size_t off)
        { return (const void*) (((const uint8_t*)buf->data) + off); }

/* Mutable accessors. */
BUFFER_INLINE__ void* buffer_data(BUFFER* buf)
        { return buf->data; }
BUFFER_INLINE__ void* buffer_data_at(BUFFER* buf, size_t off)
        { return (void*) (((uint8_t*)buf->data) + off); }

/* Inserting N bytes.
 * The _raw variant on success returns pointer where app is supposed to write
 * N bytes; or NULL on error. */
void* buffer_insert_raw(BUFFER* buf, size_t off, size_t n);
int buffer_insert(BUFFER* buf, size_t off, const void* data, size_t n);

/* Appending.
 * The _raw variant on success returns pointer where app is supposed to write
 * N bytes; or NULL on error. */
BUFFER_INLINE__ void* buffer_append_raw(BUFFER* buf, size_t n)
        { return buffer_insert_raw(buf, buf->size, n); }
BUFFER_INLINE__ int buffer_append(BUFFER* buf, const void* data, size_t n)
        { return buffer_insert(buf, buf->size, data, n); }

/* Remove N bytes from the given offset. */
void buffer_remove(BUFFER* buf, size_t off, size_t n);

/* Remove all buffer contents. */
BUFFER_INLINE__ void buffer_clear(BUFFER* buf)
        { buffer_remove(buf, 0, buf->size); }

/* Take over the responsibility of the buffer contents. Caller then
 * eventually must free() the returned block. */
BUFFER_INLINE__ void* buffer_acquire(BUFFER* buf, size_t* p_size)
        { void* data = buf->data; if(p_size != NULL) *p_size = buf->size;
          buffer_init(buf); return data; }

/* Swap contents of two buffers. */
BUFFER_INLINE__ void buffer_swap(BUFFER* buf1, BUFFER* buf2)
        { BUFFER tmp; tmp = *buf1; *buf1 = *buf2; *buf2 = tmp; }


/***********************
 *** STACK structure ***
 ***********************/

/* This is just an inline wrapper of the BUFFER structure. */
typedef struct STACK STACK;
struct STACK { BUFFER buf; };

#define STACK_INITIALIZER           { BUFFER_INITIALIZER }

BUFFER_INLINE__ void stack_init(STACK* stack)
        { buffer_init(&stack->buf); }
BUFFER_INLINE__ void stack_fini(STACK* stack)
        { buffer_fini(&stack->buf); }

BUFFER_INLINE__ int stack_reserve(STACK* stack, size_t n)
        { return buffer_reserve(&stack->buf, n); }

BUFFER_INLINE__ size_t stack_size(const STACK* stack)
        { return buffer_size(&stack->buf); }
BUFFER_INLINE__ int stack_is_empty(const STACK* stack)
        { return buffer_is_empty(&stack->buf); }

BUFFER_INLINE__ const void* stack_const_data(const STACK* stack)
        { return buffer_const_data(&stack->buf); }
BUFFER_INLINE__ void* stack_data(STACK* stack)
        { return buffer_data(&stack->buf); }

BUFFER_INLINE__ void* stack_push_raw(STACK* stack, size_t n)
        { return buffer_append_raw(&stack->buf, n); }
BUFFER_INLINE__ int stack_push(STACK* stack, const void* data, size_t n)
        { return buffer_append(&stack->buf, data, n); }
BUFFER_INLINE__ int stack_push_int8(STACK* stack, int8_t i8)
        { return stack_push(stack, (void*) &i8, sizeof(int8_t)); }
BUFFER_INLINE__ int stack_push_uint8(STACK* stack, uint8_t u8)
        { return stack_push(stack, (void*) &u8, sizeof(uint8_t)); }
BUFFER_INLINE__ int stack_push_int16(STACK* stack, int16_t i16)
        { return stack_push(stack, (void*) &i16, sizeof(int16_t)); }
BUFFER_INLINE__ int stack_push_uint16(STACK* stack, uint16_t u16)
        { return stack_push(stack, (void*) &u16, sizeof(uint16_t)); }
BUFFER_INLINE__ int stack_push_int32(STACK* stack, int32_t i32)
        { return stack_push(stack, (void*) &i32, sizeof(int32_t)); }
BUFFER_INLINE__ int stack_push_uint32(STACK* stack, uint32_t u32)
        { return stack_push(stack, (void*) &u32, sizeof(uint32_t)); }
BUFFER_INLINE__ int stack_push_int64(STACK* stack, int64_t i64)
        { return stack_push(stack, (void*) &i64, sizeof(int64_t)); }
BUFFER_INLINE__ int stack_push_uint64(STACK* stack, uint64_t u64)
        { return stack_push(stack, (void*) &u64, sizeof(uint64_t)); }
BUFFER_INLINE__ int stack_push_ptr(STACK* stack, const void* ptr)
        { return stack_push(stack, (void*) &ptr, sizeof(void*)); }

BUFFER_INLINE__ const void* stack_peek_raw(const STACK* stack, size_t n)
        { return buffer_const_data_at(&stack->buf, stack_size(stack) - n); }
BUFFER_INLINE__ void stack_peek(const STACK* stack, void* addr, size_t n)
        { memcpy(addr, buffer_const_data_at(&stack->buf, stack_size(stack) - n), n); }
BUFFER_INLINE__ int8_t stack_peek_int8(const STACK* stack)
        { int8_t ret; stack_peek(stack, (void*) &ret, sizeof(int8_t)); return ret; }
BUFFER_INLINE__ uint8_t stack_peek_uint8(const STACK* stack)
        { uint8_t ret; stack_peek(stack, (void*) &ret, sizeof(uint8_t)); return ret; }
BUFFER_INLINE__ int16_t stack_peek_int16(const STACK* stack)
        { int16_t ret; stack_peek(stack, (void*) &ret, sizeof(int16_t)); return ret; }
BUFFER_INLINE__ uint16_t stack_peek_uint16(const STACK* stack)
        { uint16_t ret; stack_peek(stack, (void*) &ret, sizeof(uint16_t)); return ret; }
BUFFER_INLINE__ int32_t stack_peek_int32(const STACK* stack)
        { int32_t ret; stack_peek(stack, (void*) &ret, sizeof(int32_t)); return ret; }
BUFFER_INLINE__ uint32_t stack_peek_uint32(const STACK* stack)
        { uint32_t ret; stack_peek(stack, (void*) &ret, sizeof(uint32_t)); return ret; }
BUFFER_INLINE__ int64_t stack_peek_int64(const STACK* stack)
        { int64_t ret; stack_peek(stack, (void*) &ret, sizeof(int64_t)); return ret; }
BUFFER_INLINE__ uint64_t stack_peek_uint64(const STACK* stack)
        { uint64_t ret; stack_peek(stack, (void*) &ret, sizeof(uint64_t)); return ret; }
BUFFER_INLINE__ void* stack_peek_ptr(const STACK* stack)
        { void* ret; stack_peek(stack, (void*) &ret, sizeof(void*)); return ret; }

BUFFER_INLINE__ void* stack_pop_raw(STACK* stack, size_t n)
        { stack->buf.size -= n; return buffer_data_at(&stack->buf, stack->buf.size); }
BUFFER_INLINE__ void stack_pop(STACK* stack, void* addr, size_t n)
        { stack_peek(stack, addr, n); buffer_remove(&stack->buf, stack_size(stack) - n, n); }
BUFFER_INLINE__ int8_t stack_pop_int8(STACK* stack)
        { int8_t ret; stack_pop(stack, (void*) &ret, sizeof(int8_t)); return ret; }
BUFFER_INLINE__ uint8_t stack_pop_uint8(STACK* stack)
        { uint8_t ret; stack_pop(stack, (void*) &ret, sizeof(uint8_t)); return ret; }
BUFFER_INLINE__ int16_t stack_pop_int16(STACK* stack)
        { int16_t ret; stack_pop(stack, (void*) &ret, sizeof(int16_t)); return ret; }
BUFFER_INLINE__ uint16_t stack_pop_uint16(STACK* stack)
        { uint16_t ret; stack_pop(stack, (void*) &ret, sizeof(uint16_t)); return ret; }
BUFFER_INLINE__ int32_t stack_pop_int32(STACK* stack)
        { int32_t ret; stack_pop(stack, (void*) &ret, sizeof(int32_t)); return ret; }
BUFFER_INLINE__ uint32_t stack_pop_uint32(STACK* stack)
        { uint32_t ret; stack_pop(stack, (void*) &ret, sizeof(uint32_t)); return ret; }
BUFFER_INLINE__ int64_t stack_pop_int64(STACK* stack)
        { int64_t ret; stack_pop(stack, (void*) &ret, sizeof(int64_t)); return ret; }
BUFFER_INLINE__ uint64_t stack_pop_uint64(STACK* stack)
        { uint64_t ret; stack_pop(stack, (void*) &ret, sizeof(uint64_t)); return ret; }
BUFFER_INLINE__ void* stack_pop_ptr(STACK* stack)
        { void* ret; stack_pop(stack, (void*) &ret, sizeof(void*)); return ret; }

BUFFER_INLINE__ void stack_clear(STACK* stack)
        { buffer_clear(&stack->buf); }

BUFFER_INLINE__ void* stack_acquire(STACK* stack, size_t* p_size)
        { return buffer_acquire(&stack->buf, p_size); }

BUFFER_INLINE__ void stack_swap(STACK* stack1, STACK* stack2)
        { buffer_swap(&stack1->buf, &stack2->buf); }


/***********************
 *** ARRAY structure ***
 ***********************/

/* These are just an inline wrappers of the BUFFER structure. */
typedef struct ARRAY_int8       ARRAY_int8;
typedef struct ARRAY_uint8      ARRAY_uint8;
typedef struct ARRAY_int16      ARRAY_int16;
typedef struct ARRAY_uint16     ARRAY_uint16;
typedef struct ARRAY_int32      ARRAY_int32;
typedef struct ARRAY_uint32     ARRAY_uint32;
typedef struct ARRAY_int64      ARRAY_int64;
typedef struct ARRAY_uint64     ARRAY_uint64;
typedef struct ARRAY_ptr        ARRAY_ptr;

struct ARRAY_int8       { BUFFER buf; };
struct ARRAY_uint8      { BUFFER buf; };
struct ARRAY_int16      { BUFFER buf; };
struct ARRAY_uint16     { BUFFER buf; };
struct ARRAY_int32      { BUFFER buf; };
struct ARRAY_uint32     { BUFFER buf; };
struct ARRAY_int64      { BUFFER buf; };
struct ARRAY_uint64     { BUFFER buf; };
struct ARRAY_ptr        { BUFFER buf; };


#define ARRAY_int8_INITIALIZER      { BUFFER_INITIALIZER }
#define ARRAY_uint8_INITIALIZER     { BUFFER_INITIALIZER }
#define ARRAY_int16_INITIALIZER     { BUFFER_INITIALIZER }
#define ARRAY_uint16_INITIALIZER    { BUFFER_INITIALIZER }
#define ARRAY_int32_INITIALIZER     { BUFFER_INITIALIZER }
#define ARRAY_uint32_INITIALIZER    { BUFFER_INITIALIZER }
#define ARRAY_int64_INITIALIZER     { BUFFER_INITIALIZER }
#define ARRAY_uint64_INITIALIZER    { BUFFER_INITIALIZER }
#define ARRAY_ptr_INITIALIZER       { BUFFER_INITIALIZER }


BUFFER_INLINE__ void array_int8_init(ARRAY_int8* array)
        { buffer_init(&array->buf); }
BUFFER_INLINE__ void array_uint8_init(ARRAY_uint8* array)
        { buffer_init(&array->buf); }
BUFFER_INLINE__ void array_int16_init(ARRAY_int16* array)
        { buffer_init(&array->buf); }
BUFFER_INLINE__ void array_uint16_init(ARRAY_uint16* array)
        { buffer_init(&array->buf); }
BUFFER_INLINE__ void array_int32_init(ARRAY_int32* array)
        { buffer_init(&array->buf); }
BUFFER_INLINE__ void array_uint32_init(ARRAY_uint32* array)
        { buffer_init(&array->buf); }
BUFFER_INLINE__ void array_int64_init(ARRAY_int64* array)
        { buffer_init(&array->buf); }
BUFFER_INLINE__ void array_uint64_init(ARRAY_uint64* array)
        { buffer_init(&array->buf); }
BUFFER_INLINE__ void array_ptr_init(ARRAY_ptr* array)
        { buffer_init(&array->buf); }

BUFFER_INLINE__ void array_int8_fini(ARRAY_int8* array)
        { buffer_fini(&array->buf); }
BUFFER_INLINE__ void array_uint8_fini(ARRAY_uint8* array)
        { buffer_fini(&array->buf); }
BUFFER_INLINE__ void array_int16_fini(ARRAY_int16* array)
        { buffer_fini(&array->buf); }
BUFFER_INLINE__ void array_uint16_fini(ARRAY_uint16* array)
        { buffer_fini(&array->buf); }
BUFFER_INLINE__ void array_int32_fini(ARRAY_int32* array)
        { buffer_fini(&array->buf); }
BUFFER_INLINE__ void array_uint32_fini(ARRAY_uint32* array)
        { buffer_fini(&array->buf); }
BUFFER_INLINE__ void array_int64_fini(ARRAY_int64* array)
        { buffer_fini(&array->buf); }
BUFFER_INLINE__ void array_uint64_fini(ARRAY_uint64* array)
        { buffer_fini(&array->buf); }
BUFFER_INLINE__ void array_ptr_fini(ARRAY_ptr* array)
        { buffer_fini(&array->buf); }

BUFFER_INLINE__ const int8_t* array_int8_const_data(const ARRAY_int8* array)
        { return (const int8_t*) buffer_const_data(&array->buf); }
BUFFER_INLINE__ const uint8_t* array_uint8_const_data(const ARRAY_uint8* array)
        { return (const uint8_t*) buffer_const_data(&array->buf); }
BUFFER_INLINE__ const int16_t* array_int16_const_data(const ARRAY_int16* array)
        { return (const int16_t*) buffer_const_data(&array->buf); }
BUFFER_INLINE__ const uint16_t* array_uint16_const_data(const ARRAY_uint16* array)
        { return (const uint16_t*) buffer_const_data(&array->buf); }
BUFFER_INLINE__ const int32_t* array_int32_const_data(const ARRAY_int32* array)
        { return (const int32_t*) buffer_const_data(&array->buf); }
BUFFER_INLINE__ const uint32_t* array_uint32_const_data(const ARRAY_uint32* array)
        { return (const uint32_t*) buffer_const_data(&array->buf); }
BUFFER_INLINE__ const int64_t* array_int64_const_data(const ARRAY_int64* array)
        { return (const int64_t*) buffer_const_data(&array->buf); }
BUFFER_INLINE__ const uint64_t* array_uint64_const_data(const ARRAY_uint64* array)
        { return (const uint64_t*) buffer_const_data(&array->buf); }
BUFFER_INLINE__ const void** array_ptr_const_data(const ARRAY_ptr* array)
        { return (const void**) buffer_const_data(&array->buf); }

BUFFER_INLINE__ int8_t* array_int8_data(ARRAY_int8* array)
        { return (int8_t*) buffer_data(&array->buf); }
BUFFER_INLINE__ uint8_t* array_uint8_data(ARRAY_uint8* array)
        { return (uint8_t*) buffer_data(&array->buf); }
BUFFER_INLINE__ int16_t* array_int16_data(ARRAY_int16* array)
        { return (int16_t*) buffer_data(&array->buf); }
BUFFER_INLINE__ uint16_t* array_uint16_data(ARRAY_uint16* array)
        { return (uint16_t*) buffer_data(&array->buf); }
BUFFER_INLINE__ int32_t* array_int32_data(ARRAY_int32* array)
        { return (int32_t*) buffer_data(&array->buf); }
BUFFER_INLINE__ uint32_t* array_uint32_data(ARRAY_uint32* array)
        { return (uint32_t*) buffer_data(&array->buf); }
BUFFER_INLINE__ int64_t* array_int64_data(ARRAY_int64* array)
        { return (int64_t*) buffer_data(&array->buf); }
BUFFER_INLINE__ uint64_t* array_uint64_data(ARRAY_uint64* array)
        { return (uint64_t*) buffer_data(&array->buf); }
BUFFER_INLINE__ void** array_ptr_data(ARRAY_ptr* array)
        { return (void**) buffer_data(&array->buf); }

BUFFER_INLINE__ size_t array_int8_size(const ARRAY_int8* array)
        { return (buffer_size(&array->buf) / sizeof(int8_t)); }
BUFFER_INLINE__ size_t array_uint8_size(const ARRAY_uint8* array)
        { return (buffer_size(&array->buf) / sizeof(uint8_t)); }
BUFFER_INLINE__ size_t array_int16_size(const ARRAY_int16* array)
        { return (buffer_size(&array->buf) / sizeof(int16_t)); }
BUFFER_INLINE__ size_t array_uint16_size(const ARRAY_uint16* array)
        { return (buffer_size(&array->buf) / sizeof(uint16_t)); }
BUFFER_INLINE__ size_t array_int32_size(const ARRAY_int32* array)
        { return (buffer_size(&array->buf) / sizeof(int32_t)); }
BUFFER_INLINE__ size_t array_uint32_size(const ARRAY_uint32* array)
        { return (buffer_size(&array->buf) / sizeof(uint32_t)); }
BUFFER_INLINE__ size_t array_int64_size(const ARRAY_int64* array)
        { return (buffer_size(&array->buf) / sizeof(int64_t)); }
BUFFER_INLINE__ size_t array_uint64_size(const ARRAY_uint64* array)
        { return (buffer_size(&array->buf) / sizeof(uint64_t)); }
BUFFER_INLINE__ size_t array_ptr_size(const ARRAY_ptr* array)
        { return (buffer_size(&array->buf) / sizeof(void*)); }

BUFFER_INLINE__ int array_int8_is_empty(ARRAY_int8* array)
        { return buffer_is_empty(&array->buf); }
BUFFER_INLINE__ int array_uint8_is_empty(ARRAY_uint8* array)
        { return buffer_is_empty(&array->buf); }
BUFFER_INLINE__ int array_int16_is_empty(ARRAY_int16* array)
        { return buffer_is_empty(&array->buf); }
BUFFER_INLINE__ int array_uint16_is_empty(ARRAY_uint16* array)
        { return buffer_is_empty(&array->buf); }
BUFFER_INLINE__ int array_int32_is_empty(ARRAY_int32* array)
        { return buffer_is_empty(&array->buf); }
BUFFER_INLINE__ int array_uint32_is_empty(ARRAY_uint32* array)
        { return buffer_is_empty(&array->buf); }
BUFFER_INLINE__ int array_int64_is_empty(ARRAY_int64* array)
        { return buffer_is_empty(&array->buf); }
BUFFER_INLINE__ int array_uint64_is_empty(ARRAY_uint64* array)
        { return buffer_is_empty(&array->buf); }
BUFFER_INLINE__ int array_ptr_is_empty(ARRAY_ptr* array)
        { return buffer_is_empty(&array->buf); }

BUFFER_INLINE__ int8_t array_int8_get(const ARRAY_int8* array, size_t index)
        { return (array_int8_const_data(array))[index]; }
BUFFER_INLINE__ uint8_t array_uint8_get(const ARRAY_uint8* array, size_t index)
        { return (array_uint8_const_data(array))[index]; }
BUFFER_INLINE__ int16_t array_int16_get(const ARRAY_int16* array, size_t index)
        { return (array_int16_const_data(array))[index]; }
BUFFER_INLINE__ uint16_t array_uint16_get(const ARRAY_uint16* array, size_t index)
        { return (array_uint16_const_data(array))[index]; }
BUFFER_INLINE__ int32_t array_int32_get(const ARRAY_int32* array, size_t index)
        { return (array_int32_const_data(array))[index]; }
BUFFER_INLINE__ uint32_t array_uint32_get(const ARRAY_uint32* array, size_t index)
        { return (array_uint32_const_data(array))[index]; }
BUFFER_INLINE__ int64_t array_int64_get(const ARRAY_int64* array, size_t index)
        { return (array_int64_const_data(array))[index]; }
BUFFER_INLINE__ uint64_t array_uint64_get(const ARRAY_uint64* array, size_t index)
        { return (array_uint64_const_data(array))[index]; }
BUFFER_INLINE__ const void* array_ptr_get(const ARRAY_ptr* array, size_t index)
        { return (array_ptr_const_data(array))[index]; }

BUFFER_INLINE__ void array_int8_set(ARRAY_int8* array, size_t index, int8_t i8)
        { (array_int8_data(array))[index] = i8; }
BUFFER_INLINE__ void array_uint8_set(ARRAY_uint8* array, size_t index, int8_t u8)
        { (array_uint8_data(array))[index] = u8; }
BUFFER_INLINE__ void array_int16_set(ARRAY_int16* array, size_t index, int16_t i16)
        { (array_int16_data(array))[index] = i16; }
BUFFER_INLINE__ void array_uint16_set(ARRAY_uint16* array, size_t index, int16_t u16)
        { (array_uint16_data(array))[index] = u16; }
BUFFER_INLINE__ void array_int32_set(ARRAY_int32* array, size_t index, int32_t i32)
        { (array_int32_data(array))[index] = i32; }
BUFFER_INLINE__ void array_uint32_set(ARRAY_uint32* array, size_t index, int32_t u32)
        { (array_uint32_data(array))[index] = u32; }
BUFFER_INLINE__ void array_int64_set(ARRAY_int64* array, size_t index, int64_t i64)
        { (array_int64_data(array))[index] = i64; }
BUFFER_INLINE__ void array_uint64_set(ARRAY_uint64* array, size_t index, int64_t u64)
        { (array_uint64_data(array))[index] = u64; }
BUFFER_INLINE__ void array_ptr_set(ARRAY_ptr* array, size_t index, void* ptr)
        { (array_ptr_data(array))[index] = ptr; }

BUFFER_INLINE__ int array_int8_insert_raw(ARRAY_int8* array, size_t index)
        { return (buffer_insert_raw(&array->buf, index * sizeof(int8_t), sizeof(int8_t)) != NULL ? 0 : -1); }
BUFFER_INLINE__ int array_uint8_insert_raw(ARRAY_uint8* array, size_t index)
        { return (buffer_insert_raw(&array->buf, index * sizeof(uint8_t), sizeof(uint8_t)) != NULL ? 0 : -1); }
BUFFER_INLINE__ int array_int16_insert_raw(ARRAY_int16* array, size_t index)
        { return (buffer_insert_raw(&array->buf, index * sizeof(int16_t), sizeof(int16_t)) != NULL ? 0 : -1); }
BUFFER_INLINE__ int array_uint16_insert_raw(ARRAY_uint16* array, size_t index)
        { return (buffer_insert_raw(&array->buf, index * sizeof(uint16_t), sizeof(uint16_t)) != NULL ? 0 : -1); }
BUFFER_INLINE__ int array_int32_insert_raw(ARRAY_int32* array, size_t index)
        { return (buffer_insert_raw(&array->buf, index * sizeof(int32_t), sizeof(int32_t)) != NULL ? 0 : -1); }
BUFFER_INLINE__ int array_uint32_insert_raw(ARRAY_uint32* array, size_t index)
        { return (buffer_insert_raw(&array->buf, index * sizeof(uint32_t), sizeof(uint32_t)) != NULL ? 0 : -1); }
BUFFER_INLINE__ int array_int64_insert_raw(ARRAY_int64* array, size_t index)
        { return (buffer_insert_raw(&array->buf, index * sizeof(int64_t), sizeof(int64_t)) != NULL ? 0 : -1); }
BUFFER_INLINE__ int array_uint64_insert_raw(ARRAY_uint64* array, size_t index)
        { return (buffer_insert_raw(&array->buf, index * sizeof(uint64_t), sizeof(uint64_t)) != NULL ? 0 : -1); }
BUFFER_INLINE__ int array_ptr_insert_raw(ARRAY_ptr* array, size_t index)
        { return (buffer_insert_raw(&array->buf, index * sizeof(void*), sizeof(void*)) != NULL ? 0 : -1); }

BUFFER_INLINE__ int array_int8_insert(ARRAY_int8* array, size_t index, int8_t i8)
        {
            int8_t* addr;
            addr = (int8_t*) buffer_insert_raw(&array->buf, index * sizeof(int8_t), sizeof(int8_t));
            if(addr != NULL)
                *addr = i8;
            return (addr != NULL) ? 0 : -1;
        }
BUFFER_INLINE__ int array_uint8_insert(ARRAY_uint8* array, size_t index, uint8_t u8)
        {
            uint8_t* addr;
            addr = (uint8_t*) buffer_insert_raw(&array->buf, index * sizeof(uint8_t), sizeof(uint8_t));
            if(addr != NULL)
                *addr = u8;
            return (addr != NULL) ? 0 : -1;
        }
BUFFER_INLINE__ int array_int16_insert(ARRAY_int16* array, size_t index, int16_t i16)
        {
            int16_t* addr;
            addr = (int16_t*) buffer_insert_raw(&array->buf, index * sizeof(int16_t), sizeof(int16_t));
            if(addr != NULL)
                *addr = i16;
            return (addr != NULL) ? 0 : -1;
        }
BUFFER_INLINE__ int array_uint16_insert(ARRAY_uint16* array, size_t index, uint16_t u16)
        {
            uint16_t* addr;
            addr = (uint16_t*) buffer_insert_raw(&array->buf, index * sizeof(uint16_t), sizeof(uint16_t));
            if(addr != NULL)
                *addr = u16;
            return (addr != NULL) ? 0 : -1;
        }
BUFFER_INLINE__ int array_int32_insert(ARRAY_int32* array, size_t index, int32_t i32)
        {
            int32_t* addr;
            addr = (int32_t*) buffer_insert_raw(&array->buf, index * sizeof(int32_t), sizeof(int32_t));
            if(addr != NULL)
                *addr = i32;
            return (addr != NULL) ? 0 : -1;
        }
BUFFER_INLINE__ int array_uint32_insert(ARRAY_uint32* array, size_t index, uint32_t u32)
        {
            uint32_t* addr;
            addr = (uint32_t*) buffer_insert_raw(&array->buf, index * sizeof(uint32_t), sizeof(uint32_t));
            if(addr != NULL)
                *addr = u32;
            return (addr != NULL) ? 0 : -1;
        }
BUFFER_INLINE__ int array_int64_insert(ARRAY_int64* array, size_t index, int64_t i64)
        {
            int64_t* addr;
            addr = (int64_t*) buffer_insert_raw(&array->buf, index * sizeof(int64_t), sizeof(int64_t));
            if(addr != NULL)
                *addr = i64;
            return (addr != NULL) ? 0 : -1;
        }
BUFFER_INLINE__ int array_uint64_insert(ARRAY_uint64* array, size_t index, uint64_t u64)
        {
            uint64_t* addr;
            addr = (uint64_t*) buffer_insert_raw(&array->buf, index * sizeof(uint64_t), sizeof(uint64_t));
            if(addr != NULL)
                *addr = u64;
            return (addr != NULL) ? 0 : -1;
        }
BUFFER_INLINE__ int array_ptr_insert(ARRAY_uint64* array, size_t index, void* ptr)
        {
            void** addr;
            addr = (void**) buffer_insert_raw(&array->buf, index * sizeof(void*), sizeof(void*));
            if(addr != NULL)
                *addr = ptr;
            return (addr != NULL) ? 0 : -1;
        }

BUFFER_INLINE__ int array_int8_insert_n_raw(ARRAY_int8* array, size_t index, size_t n)
        { return (buffer_insert_raw(&array->buf, index * sizeof(int8_t), n * sizeof(int8_t)) != NULL ? 0 : -1); }
BUFFER_INLINE__ int array_uint8_insert_n_raw(ARRAY_uint8* array, size_t index, size_t n)
        { return (buffer_insert_raw(&array->buf, index * sizeof(uint8_t), n * sizeof(uint8_t)) != NULL ? 0 : -1); }
BUFFER_INLINE__ int array_int16_insert_n_raw(ARRAY_int16* array, size_t index, size_t n)
        { return (buffer_insert_raw(&array->buf, index * sizeof(int16_t), n * sizeof(int16_t)) != NULL ? 0 : -1); }
BUFFER_INLINE__ int array_uint16_insert_n_raw(ARRAY_uint16* array, size_t index, size_t n)
        { return (buffer_insert_raw(&array->buf, index * sizeof(uint16_t), n * sizeof(uint16_t)) != NULL ? 0 : -1); }
BUFFER_INLINE__ int array_int32_insert_n_raw(ARRAY_int32* array, size_t index, size_t n)
        { return (buffer_insert_raw(&array->buf, index * sizeof(int32_t), n * sizeof(int32_t)) != NULL ? 0 : -1); }
BUFFER_INLINE__ int array_uint32_insert_n_raw(ARRAY_uint32* array, size_t index, size_t n)
        { return (buffer_insert_raw(&array->buf, index * sizeof(uint32_t), n * sizeof(uint32_t)) != NULL ? 0 : -1); }
BUFFER_INLINE__ int array_int64_insert_n_raw(ARRAY_int64* array, size_t index, size_t n)
        { return (buffer_insert_raw(&array->buf, index * sizeof(int64_t), n * sizeof(int64_t)) != NULL ? 0 : -1); }
BUFFER_INLINE__ int array_uint64_insert_n_raw(ARRAY_uint64* array, size_t index, size_t n)
        { return (buffer_insert_raw(&array->buf, index * sizeof(uint64_t), n * sizeof(uint64_t)) != NULL ? 0 : -1); }
BUFFER_INLINE__ int array_ptr_insert_n_raw(ARRAY_ptr* array, size_t index, size_t n)
        { return (buffer_insert_raw(&array->buf, index * sizeof(void*), n * sizeof(void*)) != NULL ? 0 : -1); }

BUFFER_INLINE__ int array_int8_append_raw(ARRAY_int8* array)
        { return (buffer_append_raw(&array->buf, sizeof(int8_t)) != NULL ? 0 : -1); }
BUFFER_INLINE__ int array_uint8_append_raw(ARRAY_uint8* array)
        { return (buffer_append_raw(&array->buf, sizeof(uint8_t)) != NULL ? 0 : -1); }
BUFFER_INLINE__ int array_int16_append_raw(ARRAY_int16* array)
        { return (buffer_append_raw(&array->buf, sizeof(int16_t)) != NULL ? 0 : -1); }
BUFFER_INLINE__ int array_uint16_append_raw(ARRAY_uint16* array)
        { return (buffer_append_raw(&array->buf, sizeof(uint16_t)) != NULL ? 0 : -1); }
BUFFER_INLINE__ int array_int32_append_raw(ARRAY_int32* array)
        { return (buffer_append_raw(&array->buf, sizeof(int32_t)) != NULL ? 0 : -1); }
BUFFER_INLINE__ int array_uint32_append_raw(ARRAY_uint32* array)
        { return (buffer_append_raw(&array->buf, sizeof(uint32_t)) != NULL ? 0 : -1); }
BUFFER_INLINE__ int array_int64_append_raw(ARRAY_int64* array)
        { return (buffer_append_raw(&array->buf, sizeof(int64_t)) != NULL ? 0 : -1); }
BUFFER_INLINE__ int array_uint64_append_raw(ARRAY_uint64* array)
        { return (buffer_append_raw(&array->buf, sizeof(uint64_t)) != NULL ? 0 : -1); }
BUFFER_INLINE__ int array_ptr_append_raw(ARRAY_ptr* array)
        { return (buffer_append_raw(&array->buf, sizeof(void*)) != NULL ? 0 : -1); }

BUFFER_INLINE__ int array_int8_append(ARRAY_int8* array, int8_t i8)
        {
            int8_t* addr;
            addr = (int8_t*) buffer_append_raw(&array->buf, sizeof(int8_t));
            if(addr != NULL)
                *addr = i8;
            return (addr != NULL) ? 0 : -1;
        }
BUFFER_INLINE__ int array_uint8_append(ARRAY_uint8* array, uint8_t u8)
        {
            uint8_t* addr;
            addr = (uint8_t*) buffer_append_raw(&array->buf, sizeof(uint8_t));
            if(addr != NULL)
                *addr = u8;
            return (addr != NULL) ? 0 : -1;
        }
BUFFER_INLINE__ int array_int16_append(ARRAY_int16* array, int16_t i16)
        {
            int16_t* addr;
            addr = (int16_t*) buffer_append_raw(&array->buf, sizeof(int16_t));
            if(addr != NULL)
                *addr = i16;
            return (addr != NULL) ? 0 : -1;
        }
BUFFER_INLINE__ int array_uint16_append(ARRAY_uint16* array, uint16_t u16)
        {
            uint16_t* addr;
            addr = (uint16_t*) buffer_append_raw(&array->buf, sizeof(uint16_t));
            if(addr != NULL)
                *addr = u16;
            return (addr != NULL) ? 0 : -1;
        }
BUFFER_INLINE__ int array_int32_append(ARRAY_int32* array, int32_t i32)
        {
            int32_t* addr;
            addr = (int32_t*) buffer_append_raw(&array->buf, sizeof(int32_t));
            if(addr != NULL)
                *addr = i32;
            return (addr != NULL) ? 0 : -1;
        }
BUFFER_INLINE__ int array_uint32_append(ARRAY_uint32* array, uint32_t u32)
        {
            uint32_t* addr;
            addr = (uint32_t*) buffer_append_raw(&array->buf, sizeof(uint32_t));
            if(addr != NULL)
                *addr = u32;
            return (addr != NULL) ? 0 : -1;
        }
BUFFER_INLINE__ int array_int64_append(ARRAY_int64* array, int64_t i64)
        {
            int64_t* addr;
            addr = (int64_t*) buffer_append_raw(&array->buf, sizeof(int64_t));
            if(addr != NULL)
                *addr = i64;
            return (addr != NULL) ? 0 : -1;
        }
BUFFER_INLINE__ int array_uint64_append(ARRAY_uint64* array, uint64_t u64)
        {
            uint64_t* addr;
            addr = (uint64_t*) buffer_append_raw(&array->buf, sizeof(uint64_t));
            if(addr != NULL)
                *addr = u64;
            return (addr != NULL) ? 0 : -1;
        }
BUFFER_INLINE__ int array_ptr_append(ARRAY_ptr* array, void* ptr)
        {
            void** addr;
            addr = (void**) buffer_append_raw(&array->buf, sizeof(void*));
            if(addr != NULL)
                *addr = ptr;
            return (addr != NULL) ? 0 : -1;
        }

BUFFER_INLINE__ int array_int8_append_n(ARRAY_int8* array, size_t n)
        { return (buffer_append_raw(&array->buf, n * sizeof(int8_t)) != NULL ? 0 : -1); }
BUFFER_INLINE__ int array_uint8_append_n(ARRAY_uint8* array, size_t n)
        { return (buffer_append_raw(&array->buf, n * sizeof(uint8_t)) != NULL ? 0 : -1); }
BUFFER_INLINE__ int array_int16_append_n(ARRAY_int16* array, size_t n)
        { return (buffer_append_raw(&array->buf, n * sizeof(int16_t)) != NULL ? 0 : -1); }
BUFFER_INLINE__ int array_uint16_append_n(ARRAY_uint16* array, size_t n)
        { return (buffer_append_raw(&array->buf, n * sizeof(uint16_t)) != NULL ? 0 : -1); }
BUFFER_INLINE__ int array_int32_append_n(ARRAY_int32* array, size_t n)
        { return (buffer_append_raw(&array->buf, n * sizeof(int32_t)) != NULL ? 0 : -1); }
BUFFER_INLINE__ int array_uint32_append_n(ARRAY_uint32* array, size_t n)
        { return (buffer_append_raw(&array->buf, n * sizeof(uint32_t)) != NULL ? 0 : -1); }
BUFFER_INLINE__ int array_int64_append_n(ARRAY_int64* array, size_t n)
        { return (buffer_append_raw(&array->buf, n * sizeof(int64_t)) != NULL ? 0 : -1); }
BUFFER_INLINE__ int array_uint64_append_n(ARRAY_uint64* array, size_t n)
        { return (buffer_append_raw(&array->buf, n * sizeof(uint64_t)) != NULL ? 0 : -1); }
BUFFER_INLINE__ int array_ptr_append_n(ARRAY_ptr* array, size_t n)
        { return (buffer_append_raw(&array->buf, n * sizeof(void*)) != NULL ? 0 : -1); }

BUFFER_INLINE__ void array_int8_remove(ARRAY_int8* array, size_t index)
        { buffer_remove(&array->buf, index * sizeof(int8_t), sizeof(int8_t)); }
BUFFER_INLINE__ void array_uint8_remove(ARRAY_uint8* array, size_t index)
        { buffer_remove(&array->buf, index * sizeof(uint8_t), sizeof(uint8_t)); }
BUFFER_INLINE__ void array_int16_remove(ARRAY_int16* array, size_t index)
        { buffer_remove(&array->buf, index * sizeof(int16_t), sizeof(int16_t)); }
BUFFER_INLINE__ void array_uint16_remove(ARRAY_uint16* array, size_t index)
        { buffer_remove(&array->buf, index * sizeof(uint16_t), sizeof(uint16_t)); }
BUFFER_INLINE__ void array_int32_remove(ARRAY_int32* array, size_t index)
        { buffer_remove(&array->buf, index * sizeof(int32_t), sizeof(int32_t)); }
BUFFER_INLINE__ void array_uint32_remove(ARRAY_uint32* array, size_t index)
        { buffer_remove(&array->buf, index * sizeof(uint32_t), sizeof(uint32_t)); }
BUFFER_INLINE__ void array_int64_remove(ARRAY_int64* array, size_t index)
        { buffer_remove(&array->buf, index * sizeof(int64_t), sizeof(int64_t)); }
BUFFER_INLINE__ void array_uint64_remove(ARRAY_uint64* array, size_t index)
        { buffer_remove(&array->buf, index * sizeof(uint64_t), sizeof(uint64_t)); }
BUFFER_INLINE__ void array_ptr_remove(ARRAY_ptr* array, size_t index)
        { buffer_remove(&array->buf, index * sizeof(void*), sizeof(void*)); }

BUFFER_INLINE__ void array_int8_remove_n(ARRAY_int8* array, size_t index, size_t n)
        { buffer_remove(&array->buf, index * sizeof(int8_t), n * sizeof(int8_t)); }
BUFFER_INLINE__ void array_uint8_remove_n(ARRAY_uint8* array, size_t index, size_t n)
        { buffer_remove(&array->buf, index * sizeof(uint8_t), n * sizeof(uint8_t)); }
BUFFER_INLINE__ void array_int16_remove_n(ARRAY_int16* array, size_t index, size_t n)
        { buffer_remove(&array->buf, index * sizeof(int16_t), n * sizeof(int16_t)); }
BUFFER_INLINE__ void array_uint16_remove_n(ARRAY_uint16* array, size_t index, size_t n)
        { buffer_remove(&array->buf, index * sizeof(uint16_t), n * sizeof(uint16_t)); }
BUFFER_INLINE__ void array_int32_remove_n(ARRAY_int32* array, size_t index, size_t n)
        { buffer_remove(&array->buf, index * sizeof(int32_t), n * sizeof(int32_t)); }
BUFFER_INLINE__ void array_uint32_remove_n(ARRAY_uint32* array, size_t index, size_t n)
        { buffer_remove(&array->buf, index * sizeof(uint32_t), n * sizeof(uint32_t)); }
BUFFER_INLINE__ void array_int64_remove_n(ARRAY_int64* array, size_t index, size_t n)
        { buffer_remove(&array->buf, index * sizeof(int64_t), n * sizeof(int64_t)); }
BUFFER_INLINE__ void array_uint64_remove_n(ARRAY_uint64* array, size_t index, size_t n)
        { buffer_remove(&array->buf, index * sizeof(uint64_t), n * sizeof(uint64_t)); }
BUFFER_INLINE__ void array_ptr_remove_n(ARRAY_ptr* array, size_t index, size_t n)
        { buffer_remove(&array->buf, index * sizeof(void*), n * sizeof(void*)); }

BUFFER_INLINE__ int8_t* array_int8_acquire(ARRAY_int8* array, size_t* p_size)
        { void* data; data = buffer_acquire(&array->buf, p_size);
          if(p_size != NULL) *p_size /= sizeof(int8_t);
          return (int8_t*) data; }
BUFFER_INLINE__ uint8_t* array_uint8_acquire(ARRAY_uint8* array, size_t* p_size)
        { void* data; data = buffer_acquire(&array->buf, p_size);
          if(p_size != NULL) *p_size /= sizeof(uint8_t);
          return (uint8_t*) data; }
BUFFER_INLINE__ int16_t* array_int16_acquire(ARRAY_int16* array, size_t* p_size)
        { void* data; data = buffer_acquire(&array->buf, p_size);
          if(p_size != NULL) *p_size /= sizeof(int16_t);
          return (int16_t*) data; }
BUFFER_INLINE__ uint16_t* array_uint16_acquire(ARRAY_uint16* array, size_t* p_size)
        { void* data; data = buffer_acquire(&array->buf, p_size);
          if(p_size != NULL) *p_size /= sizeof(uint16_t);
          return (uint16_t*) data; }
BUFFER_INLINE__ int32_t* array_int32_acquire(ARRAY_int32* array, size_t* p_size)
        { void* data; data = buffer_acquire(&array->buf, p_size);
          if(p_size != NULL) *p_size /= sizeof(int32_t);
          return (int32_t*) data; }
BUFFER_INLINE__ uint32_t* array_uint32_acquire(ARRAY_uint32* array, size_t* p_size)
        { void* data; data = buffer_acquire(&array->buf, p_size);
          if(p_size != NULL) *p_size /= sizeof(uint32_t);
          return (uint32_t*) data; }
BUFFER_INLINE__ int64_t* array_int64_acquire(ARRAY_int64* array, size_t* p_size)
        { void* data; data = buffer_acquire(&array->buf, p_size);
          if(p_size != NULL) *p_size /= sizeof(int64_t);
          return (int64_t*) data; }
BUFFER_INLINE__ uint64_t* array_uint64_acquire(ARRAY_uint64* array, size_t* p_size)
        { void* data; data = buffer_acquire(&array->buf, p_size);
          if(p_size != NULL) *p_size /= sizeof(uint64_t);
          return (uint64_t*) data; }
BUFFER_INLINE__ void** array_ptr_acquire(ARRAY_ptr* array, size_t* p_size)
        { void* data; data = buffer_acquire(&array->buf, p_size);
          if(p_size != NULL) *p_size /= sizeof(void*);
          return (void**) data; }


#ifdef __cplusplus
}  /* extern "C" { */
#endif

#endif  /* CRE_BUFFER_H */
