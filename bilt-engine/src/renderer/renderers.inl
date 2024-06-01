#pragma once

#include "defines.h"

typedef enum backend_api {
    VULKAN_API,
    OPENGL_API,
    DIRECTX_API,
} backend_api;

typedef struct backend_context {
    struct active_platform* active_plat;
    u64 frame_count;
    
    b8(*initialize_backend)(struct backend_context* context, const char* name, struct active_platform* active_plat);
    void(*shutdown_backend)(struct backend_context* context);
    void(*handle_resize)(struct backend_context* context, u16 width, u16 height);
    b8(*start_frame)(struct backend_context* context, f32 delta_time);
    b8(*end_frame)(struct backend_context* context, f32 delta_time);
} backend_context;

typedef struct frame_data {
    f32 delta_time;
} frame_data;