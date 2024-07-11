#pragma once

#include "defines.h"
#include "main/assertions.h"

#include <vulkan/vulkan.h>

#define VK_CHECK(expr)                   \
    {                                    \
        BILT_ASSERT(expr == VK_SUCCESS); \
    }                                    \


typedef struct vulkan_swapchain_support_info {
    VkSurfaceCapabilitiesKHR capabilities;
    u32 format_count;
    VkSurfaceFormatKHR* formats;
    u32 present_mode_count;
    VkPresentModeKHR* present_modes;
} vulkan_swapchain_support_info;

typedef struct vk_device {
    VkPhysicalDevice physical_device;
    VkDevice logical_device;
    vulkan_swapchain_support_info swapchain_support;
    i32 graphics_queue_index;
    i32 present_queue_index;
    i32 transfer_queue_index;

    VkQueue graphics_queue;
    VkQueue present_queue;
    VkQueue transfer_queue;

    VkPhysicalDeviceProperties properties;
    VkPhysicalDeviceFeatures features;
    VkPhysicalDeviceMemoryProperties memory;
} vk_device;

typedef struct vk_context {
    VkInstance vk_instance;
    VkAllocationCallbacks* vk_allocator;
    VkSurfaceKHR vk_surface;
#if defined(_DEBUG)
    VkDebugUtilsMessengerEXT vk_debugger; // Debugger is only available in debug mode
#endif

    vk_device device;
} vk_context;

