#pragma once
#include "main/app.h"

typedef struct gameObject {
    app_init init;
    b8 (*initialize)(struct gameObject* game);
    b8 (*update)(struct gameObject* game, f32 delta_time);
    b8 (*render)(struct gameObject* game, f32 delta_time);
    void (*onResize)(struct gameObject* game, u32 width, u32 height);
    void* state;
} gameObject;