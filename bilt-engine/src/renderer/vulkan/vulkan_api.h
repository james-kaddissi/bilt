#pragma once

#include "renderer/backend.h"

b8 initialize_vulkan_api(backend_context* context, const char* name, struct active_platform* active_plat);
void disable_vulkan_api(backend_context* context);
void vulkan_handle_resize(backend_context* context, u16 width, u16 height);
b8 vulkan_start_frame(backend_context* context, f32 delta_time);
b8 vulkan_end_frame(backend_context* context, f32 delta_time);