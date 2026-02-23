#include "application.h"
#include "platform/platform.h"
#include "game_types.h"
#include "logger.h"
#include "kmemory.h"
#include "input.h"
#include "event.h"
#include "clock.h"

typedef struct application_state
{
    game* game_instance;
    b8 is_running;
    b8 is_suspended;
    platform_state plat_state;
    clock clock;
    int width;
    int height;
    f64 last_time;
}application_state;

//application should have only one instance and it should be private.
static application_state app_state;
static b8 Initilized = FALSE; //ensure that one instance of application is running

b8 application_on_event(u16 code, void* sender, void* listener_inst, event_context data);
b8 application_on_key(u16 code, void* sender, void* listener_inst, event_context data);

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
    input_initilize();
    event_initilize();

    KDEBUG(get_memory_usage_str());
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

    event_register(EVENT_CODE_APPLICATION_QUIT,0,application_on_event);
    event_register(EVENT_CODE_KEY_PRESSED,0,application_on_key);
    event_register(EVENT_CODE_KEY_RELEASED,0,application_on_key);


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
    clock_start(&app_state.clock);
    clock_update(&app_state.clock);
    f64 target_frame_time = 1.0/120.0;
    app_state.last_time = app_state.clock.elapsed_time;
    while (app_state.is_running)
    {
        if(!platform_pump_messages(&app_state.plat_state))
        {
            app_state.is_running = FALSE;
        }

        if(!app_state.is_suspended)
        {
            clock_update(&app_state.clock);
            f64 current_time = app_state.clock.elapsed_time;
            f64 delta_time = current_time - app_state.last_time;
            f64 frame_start_time = platform_get_absolute_time();

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
            KDEBUG("delta time %lf", delta_time);
            f64 frame_end_time = platform_get_absolute_time();
            f64 elapsed_frame_time = frame_end_time-frame_start_time;
            f64 remaining_seconds = target_frame_time - elapsed_frame_time;
            //if the frame is finished executing before the target time return handle to os.
            if(remaining_seconds > 0)
            {
                u64 remaining_ms = remaining_seconds * 1000;
                if(remaining_ms > 0)
                {
                    platform_sleep(remaining_ms-1);
                }
            }
            input_update(delta_time );
            app_state.last_time = current_time;
        }
    }
    
    app_state.is_running = FALSE;

    event_unregister(EVENT_CODE_APPLICATION_QUIT,0,application_on_event);
    event_unregister(EVENT_CODE_KEY_PRESSED,0,application_on_key);
    event_unregister(EVENT_CODE_KEY_RELEASED,0,application_on_key);

    platform_shutdown(&app_state.plat_state);
    shutdown_logging();
    input_shutdown();
    event_shutdown();

    return TRUE;
}

b8 application_on_event(u16 code, void* sender, void* listener_inst, event_context data)
{
    switch (code)
    {
    case EVENT_CODE_APPLICATION_QUIT:
        {
            KINFO("EVENT_CODE_APPLICATION_QUIT recceived quitting application...\n");
            app_state.is_running = FALSE;
            return TRUE;
        }
    }
    return FALSE;
}
b8 application_on_key(u16 code, void* sender, void* listener_inst, event_context context)
{
    if(code == EVENT_CODE_KEY_PRESSED)
    {
        u16 keyCode = context.data.u16[0];
        if(keyCode == KEY_ESCAPE)
        {
            event_context contextData = {};
            event_fire(EVENT_CODE_APPLICATION_QUIT,0,contextData);
            return TRUE;
        }
    }
    else if(code == EVENT_CODE_KEY_RELEASED)
    {
        u16 keyCode = context.data.u16[0];
        if(keyCode == KEY_A)
        {
            KDEBUG("EXPLICIT A-key released");
        }
        else
        {
            KDEBUG("'%c' key released in window", keyCode);
        }
    }
    return FALSE;
}
