#pragma once

#include "defines.h"

struct game;
typedef struct application_config
{
    i16 window_start_pos_x; //window start position x if applicable
    i16 window_start_post_y; //window start position y if applicable
    i16 window_width; //window width
    i16 window_height; // window height
    const char* app_name;
} application_config;

KAPI b8 create_application(struct game* game_inst);

KAPI b8 run_application();