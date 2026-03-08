#pragma once

#include "Renderer/renderer_backend.h"

b8 vulkan_renderer_initilize(renderer_backend* _backend, const char* app_name);
void vulkan_renderer_shutdown(renderer_backend* _backend);
b8 vulkan_renderer_begin_frame(renderer_backend* _backend, f32 delta_time);
b8 vulkan_renderer_end_frame(renderer_backend* _backend, f32 delta_time);
void vulkan_renderer_resize(renderer_backend* _backend, int width, int height);