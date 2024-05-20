#include "platforms/unify_platforms.h"

#if PLATFORM_WINDOWS_32

#include <windows.h>
#include <windowsx.h>

typedef struct active_state {
    HINSTANCE hinstance;
    HWND hwnd;
}active_state;

b8 initialize_platform(active_platform* active_plat, const char* name, i32 x, i32 y, i32 width, i32 height) {
    active_plat->active_state = malloc(sizeof(active_state));
    active_state *state = (active_state *)active_plat->active_state;

    state->hinstance = GetModuleHandleA(0);

    HICON icon = LoadIcon(state->hinstance, IDI_APPLICATION);
}

#endif