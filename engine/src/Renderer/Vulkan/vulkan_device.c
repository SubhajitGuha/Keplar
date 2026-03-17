#include "vulkan_device.h"
#include "core/logger.h"
#include "containers/darray.h"
#include "core/kmemory.h"
#include "containers/kstring.h"

typedef struct vulkan_physical_device_requirements
{
    b8 graphics;
    b8 compute;
    b8 transfer;
    b8 present;
    b8 discreate_gpu;
    b8 sampler_anisotropy;
    //darray of all extensions that are needed
    char** device_extension_names;
}vulkan_physical_device_requirements;

typedef struct vulkan_physical_device_queue_family_info
{
    u32 graphics_family_index;
    u32 compute_family_index;
    u32 transfer_family_index;
    u32 present_family_index;
} vulkan_physical_device_queue_family_info;

b8 physical_device_meets_requirements(VkPhysicalDevice device,
    VkSurfaceKHR surface,
    const VkPhysicalDeviceProperties* properties,
    const VkPhysicalDeviceFeatures* features,
    const vulkan_physical_device_requirements* requirements,
    vulkan_physical_device_queue_family_info* out_queue_family_info,
    vulkan_swapchain_support_info* out_swapchain_support
);

// b8
b8 select_physical_device(vulkan_context* context);
b8 vulkan_device_create(vulkan_context* context)
{
    if(!select_physical_device(context))
    {
        return FALSE;
    }
    u32 indices[4];
    u8 index_count = 0;
    indices[index_count++] = context->device.graphics_family_index;
    indices[index_count++] = context->device.compute_family_index;
    indices[index_count++] = context->device.transfer_family_index;
    indices[index_count++] = context->device.present_family_index;

    //choose the unique queue families
    u32 unique_queue_indices = 0;
    u32 queue_indices[index_count];
    kset_memory(&queue_indices,sizeof(u32)*index_count,-100);
    for(int i=0;i<index_count;i++)
    {
        b8 isPresent = FALSE;
        for(int j=0;j<index_count;j++)
        {
            if(indices[i] == queue_indices[j])
            {
                isPresent = TRUE;
                break;
            }
        }
        if(!isPresent )
        queue_indices[unique_queue_indices++] = indices[i];
        
    }
    VkDeviceQueueCreateInfo queue_ci[unique_queue_indices];
    for(int i=0; i<unique_queue_indices; i++)
    {
        queue_ci[i].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queue_ci[i].pNext = 0;
        queue_ci[i].flags = 0;
        queue_ci[i].queueCount = 1;
        queue_ci[i].queueFamilyIndex = queue_indices[i];
        float priority = 1.0f;
        queue_ci[i].pQueuePriorities = &priority;
    }

    // Request device features.
    // TODO: should be config driven
    VkPhysicalDeviceFeatures device_features = {};
    device_features.samplerAnisotropy = VK_TRUE;  // Request anistrophy

    //create a logical device
    VkDeviceCreateInfo deviceCi = {};
    deviceCi.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    deviceCi.pNext = 0;
    deviceCi.queueCreateInfoCount = unique_queue_indices;
    deviceCi.pQueueCreateInfos = queue_ci;
    deviceCi.pEnabledFeatures = &device_features;

    // TODO: should be config driven
    deviceCi.enabledExtensionCount = 1;
    const char* extension_names = VK_KHR_SWAPCHAIN_EXTENSION_NAME;
    deviceCi.ppEnabledExtensionNames = &extension_names;

    // Deprecated and ignored, so pass nothing.
    deviceCi.enabledLayerCount = 0;
    deviceCi.ppEnabledLayerNames = 0;

    VK_CHECK(vkCreateDevice(context->device.physical_device, &deviceCi, context->vulkan_alloc_callback, &context->device.logical_device));
    KINFO(" Vulkan logical device created");
    
    vkGetDeviceQueue(
        context->device.logical_device,
        context->device.graphics_family_index,
        0,
        &context->device.graphicsQueue);

    vkGetDeviceQueue(
        context->device.logical_device,
        context->device.compute_family_index,
        0,
        &context->device.computeQueue);

    vkGetDeviceQueue(
        context->device.logical_device,
        context->device.transfer_family_index,
        0,
        &context->device.transferQueue);

    vkGetDeviceQueue(
        context->device.logical_device,
        context->device.present_family_index,
        0,
        &context->device.presentQueue);
    KINFO("Queue obtained");
    return TRUE;
}

void vulkan_device_destroy(vulkan_context* context)
{
    context->device.graphicsQueue = 0;
    context->device.computeQueue = 0;
    context->device.transferQueue = 0;
    context->device.presentQueue = 0;

    KINFO("destroying vulkan logical device");
    if(context->device.logical_device)
    {
        vkDestroyDevice(context->device.logical_device, context->vulkan_alloc_callback);
    }

    context->device.physical_device = 0;

    if(context->device.swapchain_support.surface_formats)
    {
        kfree(context->device.swapchain_support.surface_formats, sizeof(VkSurfaceFormatKHR)*context->device.swapchain_support.format_count, MEMORY_TAG_RENDERER);
        context->device.swapchain_support.surface_formats = 0;
        context->device.swapchain_support.format_count = 0;
    }

    if(context->device.swapchain_support.present_modes)
    {
        kfree(context->device.swapchain_support.present_modes, sizeof(VkPresentModeKHR)*context->device.swapchain_support.present_mode_count, MEMORY_TAG_RENDERER);
        context->device.swapchain_support.present_modes = 0;
        context->device.swapchain_support.present_mode_count = 0;
    }

    kzero_memory(&context->device.swapchain_support.surface_caps, sizeof(context->device.swapchain_support.surface_caps));

    context->device.graphics_family_index = -1;
    context->device.compute_family_index = -1;
    context->device.transfer_family_index = -1;
    context->device.present_family_index = -1;
}

void vulkan_device_query_swapchain_support(
     VkPhysicalDevice device,
     VkSurfaceKHR surface,
     vulkan_swapchain_support_info* out_support_info)
{
    VK_CHECK(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device,surface,&out_support_info->surface_caps));
    VK_CHECK(vkGetPhysicalDeviceSurfaceFormatsKHR(device,surface,&out_support_info->format_count,0));
    if(out_support_info->format_count!=0)
    {
        //allocate memory for surface formats
        if(!out_support_info->surface_formats)
        {
            //free it if not surface accepted
            out_support_info->surface_formats = kallocate(sizeof(VkSurfaceFormatKHR)*out_support_info->format_count, MEMORY_TAG_RENDERER);
        }
        VK_CHECK(vkGetPhysicalDeviceSurfaceFormatsKHR(device,surface,&out_support_info->format_count,out_support_info->surface_formats));
    }
    VK_CHECK(vkGetPhysicalDeviceSurfacePresentModesKHR(device,surface,&out_support_info->present_mode_count,0));
    if(out_support_info->present_mode_count!=0)
    {
        //allocate memory for present modes
        if(!out_support_info->present_modes)
        {
            //free it if not surface accepted
            out_support_info->present_modes = kallocate(sizeof(VkPresentModeKHR)*out_support_info->present_mode_count, MEMORY_TAG_RENDERER);
        }
        VK_CHECK(vkGetPhysicalDeviceSurfacePresentModesKHR(device,surface,&out_support_info->present_mode_count,0));
    }
}
b8 select_physical_device(vulkan_context* context)
{
    u32 num_physical_devices = 0;
    VK_CHECK(vkEnumeratePhysicalDevices(context->vulkan_instance,&num_physical_devices,0));

    if(num_physical_devices == 0)
    {
        KFATAL("no physical devices found.");
        return FALSE;
    }

    VkPhysicalDevice physical_devices[num_physical_devices];
    VK_CHECK(vkEnumeratePhysicalDevices(context->vulkan_instance,&num_physical_devices, physical_devices));

    for(int i=0;i<num_physical_devices;i++)
    {
        VkPhysicalDeviceProperties device_properties;
        vkGetPhysicalDeviceProperties(physical_devices[i], &device_properties);

        VkPhysicalDeviceFeatures device_features;
        vkGetPhysicalDeviceFeatures(physical_devices[i], &device_features);

        VkPhysicalDeviceMemoryProperties device_memory;
        vkGetPhysicalDeviceMemoryProperties(physical_devices[i], &device_memory);

        vulkan_physical_device_requirements requirements = {};
        requirements.discreate_gpu = TRUE;
        requirements.compute = TRUE;
        requirements.graphics = TRUE;
        requirements.transfer = TRUE;
        requirements.present = TRUE;
        requirements.sampler_anisotropy = TRUE;
        requirements.device_extension_names = darray_create(const char*);
        darray_push(requirements.device_extension_names,&VK_KHR_SWAPCHAIN_EXTENSION_NAME);
        
        vulkan_physical_device_queue_family_info queue_info = {};
        b8 result = physical_device_meets_requirements(
            physical_devices[i],
            context->vulkan_surface,
            &device_properties,
            &device_features,
            &requirements,
            &queue_info,
            &context->device.swapchain_support);
        if(result)
        {
            //print out the device info of the selected device
            KINFO("Selected device: '%s'.", device_properties.deviceName);

            //GPU type
            switch(device_properties.deviceType)
            {
                default:
                case VK_PHYSICAL_DEVICE_TYPE_OTHER:
                    KINFO("GPU type is unknown");
                    break;
                case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU:
                    KINFO("GPU type is Descrete");
                    break;
                case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU:
                    KINFO("GPU type is Integreted");
                    break;
                case VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU:
                    KINFO("GPU type is Virtual");
                    break;
                case VK_PHYSICAL_DEVICE_TYPE_CPU:
                    KINFO("GPU type is CPU");
                    break;
            }

            //Driver version
            KINFO("GPU Driver Version: %d.%d.%d", 
                VK_API_VERSION_MAJOR(device_properties.driverVersion),
                VK_API_VERSION_MINOR(device_properties.driverVersion),
                VK_API_VERSION_PATCH(device_properties.driverVersion));

            //Vulakn api version
            KINFO("Vulkan API Version: %d.%d.%d",
                VK_API_VERSION_MAJOR(device_properties.apiVersion),
                VK_API_VERSION_MINOR(device_properties.apiVersion),
                VK_API_VERSION_PATCH(device_properties.apiVersion));

            //Memory info
            for(int j=0; j<device_memory.memoryHeapCount; ++j)
            {
                f32 memory_size_gb = (((f32)device_memory.memoryHeaps[j].size) / 1024.0f/1024.0f/1024.0f);
                
                if(device_memory.memoryHeaps[j].flags & VK_MEMORY_HEAP_DEVICE_LOCAL_BIT)
                {
                    KINFO("Resident GPU memory: %.2f Gb", memory_size_gb);
                }
                else{
                    KINFO("Shared System Memory: %.2f Gb", memory_size_gb);
                }
            }

            context->device.physical_device = physical_devices[i];
            context->device.graphics_family_index = queue_info.graphics_family_index;
            context->device.compute_family_index = queue_info.compute_family_index;
            context->device.transfer_family_index = queue_info.transfer_family_index;
            context->device.present_family_index = queue_info.present_family_index;

            context->device.features = device_features;
            context->device.properties = device_properties;
            context->device.memory = device_memory;
            break;
        }
    }
    //if no physical device was selected
    if(!context->device.physical_device)
    {
        KERROR("No Physical device were found which meet the requitrement");
        return FALSE;
    }

    KINFO("Physical Device selected.");
    return TRUE;
}

b8 physical_device_meets_requirements(VkPhysicalDevice device,
    VkSurfaceKHR surface,
    const VkPhysicalDeviceProperties* properties,
    const VkPhysicalDeviceFeatures* features,
    const vulkan_physical_device_requirements* requirements,
    vulkan_physical_device_queue_family_info* out_queue_family_info,
    vulkan_swapchain_support_info* out_swapchain_support
)
{
    out_queue_family_info->graphics_family_index = -1;
    out_queue_family_info->compute_family_index = -1;
    out_queue_family_info->present_family_index = -1;
    out_queue_family_info->transfer_family_index = -1;

    if(requirements->discreate_gpu)
    {
        if(properties->deviceType != VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
        {
            return FALSE;
        }
    }


    u32 queue_family_property_count = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device,&queue_family_property_count,0);
    if(queue_family_property_count==0)
        return FALSE;
    VkQueueFamilyProperties queue_family_properties[queue_family_property_count];
    vkGetPhysicalDeviceQueueFamilyProperties(device,&queue_family_property_count,queue_family_properties);

    u8 min_transfer_score = 255;
    for(int i=0;i<queue_family_property_count;i++)
    {
        u8 current_transfer_score = 0;
        if(queue_family_properties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
        {
            out_queue_family_info->graphics_family_index = i;
            ++current_transfer_score;
        }
        if(queue_family_properties[i].queueFlags & VK_QUEUE_COMPUTE_BIT)
        {
            out_queue_family_info->compute_family_index = i;
            ++current_transfer_score;
        }
        if(queue_family_properties[i].queueFlags & VK_QUEUE_TRANSFER_BIT)
        {
            out_queue_family_info->transfer_family_index = i;
            if(current_transfer_score <= min_transfer_score)
            {
                min_transfer_score = current_transfer_score;
                out_queue_family_info->transfer_family_index = i;
            }
        }

        //present queue
        VkBool32 supports_present = VK_FALSE;
        VK_CHECK(vkGetPhysicalDeviceSurfaceSupportKHR(device,i,surface,&supports_present));
        if(supports_present)
        {
            out_queue_family_info->present_family_index = i;
        }
    }

    if(
        (!requirements->graphics || (requirements->graphics && out_queue_family_info->graphics_family_index != -1)) &&
        (!requirements->compute || (requirements->compute && out_queue_family_info->compute_family_index != -1)) && 
        (!requirements->transfer || (requirements->transfer && out_queue_family_info->transfer_family_index != -1)) &&
        (!requirements->present || (requirements->present && out_queue_family_info->present_family_index != -1)))
        
        {
            KINFO("Device meets requirements");
            KTRACE("Graphics Family Index: %i", out_queue_family_info->graphics_family_index);
            KTRACE("Compute Family Index:  %i", out_queue_family_info->compute_family_index);
            KTRACE("Present Family Index:  %i", out_queue_family_info->present_family_index);
            KTRACE("Transfer Family Index: %i", out_queue_family_info->transfer_family_index);
            
            //Query Swapchain support
            vulkan_device_query_swapchain_support(device,surface,out_swapchain_support);
            if(out_swapchain_support->format_count < 1 || out_swapchain_support->present_mode_count < 1)
            {
                //free memory
                if(out_swapchain_support->surface_formats)
                {
                    kfree(out_swapchain_support->surface_formats,sizeof(VkSurfaceFormatKHR)*out_swapchain_support->format_count,MEMORY_TAG_RENDERER);
                }
                if(out_swapchain_support->present_modes)
                {
                    kfree(out_swapchain_support->present_modes,sizeof(VkPresentModeKHR)*out_swapchain_support->present_mode_count, MEMORY_TAG_RENDERER);
                }
                KINFO("required swapchain support not present, skipping device");
                return FALSE;
            }

            //device extensions
            if(requirements->device_extension_names)
            {
                u32 available_extension_count = 0;
                VkExtensionProperties* available_extensions = 0;
                VK_CHECK(vkEnumerateDeviceExtensionProperties(device,0,&available_extension_count,0));
                if(available_extension_count > 0)
                {
                    available_extensions = kallocate(sizeof(VkExtensionProperties)*available_extension_count,MEMORY_TAG_RENDERER);
                    VK_CHECK(vkEnumerateDeviceExtensionProperties(device,0,&available_extension_count,available_extensions));
                    for(int i=0;i<darray_length(requirements->device_extension_names);i++)
                    {
                        b8 isExtPresent = FALSE;
                        for(int j=0;j<available_extension_count;j++)
                        {
                            if(string_compare(requirements->device_extension_names[i],available_extensions[j].extensionName))
                            {
                                isExtPresent = TRUE;
                                break;
                            }

                        }
                        if(!isExtPresent)
                        {
                            KINFO("Extension '%s' is not present in the current device...skipping", requirements->device_extension_names[i]);
                            //make sure to free the available extension allocation
                            kfree(available_extensions, sizeof(VkExtensionProperties)*available_extension_count, MEMORY_TAG_RENDERER);
                            return FALSE;
                        }
                    }
                    kfree(available_extensions, sizeof(VkExtensionProperties)*available_extension_count, MEMORY_TAG_RENDERER);
                }
            }

            //query for sampler anisotropy
            if(requirements->sampler_anisotropy && !features->samplerAnisotropy)
            {
                KINFO("Device does not support samplerAnisotropy, skipping.");
                return FALSE;
            }
            return TRUE;
        }
        return FALSE;
}