#include "renderer_frontend.h"
#include "core/kmemory.h"
#include "renderer_backend.h"
#include "core/logger.h"

static renderer_backend* _backend;

b8 renderer_initilize(const char* application_name, struct platform_state* plat_state)
{
    _backend = kallocate(sizeof(renderer_backend),MEMORY_TAG_RENDERER);
    renderer_backend_initilize(_backend);

    _backend->plat_state = plat_state;
    _backend->initilize(_backend, application_name);
}

void renderer_shutdown()
{
    _backend->shutdown(_backend);
    renderer_backend_destroy(_backend);
    kfree(_backend,sizeof(renderer_backend),MEMORY_TAG_RENDERER);
}

b8 renderer_resize(int width, int height)
{
    _backend->resize(_backend,width, height);
}

b8 begin_frame(f32 delta_time)
{
    return _backend->begin_frame(_backend,delta_time);
}

b8 end_frame(f32 delta_time)
{
    b8 res = _backend->end_frame(_backend,delta_time);
    _backend->frame_number++;
    return res;
}
b8 renderer_draw_frame(f32 delta_time)
{
    //if begin frame returns true, mid frame operation may continue
    if(begin_frame(delta_time))
    {
        b8 res = end_frame(delta_time);

        if(!res)
        {
            KERROR("Renderer End Frame Failed to execute ... shutting down application");
            return FALSE;
        }
    }

    return TRUE;
}