#include "renderer/vulkan/vulkan_device.h"
#include "main/output.h"
#include "main/bilt_memory.h"
#include "dsa/bilt_string.h"
#include "dsa/bilt_array.h"

typedef struct vulkan_physical_device_requirements {
    b8 graphics;
    b8 present;
    b8 compute;
    b8 transfer;
    const char** device_extension_names;
    b8 sampler_anisotropy;
    b8 discrete_gpu;
} vulkan_physical_device_requirements;

typedef struct vulkan_physical_device_queue_family_info {
    u32 graphics_family_index;
    u32 present_family_index;
    u32 compute_family_index;
    u32 transfer_family_index;
} vulkan_physical_device_queue_family_info;

b8 select_physical_device(vk_context* context);
b8 physical_device_meets_requirements(
    VkPhysicalDevice device,
    VkSurfaceKHR surface,
    const VkPhysicalDeviceProperties* properties,
    const VkPhysicalDeviceFeatures* features,
    const vulkan_physical_device_requirements* requirements,
    vulkan_physical_device_queue_family_info* out_queue_family_info,
    vulkan_swapchain_support_info* out_swapchain_support);

b8 initialize_vulkan_device(vk_context* context) {
    if (!select_physical_device(context)) {
        return FALSE;
    }

    return TRUE;
}
void disable_vulkan_device(vk_context* context) {

}

void vulkan_device_query_swapchain_support(
    VkPhysicalDevice physical_device,
    VkSurfaceKHR surface,
    vulkan_swapchain_support_info* out_support_info) {
    VK_CHECK(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(
        physical_device,
        surface,
        &out_support_info->capabilities));

    VK_CHECK(vkGetPhysicalDeviceSurfaceFormatsKHR(
        physical_device,
        surface,
        &out_support_info->format_count,
        0));

    if (out_support_info->format_count != 0) {
        if (!out_support_info->formats) {
            out_support_info->formats = mallocate(sizeof(VkSurfaceFormatKHR) * out_support_info->format_count, MEM_TYPE_RENDERER);
        }
        VK_CHECK(vkGetPhysicalDeviceSurfaceFormatsKHR(
            physical_device,
            surface,
            &out_support_info->format_count,
            out_support_info->formats));
    }

    VK_CHECK(vkGetPhysicalDeviceSurfacePresentModesKHR(
        physical_device,
        surface,
        &out_support_info->present_mode_count,
        0));
    if (out_support_info->present_mode_count != 0) {
        if (!out_support_info->present_modes) {
            out_support_info->present_modes = mallocate(sizeof(VkPresentModeKHR) * out_support_info->present_mode_count, MEM_TYPE_RENDERER);
        }
        VK_CHECK(vkGetPhysicalDeviceSurfacePresentModesKHR(
            physical_device,
            surface,
            &out_support_info->present_mode_count,
            out_support_info->present_modes));
    }
}

b8 select_physical_device(vk_context* context) {
    u32 physical_device_count = 0;
    VK_CHECK(vkEnumeratePhysicalDevices(context->vk_instance, &physical_device_count, 0));
    if (physical_device_count == 0) {
        LOG_FATAL("No devices which support Vulkan were found.");
        return FALSE;
    }

    VkPhysicalDevice physical_devices[physical_device_count];
    VK_CHECK(vkEnumeratePhysicalDevices(context->vk_instance, &physical_device_count, physical_devices));
    for (u32 i = 0; i < physical_device_count; ++i) {
        VkPhysicalDeviceProperties properties;
        vkGetPhysicalDeviceProperties(physical_devices[i], &properties);

        VkPhysicalDeviceFeatures features;
        vkGetPhysicalDeviceFeatures(physical_devices[i], &features);

        VkPhysicalDeviceMemoryProperties memory;
        vkGetPhysicalDeviceMemoryProperties(physical_devices[i], &memory);

        vulkan_physical_device_requirements requirements = {};
        requirements.graphics = TRUE;
        requirements.present = TRUE;
        requirements.transfer = TRUE;
        requirements.sampler_anisotropy = TRUE;
        requirements.discrete_gpu = TRUE;
        requirements.device_extension_names = create_array(const char*);
        push_array(requirements.device_extension_names, &VK_KHR_SWAPCHAIN_EXTENSION_NAME);

        vulkan_physical_device_queue_family_info queue_info = {};
        b8 result = physical_device_meets_requirements(
            physical_devices[i],
            context->vk_surface,
            &properties,
            &features,
            &requirements,
            &queue_info,
            &context->device.swapchain_support);

        if (result) {
            LOG_INFO("Selected device: '%s'.", properties.deviceName);
           
            switch (properties.deviceType) {
                default:
                case VK_PHYSICAL_DEVICE_TYPE_OTHER:
                    LOG_INFO("GPU type is Unknown.");
                    break;
                case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU:
                    LOG_INFO("GPU type is Integrated.");
                    break;
                case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU:
                    LOG_INFO("GPU type is Descrete.");
                    break;
                case VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU:
                    LOG_INFO("GPU type is Virtual.");
                    break;
                case VK_PHYSICAL_DEVICE_TYPE_CPU:
                    LOG_INFO("GPU type is CPU.");
                    break;
            }

            LOG_INFO(
                "GPU Driver version: %d.%d.%d",
                VK_VERSION_MAJOR(properties.driverVersion),
                VK_VERSION_MINOR(properties.driverVersion),
                VK_VERSION_PATCH(properties.driverVersion));

            LOG_INFO(
                "Vulkan API version: %d.%d.%d",
                VK_VERSION_MAJOR(properties.apiVersion),
                VK_VERSION_MINOR(properties.apiVersion),
                VK_VERSION_PATCH(properties.apiVersion));

            for (u32 j = 0; j < memory.memoryHeapCount; ++j) {
                f32 memory_size_gib = (((f32)memory.memoryHeaps[j].size) / 1024.0f / 1024.0f / 1024.0f);
                if (memory.memoryHeaps[j].flags & VK_MEMORY_HEAP_DEVICE_LOCAL_BIT) {
                    LOG_INFO("Local GPU memory: %.2f GiB", memory_size_gib);
                } else {
                    LOG_INFO("Shared System memory: %.2f GiB", memory_size_gib);
                }
            }

            context->device.physical_device = physical_devices[i];
            context->device.graphics_queue_index = queue_info.graphics_family_index;
            context->device.present_queue_index = queue_info.present_family_index;
            context->device.transfer_queue_index = queue_info.transfer_family_index;


            context->device.properties = properties;
            context->device.features = features;
            context->device.memory = memory;
            break;
        }
    }


    if (!context->device.physical_device) {
        LOG_ERROR("No physical devices were found which meet the requirements.");
        return FALSE;
    }

    LOG_INFO("Physical device selected.");
    return TRUE;
}

b8 physical_device_meets_requirements(
    VkPhysicalDevice device,
    VkSurfaceKHR surface,
    const VkPhysicalDeviceProperties* properties,
    const VkPhysicalDeviceFeatures* features,
    const vulkan_physical_device_requirements* requirements,
    vulkan_physical_device_queue_family_info* out_queue_info,
    vulkan_swapchain_support_info* out_swapchain_support) {
    out_queue_info->graphics_family_index = -1;
    out_queue_info->present_family_index = -1;
    out_queue_info->compute_family_index = -1;
    out_queue_info->transfer_family_index = -1;

    if (requirements->discrete_gpu) {
        if (properties->deviceType != VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
            LOG_INFO("Device is not a discrete GPU, and one is required. Skipping.");
            return FALSE;
        }
    }

    u32 queue_family_count = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count, 0);
    VkQueueFamilyProperties queue_families[queue_family_count];
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count, queue_families);

    
    LOG_INFO("Graphics | Present | Compute | Transfer | Name");
    u8 min_transfer_score = 255;
    for (u32 i = 0; i < queue_family_count; ++i) {
        u8 current_transfer_score = 0;


        if (queue_families[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            out_queue_info->graphics_family_index = i;
            ++current_transfer_score;
        }


        if (queue_families[i].queueFlags & VK_QUEUE_COMPUTE_BIT) {
            out_queue_info->compute_family_index = i;
            ++current_transfer_score;
        }


        if (queue_families[i].queueFlags & VK_QUEUE_TRANSFER_BIT) {
            if (current_transfer_score <= min_transfer_score) {
                min_transfer_score = current_transfer_score;
                out_queue_info->transfer_family_index = i;
            }
        }

        VkBool32 supports_present = VK_FALSE;
        VK_CHECK(vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &supports_present));
        if (supports_present) {
            out_queue_info->present_family_index = i;
        }
    }

    LOG_INFO("       %d |       %d |       %d |        %d | %s",
          out_queue_info->graphics_family_index != -1,
          out_queue_info->present_family_index != -1,
          out_queue_info->compute_family_index != -1,
          out_queue_info->transfer_family_index != -1,
          properties->deviceName);

    if (
        (!requirements->graphics || (requirements->graphics && out_queue_info->graphics_family_index != -1)) &&
        (!requirements->present || (requirements->present && out_queue_info->present_family_index != -1)) &&
        (!requirements->compute || (requirements->compute && out_queue_info->compute_family_index != -1)) &&
        (!requirements->transfer || (requirements->transfer && out_queue_info->transfer_family_index != -1))) {
        LOG_INFO("Device meets queue requirements.");
        LOG_TRACE("Graphics Family Index: %i", out_queue_info->graphics_family_index);
        LOG_TRACE("Present Family Index:  %i", out_queue_info->present_family_index);
        LOG_TRACE("Transfer Family Index: %i", out_queue_info->transfer_family_index);
        LOG_TRACE("Compute Family Index:  %i", out_queue_info->compute_family_index);

        vulkan_device_query_swapchain_support(
            device,
            surface,
            out_swapchain_support);

        if (out_swapchain_support->format_count < 1 || out_swapchain_support->present_mode_count < 1) {
            if (out_swapchain_support->formats) {
                mfree(out_swapchain_support->formats, sizeof(VkSurfaceFormatKHR) * out_swapchain_support->format_count, MEM_TYPE_RENDERER);
            }
            if (out_swapchain_support->present_modes) {
                mfree(out_swapchain_support->present_modes, sizeof(VkPresentModeKHR) * out_swapchain_support->present_mode_count, MEM_TYPE_RENDERER);
            }
            LOG_INFO("Required swapchain support not present, skipping device.");
            return FALSE;
        }

        if (requirements->device_extension_names) {
            u32 available_extension_count = 0;
            VkExtensionProperties* available_extensions = 0;
            VK_CHECK(vkEnumerateDeviceExtensionProperties(
                device,
                0,
                &available_extension_count,
                0));
            if (available_extension_count != 0) {
                available_extensions = mallocate(sizeof(VkExtensionProperties) * available_extension_count, MEM_TYPE_RENDERER);
                VK_CHECK(vkEnumerateDeviceExtensionProperties(
                    device,
                    0,
                    &available_extension_count,
                    available_extensions));

                u32 required_extension_count = get_array_length(requirements->device_extension_names);
                for (u32 i = 0; i < required_extension_count; ++i) {
                    b8 found = FALSE;
                    for (u32 j = 0; j < available_extension_count; ++j) {
                        if (strEq(requirements->device_extension_names[i], available_extensions[j].extensionName)) {
                            found = TRUE;
                            break;
                        }
                    }

                    if (!found) {
                        LOG_INFO("Required extension not found: '%s', skipping device.", requirements->device_extension_names[i]);
                        mfree(available_extensions, sizeof(VkExtensionProperties) * available_extension_count, MEM_TYPE_RENDERER);
                        return FALSE;
                    }
                }
            }
            mfree(available_extensions, sizeof(VkExtensionProperties) * available_extension_count, MEM_TYPE_RENDERER);
        }

        // Sampler anisotropy
        if (requirements->sampler_anisotropy && !features->samplerAnisotropy) {
            LOG_INFO("Device does not support samplerAnisotropy, skipping.");
            return FALSE;
        }

        // Device meets all requirements.
        return TRUE;
    }

    return FALSE;
}