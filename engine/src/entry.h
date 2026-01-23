#pragma once

#include "game_types.h"
#include "core/logger.h"
#include "core/application.h"

extern b8 initilize_game(game* game_init);

int main()
{
    game _game;
    if(!initilize_game(&_game))
    {
        KERROR("Failed to initilize game!");
        return -1;
    }

    if(!_game.initilize || !_game.render || !_game.update || !_game.on_resize)
    {
        KERROR("all function pointers must be assigned in game!");
        return -2;
    }

    if(!create_application(&_game))
    {
        KERROR("Failed to create application!");
        return 1;
    }

    if(!run_application())
    {
        KERROR("Failed to run application");
        return 2;
    }

    return 0;
}