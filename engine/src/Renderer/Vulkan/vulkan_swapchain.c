#include "vulkan_swapchain.h"
#include "vulkan_device.h"
void create(vulkan_context* context, u32 width, u32 height, vulkan_swapchain* swapchain)
{
    // vulkan_swapchain_support_info support_info = {};
    // vulkan_device_query_swapchain_support(context->device.physical_device, context->vulkan_surface,&support_info);
    // support_info.
    // VkSwapchainCreateInfoKHR swapchain_ci = {};
    // swapchain_ci.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    // swapchain_ci.
    // vkCreateSwapchainKHR(context->device.logical_device,&swapchain_ci,context->allocator,&swapchain->swapchain_handle);
}
void destroy(vulkan_context* context, vulkan_swapchain* swapchain)
{

}

void vulkan_swapchain_create(vulkan_context* context, u32 width, u32 height, vulkan_swapchain* swapchain)
{
    
}
void vulkan_swapchain_recreate(vulkan_context* context, u32 width, u32 height, vulkan_swapchain* swapchain)
{

}

void vulkan_swapchain_destroy(vulkan_context* context, vulkan_swapchain* swapchain)
{

}

void vulkan_swapchain_aquire_next_image_index(vulkan_context* context, vulkan_swapchain* swapchain, u64 timeout_ns, VkSemaphore image_available_semaphore, VkFence fence, u32* out_image_index)
{

}

void vulkan_swapchain_present(vulkan_context* context, vulkan_swapchain* swapchain, VkQueue graphics_queue, VkQueue present_queue, VkSemaphore render_compleat_semaphore, u32 present_image_index)
{

}