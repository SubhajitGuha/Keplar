#pragma once
#include "defines.h"
#include "vulkan/vulkan.h"
#include "core/kassert.h"

typedef struct vulkan_swapchain_support_info
{
    VkSurfaceCapabilitiesKHR surface_caps;
    u32 format_count;
    VkSurfaceFormatKHR* surface_formats;
    u32 present_mode_count;
    VkPresentModeKHR* present_modes;
}vulkan_swapchain_support_info;

typedef struct vulkan_device
{
    VkPhysicalDevice physical_device;
    VkDevice logical_device;
    u32 graphics_family_index;
    u32 compute_family_index;
    u32 transfer_family_index;
    u32 present_family_index;

    VkQueue graphicsQueue;
    VkQueue transferQueue;
    VkQueue computeQueue;
    VkQueue presentQueue;
    
    VkPhysicalDeviceProperties properties;
    VkPhysicalDeviceFeatures features;
    VkPhysicalDeviceMemoryProperties memory;

    vulkan_swapchain_support_info swapchain_support;
} vulkan_device;

typedef struct vulkan_context
{
    VkInstance vulkan_instance;
    VkAllocationCallbacks* vulkan_alloc_callback;
    vulkan_device device;
    VkSurfaceKHR vulkan_surface;
    #if defined(_DEBUG)
        VkDebugUtilsMessengerEXT debug_utils_messanger;
    #endif
} vulkan_context;

#define VK_CHECK(val)           \
{                               \
    KASSERT(val == VK_SUCCESS)  \
}