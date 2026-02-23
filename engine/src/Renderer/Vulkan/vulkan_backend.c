#include "vulkan_backend.h"

static vulkan_context context;
b8 vulkan_renderer_initilize(struct renderer_backend* _backend, const char* app_name)
{
    context.vulkan_alloc_callback = 0;

    VkApplicationInfo appInfo = {};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.apiVersion = 100;
    appInfo.applicationVersion = 100;
    appInfo.pApplicationName = app_name;
    appInfo.engineVersion = 100;
    appInfo.pNext = 0;

    VkInstanceCreateInfo ci = {};
    ci.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    ci.pApplicationInfo = &appInfo;
    ci.pNext = 0;
    ci.flags = 0;
    VkResult res = vkCreateInstance(&ci,context.vulkan_alloc_callback,&context.vulkan_instance);
    if(res == VK_SUCCESS)
    {
        return TRUE;
    }
    else{
        return FALSE;
    }
}
void vulkan_renderer_shutdown(struct renderer_backend* _backend)
{

}
b8 vulkan_renderer_begin_frame(struct renderer_backend* _backend, f32 delta_time)
{
    return TRUE;
}
b8 vulkan_renderer_end_frame(struct renderer_backend* _backend, f32 delta_time)
{
    return TRUE;
}
void vulkan_renderer_resize(struct renderer_backend* _backend, int width, int height)
{

}