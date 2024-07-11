#include "renderer/vulkan/vulkan_api.h"
#include "renderer/vulkan/vulkan_platforms.h"
#include "renderer/vulkan/vulkan_defines.inl"
#include "renderer/vulkan/vulkan_device.h"

#include "main/output.h"
#include "dsa/bilt_string.h"
#include "dsa/bilt_array.h"
#include "platforms/unify_platforms.h"

static vk_context vk;

VKAPI_ATTR VkBool32 VKAPI_CALL vk_debug(VkDebugUtilsMessageSeverityFlagBitsEXT message_severity, VkDebugUtilsMessageTypeFlagsEXT message_type, const VkDebugUtilsMessengerCallbackDataEXT* callback_data, void* data);

b8 initialize_vulkan_api(backend_context* context, const char* name, struct active_platform* active_plat) {
    vk.vk_allocator = 0;

    // initialize Vulkan instance
    VkApplicationInfo vk_app_info = {VK_STRUCTURE_TYPE_APPLICATION_INFO};
    vk_app_info.apiVersion = VK_API_VERSION_1_3;
    vk_app_info.pApplicationName = name;
    vk_app_info.applicationVersion = VK_MAKE_API_VERSION(1, 1, 0,0); // version of game
    vk_app_info.pEngineName = "Bilt";
    vk_app_info.engineVersion = VK_MAKE_API_VERSION(1, 1, 0, 0); // major, minor, patch
    VkInstanceCreateInfo vk_create_info = {VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO};
    vk_create_info.pApplicationInfo = &vk_app_info;
    const char** extensions = create_array(const char*);
    push_array(extensions, &VK_KHR_SURFACE_EXTENSION_NAME);
    get_extensions(&extensions);
    #if defined(_DEBUG)
    push_array(extensions, &VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    LOG_DEBUG("Required vulkan extensions: ");
    for (u32 i = 0; i < get_array_length(extensions); ++i) {
        LOG_DEBUG(extensions[i]);
    }
    #endif
    vk_create_info.enabledExtensionCount = get_array_length(extensions);
    vk_create_info.ppEnabledExtensionNames = extensions;
    const char** validNames = 0;
    u32 validCount = 0;
    #if defined(_DEBUG)
    validNames = create_array(const char*);
    push_array(validNames, &"VK_LAYER_KHRONOS_validation");
    validCount = get_array_length(validNames);
    u32 currentLayerCount = 0;
    VK_CHECK(vkEnumerateInstanceLayerProperties(&currentLayerCount, 0));
    VkLayerProperties* layer_properties = create_array_sized(VkLayerProperties, currentLayerCount);
    VK_CHECK(vkEnumerateInstanceLayerProperties(&currentLayerCount, layer_properties));
    for(u32 i = 0; i < validCount; ++i) {
        b8 layerFound = FALSE;
        for(u32 j = 0; j < currentLayerCount; ++j) {
            if(strEq(validNames[i], layer_properties[j].layerName)) {
                layerFound = TRUE;
                break;
            }
        }
        if(!layerFound) {
            LOG_FATAL("Required layer missing: %s", validNames[i]);
            return FALSE;
        }
    }
    #endif
    vk_create_info.enabledLayerCount = validCount;
    vk_create_info.ppEnabledLayerNames = validNames;
    VK_CHECK(vkCreateInstance(&vk_create_info, vk.vk_allocator, &vk.vk_instance));
    LOG_INFO("Vulkan context instance initialized");
#if defined(_DEBUG)
    LOG_DEBUG("Enabled Vulkan Debugger");
    u32 log_level = VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT;
    VkDebugUtilsMessengerCreateInfoEXT debug_create_info = {VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT};
    debug_create_info.messageSeverity = log_level;
    debug_create_info.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT;
    debug_create_info.pfnUserCallback = vk_debug;
    PFN_vkCreateDebugUtilsMessengerEXT func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(vk.vk_instance, "vkCreateDebugUtilsMessengerEXT");
    BILT_ASSERT_MESSAGE(func, "Failed to create debug messenger!");
    VK_CHECK(func(vk.vk_instance, &debug_create_info, vk.vk_allocator, &vk.vk_debugger));
    LOG_DEBUG("Vulkan debugger created.");
#endif

    // device initialization
    LOG_DEBUG("Creating surface...");
    if (!initialize_vulkan_surface(active_plat, &vk)) {
        LOG_ERROR("Failed to initialize Vulkan surface.");
        return FALSE;
    }
    LOG_DEBUG("Surface created...");
    
    LOG_DEBUG("Creating device...");
    if (!initialize_vulkan_device(&vk)) {
        LOG_ERROR("Failed to initialize Vulkan device.");
        return FALSE;
    }
    LOG_DEBUG("Device created...");

    LOG_INFO("Vulkan initialized.")
    return TRUE;
}

void disable_vulkan_api(backend_context* context){
    // disable debugger
    LOG_DEBUG("Disabling vulkan.");
    if(vk.vk_debugger) {
        PFN_vkDestroyDebugUtilsMessengerEXT func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(vk.vk_instance, "vkDestroyDebugUtilsMessengerEXT");
        func(vk.vk_instance, vk.vk_debugger, vk.vk_allocator);
    }
    // disable vulkan instance
    vkDestroyInstance(vk.vk_instance, vk.vk_allocator);
}
void vulkan_handle_resize(backend_context* context, u16 width, u16 height){

}
b8 vulkan_start_frame(backend_context* context, f32 delta_time){
    return TRUE;
}
b8 vulkan_end_frame(backend_context* context, f32 delta_time) {
    return TRUE;
}

VKAPI_ATTR VkBool32 VKAPI_CALL vk_debug(VkDebugUtilsMessageSeverityFlagBitsEXT message_severity, VkDebugUtilsMessageTypeFlagsEXT message_type, const VkDebugUtilsMessengerCallbackDataEXT* callback_data, void* data) {
    switch(message_severity) {
        default:
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
            LOG_ERROR(callback_data->pMessage);
            break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
            LOG_WARN(callback_data->pMessage);
            break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
            LOG_INFO(callback_data->pMessage);
            break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
            LOG_TRACE(callback_data->pMessage);
            break;
    }
    return VK_FALSE;
}