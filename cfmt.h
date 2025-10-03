/*
Authors:   tspike (github.com/tspike2k)
Copyright: Copyright (c) 2025
License:   Boost Software License 1.0 (See LICENSE.txt or https://www.boost.org/LICENSE_1_0.txt)
*/

#ifndef CFMT_H
#define CFMT_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h> // size_t

typedef struct{
    uint32_t info; // TODO: This should contain both the arg type and an array count. Array count is limited!
    union{
        int64_t  data_int;
        uint64_t data_uint;
        void    *data_pointer;
    };
} Fmt_Arg;

typedef void (*Fmt_Put)(void *data, const char *text, size_t text_count);

// NOTE: The macros fmt_buffer and fmt_out both expand to a scoped code block. As such,
// fmt_buffer can't return a value like a normal function. But the caller needs to know
// how much text was written into the buffer. To simplify that issue, we provide the
// Fmt_Buffer data type which is passed by pointer to the fmt_buffer macro.
typedef struct{
    char*  data;
    size_t size;
    size_t used;
} Fmt_Buffer;

#define Fmt__Array_Len(a) (sizeof((a)) / sizeof((a)[0]))

// NOTE: If zero arguments are passed as varargs then fmt_args will still have a length of 1.
// This shouldn't cause much of an issue as that bug can be pretty easily found, but it is a
// limitation of this approach.
#define fmt_buffer(s, dest, ...){                                      \
    Fmt_Arg fmt_args[] = {__VA_ARGS__};                                \
    fmt_buffer_raw((s), dest, &fmt_args[0], Fmt__Array_Len(fmt_args)); \
}

#define fmt_out(s, ...){                                      \
    Fmt_Arg fmt_args[] = {__VA_ARGS__};                       \
    fmt_out_raw((s), &fmt_args[0], Fmt__Array_Len(fmt_args)); \
}

Fmt_Arg fmt_i(int64_t value);

Fmt_Buffer fmt_make_buffer(char* buffer, size_t buffer_size);
void fmt_buffer_raw(const char *fmt_string, Fmt_Buffer *dest, Fmt_Arg* args, size_t args_count);
void fmt_out_raw(const char *fmt_string, Fmt_Arg* args, size_t args_count);
void fmt_stdout_put(const char* text, size_t text_count);

#endif // CFMT_H
