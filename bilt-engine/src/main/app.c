#include "app.h"
#include "game_object.h"
#include "output.h"
#include "platforms/unify_platforms.h"

// holds information on the running state of the application
typedef struct appState {
    gameObject* game;
    b8 isAlive;
    b8 isTerminated;
    active_platform active_plat;
    i16 width;
    i16 height;
    f64 prevStep;
} appState;

static b8 hasInit = FALSE;
static appState as;

b8 initialize_application(gameObject* game) {
    // initialize backend
    if(hasInit){
        LOG_ERROR("Can only create an application once!");
        return FALSE;
    }
    as.game = game;
    start_log();

    as.isAlive = TRUE;
    as.isTerminated = FALSE;
    // initialize platform
    if(!initialize_platform(&as.active_plat, game->init.name, game->init.initPosX, game->init.initPosY, game->init.initWidth, game->init.initHeight)) {
        return FALSE;
    }

    // initialize game
    if(!as.game->initialize(as.game)) {
        LOG_FATAL("Game initialization failed!");
        return FALSE;
    }
    // handle resize
    as.game->onResize(as.game,as.width, as.height);
    hasInit = TRUE;
    return TRUE;
}

b8 run_application() {
    // main application loop
    while(as.isAlive) {
        if(!message_loop(&as.active_plat)) {
            as.isAlive = FALSE;
        }
        // update and render game if not terminated
        if(!as.isTerminated) {
            if(!as.game->update(as.game, (f32)0)) {
                LOG_FATAL("Update sequence failed. Terminating process.");
                as.isAlive = FALSE;
                break;
            }
            if(!as.game->render(as.game, (f32)0)) {
                LOG_FATAL("Renderer failed. Terminating process.");
                as.isAlive = FALSE;
                break;
            }
        }
    }
    as.isAlive = FALSE;
    // deactivate platform
    deactivate_platform(&as.active_plat);
    return TRUE;
}
