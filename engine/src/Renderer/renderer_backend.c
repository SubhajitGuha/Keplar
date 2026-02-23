
#include "renderer_backend.h"
#include "Vulkan/vulkan_backend.h"

void renderer_backend_initilize(struct renderer_backend* _backend)
{

    _backend->initilize = vulkan_renderer_initilize;
    _backend->shutdown = vulkan_renderer_shutdown;
    _backend->resize = vulkan_renderer_resize;
    _backend->begin_frame = vulkan_renderer_begin_frame;
    _backend->end_frame = vulkan_renderer_end_frame;
}

void renderer_backend_destroy(struct renderer_backend* _backend)
{
    _backend->initilize = 0;
    _backend->shutdown = 0;
    _backend->resize = 0;
    _backend->begin_frame = 0;
    _backend->end_frame = 0;
}
