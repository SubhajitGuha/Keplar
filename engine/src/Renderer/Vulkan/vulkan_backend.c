#include "vulkan_backend.h"
#include "containers/darray.h"
#include "containers/kstring.h"
#include "core/logger.h"
#include "platform/platform.h"
#include "vulkan_platform.h"
#include "vulkan_device.h"
#include "vulkan_types.inl"

// Provided by VK_EXT_debug_utils
VKAPI_ATTR VkBool32 VKAPI_CALL debug_utils_message_callback(
    VkDebugUtilsMessageSeverityFlagBitsEXT      messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT             messageTypes,
    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
    void*                                       pUserData);
    
static vulkan_context context;
b8 vulkan_renderer_initilize(renderer_backend* _backend, const char* app_name)
{
    context.vulkan_alloc_callback = 0;

    VkApplicationInfo appInfo = {};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.apiVersion = VK_API_VERSION_1_3;
    appInfo.applicationVersion = VK_MAKE_VERSION(1,0,0);
    appInfo.pApplicationName = app_name;
    appInfo.engineVersion = VK_MAKE_VERSION(1,0,0);
    appInfo.pNext = 0;

    VkInstanceCreateInfo ci = {};
    ci.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    ci.pApplicationInfo = &appInfo;
    ci.pNext = 0;
    ci.flags = 0;

    const char** required_extensions = darray_create(const char*);
    darray_push(required_extensions, &VK_KHR_SURFACE_EXTENSION_NAME);
    platform_get_required_extension_name(&required_extensions); //platform specific extension

    #if defined(_DEBUG)
    darray_push(required_extensions, &VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    KDEBUG("Required Extensions:");
    u32 length = darray_length(required_extensions);
    for (u32 i = 0; i < length; ++i) {
        KDEBUG(required_extensions[i]);
    }
    #endif

    ci.enabledExtensionCount = darray_length(required_extensions);
    ci.ppEnabledExtensionNames = required_extensions;

    const char** required_validation_layer_names = 0;
    u32 required_validation_layer_count = 0;

    #if defined(_DEBUG)
    required_validation_layer_names = darray_create(const char*);
    darray_push(required_validation_layer_names, &"VK_LAYER_KHRONOS_validation");
    required_validation_layer_count = darray_length(required_validation_layer_names);

    u32 available_layers = 0;
    VK_CHECK(vkEnumerateInstanceLayerProperties(&available_layers, 0));
    VkLayerProperties* available_layer_names = darray_reserve(VkLayerProperties,available_layers);
    VK_CHECK(vkEnumerateInstanceLayerProperties(&available_layers, available_layer_names));
    //print the info
    KINFO("available layer property names");
    for(int i=0;i<available_layers;i++)
    {
        KINFO(available_layer_names[i].layerName);
        KINFO("^^^ description ^^^ %s\n", available_layer_names[i].description);
    }

    //check if the required layer is present in the available layer list
    for(int i=0;i<required_validation_layer_count;i++)
    {
        KINFO("searching for layer ... %s", required_validation_layer_names[i]);
        b8 found = FALSE;
        for(int j=0;j<available_layers;j++)
        {
            if(string_compare(required_validation_layer_names[i], available_layer_names[j].layerName))
            {
                KINFO("Layer found");

                found = TRUE;
                break;
            }
        }
        if(!found)
        {
            KFATAL("layer not found...");
            return FALSE;
        }
    }
    KINFO("all layers are present.");
    #endif

    ci.enabledLayerCount = required_validation_layer_count;
    ci.ppEnabledLayerNames = required_validation_layer_names;
    VK_CHECK(vkCreateInstance(&ci,context.vulkan_alloc_callback,&context.vulkan_instance));
    KINFO("Vulkan Instance created");

    //debugger
    #if defined(_DEBUG)
    KDEBUG("creating vulkan debugger");
    u32 log_severity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT |
     VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
     VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT |
     VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT;

    VkDebugUtilsMessengerCreateInfoEXT debug_create_info = {};
    debug_create_info.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    debug_create_info.messageSeverity = log_severity;
    debug_create_info.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    debug_create_info.pfnUserCallback = debug_utils_message_callback;

    PFN_vkCreateDebugUtilsMessengerEXT func= (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(context.vulkan_instance, "vkCreateDebugUtilsMessengerEXT");
    KASSERT_MSG(func, "failed to create debug messanger");
    VK_CHECK(func(context.vulkan_instance,&debug_create_info,context.vulkan_alloc_callback, &context.debug_utils_messanger));
    KDEBUG("Vulkan debugger created");
    #endif

    b8 surface_creation_result = platform_create_vulkan_surface(_backend->plat_state,&context);
    if(!surface_creation_result)
    {
        KERROR("shutting down renderer");
        return FALSE;
    }
    b8 device_create_result = vulkan_device_create(&context);
    if(!device_create_result)
    {
        KERROR("Vulkan device creation failed, shutting down renderer");
        return FALSE;
    }

    KINFO("vulkan renderer Initilized successfully");
    return TRUE;
}
void vulkan_renderer_shutdown(renderer_backend* _backend)
{
    vulkan_device_destroy(&context);
}
b8 vulkan_renderer_begin_frame(renderer_backend* _backend, f32 delta_time)
{
    return TRUE;
}
b8 vulkan_renderer_end_frame(renderer_backend* _backend, f32 delta_time)
{
    return TRUE;
}
void vulkan_renderer_resize(renderer_backend* _backend, int width, int height)
{

}

VkBool32 debug_utils_message_callback(
    VkDebugUtilsMessageSeverityFlagBitsEXT      messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT             messageTypes,
    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
    void*                                       pUserData)
    {
        switch(messageSeverity)
        {
            default:
            case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
                KERROR(pCallbackData->pMessage);
                break;
            case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
                KWARN(pCallbackData->pMessage);
                break;
            case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
                KINFO(pCallbackData->pMessage);
                break;
            case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
                KTRACE(pCallbackData->pMessage);
                break;
        }
        return VK_FALSE;
    }