#pragma once

#include "renderer/renderers.inl"

struct visual_data;
struct active_platform;

b8 enable_renderer(const char* name, struct active_platform* active_plat);
void disable_renderer();

void handle_resizing(u16 width, u16 height);

b8 generate_frame(frame_data* fdata);