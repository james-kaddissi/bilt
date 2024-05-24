#pragma once

#include "main/app.h"
#include "main/output.h"
#include "game_object.h"
#include "main/bilt_memory.h"

extern b8 initializeGame(gameObject* gameRes);

int main(void) {
    enable_memory();
    
    gameObject game;
    if(!initializeGame(&game)) {
        LOG_FATAL("Game initialization failed!");
        return -1;
    }

    if (!game.render || !game.update || !game.initialize || !game.onResize) {
        LOG_FATAL("GameObject pointers not setup properly!");
        return -2;
    }

    if (!initialize_application(&game)) {
        LOG_INFO("Application init failed!");
        return 1;
    }

    if(!run_application()) {
        LOG_INFO("Application termination failed!");
        return 2;
    }

    disable_memory();
    return 0;
}