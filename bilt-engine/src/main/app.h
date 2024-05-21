/*
This header file contains the relevant definitions of application initialization functions.
*/
#pragma once

#include "defines.h"

struct gameObject;

typedef struct app_init {
    i16 initPosX; // starting window x position
    i16 initPosY; // y position
    i16 initWidth; // starting window width
    i16 initHeight; // height
    char* name; // application name
} app_init;

BILT_API b8 initialize_application(struct gameObject* game);
BILT_API b8 run_application();