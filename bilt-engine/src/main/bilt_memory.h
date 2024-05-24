/*
This header file contains the definitions for memory types and functions used by the game layer
*/


#pragma once

#include "defines.h"

typedef enum mem_type {
    MEM_TYPE_UNSPEC,
    
    MEM_TYPE_STRING,
    MEM_TYPE_ARRAY,
    MEM_TYPE_DICTIONARY,
    MEM_TYPE_QUEUE,
    MEM_TYPE_BINST,

    MEM_TYPE_APP,
    MEM_TYPE_GAME,
    MEM_TYPE_TASK,

    MEM_TYPE_RENDERER,
    MEM_TYPE_TEXTURE,
    MEM_TYPE_MATERIAL,
    MEM_TYPE_SHADER,

    MEM_TYPE_SOUND,  

    MEM_TYPE_OBJECT,
    MEM_TYPE_OBJECT_NODE,
    MEM_TYPE_SCENE,

    MEM_TYPE_TRANSFORM, 

    MAX_MEM
} mem_type;

BILT_API void enable_memory();
BILT_API void disable_memory();

// memory functions API
BILT_API void* mallocate(u64 size, mem_type type);
BILT_API void mfree(void* block, u64 size, mem_type type);
BILT_API void* mzero(void* block, u64 size);
BILT_API void* mcopy(void* block, const void* source, u64 size);
BILT_API void* mset(void* dest, i32 value, u64 size);
// debug functions
BILT_API char* get_total_mallocation(); 