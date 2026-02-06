#include "input.h"
#include "kmemory.h"
#include "logger.h"
#include "event.h"

typedef struct key_state
{
    b8 keys[256];
} key_state;

typedef struct mouse_state
{
    i16 x;
    i16 y;
    b8 buttons[BUTTON_MAX_BUTTONS];
} mouse_state;

typedef struct input_state
{
    key_state previous_key_state;
    key_state current_key_state;

    mouse_state previous_mouse_state;
    mouse_state current_mouse_state;
}input_state;

static b8 is_initilized = FALSE;
static input_state state = {};

void input_initilize()
{
    if(is_initilized == TRUE)
    {
        KWARN("input_initilize() is being called more than once");
        return;
    }
    is_initilized = TRUE;
    kzero_memory(&state, sizeof(state));
}
void input_shut_down()
{
    is_initilized = FALSE;
}
void input_update(f64 delta_time)
{
    if(!is_initilized)
    {
        KWARN("make sure to call input_initilize() at first");

        return;
    }
    kcopy_memory(&state.previous_key_state, &state.current_key_state, sizeof(key_state));
    kcopy_memory(&state.previous_mouse_state, &state.current_mouse_state, sizeof(mouse_state));
}

void input_process_key(Keys key, b8 pressed)
{
    if(!is_initilized)
    {
        KWARN("make sure to call input_initilize() at first");

        return;
    }

    if(state.current_key_state.keys[key] != pressed)
    {
        state.current_key_state.keys[key] = pressed;

        //fire off an event
        event_context context;
        context.data.u16[0] = key;
        event_fire(pressed? EVENT_CODE_KEY_PRESSED: EVENT_CODE_KEY_RELEASED,0,context);
    }
}

void process_mouse_button(Buttons button, b8 pressed)
{
    if(!is_initilized)
    {
        KWARN("make sure to call input_initilize() at first");
        return;
    }

    if(state.current_mouse_state.buttons[button] != pressed)
    {
        state.current_mouse_state.buttons[button] = pressed;

        //fire event
        event_context context;
        context.data.u16[0] = button;
        event_fire(pressed?EVENT_CODE_BUTTON_PRESSED:EVENT_CODE_BUTTON_RELEASED,0,context);
    }
}
void process_mouse_move(i16 x, i16 y)
{
    if(!is_initilized)
    {
        KWARN("make sure to call input_initilize() at first");
        return;
    }
    if(state.current_mouse_state.x != x || state.current_mouse_state.y != y)
    {
        state.current_mouse_state.x = x;
        state.current_mouse_state.y = y;

        //fire an event
        event_context context;
        context.data.i16[0] = x;
        context.data.i16[1] = y;
        event_fire(EVENT_CODE_MOUSE_MOVED,0,context);
    }

}
void process_mouse_wheel(i8 delta)
{
    //NOTE no internal state

    event_context context;
    context.data.i8[0] = delta;
    event_fire(EVENT_CODE_MOUSE_WHEEL, 0,context);
}

KAPI b8 input_is_key_down(Keys key)
{
    if(!is_initilized)
        return FALSE;
    return state.current_key_state.keys[key]==TRUE;
}
KAPI b8 input_is_key_up(Keys key)
{
    if(!is_initilized)
        return FALSE;
    return state.current_key_state.keys[key]==FALSE;
}
KAPI b8 input_was_key_down(Keys key)
{
     if(!is_initilized)
        return FALSE;
    return state.previous_key_state.keys[key]==TRUE;

}
KAPI b8 input_was_key_up(Keys key)
{
    if(!is_initilized)
        return FALSE;
    return state.previous_key_state.keys[key]==FALSE;
}

KAPI b8 input_is_button_down(Buttons button)
{
    if(!is_initilized)
        return FALSE;
    return state.current_mouse_state.buttons[button] == TRUE;
}
KAPI b8 input_is_button_up(Buttons button)
{
    if(!is_initilized)
        return FALSE;
    return state.current_mouse_state.buttons[button] == FALSE;
}
KAPI b8 input_was_button_down(Buttons button)
{
    if(!is_initilized)
        return FALSE;
    return state.previous_mouse_state.buttons[button] == TRUE;
}
KAPI b8 input_was_button_up(Buttons button)
{
    if(!is_initilized)
        return FALSE;
    return state.previous_mouse_state.buttons[button] == FALSE;
}
KAPI void input_get_mouse_position(i32* x, i32* y)
{
    if(!is_initilized)
    {
        *x = 0;
        *y = 0;
        return;
    }
    *x = state.current_mouse_state.x;
    *y = state.current_mouse_state.y;
}
KAPI void input_get_previous_mouse_position(i32* x, i32* y)
{
    if(!is_initilized)
    {
        *x = 0;
        *y = 0;
        return;
    }
    *x = state.previous_mouse_state.x;
    *y = state.previous_mouse_state.y;
}
