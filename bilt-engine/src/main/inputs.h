#pragma once

#include "defines.h"

typedef enum mouse_buttons {
    LEFT_BUTTON,
    RIGHT_BUTTON,
    MIDDLE_BUTTON,
    NUM_BUTTONS
} mouse_buttons;

#define KEY_DEF(key, code) KEY_##key = code

typedef enum keys { // based on ascii values, other hardware (mac keyboards) need separate code
    KEY_DEF(BACKSPACE, 0x08),
    KEY_DEF(ENTER, 0x0D),
    KEY_DEF(TAB, 0x09),
    KEY_DEF(SHIFT, 0x10),
    KEY_DEF(CONTROL, 0x11),
    KEY_DEF(PAUSE, 0x13),
    KEY_DEF(CAPITAL, 0x14),
    KEY_DEF(ESCAPE, 0x1B),
    KEY_DEF(CONVERT, 0x1C),
    KEY_DEF(NONCONVERT, 0x1D),
    KEY_DEF(ACCEPT, 0x1E),
    KEY_DEF(MODECHANGE, 0x1F),
    KEY_DEF(SPACE, 0x20),
    KEY_DEF(PRIOR, 0x21),
    KEY_DEF(NEXT, 0x22),
    KEY_DEF(END, 0x23),
    KEY_DEF(HOME, 0x24),
    KEY_DEF(LEFT, 0x25),
    KEY_DEF(UP, 0x26),
    KEY_DEF(RIGHT, 0x27),
    KEY_DEF(DOWN, 0x28),
    KEY_DEF(SELECT, 0x29),
    KEY_DEF(PRINT, 0x2A),
    KEY_DEF(EXECUTE, 0x2B),
    KEY_DEF(SNAPSHOT, 0x2C),
    KEY_DEF(INSERT, 0x2D),
    KEY_DEF(DELETE, 0x2E),
    KEY_DEF(HELP, 0x2F), 
    KEY_DEF(A, 0x41),
    KEY_DEF(B, 0x42),
    KEY_DEF(C, 0x43),
    KEY_DEF(D, 0x44),
    KEY_DEF(E, 0x45),
    KEY_DEF(F, 0x46),
    KEY_DEF(G, 0x47),
    KEY_DEF(H, 0x48),
    KEY_DEF(I, 0x49),
    KEY_DEF(J, 0x4A),
    KEY_DEF(K, 0x4B),
    KEY_DEF(L, 0x4C),
    KEY_DEF(M, 0x4D),
    KEY_DEF(N, 0x4E),
    KEY_DEF(O, 0x4F),
    KEY_DEF(P, 0x50),
    KEY_DEF(Q, 0x51),
    KEY_DEF(R, 0x52),
    KEY_DEF(S, 0x53),
    KEY_DEF(T, 0x54),
    KEY_DEF(U, 0x55),
    KEY_DEF(V, 0x56),
    KEY_DEF(W, 0x57),
    KEY_DEF(X, 0x58),
    KEY_DEF(Y, 0x59),
    KEY_DEF(Z, 0x5A),
    KEY_DEF(LWIN, 0x5B),
    KEY_DEF(RWIN, 0x5C),
    KEY_DEF(APPS, 0x5D),
    KEY_DEF(SLEEP, 0x5F),
    KEY_DEF(NUMPAD0, 0x60),
    KEY_DEF(NUMPAD1, 0x61),
    KEY_DEF(NUMPAD2, 0x62),
    KEY_DEF(NUMPAD3, 0x63),
    KEY_DEF(NUMPAD4, 0x64),
    KEY_DEF(NUMPAD5, 0x65),
    KEY_DEF(NUMPAD6, 0x66),
    KEY_DEF(NUMPAD7, 0x67),
    KEY_DEF(NUMPAD8, 0x68),
    KEY_DEF(NUMPAD9, 0x69),
    KEY_DEF(MULTIPLY, 0x6A),
    KEY_DEF(ADD, 0x6B),
    KEY_DEF(SEPARATOR, 0x6C),
    KEY_DEF(SUBTRACT, 0x6D),
    KEY_DEF(DECIMAL, 0x6E),
    KEY_DEF(DIVIDE, 0x6F),
    KEY_DEF(F1, 0x70),
    KEY_DEF(F2, 0x71),
    KEY_DEF(F3, 0x72),
    KEY_DEF(F4, 0x73),
    KEY_DEF(F5, 0x74),
    KEY_DEF(F6, 0x75),
    KEY_DEF(F7, 0x76),
    KEY_DEF(F8, 0x77),
    KEY_DEF(F9, 0x78),
    KEY_DEF(F10, 0x79),
    KEY_DEF(F11, 0x7A),
    KEY_DEF(F12, 0x7B),
    KEY_DEF(F13, 0x7C),
    KEY_DEF(F14, 0x7D),
    KEY_DEF(F15, 0x7E),
    KEY_DEF(F16, 0x7F),
    KEY_DEF(F17, 0x80),
    KEY_DEF(F18, 0x81),
    KEY_DEF(F19, 0x82),
    KEY_DEF(F20, 0x83),
    KEY_DEF(F21, 0x84),
    KEY_DEF(F22, 0x85),
    KEY_DEF(F23, 0x86),
    KEY_DEF(F24, 0x87),
    KEY_DEF(NUMLOCK, 0x90),
    KEY_DEF(SCROLL, 0x91),
    KEY_DEF(NUMPAD_EQUAL, 0x92),
    KEY_DEF(LSHIFT, 0xA0),
    KEY_DEF(RSHIFT, 0xA1),
    KEY_DEF(LCONTROL, 0xA2),
    KEY_DEF(RCONTROL, 0xA3),
    KEY_DEF(LMENU, 0xA4),
    KEY_DEF(RMENU, 0xA5),
    KEY_DEF(SEMICOLON, 0xBA),
    KEY_DEF(PLUS, 0xBB),
    KEY_DEF(COMMA, 0xBC),
    KEY_DEF(MINUS, 0xBD),
    KEY_DEF(PERIOD, 0xBE),
    KEY_DEF(SLASH, 0xBF),
    KEY_DEF(GRAVE, 0xC0),
    NUM_KEYS
} keys;

void enable_inputs();
void disable_inputs();
void update_inputs(f64 deltaTime);

// processing functions
void process_key_input(keys key, b8 pressState);
void process_button_input(mouse_buttons button, b8 pressState);
void process_mouse_movement(i16 x, i16 y);
void process_mouse_scroll(i8 delta);

// boolean key and button flags
BILT_API b8 keyDown(keys key);
BILT_API b8 keyUp(keys key);
BILT_API b8 keyLastDown(keys key);
BILT_API b8 keyLastUp(keys key);
BILT_API b8 buttonDown(mouse_buttons button);
BILT_API b8 buttonUp(mouse_buttons button);
BILT_API b8 buttonLastDown(mouse_buttons button);
BILT_API b8 buttonLastUp(mouse_buttons button);
BILT_API void getMousePosition(i32 *x, i32 *y);
BILT_API void getLastMousePosition(i32 *x, i32* y);

