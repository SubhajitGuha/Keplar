#include "vulkan_image.h"
#include "core/logger.h"

void vulkan_image_create(
    vulkan_context* context,
    VkImageType image_type,
    u32 width,
    u32 height,
    VkFormat format,
    VkImageTiling tiling,
    VkImageUsageFlags usage_flags,
    VkMemoryPropertyFlags memory_flags,
    b32 create_view,
    VkImageAspectFlags view_aspect_flags,
    vulkan_image* out_image    
)
{

    VkImageCreateInfo image_ci = {};
    image_ci.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    image_ci.pNext = 0;
    image_ci.usage = usage_flags,
    image_ci.extent.width = width;
    image_ci.extent.height = height;
    image_ci.extent.depth = 1;
    image_ci.arrayLayers = 1;
    image_ci.mipLevels = 4;
    image_ci.format = format;
    image_ci.imageType = image_type;
    image_ci.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    image_ci.sharingMode = VK_SHARING_MODE_EXCLUSIVE; // TODO: configurable
    image_ci.samples = VK_SAMPLE_COUNT_1_BIT; //TODO configurable
    image_ci.tiling = tiling;

    //not requires for "image_ci.sharingMode = VK_SHARING_MODE_EXCLUSIVE"
    image_ci.queueFamilyIndexCount = 0;
    image_ci.pQueueFamilyIndices = 0;


    VK_CHECK(vkCreateImage(context->device.logical_device,&image_ci,context->allocator,&out_image->image_handle));

    
    //query memory requirements
    VkMemoryRequirements memory_requirements;
    vkGetImageMemoryRequirements(context->device.logical_device,out_image->image_handle,&memory_requirements);
    
    i32 memory_type = context->find_memory_index(memory_requirements.memoryTypeBits, memory_flags);
    if(memory_type==-1)
    {
        KERROR("Required memory type not found. Image not valid.");
    }
    
    //allocate memory
    VkMemoryAllocateInfo memory_alloc_info = {};
    memory_alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    memory_alloc_info.pNext = 0;
    memory_alloc_info.allocationSize = memory_requirements.size;
    memory_alloc_info.memoryTypeIndex = memory_type;
    VK_CHECK(vkAllocateMemory(context->device.logical_device,&memory_alloc_info,context->allocator,&out_image->memory));

    VK_CHECK(vkBindImageMemory(context->device.logical_device, out_image->image_handle, out_image->memory, 0)); //TODO configurable memory offset

    // Create view
        if (create_view) {
            out_image->image_view = 0;
            vulkan_image_view_create(context, format, out_image, view_aspect_flags);
        }
}

void vulkan_image_view_create(
    vulkan_context* context,
    VkFormat format,
    vulkan_image* image,
    VkImageAspectFlags aspect_flags
)
{

    VkImageViewCreateInfo image_view_ci = {};
    image_view_ci.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    image_view_ci.format = format;
    image_view_ci.flags = 0;
    image_view_ci.image = image->image_handle;
    image_view_ci.components.r = VK_COMPONENT_SWIZZLE_R;
    image_view_ci.components.g = VK_COMPONENT_SWIZZLE_G;
    image_view_ci.components.b = VK_COMPONENT_SWIZZLE_B;
    image_view_ci.components.a = VK_COMPONENT_SWIZZLE_A;
    image_view_ci.viewType = VK_IMAGE_VIEW_TYPE_2D; //configurable
    image_view_ci.subresourceRange.aspectMask = aspect_flags;

    //TODO configurable
    image_view_ci.subresourceRange.layerCount = 1;
    image_view_ci.subresourceRange.levelCount = 1;
    image_view_ci.subresourceRange.baseArrayLayer = 0;
    image_view_ci.subresourceRange.baseMipLevel = 0;

    VK_CHECK(vkCreateImageView(context->device.logical_device,&image_view_ci,context->allocator,&image->image_view));
}

void vulkan_image_destroy(vulkan_context* context, vulkan_image* image)
{
    if(image->image_view)
    {
        vkDestroyImageView(context->device.logical_device,image->image_view,context->allocator);
        image->image_view = 0;
    }

    if(image->memory)
    {
        vkFreeMemory(context->device.logical_device, image->memory, context->allocator);
        image->memory = 0;
    }
    if(image->image_handle)
    {
        vkDestroyImage(context->device.logical_device, image->image_handle, context->allocator);
        image->image_handle = 0;
    }
}