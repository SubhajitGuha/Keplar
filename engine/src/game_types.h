#pragma once
#include "core/application.h"

typedef struct game
{
    application_config application_config;

    b8 (*initilize) (struct game* game_inst); //func pointer that initilize game state

    b8 (*update) (struct game* game_inst);

    b8 (*render) (struct game* game_inst);

    void (*on_resize) (struct game* game_inst, int width, int height);

    // Game-specific game state. Created and managed by the game.
    void* game_state;
} game;