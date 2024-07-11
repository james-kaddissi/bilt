#pragma once
#include "defines.h"

struct active_platform;
struct vk_context;

b8 initialize_vulkan_surface(
    struct active_platform* active_plat,
    struct vk_context* vk
);

void get_extensions(const char*** names);

