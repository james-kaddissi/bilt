/*
 The purpose of this header file is to define behaviors and formatting for assertions in the engine.
*/
#pragma once

#include "defines.h"

// enables assertions (comment to disable)
#define ASSERTIONS_ON

// define the debugBreak macro based on the compiler
#ifdef ASSERTIONS_ON
#if _MSC_VER
#include <intrin.h>
#define debugBreak() __debugbreak()
#else
#define debugBreak() __builtin_trap()
#endif

BILT_API void assertion_failure(const char *expression, const char *message, const char *file, i32 line);

// custom definition for assertion
#define BILT_ASSERT(expr)                                     \
    {                                                         \
        if (expr)                                             \
        {                                                     \
        }                                                     \
        else                                                  \
        {                                                     \
            assertion_failure(#expr, "", __FILE__, __LINE__); \
            debugBreak();                                     \
        }                                                     \
    }                                                         \
// assertion with a desired message output
#define BILT_ASSERT_MESSAGE(expr, message)                         \
    {                                                              \
        if (expr)                                                  \
        {                                                          \
        }                                                          \
        else                                                       \
        {                                                          \
            assertion_failure(#expr, message, __FILE__, __LINE__); \
            debugBreak();                                          \
        }                                                          \
    }

#ifdef _DEBUG // macro compiler defines if in debug build
// same behavior as normal assertion, just only enabled in debug builds.
#define BILT_ASSERT_DEBUG(expr)                                          \
    {                                                                    \
        if (expr)                                                        \
        {                                                                \
        }                                                                \
        else                                                             \
        {                                                                \
            assertion_failure(#expr, "" message "", __FILE__, __LINE__); \
            debugBreak();                                                \
        }                                                                \
    }
#else
#define BILT_ASSERT_DEBUG(expr) // do nothing
#endif

#else // assertions_on not defined: do nothing
#define BILT_ASSERT(expr)
#define BILT_ASSERT_MESSAGE(expr, message);
#define BILT_ASSERT_DEBUG(expr);
#endif