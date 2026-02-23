
#pragma once

#include "defines.h"

typedef enum renderer_backend_type
{
    VULKAN,
    OPENGL,
    METAL,
    DX11,
    DX12,
}renderer_backend_type;

typedef struct renderer_backend
{
    struct platform_state* plat_state;
    u64 frame_number;

    //function pointers that the backend renderer API needs to implement
    b8(*initilize) (struct renderer_backend* _backend, const char* app_name);
    void(*shutdown) (struct renderer_backend* _backend);
    b8(*begin_frame)(struct renderer_backend* _backend, f32 delta_time);
    b8(*end_frame)(struct renderer_backend* _backend, f32 delta_time);
    void(*resize)(struct renderer_backend* _backend, int width, int height);
}renderer_backend;

typedef struct renderer_packet
{
    f32 delta_time;
}renderer_packet;