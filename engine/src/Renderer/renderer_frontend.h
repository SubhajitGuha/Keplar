#pragma once

#include "defines.h"
#include "renderer_types.inl"

b8 renderer_initilize(const char* application_name, struct platform_state* plat_state);

void renderer_shutdown();

b8 renderer_resize(int width, int height);

b8 renderer_draw_frame(f32 delta_time);