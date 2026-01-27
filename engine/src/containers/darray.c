#include "darray.h"
#include "core/kmemory.h"
#include "core/logger.h"

KAPI void* _darray_create(u64 length, u64 stride)
{
    u64 field_size = DARRAY_MAX_ENUM * sizeof(u64);
    u64 array_length = length * stride;
    u64* ptr = kallocate(array_length+field_size,MEMORY_TAG_DARRAY);
    ptr[DARRAY_CAPACITY] = length;
    ptr[DARRAY_LENGTH] = 0;
    ptr[DARRAY_STRIDE] = stride;

    return (void*)(ptr+DARRAY_MAX_ENUM);
}

KAPI void _darray_destroy(void* array)
{
    u64* fields = (u64*)array - DARRAY_MAX_ENUM;
    u64 headder_size = DARRAY_MAX_ENUM * sizeof(u64);
    u64 size = headder_size + fields[DARRAY_CAPACITY] * fields[DARRAY_STRIDE];
    kfree(fields,size,MEMORY_TAG_DARRAY);
}

KAPI void _darray_set_field(void* array, u64 field, u64 value)
{
    u64* fields = (u64*)array - DARRAY_MAX_ENUM;
    fields[field] = value;
}

KAPI u64 _darray_get_field(void* array, u64 field)
{
    u64* fields = (u64*)array - DARRAY_MAX_ENUM;
    return fields[field];
}

void* _darray_resize(void* array)
{
    u64 length = darray_length(array);
    u64 stride = darray_stride(array);
    void* temp = _darray_create(
        (RESIZE_FACTOR * darray_capacity(array)),
        stride);
    kcopy_memory(temp, array, length * stride);

    _darray_set_field(temp, DARRAY_LENGTH, length);
    _darray_destroy(array);
    return temp;
}

KAPI void* _darray_push(void* array, const void* value_ptr)
{
    u64 length = darray_length(array);
    u64 capacity = darray_capacity(array);
    u64 stride = darray_stride(array);
    if(length >= capacity)
    {
        array = _darray_resize(array);
    }
    void* address = array + length * stride;
    kcopy_memory(address,value_ptr, stride);
    _darray_set_field(array,DARRAY_LENGTH, length + 1);
    return array;
}

KAPI void* _darray_pop(void* array, void* dst)
{
    u64 length = darray_length(array);
    u64 stride = darray_stride(array);
    if(length==0)
    {
        KERROR("length of the container is zero, try pushing elements first");
        return array;
    }
    void* address = array + (length-1) * stride;
    kcopy_memory(dst,address,stride);
    _darray_set_field(array, DARRAY_LENGTH, length-1);
    return array;
}

KAPI void* _darray_insert_at(void* array, u64 index, const void* value_ptr)
{
    u64 length = darray_length(array);
    u64 capacity = darray_capacity(array);
    u64 stride = darray_stride(array);
    if(index >= length)
    {
        KERROR("Index out of bounds, index: %lu length: %lu , index must be less than length!", index, length);
        return array;
    }
    if(length >= capacity)
    {
        array = _darray_resize(array);
    }

    void* address = array;

    //if not last element then copy the rest from index outward
    if(index != length-1)
    {
        kcopy_memory(address + (index+1)*stride, address + index * stride, (length-index)*stride);

    }
    kcopy_memory(address + index * stride, value_ptr, stride);
    _darray_set_field(array,DARRAY_LENGTH,length+1);
    return array;
}

KAPI void* _darray_pop_at(void* array, u64 index, void* dst)
{
    u64 length = darray_length(array);
    u64 stride = darray_stride(array);
    if(index >= length)
    {
        KERROR("Index out of bounds, index: %lu length: %lu , index must be less than length!", index, length);
        return array;
    }

    void* address = array;

    kcopy_memory(dst, address + index * stride, stride);

    //if not last element, copy inward
    if(index != length-1)
    {
        kcopy_memory(address + index * stride, address + (index + 1) * stride, (length-index)*stride);
    }
    _darray_set_field(array,DARRAY_LENGTH,length-1);
    return array;
}