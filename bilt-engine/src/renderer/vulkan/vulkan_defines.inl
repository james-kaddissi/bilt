#pragma once

#include "defines.h"
#include "main/assertions.h"

#include <vulkan/vulkan.h>

#define VK_CHECK(expr)                   \
    {                                    \
        BILT_ASSERT(expr == VK_SUCCESS); \
    }                                    \

typedef struct vk_context {
    VkInstance vk_instance;
    VkAllocationCallbacks* vk_allocator;
    #if defined(_DEBUG)
    VkDebugUtilsMessengerEXT vk_debugger;
    #endif
} vk_context;

