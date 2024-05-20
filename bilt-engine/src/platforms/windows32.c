#include "platforms/unify_platforms.h"
#include "main/output.h"
#if PLATFORM_WINDOWS_32

#include <windows.h>
#include <windowsx.h>

LRESULT CALLBACK win_popups(HWND hwnd, u32 msg, WPARAM wparam, LPARAM lparam);

typedef struct active_state {
    HINSTANCE hinstance;
    HWND hwnd;
}active_state;

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

    return TRUE; // successful platform initialization
}

#endif