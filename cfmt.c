/*
Authors:   tspike (github.com/tspike2k)
Copyright: Copyright (c) 2025
License:   Boost Software License 1.0 (See LICENSE.txt or https://www.boost.org/LICENSE_1_0.txt)
*/
#include "cfmt.h"

#include <string.h> // memcpy
#include <assert.h>

#define Fmt__Temp_Buffer_Size 512
#define Fmt__Stdout_Buffer_Size 1024

static const char *fmt__int_table = "0123456789abcdefxp";
static char   fmt__stdout_buffer[Fmt__Stdout_Buffer_Size];
static size_t fmt__stdout_buffer_used;

enum{
    Fmt_Type_None,
    Fmt_Type_Unsigned_Integer,
    Fmt_Type_Signed_Integer,
    Fmt_Type_Float,
};

Fmt_Arg fmt_i(int64_t value){
    Fmt_Arg result;
    result.info = Fmt_Type_Signed_Integer;
    result.data_int = value;
    return result;
}

// TODO: This would be of greater use to people if the output were to *begin* at the start
// of dest. Investigate how to do this while still being performant.
static size_t fmt__s64(char *buffer, size_t buffer_length, int64_t value, uint32_t base){
    assert(base <= 16);

    size_t cursor = buffer_length;
    for(size_t i = cursor; i > 0; i--){
        if(value == 0) break;

        cursor = i-1;
        char c = fmt__int_table[value % base];
        buffer[cursor] = c;
        value /= base;
    }
    return cursor;
}

static void fmt__arg(void *dest, Fmt_Put put, Fmt_Arg arg){
    char temp_buffer[Fmt__Temp_Buffer_Size];
    switch(arg.info){
        default: break;

        case Fmt_Type_Signed_Integer:{
            size_t cursor = fmt__s64(&temp_buffer[0], Fmt__Temp_Buffer_Size, arg.data_int, 10);
            put(dest, &temp_buffer[cursor], Fmt__Temp_Buffer_Size - cursor);
        } break;
    }
}

#ifdef __gnu_linux__

#include <unistd.h>

// TODO: Is there a pow function for integers?
#define fmt__pow __builtin_powif

void fmt_stdout_flush(){
    if(fmt__stdout_buffer_used > 0){
        write(1, &fmt__stdout_buffer[0], fmt__stdout_buffer_used);
        fmt__stdout_buffer_used = 0;
    }
}

#endif // __gnu_linux__

static void fmt__stdout(void *dest, const char *text, size_t text_count){
    while(text_count > 0){
        size_t availible = Fmt__Stdout_Buffer_Size - fmt__stdout_buffer_used;
        size_t to_write  = text_count < availible ? text_count : availible;

        memcpy(&fmt__stdout_buffer[fmt__stdout_buffer_used], text, to_write);
        fmt__stdout_buffer_used += to_write;
        if(fmt__stdout_buffer_used == Fmt__Stdout_Buffer_Size || text[text_count-1] == '\n'){
            fmt_stdout_flush();
        }

        text_count -= to_write;
        text += to_write;
    }
}

static size_t fmt__read_until(const char* fmt_string, size_t *reader, char delimiter_a, char delimiter_b){
    size_t start = *reader;
    char c = fmt_string[*reader];
    while(c != '\0' && c != delimiter_a && c != delimiter_b){
        (*reader)++;
        c = fmt_string[*reader];
    }
    return start;
}

static bool fmt__parse_spec(const char *fmt_string, size_t *reader, size_t *arg_index){
    assert(fmt_string[*reader] == '{');
    (*reader)++;
    size_t start = fmt__read_until(fmt_string, reader, '}', '\0');
    size_t len = *reader - start;
    if(fmt_string[*reader] == '}')
        (*reader)++;

    bool success = len > 0;

    size_t result = 0;
    for(size_t i = len; i > 0; i--){
        char c = fmt_string[start + i-1];
        if(c >= '0' && c <= '9'){
            result += (c - '0') * fmt__pow(10, len-i);
        }
        else{
            success = false;
            break;
        }
    }

    *arg_index = result;
    return success;
}

static void fmt__buffer_put(void *dest, const char* text, size_t text_count){
    Fmt_Buffer *buffer = (Fmt_Buffer*)dest;
    size_t availible = buffer->size - buffer->used;
    size_t to_copy = text_count < availible ? text_count : availible;
    memcpy(&buffer->data[buffer->used], text, to_copy);
    buffer->used += to_copy;
}

Fmt_Buffer fmt_make_buffer(char* buffer, size_t buffer_size){
    Fmt_Buffer result = {buffer, buffer_size};
    return result;
}

void fmt_buffer_raw(const char *fmt_string, Fmt_Buffer *dest, Fmt_Arg* args, size_t args_count){
    size_t reader = 0;
    while(fmt_string[reader] != '\0'){
        size_t start = fmt__read_until(fmt_string, &reader, '{', '\0');
        fmt__buffer_put(dest, &fmt_string[start], reader - start);

        if(fmt_string[reader] == '{'){
            size_t arg_index;
            if(fmt__parse_spec(fmt_string, &reader, &arg_index)
            && arg_index < args_count){
                fmt__arg(dest, fmt__buffer_put, args[arg_index]);
            }
        }
    }
    // Null terminate the end of the resulting string.
    size_t term_index = dest->used < dest->size ? dest->used : dest->size;
    dest->data[term_index] = '\0';
}

void fmt_out_raw(const char *fmt_string, Fmt_Arg* args, size_t args_count){
    size_t reader = 0;
    while(fmt_string[reader] != '\0'){
        size_t start = fmt__read_until(fmt_string, &reader, '{', '\n');
        fmt__stdout(NULL, &fmt_string[start], reader - start);
        if(fmt_string[reader] == '{'){
            size_t arg_index;
            if(fmt__parse_spec(fmt_string, &reader, &arg_index)
            && arg_index < args_count){
                fmt__arg(NULL, fmt__stdout, args[arg_index]);
            }
        }
        else if(fmt_string[reader] == '\n'){
            fmt__stdout(NULL, "\n", 1);
            fmt_stdout_flush();
            reader++;
        }
    }
}

void fmt_stdout_put(const char* text, size_t text_count){
    fmt__stdout(NULL, text, text_count);
}
