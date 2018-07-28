/*
The MIT License (MIT)

Copyright (c) 2016-2018 RabbitStreamer

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
the Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include "buffer.h"
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "error_code.h"
#include "core_utility.h"
#include "debug_utility.h"

#define DEFAULT_RECV_BUFFER_SIZE 131072
#define MAX_SOCKET_BUFFER 262144

RsBuffer::RsBuffer()
{
    buffer_size = DEFAULT_RECV_BUFFER_SIZE;
    buffer = (char*)malloc(buffer_size);
    curr_ptr = end_ptr = buffer;
}

RsBuffer::~RsBuffer()
{
    free(buffer);
    buffer = NULL;
}

int RsBuffer::current_size()
{
    return (int)(end_ptr - curr_ptr);
}

char* RsBuffer::bytes()
{
    return curr_ptr;
}

void RsBuffer::set_buffer(int size)
{
    // never exceed the max size.
    if (size > MAX_SOCKET_BUFFER) {
        return;
    }

    // the user-space buffer size limit to a max value.
    int nb_resize_buf = rs_min(size, MAX_SOCKET_BUFFER);

    // only realloc when buffer changed bigger
    if (nb_resize_buf <= buffer_size) {
        return;
    }

    // realloc for buffer change bigger.
    int start = (int)(curr_ptr - buffer);
    int nb_bytes = (int)(end_ptr - curr_ptr);

    buffer = (char*)realloc(buffer, nb_resize_buf);
    buffer_size = nb_resize_buf;
    curr_ptr = buffer + start;
    end_ptr = curr_ptr + nb_bytes;
}

char RsBuffer::read_byte()
{
    return *curr_ptr++;
}

char* RsBuffer::read_nbytes(int size)
{
    char* ptr = curr_ptr;
    curr_ptr += size;

    return ptr;
}

void RsBuffer::skip(int size)
{
    curr_ptr += size;
}

int RsBuffer::fill_buffer(RsSocket* io, int size)
{
    int ret = ERROR_SUCCESS;
    if(size < 0)
        print_backtrace();
    assert(size >= 0);
    // current buffer is enough
    if (end_ptr - curr_ptr >= size) {
        return ret;
    }

    // the bytes will be consumed
    int exists_bytes = (int)(end_ptr - curr_ptr);

    //enlarge the buffer
    if(buffer_size < size)
        set_buffer(size);//TODO,handle the error

    // the free space of buffer,
    int free_space = (int)(buffer + buffer_size - end_ptr);

    if(!exists_bytes)
        curr_ptr = end_ptr = buffer;//reset when buffer is consumed completely.
    else if(free_space < size)
    {
        // move current buffer to the beginning.
        buffer = (char*)memmove(buffer, curr_ptr, exists_bytes);
        curr_ptr = buffer;
        end_ptr = curr_ptr + exists_bytes;
        free_space = (int)(buffer + buffer_size - end_ptr);
    }

    // fill required size of bytes to the buffer.
    while (end_ptr - curr_ptr < size) {
        ssize_t nread;
        if ((ret = io->read(end_ptr, free_space, &nread)) != ERROR_SUCCESS) {
            print_backtrace();
            return ret;//TODO:FIXME, how to handle this error
        }
        assert((int)nread > 0);
        end_ptr += nread;
        free_space -= nread;
    }

    return ret;
}
