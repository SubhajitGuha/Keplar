#pragma once
#include "defines.h"

typedef enum memory_tag{
    MEMORY_TAG_UNDEFINED,
    MEMORY_TAG_ARRAY,
    MEMORY_TAG_DARRAY,
    MEMORY_TAG_DICT,
    MEMORY_TAG_RING_QUEUE,
    MEMORY_TAG_BST,
    MEMORY_TAG_STRING,
    MEMORY_TAG_APPLICATION,
    MEMORY_TAG_JOB,
    MEMORY_TAG_TEXTURE,
    MEMORY_TAG_MATERIAL_INSTANCE,
    MEMORY_TAG_RENDERER,
    MEMORY_TAG_GAME,
    MEMORY_TAG_TRANSFORM,
    MEMORY_TAG_ENTITY,
    MEMORY_TAG_ENTITY_NODE,
    MEMORY_TAG_SCENE,

    MEMORY_TAG_MAX_ENUM
} memory_tag;

KAPI void initilize_memory();

KAPI void shutdown_memory();

KAPI void* kallocate(u64 size, memory_tag tag);

KAPI void kfree(void* block, u64 size, memory_tag tag);

KAPI void* kset_memory(void* dst, u64 size, i32 value);

KAPI void* kzero_memory(void*  dst, u64 size);

KAPI void* kcopy_memory(void* dst,const void* src, u64 size);

KAPI char* get_memory_usage_str();