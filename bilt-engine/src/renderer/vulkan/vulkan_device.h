#pragma once

#include "renderer/vulkan/vulkan_defines.inl"

b8 initialize_vulkan_device(vk_context* context);
void disable_vulkan_device(vk_context* context);

void vulkan_device_query_swapchain_support(
    VkPhysicalDevice physical_device,
    VkSurfaceKHR vk_surface,
    vulkan_swapchain_support_info* out_support_info
);