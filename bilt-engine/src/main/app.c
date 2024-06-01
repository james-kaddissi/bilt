#include "main/app.h"
#include "main/output.h"
#include "main/bilt_memory.h"
#include "main/signals.h"
#include "main/inputs.h"
#include "main/clock_cycle.h"

#include "platforms/unify_platforms.h"

#include "game_object.h"

// holds information on the running state of the application
typedef struct appState {
    gameObject* game;
    b8 isAlive;
    b8 isTerminated;
    active_platform active_plat;
    i16 width;
    i16 height;
    clock clock;
    f64 prev_time;
} appState;

static b8 hasInit = FALSE;
static appState as;

b8 on_signal(u16 id, void* singer, void* listener, signal sig);
b8 on_key(u16 id, void* singer, void* listener, signal sig);

b8 initialize_application(gameObject* game) {
    // initialize backend
    if(hasInit){
        LOG_ERROR("Can only create an application once!");
        return FALSE;
    }
    as.game = game;
    start_log();
    enable_inputs();

    as.isAlive = TRUE;
    as.isTerminated = FALSE;
    //initialize signals
    if(!enable_signals()) {
        LOG_ERROR("Signal system failed enabling. Failed to continue.");
        return FALSE;
    }
    listen_for(APP_QUIT, 0, on_signal);
    listen_for(KEY_PRESSED, 0, on_key);
    listen_for(KEY_RELEASED, 0, on_key);
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
    // initialize application clock
    start_clock(&as.clock);
    update_clock(&as.clock);
    as.prev_time = as.clock.running_duration;
    f64 app_elapsed_time = 0;
    u8 frames = 0;
    f64 target_fps = 1.0f/60;


    LOG_INFO(get_total_mallocation());
    // main application loop
    while(as.isAlive) {
        if(!message_loop(&as.active_plat)) {
            as.isAlive = FALSE;
        }
        // update and render game if not terminated
        if(!as.isTerminated) {
            // update clock cycle
            update_clock(&as.clock);
            f64 curr_time = as.clock.running_duration;
            f64 delta_time = curr_time - as.prev_time;
            f64 frame_time = get_time();

            if(!as.game->update(as.game, (f32)delta_time)) {
                LOG_FATAL("Update sequence failed. Terminating process.");
                as.isAlive = FALSE;
                break;
            }
            if(!as.game->render(as.game, (f32)delta_time)) {
                LOG_FATAL("Renderer failed. Terminating process.");
                as.isAlive = FALSE;
                break;
            }
            // clock cycle cleanup
            f64 end_frame_time = get_time();
            f64 frame_delta = end_frame_time - frame_time;
            app_elapsed_time += frame_delta;
            f64 leftover_time = target_fps - frame_delta;
            if(leftover_time > 0) {
                u64 ltime = leftover_time * 1000;
                b8 smoothing = FALSE; // if true then every frame takes no quicker than the target fps, by sleeping the rest of the duration
                if (ltime > 0 && smoothing) {
                    timed_sleep(ltime-1);
                }
                frames++;
            }

            update_inputs(delta_time);
            as.prev_time = curr_time;
        }
    }
    as.isAlive = FALSE;
    // deactivate platform
    unlisten_for(APP_QUIT, 0, on_signal);
    listen_for(KEY_PRESSED, 0, on_signal);
    listen_for(KEY_RELEASED, 0, on_signal);
    disable_signals();
    disable_inputs();
    deactivate_platform(&as.active_plat);
    return TRUE;
}
b8 on_signal(u16 id, void* singer, void* listener, signal sig) {
    switch (id) {
        case APP_QUIT: {
            LOG_INFO("APP_QUIT recieved, shutting down.\n");
            as.isAlive = FALSE;
            return TRUE;
        }
    }

    return FALSE;
}

b8 on_key(u16 id, void* singer, void* listener, signal sig) {
    if (id == KEY_PRESSED) {
        u16 key_code = sig.data.u16[0];
        if (key_code == KEY_ESCAPE) {
            signal data = {};
            sing_signal(APP_QUIT, 0, data);

            return TRUE;
        } else {
            LOG_DEBUG("'%c' key pressed in window.", key_code);
        }
    } else if (id == KEY_RELEASED) {
        u16 key_code = sig.data.u16[0];
        LOG_DEBUG("'%c' key released in window.", key_code);
    }
    return FALSE;
}