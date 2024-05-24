#include "signals.h"
#include "bilt_memory.h"
#include "dsa/bilt_array.h"
#include "output.h"

#include "defines.h"

typedef struct signal_listener {
    void* listener;
    onSignal sig_call;
} signal_listener;

typedef struct signal_id {
    signal_listener* signals;
} signal_id;

typedef struct signal_state {
    signal_id listened[MAX_IDS];
} signal_state;

static b8 isEnabled = FALSE;
static signal_state state;

b8 enable_signals() {
    if(isEnabled == TRUE) {
        return FALSE;
    }
    mzero(&state, sizeof(state));
    isEnabled = TRUE;
    return TRUE;
}
void disable_signals() {
    for(u16 i = 0; i < MAX_IDS; ++i) {
        if(state.listened[i].signals != 0) {
            destroy_array(state.listened[i].signals);
            state.listened[i].signals = 0;
        }
    }
}

b8 listen_for(u16 id, void* listener, onSignal sig) {
    if(isEnabled == FALSE) {
        return FALSE;
    }
    if(state.listened[id].signals == 0) {
        state.listened[id].signals = create_array(signal_listener);
    }
    u64 listener_count = get_array_length(state.listened[id].signals);
    for(u64 i = 0; i < listener_count; ++i) {
        if(state.listened[id].signals[i].listener == listener) {
            LOG_WARN("Already listening for signal with this ID.");
            return FALSE;
        }
    }
    signal_listener sign;
    sign.listener = listener;
    sign.sig_call = sig;
    push_array(state.listened[id].signals, sign);

    return TRUE;
}

b8 unlisten_for(u16 id, void* listener, onSignal sig) {
    if(isEnabled == FALSE) {
        return FALSE;
    }

    if(state.listened[id].signals == 0) {
        LOG_WARN("Not listening for signal with this ID.");
        return FALSE;
    }

    u64 listened_count = get_array_length(state.listened[id].signals);
    for(u64 i = 0; i < listened_count; ++i) {
        signal_listener e = state.listened[id].signals[i];
        if(e.listener == listener && e.sig_call == sig) {
            signal_listener temps;
            remove_array(state.listened[id].signals, i, &temps);
            return TRUE;
        }
    }
    return FALSE;
}

b8 sing_signal(u16 id, void* singer, signal data) {
    if(isEnabled == FALSE) {
        return FALSE;
    }

    if(state.listened[id].signals == 0) {
        return FALSE;
    }

    u64 listened_count = get_array_length(state.listened[id].signals);
    for(u64 i = 0; i < listened_count; ++i) {
        signal_listener e = state.listened[id].signals[i];
        if(e.sig_call(id, singer, e.listener, data)) {
            return TRUE;
        }
    }

    return FALSE;
}