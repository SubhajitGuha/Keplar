#include <entry.h>
#include "game.h"
#include <core/kmemory.h>
#include <containers/darray.h>

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
    _game->game_state = kallocate(sizeof(game_state), MEMORY_TAG_APPLICATION);

    f32* array = darray_reserve(f32,1);
    darray_push(array,33.3f);
    darray_push(array,363.3f);
    darray_push_at(array,0,235.6f);
    float x;
    darray_pop(array,&x);
    darray_pop_at(array,0,&x);
    darray_push(array,133.3f);
    darray_push(array,9.3f);
    for(int i=0;i<darray_length(array);i++)
    {
        KDEBUG("%f\n", array[i]);
    }
    return TRUE;
}