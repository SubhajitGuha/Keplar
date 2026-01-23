#pragma once

#include <defines.h>
#include <game_types.h>

typedef struct game_state
{
    f32 delta_time;
}game_state;

b8 game_initilize(game* _game);

b8 game_update(game* _game);

b8 game_render(game* _game);

void game_on_resize(game* _game, int width, int height);