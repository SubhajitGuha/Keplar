#include "platform.h"

#if KPLATFORM_WINDOWS
#include <Windows.h>
#include <windowsx.h>
#include "core/logger.h"

typedef struct internal_state
{
    HINSTANCE hinstance; //process handle
    HWND hwnd; //window handle
} internal_state;

LRESULT CALLBACK win32_process_message(HWND hwnd, u32 msg, WPARAM w_param, LPARAM l_param);

b8 platform_startup(
    platform_state* plat_state,
    const char* appName,
    i32 x,
    i32 y,
    i32 width,
    i32 height
)
{
    plat_state->internal_state = malloc(sizeof(internal_state));
    internal_state* state = (internal_state*)plat_state->internal_state;

    state->hinstance = GetModuleHandleA(0);

    // setup and register window class
    //1. fill the WINDOWCLASS
    HICON icon = LoadIcon(state->hinstance, IDI_APPLICATION);
    WNDCLASS wc;
    memset(&wc, 0, sizeof(wc));
    wc.style = CS_DBLCLKS; //get double clicks
    wc.lpfnWndProc = win32_process_message; //window procedure handles events (takes func pointer)
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = state->hinstance;
    wc.hIcon = icon;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW); //handle cursor manually
    wc.hbrBackground = NULL; //Transparent
    wc.lpszClassName = "Keplar_window_class";

    //2. Register the WINDOWCLASS
    if(!RegisterClassA(&wc))
    {
        MessageBoxA(0, "window registration failed", "Error", MB_ICONEXCLAMATION | MB_OK);
        return FALSE;
    }

    //3. create window
    u32 client_x = x;
    u32 client_y = y;
    u32 client_width = width;
    u32 client_height = height;
    
    u32 window_x = client_x;
    u32 window_y = client_y;
    u32 window_width = client_width;
    u32 window_height = client_height;

    u32 window_style = WS_OVERLAPPED | WS_SYSMENU | WS_CAPTION;
    u32 window_ex_style = WS_EX_APPWINDOW;

    window_style |= WS_MAXIMIZEBOX;
    window_style |= WS_MINIMIZEBOX;
    window_style |= WS_THICKFRAME;

    //obtain size of the border
    RECT border_rect = {0,0,0,0};
    AdjustWindowRectEx(&border_rect, window_style, 0, window_ex_style);

    window_x += border_rect.left;
    window_y += border_rect.top;

    //grow by the size of the os border.
    window_width += border_rect.right - border_rect.left;
    window_height += border_rect.bottom - border_rect.top;

    HWND handle = CreateWindowExA(window_ex_style, "Keplar_window_class", appName,
        window_style,window_x,window_y,
        window_width, window_height,0,0,state->hinstance,0
    );
    if(handle == NULL)
    {
        KFETAL("window creation failed");
        return FALSE;
    }
    else{
        state->hwnd = handle;
    }

    //4. show window
    b32 should_activate = 1; //if the window dosent accept input this should be false
    i32 show_window_command_flags = should_activate? SW_SHOW : SW_SHOWNOACTIVATE;
    //if initially minimized, use SW_MINIMIZE : SW_MINIMIZENOACTIVE;
    //if initially maximized, use SW_SHOWMAXIMIZED : SW_MAXIMIZE
    ShowWindow(state->hwnd, show_window_command_flags);

    return TRUE;
}

void platform_shutdown(platform_state* plat_state)
{
    internal_state* state = (internal_state*) plat_state;
    if(state->hwnd)
    {
        DestroyWindow(state->hwnd);
        state->hwnd = NULL;
    }
}

b8 platform_pump_messages(platform_state* plat_state)
{
    MSG message;
    while(PeekMessageA(&message,NULL,0,0,PM_REMOVE))
    {
        TranslateMessage(&message);
        DispatchMessageA(&message);
    }
    return TRUE;
}

void* platform_allocate(u64 size, b8 aligned)
{
    return malloc(size);
}
void platform_free(void* block, b8 aligned)
{
    free(block);
}
void* platform_zero_memory(void* block, u64 size)
{
    return memset(block, 0,size);
}
void* platform_copy_memory(void* dest, void* src, u64 size)
{
    return memcpy(dest, src,size);
}
void* patform_set_memory(void* dest, i32 value, u64 size)
{
    return memset(dest, value, size);
}

void platform_console_write(const char* message, u8 color)
{
    HANDLE console_handle = GetStdHandle(STD_OUTPUT_HANDLE);
    // FETAL, ERROR, WARN, INFO, DEBUG, TRACE
    static u8 levels[6] = {64, 4, 6, 2, 1, 8};
    SetConsoleTextAttribute(console_handle, levels[color]);
    OutputDebugString(message);
    u64 length = strlen(message);
    LPDWORD number_written = 0;
    WriteConsoleA(console_handle, message, (DWORD)length, number_written, 0);
}

void platform_console_write_error(const char* message, u8 color)
{
    HANDLE console_handle = GetStdHandle(STD_ERROR_HANDLE);
    // FETAL, ERROR, WARN, INFO, DEBUG, TRACE
    static u8 levels[6] = {64, 4, 6, 2, 1, 8};
    SetConsoleTextAttribute(console_handle, levels[color]);
    OutputDebugString(message);
    u64 length = strlen(message);
    LPDWORD number_written = 0;
    WriteConsoleA(console_handle, message, (DWORD)length, number_written, 0);
}

f64 platform_get_absolute_time()
{
    //TODO
    return 0.0;
}

void platform_sleep(u64 ms)
{
    //TODO
}
LRESULT CALLBACK win32_process_message(HWND hwnd, u32 msg, WPARAM w_param, LPARAM l_param)
{
    switch (msg)
    {
    case WM_ERASEBKGND:
        //notify the os that erasing will be handled by the application to handle flicker
        return 1;
    case WM_CLOSE:
        // fire an event for app to quit
        return 0;
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    case WM_SIZE:
        // Get the updated size.
            // RECT r;
            // GetClientRect(hwnd, &r);
            // u32 width = r.right - r.left;
            // u32 height = r.bottom - r.top;
            // TODO: Fire an event for window resize.
        break;
        case WM_KEYDOWN:
        case WM_SYSKEYDOWN:
        case WM_KEYUP:
        case WM_SYSKEYUP: {
            // Key pressed/released
            //b8 pressed = (msg == WM_KEYDOWN || msg == WM_SYSKEYDOWN);
            // TODO: input processing

        } break;
                case WM_MOUSEMOVE: {
            // Mouse move
            //i32 x_position = GET_X_LPARAM(l_param);
            //i32 y_position = GET_Y_LPARAM(l_param);
            // TODO: input processing.
        } break;
        case WM_MOUSEWHEEL: {
            // i32 z_delta = GET_WHEEL_DELTA_WPARAM(w_param);
            // if (z_delta != 0) {
            //     // Flatten the input to an OS-independent (-1, 1)
            //     z_delta = (z_delta < 0) ? -1 : 1;
            //     // TODO: input processing.
            // }
        } break;
        case WM_LBUTTONDOWN:
        case WM_MBUTTONDOWN:
        case WM_RBUTTONDOWN:
        case WM_LBUTTONUP:
        case WM_MBUTTONUP:
        case WM_RBUTTONUP: {
            //b8 pressed = msg == WM_LBUTTONDOWN || msg == WM_RBUTTONDOWN || msg == WM_MBUTTONDOWN;
            // TODO: input processing.
        } break;
    }
    return DefWindowProcA(hwnd, msg, w_param, l_param);
}

#endif