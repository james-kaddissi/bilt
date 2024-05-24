#include "dsa/bilt_array.h"

#include "main/bilt_memory.h"
#include "main/output.h"

void* _create_array(u64 length, u64 step) {
    u64 hsize = ARRAY_FIELD * sizeof(u64);
    u64 arr_size = length * step;
    u64* temp = mallocate(hsize + arr_size, MEM_TYPE_ARRAY);
    mset(temp, 0, hsize + arr_size);
    temp[ARRAY_SIZE] = length;
    temp[ARRAY_LENGTH] = 0;
    temp[ARRAY_STEP] = step;
    return (void*)(temp + ARRAY_FIELD);
}

void _destroy_array(void* arr) {
    u64* h = (u64*)arr - ARRAY_FIELD;
    u64 hsize = ARRAY_FIELD * sizeof(u64);
    u64 total_size = hsize + h[ARRAY_SIZE] * h[ARRAY_STEP];
    mfree(h, total_size, MEM_TYPE_ARRAY);
}

u64 _get_array_field(void* arr, u64 field) {
    u64* h = (u64*)arr - ARRAY_FIELD;
    return h[field];
}

void _set_array_field(void* arr, u64 field, u64 value) {
    u64* h = (u64*)arr - ARRAY_FIELD;
    h[field] = value;
}

void* _resize_array(void* arr) {
    u64 length = get_array_length(arr);
    u64 step = get_array_step(arr);
    void* temp = _create_array(
        (ARRAY_SCALE * get_array_size(arr)),
        step);
    mcopy(temp, arr, length * step);

    _set_array_field(temp, ARRAY_LENGTH, length);
    _destroy_array(arr);
    return temp;
}

void* _push_array(void* arr, const void* ptr) {
    u64 length = get_array_length(arr);
    u64 step = get_array_step(arr);
    if (length >= get_array_size(arr)) {
        arr = _resize_array(arr);
    }

    u64 sum = (u64)arr;
    sum += (length * step);
    mcopy((void*)sum, ptr, step);
    _set_array_field(arr, ARRAY_LENGTH, length + 1);
    return arr;
}

void _pop_array(void* arr, void* dest) {
    u64 length = get_array_length(arr);
    u64 step = get_array_step(arr);
    u64 sum = (u64)arr;
    sum += ((length - 1) * step);
    mcopy(dest, (void*)sum, step);
    _set_array_field(arr, ARRAY_LENGTH, length - 1);
}

void* _remove_array(void* array, u64 i, void* dest) {
    u64 length = get_array_length(array);
    u64 step = get_array_step(array);
    if (i >= length) {
        LOG_ERROR("Index outside the bounds of this array! Length: %i, i: %i", length, i);
        return array;
    }

    u64 sum = (u64)array;
    mcopy(dest, (void*)(sum + (i * step)), step);

    if (i != length - 1) {
        mcopy(
            (void*)(sum + (i * step)),
            (void*)(sum + ((i + 1) * step)),
            step * (length - i));
    }

    _set_array_field(array, ARRAY_LENGTH, length - 1);
    return array;
}

void* _darray_insert_at(void* arr, u64 i, void* ptr) {
    u64 length = get_array_length(arr);
    u64 step = get_array_step(arr);
    if (i >= length) {
        LOG_ERROR("Index beyond max length! Length: %i, i: %i", length, i);
        return arr;
    }
    if (length >= get_array_size(arr)) {
        arr = _resize_array(arr);
    }

    u64 sum = (u64)arr;


    if (i != length - 1) {
        mcopy(
            (void*)(sum + ((i + 1) * step)),
            (void*)(sum + (i * step)),
            step * (length - i));
    }

    mcopy((void*)(sum + (i * step)), ptr, step);

    _set_array_field(arr, ARRAY_LENGTH, length + 1);
    return arr;
}