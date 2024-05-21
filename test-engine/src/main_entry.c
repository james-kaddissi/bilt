#include "game.h"
#include <main_entry.h>
#include <platforms/unify_platforms.h>

b8 initializeGame(gameObject* gameRes) {
    gameRes->init.initPosX = 100;
    gameRes->init.initPosY = 100;
    gameRes->init.initWidth = 1280;
    gameRes->init.initHeight = 720;
    gameRes->init.name = "Bilt Game Engine";
    gameRes->update = gameUpdate;
    gameRes->render = gameRender;
    gameRes->initialize = gameInitialize;
    gameRes->onResize = gameOnResize;
    gameRes->state = mem_alloc(sizeof(gameState), FALSE);
    return TRUE;
}