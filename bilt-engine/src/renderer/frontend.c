#include "main/output.h"
#include "main/bilt_memory.h"

#include "renderer/frontend.h"
#include "renderer/backend.h"

struct active_platform;
struct backend_context* context = 0;

b8 enable_renderer(const char* name, struct active_platform* active_plat) {
    context = mallocate(sizeof(backend_context), MEM_TYPE_RENDERER);
    initialize_backend(VULKAN_API, active_plat, context);
    context->frame_count = 0;

    if(!context->initialize_backend(context, name, active_plat)) {
        LOG_FATAL("Backend failed to initialize.");
        return FALSE;
    }
    return TRUE;
}

void disable_renderer() {
    context->shutdown_backend(context);
    mfree(context, sizeof(backend_context), MEM_TYPE_RENDERER);
}

b8 frontend_start_frame(f32 delta_time) {
    return context->start_frame(context, delta_time);
}

b8 frontend_end_frame(f32 delta_time) {
    b8 response = context->end_frame(context, delta_time);
    context->frame_count++;
    return response;
}

b8 generate_frame(frame_data* fdata) {
    if(frontend_start_frame(fdata->delta_time)) {
        b8 response = frontend_end_frame(fdata->delta_time);
        if(!response) {
            LOG_ERROR("Frame ending failed.");
            return FALSE;
        }
    }
    return TRUE;
}