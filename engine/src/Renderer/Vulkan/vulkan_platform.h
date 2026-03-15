#pragma once

#include "vulkan_types.inl"

struct platform_state;
struct vulkan_context;
void platform_get_required_extension_name(const char*** name_array);

b8 platform_create_vulkan_surface(platform_state* plat_state, vulkan_context* context);