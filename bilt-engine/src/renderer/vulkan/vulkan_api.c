#include "renderer/vulkan/vulkan_api.h"

#include "renderer/vulkan/vulkan_defines.inl"

#include "main/output.h"

static vk_context vk;

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
    vk_create_info.enabledExtensionCount = 0;
    vk_create_info.ppEnabledExtensionNames = 0;
    vk_create_info.enabledLayerCount = 0;
    vk_create_info.ppEnabledLayerNames = 0;
    VkResult vk_result = vkCreateInstance(&vk_create_info, vk.vk_allocator, &vk.vk_instance);
    if(vk_result != VK_SUCCESS) {
        LOG_ERROR("Vulkan initializitation failed: %u", vk_result);
        return FALSE;
    }
    LOG_INFO("Vulkan initialized.")
    return TRUE;
}

void disable_vulkan_api(backend_context* context){

}
void vulkan_handle_resize(backend_context* context, u16 width, u16 height){

}
b8 vulkan_start_frame(backend_context* context, f32 delta_time){
    return TRUE;
}
b8 vulkan_end_frame(backend_context* context, f32 delta_time) {
    return TRUE;
}