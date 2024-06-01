#include "renderer/backend.h"
#include "renderer/vulkan/vulkan_api.h"

#include "main/output.h"

b8 initialize_backend(backend_api api, struct active_platform* active_plat, backend_context* context) {
    context->active_plat = active_plat;
    // select the appropriate graphics api
    if(api == VULKAN_API){
        context->initialize_backend = initialize_vulkan_api;
        context->shutdown_backend = disable_vulkan_api;
        context->handle_resize = vulkan_handle_resize;
        context->start_frame = vulkan_start_frame;
        context->end_frame = vulkan_end_frame;
        return TRUE;
    } else if(api == OPENGL_API) {
        LOG_INFO("OpenGL api not created yet.");
    } else if(api == DIRECTX_API) {
        LOG_INFO("DirectX api not created yet.");
    }
    return FALSE;
}
void disable_backend(backend_context* context) {
    context->initialize_backend = 0;
    context->shutdown_backend = 0;
    context->handle_resize = 0;
    context->start_frame = 0;
    context->end_frame = 0;
}