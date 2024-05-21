#include "unify_platforms.h"

// platform check wrapper
#if PLATFORM_LINUX

#include "main/output.h"

#include <xcb/xcb.h>
#include <X11/keysym.h>
#include <X11/XKBlib.h> 
#include <X11/Xlib.h>
#include <X11/Xlib-xcb.h>
#include <sys/time.h>

// posix versions after 1993 use time.h else use unistd.h
#if _POSIX_C_SOURCE >= 199309L
#include <time.h>
#else
#include <unistd.h>
#endif

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

typedef struct active_state {
    Display* display;
    xcb_connection_t* connection;
    xcb_window_t window;
    xcb_screen_t* screen;
    xcb_atom_t protocols;
    xcb_atom_t window_delete;
} active_state;

b8 initialize_platform(active_platform* active_plat, const char* name, i32 x, i32 y, i32 width, i32 height) {
    active_plat->active_state = malloc(sizeof(active_state));
    active_state* state = (active_state*)active_plat->active_state;

    // initialize display
    state->display = XOpenDisplay(NULL);
    XAutoRepeatOff(state->display);
    state->connection = XGetXCBConnection(state->display);

    if (xcb_connection_has_error(state->connection)) {
        LOG_FATAL("Failed to connect to X!");
        return FALSE;
    }
    const struct xcb_setup_t* setup = xcb_get_setup(state->connection);

    // screens iterator
    xcb_screen_iterator_t i = xcb_setup_roots_iterator(setup);
    int j = 0;
    for (i32 s = j; s > 0; s--) {
        xcb_screen_next(&i);
    }
    state->screen = i.data;
    state->window = xcb_generate_id(state->connection);

    // event types
    u32 mask = XCB_CW_BACK_PIXEL | XCB_CW_EVENT_MASK;
    u32 events =  XCB_EVENT_MASK_BUTTON_PRESS | XCB_EVENT_MASK_BUTTON_RELEASE | XCB_EVENT_MASK_KEY_PRESS | XCB_EVENT_MASK_KEY_RELEASE | XCB_EVENT_MASK_EXPOSURE | XCB_EVENT_MASK_POINTER_MOTION | XCB_EVENT_MASK_STRUCTURE_NOTIFY;
    u32 events_list[] = {state->screen->black_pixel, events};

    // window creation
    xcb_void_cookie_t win = xcb_create_window(state->connection, XCB_COPY_FROM_PARENT, state->window, state->screen->root,  x, y, width, height, 0, XCB_WINDOW_CLASS_INPUT_OUTPUT, state->screen->root_visual,mask,events_list);

    // title window
    xcb_change_property(state->connection,XCB_PROP_MODE_REPLACE,state->window, XCB_ATOM_WM_NAME, XCB_ATOM_STRING, 8, strlen(name), name);
    
    xcb_intern_atom_cookie_t win_del = xcb_intern_atom(state->connection, 0, strlen("WM_DELETE_WINDOW"), "WM_DELETE_WINDOW");
    xcb_intern_atom_cookie_t win_protocols = xcb_intern_atom(state->connection, 0, strlen("WM_PROTOCOLS"), "WM_PROTOCOLS");
    xcb_intern_atom_reply_t* win_del_resp = xcb_intern_atom_reply(state->connection, win_del, NULL);
    xcb_intern_atom_reply_t* win_protocols_resp = xcb_intern_atom_reply(state->connection, win_protocols, NULL);
    state->window_delete = win_del_resp->atom;
    state->protocols = win_protocols_resp->atom;

    xcb_change_property(state->connection, XCB_PROP_MODE_REPLACE, state->window, win_protocols_resp->atom, 4, 32, 1, &win_del_resp->atom);

    // map the window to the screen
    xcb_map_window(state->connection, state->window);

    i32 result = xcb_flush(state->connection);
    if (result <= 0) {
        LOG_FATAL("An error occurred while flushing: %d", result);
        return FALSE;
    }

    return TRUE;
}

void deactivate_platform(active_platform* active_plat) {
    active_state* state = (active_state*)active_plat->active_state;

    XAutoRepeatOn(state->display);

    xcb_destroy_window(state->connection, state->window);
}

b8 message_loop(active_platform* active_plat) {
    active_state* state = (active_state*)active_plat->active_state;

    xcb_generic_event_t* event;
    xcb_client_message_event_t* client_message;

    b8 quit_flagged = FALSE;

    while (event != 0) {
        event = xcb_poll_for_event(state->connection);
        if (event == 0) {
            break;
        }
        switch (event->response_type & ~0x80) {
            case XCB_KEY_PRESS:
            case XCB_KEY_RELEASE: {
                // handle key presses and releases
            } break;
            case XCB_BUTTON_PRESS:
            case XCB_BUTTON_RELEASE: {
                // handle mouse button presses and releases
            }
            case XCB_MOTION_NOTIFY:
                // handle mouse movement
                break;

            case XCB_CONFIGURE_NOTIFY: {
                // handleesizing
            }

            case XCB_CLIENT_MESSAGE: {
                client_message = (xcb_client_message_event_t*)event;

                // window close
                if (client_message->data.data32[0] == state->window_delete) {
                    quit_flagged = TRUE;
                }
            } break;
            default:
                // Something else
                break;
        }

        free(event);
    }
    return !quit_flagged;
}

void* mem_alloc(u64 size, b8 aligned) {
    return malloc(size);
}
void free_mem(void* block, b8 aligned) {
    free(block);
}
void* zero_mem(void* block, u64 size) {
    return memset(block, 0, size);
}
void* copy_mem  (void* dest, const void* source, u64 size) {
    return memcpy(dest, source, size);
}
void* set_mem(void* dest, i32 value, u64 size) {
    return memset(dest, value, size);
}

void write_console(const char* message, u8 color) {
    const char* color_strings[] = {"0;41", "1;31", "1;33", "1;32", "1;34", "1;30"};
    printf("\033[%sm%s\033[0m", color_strings[color], message);
}
void write_console_error(const char* message, u8 color) {
    const char* color_strings[] = {"0;41", "1;31", "1;33", "1;32", "1;34", "1;30"};
    printf("\033[%sm%s\033[0m", color_strings[color], message);
}

f64 platform_get_azbsolute_time() {
    struct timespec curr;
    clock_gettime(CLOCK_MONOTONIC, &curr);
    return curr.tv_sec + curr.tv_nsec * 0.000000001;
}

void timed_sleep(u64 ms) {
#if _POSIX_C_SOURCE >= 199309L
    struct timespec t;
    t.tv_sec = ms / 1000;
    t.tv_nsec = (ms % 1000) * 1000 * 1000;
    nanosleep(&t, 0);
#else
    if (ms >= 1000) {
        sleep(ms / 1000);
    }
    usleep((ms % 1000) * 1000);
#endif
}

#endif