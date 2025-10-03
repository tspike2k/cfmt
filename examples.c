/*
Authors:   tspike (github.com/tspike2k)
Copyright: Copyright (c) 2025
License:   Boost Software License 1.0 (See LICENSE.txt or https://www.boost.org/LICENSE_1_0.txt)
*/

#include "cfmt.c"

char g_buffer[16];

int main(){
    // We can write strings directly to stdout using this convenience function.
    const char *msg = "Hello, world!\n";
    fmt_stdout_put(msg, strlen(msg));

    // Formatted data can be passed directly stdout.
    fmt_out("The numbers are {1}, {2}, and {0}.\n", fmt_i(1), fmt_i(2), fmt_i(3));

    // Data can also be formatted to a fixed-size buffer. The result is null terminated
    // allowing the caller to easily pass the buffer to functions that expect a C-string.
    Fmt_Buffer buffer = fmt_make_buffer(&g_buffer[0], 15);
    fmt_buffer("The number is {0}.\n", &buffer, fmt_i(42));
    msg = &g_buffer[0];
    fmt_stdout_put(msg, strlen(msg));

    // Flush stdout so all text is actually displayed on exit.
    fmt_stdout_flush();

    return 0;
}
