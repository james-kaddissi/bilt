#pragma once

#include "renderer/renderers.inl"

struct active_platform;

b8 initialize_backend(backend_api api, struct active_platform* active_plat, backend_context* context);
void disable_backend(backend_context* context);