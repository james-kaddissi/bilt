/*
This header file contains the definitions for the signal system. The system is very simple:
Anywhere in your games code you can 'listen_for' a signal id and you will then get data from any signals with that id.
To stop receiving data from a specific signal use 'unlisten_for'. To send a signal just use 'sing_signal'.
*/
#pragma once

#include "defines.h"

typedef struct signal {
    union { // ensures the signal is 128 bytes
        i64 i64[2];
        u64 u64[2];
        f64 f64[2];
        i32 i32[4];
        u32 u32[4];
        f32 f32[4];
        i16 i16[8];
        u16 u16[8];
        i8 i8[16];
        u8 u8[16];
        char c[16];
    } data;
} signal;

typedef b8 (*onSignal)(u16 id, void* singer, void* listener, signal data);

b8 enable_signals();
void disable_signals();

BILT_API b8 listen_for(u16 id, void* listener, onSignal sig);
BILT_API b8 unlisten_for(u16 id, void* listener, onSignal sig);
BILT_API b8 sing_signal(u16 id, void* singer, signal data);

typedef enum signal_ids {
    APP_QUIT = 0x01,
    KEY_PRESSED = 0x02,
    KEY_RELEASED = 0x03,
    MBUTTON_PRESSED = 0x04,
    MBUTTON_RELEASED = 0x05,
    MOUSE_MOVED = 0x06,
    MOUSE_WHEEL = 0x07,
    WIN_RESIZED = 0x08,
    // add up to 255 engine signals here. Custom game signal support will be implemented, and ids should start at 256.
    END_SIGNALS = 0xFF
} signal_ids;