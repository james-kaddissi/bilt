#include <main/output.h>
#include <main/assertions.h>
#include <platforms/unify_platforms.h>

int main(void) {
    active_platform state;
    if(initialize_platform(&state, "Bilt Engine", 300, 300, 1280, 720)) {
        while(TRUE) {
            message_loop(&state);
        }
    }
    deactivate_platform(&state);
    return 0;
}