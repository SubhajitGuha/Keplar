#include <entry.h>
#include "game.h"

//TOODO: remove it, find a way to abstract it
#include <platform/platform.h>

b8 initilize_game(game* _game)
{
    _game->application_config.app_name = "Keplar";
    _game->application_config.window_start_pos_x = 100;
    _game->application_config.window_start_post_y = 100;
    _game->application_config.window_width = 1920;
    _game->application_config.window_height = 1080;

    _game->initilize = game_initilize;
    _game->render = game_render;
    _game->update = game_update;
    _game->on_resize = game_on_resize;

    //TOODO: remove it, find a way to abstract it
    _game->game_state = platform_allocate(sizeof(game_state), FALSE);

    return TRUE;
}