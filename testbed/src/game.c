#include "game.h"
#include <defines.h>
#include <core/logger.h>
b8 game_initilize(game* _game)
{
    KTRACE("game initilized");
    return TRUE;
}

b8 game_update(game* _game)
{
    return TRUE;
}

b8 game_render(game* _game)
{
    return TRUE;
}

void game_on_resize(game* _game,  int width, int height)
{
}