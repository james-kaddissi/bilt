#include "main/inputs.h"
#include "main/signals.h"
#include "main/bilt_memory.h"
#include "main/output.h"

typedef struct keyState {
    b8 keys[256];
} keyState;
typedef struct mouseState {
    i16 x;
    i16 y;
    u8 mouse_buttons[NUM_BUTTONS];
} mouseState;
typedef struct inputState {
    keyState currentKeyState;
    keyState lastKeyState;
    mouseState currentMouseState;
    mouseState lastMouseState;
} inputState;

static b8 inputsEnabled = FALSE;
static inputState state = {};

void enable_inputs() {
    mzero(&state, sizeof(inputState));
    inputsEnabled = TRUE;
    LOG_INFO("Inputs enabled.");
}

void disable_inputs() {
    inputsEnabled = FALSE;
}

void update_inputs(f64 deltaTime) {
    if(!inputsEnabled) return;
    mcopy(&state.lastKeyState, &state.currentKeyState, sizeof(keyState));
    mcopy(&state.lastMouseState, &state.currentMouseState, sizeof(mouseState));
}

void process_key_input(keys key, b8 pressState) {
    if(state.currentKeyState.keys[key] != pressState) {
        state.currentKeyState.keys[key] = pressState;

        signal sig;
        sig.data.u16[0] = key;
        sing_signal(pressState ? KEY_PRESSED : KEY_RELEASED, 0, sig);
    }
}

void process_mouse_input(mouse_buttons button, b8 pressState) {
    if(state.currentMouseState.mouse_buttons[button] != pressState) {
        state.currentMouseState.mouse_buttons[button] = pressState;

        signal sig;
        sig.data.u16[0] = button;
        sing_signal(pressState ? MBUTTON_PRESSED : MBUTTON_RELEASED, 0, sig);
    }
}

void process_mouse_movement(i16 x, i16 y) {
    if(state.currentMouseState.x != x || state.currentMouseState.y != y) {
        state.currentMouseState.x = x;
        state.currentMouseState.y = y;

        signal sig;
        sig.data.u16[0] = x;
        sig.data.u16[1] = y;
        sing_signal(MOUSE_MOVED, 0, sig);
    }
}

void process_mouse_scroll(i8 delta) {
    signal sig;
    sig.data.u8[0] = delta;
    sing_signal(MOUSE_WHEEL, 0, sig);
}

b8 keyDown(keys key) {
    if(!inputsEnabled) return FALSE;
    return state.currentKeyState.keys[key] == TRUE;
}
b8 keyUp(keys key) {
    if(!inputsEnabled) return TRUE;
    return state.currentKeyState.keys[key] == FALSE;
}
b8 keyLastDown(keys key) {
    if(!inputsEnabled) return FALSE;
    return state.lastKeyState.keys[key] == TRUE;
}
b8 keyLastUp(keys key) {
    if(!inputsEnabled) return TRUE;
    return state.lastKeyState.keys[key] == FALSE;
}
b8 buttonDown(mouse_buttons button) {
    if(!inputsEnabled) return FALSE;
    return state.currentMouseState.mouse_buttons[button] == TRUE;
}
b8 buttonUp(mouse_buttons button) {
    if(!inputsEnabled) return TRUE;
    return state.currentMouseState.mouse_buttons[button] == FALSE;
}
b8 buttonLastDown(mouse_buttons button) {
    if(!inputsEnabled) return FALSE;
    return state.lastMouseState.mouse_buttons[button] == TRUE;
}
b8 buttonLastUp(mouse_buttons button) {
    if(!inputsEnabled) return TRUE;
    return state.lastMouseState.mouse_buttons[button] == FALSE;
}
void getMousePosition(i32 *x, i32 *y) {
    if (!inputsEnabled) {
        *x = 0;
        *y = 0;
        return;
    }
    *x = state.currentMouseState.x;
    *y = state.currentMouseState.y;
}
void getLastMousePosition(i32 *x, i32 *y) {
    if (!inputsEnabled) {
        *x = 0;
        *y = 0;
        return;
    }
    *x = state.lastMouseState.x;
    *y = state.lastMouseState.y;
}