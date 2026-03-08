#pragma once
#include "vulkan/vulkan.h"
#include "core/kassert.h"
typedef struct vulkan_context
{
    VkInstance vulkan_instance;
    VkDevice vulkan_device;
    VkAllocationCallbacks* vulkan_alloc_callback;
    VkDebugUtilsMessengerEXT debug_utils_messanger;
} vulkan_context;

#define VK_CHECK(val)           \
{                               \
    KASSERT(val == VK_SUCCESS)  \
}