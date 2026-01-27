#pragma once

#include "defines.h"

/**
 * darray is dynamically allocated array like cpp vector.
 * parameters: length, capacity, stride
 */
#define DEFAULT_MAX_SIZE 1
#define RESIZE_FACTOR 2

enum{
    DARRAY_CAPACITY,
    DARRAY_LENGTH,
    DARRAY_STRIDE,

    DARRAY_MAX_ENUM
};

 KAPI void* _darray_create(u64 length, u64 stride);

 KAPI void _darray_destroy(void* array);

KAPI void _darray_set_field(void* array, u64 field, u64 value);

KAPI u64 _darray_get_field(void* array, u64 field);

void* _darray_resize(void* array);

KAPI void* _darray_push(void* array, const void* value_ptr);

KAPI void* _darray_pop(void* array, void* dst);

KAPI void* _darray_insert_at(void* array, u64 index, const void* value_ptr);

KAPI void* _darray_pop_at(void* array, u64 index, void* dst);

#define darray_create(type)\
_darray_create(DEFAULT_MAX_SIZE,sizeof(type))

#define darray_reserve(type, capacity)\
_darray_create(capacity, sizeof(type))

#define darray_free(array_ptr) _darray_destroy(array_ptr)

#define darray_clear(array_ptr)\
_darray_set_field(array_ptr, DARRAY_LENGTH, 0)

#define darray_length(array_ptr)\
_darray_get_field(array_ptr, DARRAY_LENGTH)

#define darray_stride(array_ptr)\
_darray_get_field(array_ptr, DARRAY_STRIDE)

#define darray_capacity(array_ptr)\
_darray_get_field(array_ptr, DARRAY_CAPACITY)

#define darray_push(array_ptr, value)\
{                                          \
    typeof(value) _darray_temp = value;     \
    array_ptr = _darray_push(array_ptr, &_darray_temp); \
}

#define darray_pop(array_ptr, dst_ptr)\
_darray_pop(array_ptr, dst_ptr)

#define darray_push_at(array_ptr, index, value)\
{                                               \
    typeof(value) _darray_temp = value;         \
    array_ptr = _darray_insert_at(array_ptr, index, &_darray_temp);\
}

#define darray_pop_at(array_ptr, index, dst_ptr)\
_darray_pop_at(array_ptr, index, dst_ptr)