#pragma once
#include "defines.h"
#include "vulkan/vulkan.h"
#include "core/kassert.h"

#define DEFAULT_DEPTH_FORMAT VK_FORMAT_D24_UNORM_S8_UINT

typedef enum vulkan_command_buffer_state
{
    COMMAND_BUFFER_STATE_READY,
    COMMAND_BUFFER_STATE_RECORDING,
    COMMAND_BUFFER_STATE_IN_RENDER_PASS,
    COMMAND_BUFFER_STATE_RECORDING_ENDED,
    COMMAND_BUFFER_STATE_SUBMITTED,
    COMMAND_BUFFER_STATE_NOT_ALLOCATED
}vulkan_command_buffer_state;
typedef struct vulkan_command_buffer
{
    VkCommandBuffer handle;
    vulkan_command_buffer_state state;
}vulkan_command_buffer;

typedef struct vulkan_swapchain_support_info
{
    VkSurfaceCapabilitiesKHR surface_caps;
    u32 format_count;
    VkSurfaceFormatKHR* surface_formats;
    u32 present_mode_count;
    VkPresentModeKHR* present_modes;
}vulkan_swapchain_support_info;

typedef struct vulkan_image
{
    VkImage image_handle;
    VkImageView image_view;
    VkDeviceMemory memory;
    u32 width;
    u32 height;
} vulkan_image;

typedef struct vulkan_swapchain
{
    VkSwapchainKHR swapchain_handle;
    VkSurfaceFormatKHR image_format;
    VkFormat depth_attachment_format;
    u8 max_frames_in_flight;
    u32 image_count;
    VkImage* images;
    VkImageView* image_views;

    vulkan_image depth_attachment;
}vulkan_swapchain;


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
    
    VkCommandPool graphics_command_pool;

    VkPhysicalDeviceProperties properties;
    VkPhysicalDeviceFeatures features;
    VkPhysicalDeviceMemoryProperties memory;

    vulkan_swapchain_support_info swapchain_support;
} vulkan_device;

typedef struct vulkan_context
{
    // The framebuffer's current width.
    u32 framebuffer_width;

    // The framebuffer's current height.
    u32 framebuffer_height;

    VkInstance vulkan_instance;
    VkAllocationCallbacks* allocator;
    vulkan_device device;
    VkSurfaceKHR vulkan_surface;
    vulkan_swapchain swapchain;
    // darray
    vulkan_command_buffer* graphics_command_buffers;
    u32 image_index;
    u32 current_frame;
    b8 recreateating_swapchain;
    #if defined(_DEBUG)
        VkDebugUtilsMessengerEXT debug_utils_messanger;
    #endif

    i32 (* find_memory_index) (u32 type_filter, u32 property_flags);
} vulkan_context;

#define VK_CHECK(val)           \
{                               \
    KASSERT(val == VK_SUCCESS)  \
}