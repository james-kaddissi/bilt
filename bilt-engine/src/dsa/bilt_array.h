#pragma once

#include "defines.h"

#define DEFAULT_ARRAY_SIZE 1
#define ARRAY_SCALE 2

enum
{
    ARRAY_SIZE,
    ARRAY_LENGTH,
    ARRAY_STEP,
    ARRAY_FIELD
};

BILT_API void *_create_array(u64 length, u64 step);
#define create_array(type) _create_array(DEFAULT_ARRAY_SIZE, sizeof(type));
#define create_array_sized(type, size) _create_array(size, sizeof(type));
BILT_API void _destroy_array(void *arr);
#define destroy_array(arr) _destroy_array(arr);

BILT_API void *_push_array(void *arr, const void *ptr);
#define push_array(arr, value)     \
    {                              \
        typeof(value) x = value;   \
        arr = _push_array(arr, &x) \
    }                              \
BILT_API void _pop_array(void *arr, void *dest);
#define pop_array(arr, ptr) _pop_array(arr, ptr);

BILT_API u64 _get_array_field(void *arr, u64 field);
#define get_array_size _get_array_field(arr, ARRAY_SIZE);
#define get_array_length _get_array_field(arr, ARRAY_LENGTH);
#define get_array_step _get_array_field(arr, ARRAY_STEP);
BILT_API void _set_array_field(void *arr, u64 field, u64 value);
#define clear_array(arr) _set_array_field(arr, ARRAY_LENGTH, 0);
#define set_array_length(arr, value) _set_array_field(arr, ARRAY_LENGTH, value);

BILT_API void *resize_array(void *arr);

BILT_API void *_remove_array(void *arr, u64 i, void *dest);
#define remove_array(arr, i, ptr) _remove_array(arr, i, ptr);
BILT_API void *_insert_array(void *arr, u64 i, void *ptr);
#define insert_array(arr, i, value)     \
    {                                   \
        typeof(value) x = value;        \
        arr = _insert_array(arr, i, &x) \
    }
