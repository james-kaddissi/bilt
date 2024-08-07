#include "platforms/unify_platforms.h"

#if PLATFORM_WINDOWS_32

#include "main/output.h"
#include "main/inputs.h"
#include "dsa/bilt_array.h"

#include <windows.h>
#include <windowsx.h>
#include <stdlib.h>

#include <vulkan/vulkan.h>
#include <vulkan/vulkan_win32.h>
#include "renderer/vulkan/vulkan_defines.inl"

LRESULT CALLBACK win_popups(HWND hwnd, u32 msg, WPARAM wparam, LPARAM lparam);

typedef struct active_state {
    HINSTANCE hinstance;
    HWND hwnd;
    VkSurfaceKHR vk_surface;
}active_state;

static f64 clock_speed;
static LARGE_INTEGER start_time;

// resposible for initializing the window and platform
b8 initialize_platform(active_platform* active_plat, const char* name, i32 x, i32 y, i32 width, i32 height) {
    active_plat->active_state = malloc(sizeof(active_state));
    active_state *state = (active_state *)active_plat->active_state;

    state->hinstance = GetModuleHandleA(0);

    // initialize window class
    HICON icon = LoadIcon(state->hinstance, IDI_APPLICATION);
    WNDCLASSA wnc;
    memset(&wnc, 0, sizeof(wnc));
    wnc.style = CS_DBLCLKS;
    wnc.lpfnWndProc = win_popups;
    wnc.cbClsExtra = 0;
    wnc.cbWndExtra = 0;
    wnc.hInstance = state->hinstance;
    wnc.hIcon = icon;
    wnc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wnc.hbrBackground = NULL;
    wnc.lpszClassName = "bilt_window_class";

    if (!RegisterClassA(&wnc)) {
        MessageBox(0, "Window initialization failed", "Error", MB_ICONEXCLAMATION | MB_OK);
        return FALSE;
    }

    // window size initialization
    u32 inner_x = x;
    u32 outer_x = inner_x;
    u32 inner_y = y;
    u32 outer_y = inner_y;
    u32 inner_width = width;
    u32 outer_width = inner_width;
    u32 inner_height = height;
    u32 outer_height = inner_height;
    // window styling
    u32 style = WS_OVERLAPPED | WS_SYSMENU | WS_CAPTION;
    u32 ex_style = WS_EX_APPWINDOW;
    style |= WS_MAXIMIZEBOX;
    style |= WS_MINIMIZEBOX;
    style |= WS_THICKFRAME;
    // get difference between client window and window
    RECT border = {0, 0, 0, 0};
    AdjustWindowRectEx(&border, style, 0, ex_style);
    // apply differences to outer values
    outer_x += border.left;
    outer_y += border.top;
    outer_width += border.right - border.left;
    outer_height += border.bottom - border.top;
    // window creation
    HWND handle = CreateWindowExA(ex_style, "bilt_window_class", name, style, outer_x, outer_y, outer_width, outer_height, 0, 0, state->hinstance, 0);

    // if window creation fails
    if (handle == 0) {
        MessageBoxA(NULL, "Window Creation Failed!", "Error!", MB_ICONEXCLAMATION | MB_OK);

        LOG_FATAL("WIndow Creation Failed!");
        return FALSE;
    } else {
        state->hwnd = handle;
    }

    // display window
    b32 input = 1; // turn off to turn off input for window
    i32 show_flags = input ? SW_SHOW : SW_SHOWNOACTIVATE;
    ShowWindow(state->hwnd, show_flags);

    LARGE_INTEGER hz;
    QueryPerformanceFrequency(&hz); // gets clock speed of CPU
    clock_speed = 1.0 / (f64)hz.QuadPart;
    QueryPerformanceCounter(&start_time);

    return TRUE; // successful platform initialization
}

// simple window shutdown procedure
void deactivate_platform(active_platform *active_plat) {
    active_state *state = (active_state *)active_plat->active_state;

    if (state->hwnd) {
        DestroyWindow(state->hwnd);
        state->hwnd = 0;
    }
}

// responsible for handling the messaging loop for the main application loop, should be referenced in games main game loop
b8 message_loop(active_platform* active_plat) {
    MSG message;
    // keep checking for messages in queue and process them one at a time, pops the last element after
    while (PeekMessageA(&message, NULL, 0, 0, PM_REMOVE)) {
        TranslateMessage(&message);
        DispatchMessageA(&message);
    }

    return TRUE;
}

// custom engine memory allocation function including aligned memory.
void *mem_alloc(u64 size, b8 aligned) {
    // temporarily ignore aligned memory
    return malloc(size);
}

// custom free implementation
void free_mem(void* block, b8 aligned) {
    free(block);
}

// custom function for zeroing memory
void *zero_mem(void* block, u64 size) {
    return memset(block, 0, size);
}

// custom copy memory function
void *copy_mem(void* dest, const void *source, u64 size) {
    return memcpy(dest, source, size);
}

// custom memory set function
void *set_mem(void* dest, i32 value, u64 size){
    return memset(dest, value, size);
}

// custom console output using color
void write_console(const char* message, u8 color) {
    HANDLE hand = GetStdHandle(STD_OUTPUT_HANDLE);
    static u8 types[6] = {64, 4, 6, 2, 1, 8}; // color identifiers for different error types
    SetConsoleTextAttribute(hand, types[color]);
    OutputDebugStringA(message);
    u64 length = strlen(message);
    LPDWORD dword = 0;
    WriteConsoleA(GetStdHandle(STD_OUTPUT_HANDLE), message, (DWORD)length, dword, 0);
}

// custom error output using color
void write_console_error(const char* message, u8 color){
    HANDLE hand = GetStdHandle(STD_ERROR_HANDLE);
    static u8 types[6] = {64, 4, 6, 2, 1, 8}; // color identifiers for different error types
    SetConsoleTextAttribute(hand, types[color]);
    OutputDebugStringA(message);
    u64 length = strlen(message);
    LPDWORD dword = 0;
    WriteConsoleA(GetStdHandle(STD_OUTPUT_HANDLE), message, (DWORD)length, dword, 0);
}

// gets the current absolute time of the application
f64 get_time() {
    LARGE_INTEGER current_time;
    QueryPerformanceCounter(&current_time);
    return (f64)current_time.QuadPart * clock_speed;
}

// sleep
void timed_sleep(u64 ms) {
    Sleep(ms);
}

void get_extensions(const char ***names) {
    push_array(*names, &"VK_KHR_win32_surface");
}

b8 initialize_vulkan_surface(active_platform* active_plat, vk_context* vk) {
    active_state *state = (active_state *)active_plat->active_state;
    VkWin32SurfaceCreateInfoKHR surface_create_info = {VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR};
    surface_create_info.hinstance = state->hinstance;
    surface_create_info.hwnd = state->hwnd;

    VkResult result = vkCreateWin32SurfaceKHR(vk->vk_instance, &surface_create_info, vk->vk_allocator, &vk->vk_surface);
    if(result != VK_SUCCESS)
    {
        LOG_FATAL("Failed to create vk_surface");
        return FALSE;
    }
    vk->vk_surface=state->vk_surface;
    return TRUE;
}

LRESULT CALLBACK win_popups(HWND hwnd, u32 msg, WPARAM wparam, LPARAM lparam) {
    switch(msg) {
        case WM_ERASEBKGND:
            return 1; // prevents both Windows and engine from handling erasing
        case WM_CLOSE:
            return 0;
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
        case WM_SIZE: {
            // implement window resizing event
        } break;
        case WM_KEYDOWN:
        case WM_SYSKEYDOWN:
        case WM_KEYUP:
        case WM_SYSKEYUP: {
            b8 pressState = (msg == WM_KEYDOWN || msg == WM_SYSKEYDOWN);
            keys key = (u16)wparam;
            process_key_input(key, pressState);
        } break;
        case WM_MOUSEMOVE: {
            i32 x = GET_X_LPARAM(lparam);
            i32 y = GET_Y_LPARAM(lparam);
            process_mouse_movement(x, y);
        } break;
        case WM_MOUSEWHEEL: {
            i32 delta = GET_WHEEL_DELTA_WPARAM(wparam);
            if (delta != 0) {
                delta = (delta < 0) ? -1: 1;
                process_mouse_scroll(delta);
            }
        } break;
        case WM_LBUTTONDOWN:
        case WM_MBUTTONDOWN:
        case WM_RBUTTONDOWN:
        case WM_LBUTTONUP:
        case WM_MBUTTONUP:
        case WM_RBUTTONUP: {
            b8 pressState = msg == WM_LBUTTONDOWN || msg == WM_RBUTTONDOWN || msg == WM_MBUTTONDOWN;
            mouse_buttons button = NUM_BUTTONS;
            switch (msg) {
                case WM_LBUTTONDOWN:
                case WM_LBUTTONUP:
                    button = LEFT_BUTTON;
                    break;
                case WM_MBUTTONDOWN:
                case WM_MBUTTONUP:
                    button = MIDDLE_BUTTON;
                    break;
                case WM_RBUTTONDOWN:
                case WM_RBUTTONUP:
                    button = RIGHT_BUTTON;
                    break;
            }
            if (button != NUM_BUTTONS) {
                process_button_input(button, pressState);
            }
        } break;
    }
    return DefWindowProcA(hwnd, msg, wparam, lparam);
}

#endif