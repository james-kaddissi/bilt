#include "game.h"

#include <main/output.h>

b8 gameInitialize(gameObject* game) {
    LOG_DEBUG("gameInitialize() called!");
    return TRUE;
}

b8 gameUpdate(gameObject* game, f32 deltaTime) {
    return TRUE;
}

b8 gameRender(gameObject* game, f32 deltaTime) {
    return TRUE;
}

void gameOnResize(gameObject* game, u32 width, u32 height) {
}