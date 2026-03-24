#include "vulkan_swapchain.h"
#include "vulkan_device.h"
#include "vulkan_image.h"
#include "core/kmemory.h"
#include "core/logger.h"

void create(vulkan_context* context, u32 width, u32 height, vulkan_swapchain* swapchain)
{
    VkExtent2D swapchain_extent = {width, height};
    swapchain->max_frames_in_flight = 2;//triple buffering

    b8 found = FALSE;
    for(int i=0;i<context->device.swapchain_support.format_count;i++)
    {
        VkSurfaceFormatKHR format = context->device.swapchain_support.surface_formats[i];
        if(format.format == VK_FORMAT_B8G8R8A8_UNORM && 
        format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
        {
            swapchain->image_format = format;
            found = TRUE;
            break;
        }
    }
    //if not found take the first format
    if(!found)
    {
        swapchain->image_format = context->device.swapchain_support.surface_formats[0];
    }

    //present mode
    VkPresentModeKHR present_mode = VK_PRESENT_MODE_FIFO_KHR;
    for(int i=0;i<context->device.swapchain_support.present_mode_count;i++)
    {
        VkPresentModeKHR mode = context->device.swapchain_support.present_modes[i];
        if(mode == VK_PRESENT_MODE_MAILBOX_KHR)
        {
            present_mode = mode;
            break;
        }
    }

    //requery swapchain support for extent
    vulkan_device_query_swapchain_support(context->device.physical_device, context->vulkan_surface, &context->device.swapchain_support);

    if(context->device.swapchain_support.surface_caps.currentExtent.width != 0xFFFFFFFF )
    {
        swapchain_extent = context->device.swapchain_support.surface_caps.currentExtent;
    }

    //make sure the extent is within gpu limits

    VkExtent2D min_extent = context->device.swapchain_support.surface_caps.minImageExtent;
    VkExtent2D max_extent = context->device.swapchain_support.surface_caps.maxImageExtent;

    swapchain_extent.width = KCLAMP(min_extent.width, max_extent.width, swapchain_extent.width);
    swapchain_extent.height = KCLAMP(min_extent.height, max_extent.height, swapchain_extent.height);

    u32 image_count = context->device.swapchain_support.surface_caps.minImageCount + 1;
    //make sure that image count dosent exceed device limits
    if(context->device.swapchain_support.surface_caps.maxImageCount > 0 && image_count > context->device.swapchain_support.surface_caps.maxImageCount)
    {
        image_count = context->device.swapchain_support.surface_caps.maxImageCount;
    }

    VkSwapchainCreateInfoKHR swapchain_ci = {};
    swapchain_ci.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    swapchain_ci.surface = context->vulkan_surface;
    swapchain_ci.imageFormat = swapchain->image_format.format;
    swapchain_ci.imageColorSpace = swapchain->image_format.colorSpace;
    swapchain_ci.imageExtent = swapchain_extent;
    swapchain_ci.imageArrayLayers = 1;
    swapchain_ci.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    swapchain_ci.minImageCount = image_count;
    swapchain_ci.presentMode = present_mode;
    if(context->device.graphics_family_index != context->device.present_family_index)
    {
        u32 queue_family_indices[] = {context->device.graphics_family_index , context->device.present_family_index};
        swapchain_ci.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        swapchain_ci.queueFamilyIndexCount = 2;
        swapchain_ci.pQueueFamilyIndices = queue_family_indices;
    }
    else{
        swapchain_ci.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        swapchain_ci.queueFamilyIndexCount = 0;
        swapchain_ci.pQueueFamilyIndices = 0;
    }
    
    swapchain_ci.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    swapchain_ci.clipped = VK_TRUE;
    swapchain_ci.preTransform = context->device.swapchain_support.surface_caps.currentTransform;
    swapchain_ci.oldSwapchain = 0;
    swapchain_ci.pNext =0; 
    VK_CHECK(vkCreateSwapchainKHR(context->device.logical_device,&swapchain_ci,context->allocator,&swapchain->swapchain_handle));

    context->current_frame = 0;

    swapchain->image_count = 0;
    VK_CHECK(vkGetSwapchainImagesKHR(context->device.logical_device, swapchain->swapchain_handle, &swapchain->image_count, 0));
    if(!swapchain->images)
    {
        swapchain->images = (VkImage*)kallocate(sizeof(VkImage)*swapchain->image_count,MEMORY_TAG_RENDERER);
    }
    if(!swapchain->image_views)
    {
        swapchain->image_views = (VkImageView*)kallocate(sizeof(VkImageView)*swapchain->image_count, MEMORY_TAG_RENDERER);
    }

    VK_CHECK(vkGetSwapchainImagesKHR(context->device.logical_device, swapchain->swapchain_handle, &swapchain->image_count, swapchain->images));

    //views
    for(int i=0;i<swapchain->image_count;i++)
    {
        VkImageViewCreateInfo image_view_ci = {};
        image_view_ci.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        image_view_ci.pNext = 0;
        image_view_ci.flags = 0;
        image_view_ci.image = swapchain->images[i];
        image_view_ci.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        image_view_ci.subresourceRange.baseArrayLayer =0;
        image_view_ci.subresourceRange.baseMipLevel = 0;
        image_view_ci.subresourceRange.layerCount =1;
        image_view_ci.subresourceRange.levelCount = 1;
        image_view_ci.viewType = VK_IMAGE_VIEW_TYPE_2D;
        image_view_ci.format = swapchain->image_format.format;
       VK_CHECK(vkCreateImageView(context->device.logical_device,&image_view_ci, context->allocator, &swapchain->image_views[i]));
    }

    //depth resources
    if(vulkan_device_check_format_support(&context->device, DEFAULT_DEPTH_FORMAT))
    {
        swapchain->depth_attachment_format = DEFAULT_DEPTH_FORMAT;
    }
    //if the default format is not present then choose any on eof the following
    else
    {
        // Format candidates
        const u64 candidate_count = 3;
        VkFormat candidates[3] = {
            VK_FORMAT_D32_SFLOAT,
            VK_FORMAT_D32_SFLOAT_S8_UINT,
            VK_FORMAT_D24_UNORM_S8_UINT};
        for (u64 i = 0; i < candidate_count; ++i) 
        {
            if(vulkan_device_check_format_support(&context->device,candidates[i]))
            {
               swapchain->depth_attachment_format = candidates[i];
            }
        }
    }

    vulkan_image_create(
        context,
        VK_IMAGE_TYPE_2D,
        swapchain_extent.width,
        swapchain_extent.height,
        swapchain->depth_attachment_format,
        VK_IMAGE_TILING_OPTIMAL,
        VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
        TRUE,
        VK_IMAGE_ASPECT_DEPTH_BIT,
        &swapchain->depth_attachment
    );

    KINFO("swapchain created successfully");
}
void destroy(vulkan_context* context, vulkan_swapchain* swapchain)
{
    vulkan_image_destroy(context, &swapchain->depth_attachment);

    for(int i=0 ; i<swapchain->image_count;i++)
    {
        vkDestroyImageView(context->device.logical_device, swapchain->image_views[i], context->allocator);
    }

    vkDestroySwapchainKHR(context->device.logical_device, swapchain->swapchain_handle,context->allocator);
}

void vulkan_swapchain_create(vulkan_context* context, u32 width, u32 height, vulkan_swapchain* swapchain)
{
    create(context,width,height,swapchain);
}
void vulkan_swapchain_recreate(vulkan_context* context, u32 width, u32 height, vulkan_swapchain* swapchain)
{
    destroy(context,swapchain);
    create(context,width,height,swapchain);
}

void vulkan_swapchain_destroy(vulkan_context* context, vulkan_swapchain* swapchain)
{
    destroy(context,swapchain);
}

b8 vulkan_swapchain_aquire_next_image_index(vulkan_context* context, vulkan_swapchain* swapchain, u64 timeout_ns, VkSemaphore image_available_semaphore, VkFence fence, u32* out_image_index)
{

    VkResult res = vkAcquireNextImageKHR(context->device.logical_device, swapchain->swapchain_handle, timeout_ns, image_available_semaphore, fence, out_image_index);

    if(res == VK_ERROR_OUT_OF_DATE_KHR)
    {
        //recreate swapchain
        vulkan_swapchain_recreate(context,context->framebuffer_width,context->framebuffer_height, swapchain);
        return FALSE;
    }
    else if(res != VK_SUCCESS && res != VK_SUBOPTIMAL_KHR)
    {
        KFATAL("failed to aquire swapchain image!");
        return FALSE;
    }

    return TRUE;
}

void vulkan_swapchain_present(vulkan_context* context, vulkan_swapchain* swapchain, VkQueue graphics_queue, VkQueue present_queue, VkSemaphore render_complete_semaphore, u32 present_image_index)
{

    // Return the image to the swapchain for presentation.
    VkPresentInfoKHR present_info = {VK_STRUCTURE_TYPE_PRESENT_INFO_KHR};
    present_info.waitSemaphoreCount = 1;
    present_info.pWaitSemaphores = &render_complete_semaphore;
    present_info.swapchainCount = 1;
    present_info.pSwapchains = &swapchain->swapchain_handle;
    present_info.pImageIndices = &present_image_index;
    present_info.pResults = 0;

    VkResult result = vkQueuePresentKHR(present_queue, &present_info);
    if(result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR)
    {
        //swapchain is out of date, suboptimal or a framebuffer resize has occurred. Trigger swapchain recreation.
        vulkan_swapchain_recreate(context, context->framebuffer_width, context->framebuffer_height, swapchain);
    } else if (result != VK_SUCCESS) {
        KFATAL("Failed to present swap chain image!");
    }
}