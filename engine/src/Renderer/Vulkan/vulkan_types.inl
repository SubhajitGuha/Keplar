#pragma once
#include "vulkan/vulkan.h"

typedef struct vulkan_context
{
    VkInstance vulkan_instance;
    VkDevice vulkan_device;
    VkAllocationCallbacks* vulkan_alloc_callback;
} vulkan_context;