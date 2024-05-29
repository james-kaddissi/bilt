#include "bilt_memory.h"

#include "output.h"
#include "platforms/unify_platforms.h"
#include "dsa/bilt_string.h"

#include <string.h>
#include <stdio.h>

// track memory allocations
struct memory_log {
    u64 total_allocation; // tracks the total memory allocated
    u64 allocated_types[MAX_MEM]; // tracks individual types memory allocated
};

static const char* mem_types_strings[MAX_MEM] = {
    "UNSPEC        ",
    "STRING        ",
    "ARRAY         ",
    "DICTIONARY    ",
    "QUEUE         ",
    "BINST         ",
    "APP           ",
    "GAME          ",
    "TASK          ",
    "RENDERER      ",
    "TEXTURE       ",
    "MATERIAL      ",
    "SHADER        ",
    "SOUND         ",  
    "OBJECT        ",
    "OBJECT_NODE   ",
    "SCENE         ",
    "TRANSFORM     ", 
};

static struct memory_log log;

void enable_memory() {
    zero_mem(&log, sizeof(log));
}

void disable_memory() {
    LOG_WARN("MEMORY CLEANUP NOT IMPLEMENTED");
}

void* mallocate(u64 size, mem_type type) {
    if(type == MEM_TYPE_UNSPEC) {
        LOG_WARN("mallocate called using an unspecified memory type."); // type unspec should generally never be used other than for quick testing
    }
    // update memory log
    log.total_allocation += size; // in bytes
    log.allocated_types[type] += size;


    void* block = mem_alloc(size, FALSE); // implement aligned memory. For now hard code FALSE.
    zero_mem(block, size);
    return block;
}

void mfree(void* block, u64 size, mem_type type) {
    if(type == MEM_TYPE_UNSPEC) {
        LOG_WARN("mfree called using an unspecified memory type."); // type unspec should generally never be used other than for quick testing
    }

    log.total_allocation -= size; // in bytes
    log.allocated_types[type] -= size;
    free_mem(block, FALSE);
}

void* mzero(void* block, u64 size) {
    return zero_mem(block, size);
}

void* mcopy(void* dest, const void* source, u64 size) {
    return copy_mem(dest, source, size);
}

void* mset(void* dest, i32 value, u64 size) {
    return set_mem(dest, value, size);
}

// do not call in release builds. testing only. 
char* get_total_mallocation() {
    const u64 size = 10000;
    char msg[size] = "Memory Usage by Types: \n";
    u64 len = strlen(msg);

    for (u32 i = 0; i < MAX_MEM; ++i) {
        char q[3] = "XB";
        float amt = 1.0f;
        if(log.allocated_types[i] >= GB) {
            q[0] = 'G';
            amt = log.allocated_types[i] / (float)GB;
        } else if(log.allocated_types[i] >= MB) {
            q[0] = 'M';
            amt = log.allocated_types[i] / (float)MB;
        } else if(log.allocated_types[i] >= KB) {
            q[0] = 'K';
            amt = log.allocated_types[i] / (float)KB;
        } else {
            q[0] = 'B';
            q[1] = 'y';
            amt = (float)log.allocated_types[i];
        }

        len += snprintf(msg + len, size, " %s: %.2f%s\n", mem_types_strings[i], amt, q);
    }
    char* result = strDupe(msg);
    return result;
}