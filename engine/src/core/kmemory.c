#include "kmemory.h"
#include "platform/platform.h"
#include "logger.h"
#include <string.h>
#include <stdio.h>

//debuging and tracking memory usage in the application
typedef struct memory_stats
{
    float total_allocated;
    float tagged_allocations[MEMORY_TAG_MAX_ENUM];
}memory_stats;

static memory_stats _stats;
static const char* tagged_allocation_names[MEMORY_TAG_MAX_ENUM] = 
{
    "MEMORY_TAG_UNDEFINED          ",
    "MEMORY_TAG_ARRAY              ",
    "MEMORY_TAG_DARRAY             ",
    "MEMORY_TAG_DICT               ",
    "MEMORY_TAG_RING_QUEUE         ",
    "MEMORY_TAG_BST                ",
    "MEMORY_TAG_STRING             ",
    "MEMORY_TAG_APPLICATION        ",
    "MEMORY_TAG_JOB                ",
    "MEMORY_TAG_TEXTURE            ",
    "MEMORY_TAG_MATERIAL_INSTANCE  ",
    "MEMORY_TAG_RENDERER           ",
    "MEMORY_TAG_GAME               ",
    "MEMORY_TAG_TRANSFORM          ",
    "MEMORY_TAG_ENTITY             ",
    "MEMORY_TAG_ENTITY_NODE        ",
    "MEMORY_TAG_SCENE              ",
};
KAPI void initilize_memory()
{
    platform_zero_memory(&_stats, sizeof(_stats));
}

KAPI void shutdown_memory()
{

}

KAPI void* kallocate(u64 size, memory_tag tag)
{

    if(tag == MEMORY_TAG_UNDEFINED)
    {
        KWARN("kallocate called with MEMORY_TAG_UNDEFINED, better tag this allocation");
    }
    void* ptr = platform_allocate(size, FALSE); //will handle allocation error in platform level

    
    _stats.total_allocated += size;
    _stats.tagged_allocations[tag]+=size;
    platform_zero_memory(ptr,size);
    return ptr;
}

KAPI void kfree(void* block, u64 size, memory_tag tag)
{
     if (tag == MEMORY_TAG_UNDEFINED) {
        KWARN("kfree called using MEMORY_TAG_UNDEFINED. Re-class this allocation.");
    }

    _stats.total_allocated -= size;
    _stats.tagged_allocations[tag] -= size;

    // TODO: Memory alignment
    platform_free(block, FALSE);
}

KAPI void* kset_memory(void* dst, u64 size, i32 value)
{
    return platform_set_memory(dst,value,size);
}

KAPI void* kzero_memory(void*  dst, u64 size)
{
    return platform_zero_memory(dst,size);
}

KAPI void* kcopy_memory(void* dst,const void* src, u64 size)
{
    return platform_copy_memory(dst,src,size);
}

//temporary solution
KAPI char* get_memory_usage_str()
{
    const u64 kb = 1024;
    const u64 mb = 1024 * 1024;
    const u64 gb = 1024 * 1024 * 1024;

    char buffer[8000] = "System memory use (tagged):\n";
    u64 offset = strlen(buffer);

    for(int i=0;i<MEMORY_TAG_MAX_ENUM;i++)
    {
        float usage = _stats.tagged_allocations[i];
        char unit[3] = "xb";
        if(usage >= gb)
        {
            unit[0] = 'G';
            usage/=gb;
        }
        else if(usage >= mb)
        {
            unit[0] = 'M';
            usage/=mb;
        }
        else if(usage >= kb)
        {
            unit[0] = 'K';
            usage/=kb;
        }
        else
        {
            unit[0] = 'B';
            unit[1] = 0;
        }

        i32 num_char_written = snprintf(buffer + offset, 8000, "%s: %.2f%s\n", tagged_allocation_names[i],usage,unit);
        offset += num_char_written;
    }
    char* output = _strdup(buffer);
    return output;
}