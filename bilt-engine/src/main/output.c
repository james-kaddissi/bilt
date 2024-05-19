#include "output.h"
#include "assertions.h"

// necessary imports for output
#include <stdio.h>
#include <string.h>
#include <stdarg.h>

b8 start_log() {
    // create log TODO
    return TRUE;
};
void stop_log() {
    // clean and clear log TODO
};

// log output function with variable arguments
void log_output(log_level level, const char* message, ...){
    const char* prefixes[6] = {"(FATAL) -> ", "(ERROR) -> ", "(WARN) -> ", "(INFO) -> ", "(DEBUG) -> ", "(TRACE) -> "};
    // b8 err = level <= 1; // determine if the log level jis an error or fatal
    
    char output[100000]; // output buffer clamped to 100k characters
    memset(output, 0, sizeof(output)); // clear output buffer

    __builtin_va_list args; // args list pointer
    va_start(args, message); // initialize the arg list
    vsnprintf(output, 100000, message, args); // format message
    va_end(args); // clean arg list
    char final_output[100000];
    sprintf(final_output, "%s%s\n", prefixes[level], output); // concatenate output and prefix and stores back in final_output
    printf("%s", final_output); // print final output
};

void assertion_failure(const char* expression, const char* message, const char* file, i32 line) {
    log_output(LOG_LEVEL_FATAL, "Assertion Failure -> %s, message: '%s', in file: %s, on line: %d\n", expression, message, file, line); // pass assertion failures to logger with variable arguments
}