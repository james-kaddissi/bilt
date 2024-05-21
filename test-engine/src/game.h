#pragma once

#include <defines.h>
#include <game_object.h>

typedef struct gameState {
    f32 delta_time;
} gameState;

b8 gameInitialize(gameObject* game);

b8 gameUpdate(gameObject* game, f32 deltaTime);

b8 gameRender(gameObject* game, f32 deltaTime);

void gameOnResize(gameObject* game, u32 width, u32 height);