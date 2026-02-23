#pragma once

#include "defines.h"
#include "vulkan_types.inl"

b8 vulkan_renderer_initilize(struct renderer_backend* _backend, const char* app_name);
void vulkan_renderer_shutdown(struct renderer_backend* _backend);
b8 vulkan_renderer_begin_frame(struct renderer_backend* _backend, f32 delta_time);
b8 vulkan_renderer_end_frame(struct renderer_backend* _backend, f32 delta_time);
void vulkan_renderer_resize(struct renderer_backend* _backend, int width, int height);