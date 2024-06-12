#include "platforms/unify_platforms.h"

// platform check wrapper
#if PLATFORM_LINUX

#include "main/output.h"
#include "main/signals.h"
#include "main/inputs.h"
#include "dsa/bilt_array.h"

#include <xcb/xcb.h>
#include <X11/keysym.h>
#include <X11/XKBlib.h> 
#include <X11/Xlib.h>
#include <X11/Xlib-xcb.h>
#include <sys/time.h>
#include <linux/time.h>

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

keys process_code(u32 kcode);

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
                xcb_key_press_event_t *keyvent = (xcb_key_press_event_t*)event;
                b8 pressState = event->response_type == XCB_KEY_PRESS;
                xcb_keycode_t code = keyvent->detail;
                KeySym ksym = XkbKeycodeToKeysym(state->display,(KeyCode)code, 0, code & ShiftMask ? 1: 0);
                keys key = process_code(ksym);
                process_key_input(key, pressState);
            } break;
            case XCB_BUTTON_PRESS:
            case XCB_BUTTON_RELEASE: {
                xcb_button_press_event_t *mevent = (xcb_button_press_event_t *)event;
                b8 pressState = event->response_type == XCB_BUTTON_PRESS;
                mouse_buttons button = NUM_BUTTONS;
                switch (mevent->detail) {
                    case XCB_BUTTON_INDEX_1:
                        button = LEFT_BUTTON;
                        break;
                    case XCB_BUTTON_INDEX_2:
                        button = MIDDLE_BUTTON;
                        break;
                    case XCB_BUTTON_INDEX_3:
                        button = RIGHT_BUTTON;
                        break;
                }
                if (button != NUM_BUTTONS) {
                    process_button_input(button, pressState);
                }
            }
            case XCB_MOTION_NOTIFY:
                xcb_motion_notify_event_t *motion = (xcb_motion_notify_event_t *)event;

                process_mouse_movement(motion->event_x, motion->event_y);
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

f64 get_time() {
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

void get_extensions(const char ***names) {
    push_array(*names, &"VK_KHR_xcb_surface");
}


keys process_code(u32 kcode) {
    switch (kcode) {
        case XK_BackSpace:
            return KEY_BACKSPACE;
        case XK_Return:
            return KEY_ENTER;
        case XK_Tab:
            return KEY_TAB;
        case XK_Pause:
            return KEY_PAUSE;
        case XK_Caps_Lock:
            return KEY_CAPITAL;
        case XK_Escape:
            return KEY_ESCAPE;
        case XK_Mode_switch:
            return KEY_MODECHANGE;
        case XK_space:
            return KEY_SPACE;
        case XK_Prior:
            return KEY_PRIOR;
        case XK_Next:
            return KEY_NEXT;
        case XK_End:
            return KEY_END;
        case XK_Home:
            return KEY_HOME;
        case XK_Left:
            return KEY_LEFT;
        case XK_Up:
            return KEY_UP;
        case XK_Right:
            return KEY_RIGHT;
        case XK_Down:
            return KEY_DOWN;
        case XK_Select:
            return KEY_SELECT;
        case XK_Print:
            return KEY_PRINT;
        case XK_Execute:
            return KEY_EXECUTE;
        case XK_Insert:
            return KEY_INSERT;
        case XK_Delete:
            return KEY_DELETE;
        case XK_Help:
            return KEY_HELP;

        case XK_Meta_L:
            return KEY_LWIN;  
        case XK_Meta_R:
            return KEY_RWIN;

        case XK_KP_0:
            return KEY_NUMPAD0;
        case XK_KP_1:
            return KEY_NUMPAD1;
        case XK_KP_2:
            return KEY_NUMPAD2;
        case XK_KP_3:
            return KEY_NUMPAD3;
        case XK_KP_4:
            return KEY_NUMPAD4;
        case XK_KP_5:
            return KEY_NUMPAD5;
        case XK_KP_6:
            return KEY_NUMPAD6;
        case XK_KP_7:
            return KEY_NUMPAD7;
        case XK_KP_8:
            return KEY_NUMPAD8;
        case XK_KP_9:
            return KEY_NUMPAD9;
        case XK_multiply:
            return KEY_MULTIPLY;
        case XK_KP_Add:
            return KEY_ADD;
        case XK_KP_Separator:
            return KEY_SEPARATOR;
        case XK_KP_Subtract:
            return KEY_SUBTRACT;
        case XK_KP_Decimal:
            return KEY_DECIMAL;
        case XK_KP_Divide:
            return KEY_DIVIDE;
        case XK_F1:
            return KEY_F1;
        case XK_F2:
            return KEY_F2;
        case XK_F3:
            return KEY_F3;
        case XK_F4:
            return KEY_F4;
        case XK_F5:
            return KEY_F5;
        case XK_F6:
            return KEY_F6;
        case XK_F7:
            return KEY_F7;
        case XK_F8:
            return KEY_F8;
        case XK_F9:
            return KEY_F9;
        case XK_F10:
            return KEY_F10;
        case XK_F11:
            return KEY_F11;
        case XK_F12:
            return KEY_F12;
        case XK_F13:
            return KEY_F13;
        case XK_F14:
            return KEY_F14;
        case XK_F15:
            return KEY_F15;
        case XK_F16:
            return KEY_F16;
        case XK_F17:
            return KEY_F17;
        case XK_F18:
            return KEY_F18;
        case XK_F19:
            return KEY_F19;
        case XK_F20:
            return KEY_F20;
        case XK_F21:
            return KEY_F21;
        case XK_F22:
            return KEY_F22;
        case XK_F23:
            return KEY_F23;
        case XK_F24:
            return KEY_F24;

        case XK_Num_Lock:
            return KEY_NUMLOCK;
        case XK_Scroll_Lock:
            return KEY_SCROLL;

        case XK_KP_Equal:
            return KEY_NUMPAD_EQUAL;

        case XK_Shift_L:
            return KEY_LSHIFT;
        case XK_Shift_R:
            return KEY_RSHIFT;
        case XK_Control_L:
            return KEY_LCONTROL;
        case XK_Control_R:
            return KEY_RCONTROL;
        case XK_Menu:
            return KEY_RMENU;

        case XK_semicolon:
            return KEY_SEMICOLON;
        case XK_plus:
            return KEY_PLUS;
        case XK_comma:
            return KEY_COMMA;
        case XK_minus:
            return KEY_MINUS;
        case XK_period:
            return KEY_PERIOD;
        case XK_slash:
            return KEY_SLASH;
        case XK_grave:
            return KEY_GRAVE;

        case XK_a:
        case XK_A:
            return KEY_A;
        case XK_b:
        case XK_B:
            return KEY_B;
        case XK_c:
        case XK_C:
            return KEY_C;
        case XK_d:
        case XK_D:
            return KEY_D;
        case XK_e:
        case XK_E:
            return KEY_E;
        case XK_f:
        case XK_F:
            return KEY_F;
        case XK_g:
        case XK_G:
            return KEY_G;
        case XK_h:
        case XK_H:
            return KEY_H;
        case XK_i:
        case XK_I:
            return KEY_I;
        case XK_j:
        case XK_J:
            return KEY_J;
        case XK_k:
        case XK_K:
            return KEY_K;
        case XK_l:
        case XK_L:
            return KEY_L;
        case XK_m:
        case XK_M:
            return KEY_M;
        case XK_n:
        case XK_N:
            return KEY_N;
        case XK_o:
        case XK_O:
            return KEY_O;
        case XK_p:
        case XK_P:
            return KEY_P;
        case XK_q:
        case XK_Q:
            return KEY_Q;
        case XK_r:
        case XK_R:
            return KEY_R;
        case XK_s:
        case XK_S:
            return KEY_S;
        case XK_t:
        case XK_T:
            return KEY_T;
        case XK_u:
        case XK_U:
            return KEY_U;
        case XK_v:
        case XK_V:
            return KEY_V;
        case XK_w:
        case XK_W:
            return KEY_W;
        case XK_x:
        case XK_X:
            return KEY_X;
        case XK_y:
        case XK_Y:
            return KEY_Y;
        case XK_z:
        case XK_Z:
            return KEY_Z;

        default:
            return 0;
    }
}
#endif