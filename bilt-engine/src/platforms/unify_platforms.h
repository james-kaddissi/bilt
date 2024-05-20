/*
This header file defines functions used in the platform-specific code.
In combination with the platform-specific code in this directory, 
it allows us to abstract away platform-specific details and use 
universal functionality in our engine.
 */
#pragma once

#include "defines.h"

// holds the state of the active platform.
typedef struct active_platform {
    void* active_state;
} active_platform;

//initializes the platform with the given parameters.
BILT_API b8 initialize_platform(
    active_platform* active_plat, // plaform struct
    const char* name, // platform name
    i32 x, // x-coordinate for initialization.
    i32 y, // y-coordinate for initialization.
    i32 width, // width for initialization.
    i32 height // height for initialization.
);


// deactivates the specified platform.
BILT_API void deactivate_platform(active_platform* active_plat);

// processes the message loop for the specified platform.
BILT_API b8 message_loop(active_platform* active_plat);

// allocates memory of the specified size.
void* mem_alloc(u64 size, b8 aligned);

// frees the specified memory block.
void free_mem(void* block, b8 aligned);

// sets the specified memory block to zero.
void* zero_mem(void* block, u64 size);

// copies memory from the source to the destination.
void* copy_mem(void* dest, const void* source, u64 size);

// sets the specified memory block to the given value.
void* set_mem(void* dest, i32 value, u64 size);

// writes a message to the console with the specified color.
void write_console(const char* message, u8 color);

// writes an error message to the console with the specified color.
void write_console_error(const char* message, u8 color);

// gets the current time.
f64 get_time();

// sleeps for the specified number of milliseconds.
void timed_sleep(u64 ms);