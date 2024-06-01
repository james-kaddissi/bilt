#pragma once

#include "defines.h"

#include <vulkan/vulkan.h>

typedef struct vk_context {
    VkInstance vk_instance;
    VkAllocationCallbacks* vk_allocator;
} vk_context;