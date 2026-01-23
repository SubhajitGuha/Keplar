#include "application.h"
#include "platform/platform.h"
#include "game_types.h"
#include "logger.h"

typedef struct application_state
{
    game* game_instance;
    b8 is_running;
    b8 is_suspended;
    platform_state plat_state;
    int width;
    int height;
    f64 last_time;
}application_state;

//application should have only one instance and it should be private.
static application_state app_state;
static b8 Initilized = FALSE; //ensure that one instance of application is running

b8 create_application(struct game* game_inst)
{
    if(Initilized)
    {
        KFETAL("create_application called more than once");
        return FALSE;
    }

    app_state.is_running = TRUE;
    app_state.is_suspended = FALSE;

    app_state.game_instance = game_inst;

    initilize_logging();

    //initilize platform.
    if(!platform_startup(
        &app_state.plat_state,
        app_state.game_instance->application_config.app_name,
        app_state.game_instance->application_config.window_start_pos_x,
        app_state.game_instance->application_config.window_start_post_y,
        app_state.game_instance->application_config.window_width,
        app_state.game_instance->application_config.window_height))
    {
        KFETAL("Cannot startup platform!");
        return FALSE;
    }

    if(!app_state.game_instance->initilize(app_state.game_instance))
    {
        KERROR("Failed to initilize game instance");
        return FALSE;
    }

    app_state.game_instance->on_resize(app_state.game_instance, app_state.width, app_state.height);
    Initilized = TRUE;
    return TRUE;
}

b8 run_application()
{
    while (app_state.is_running)
    {
        if(!platform_pump_messages(&app_state.plat_state))
        {
            app_state.is_running = FALSE;
        }

        if(!app_state.is_suspended)
        {
            if(!app_state.game_instance->update(app_state.game_instance))
            {
                app_state.is_running = FALSE;
                KFETAL("game update failed!");
                break;
            }
    
            if(!app_state.game_instance->render(app_state.game_instance))
            {
                app_state.is_running = FALSE;
                KFETAL("game rendering failed!");
                break;
            }
        }
    }
    
    app_state.is_running = FALSE;
    platform_shutdown(&app_state.plat_state);
    return TRUE;
}