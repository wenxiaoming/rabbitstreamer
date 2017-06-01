/*
The MIT License (MIT)

Copyright (c) 2016-2017 RabbitStreamer

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

#include "rs_streamer.h"

Streamer::Streamer()
{
    curr = base = NULL;
    size = 0;
}

Streamer::~Streamer()
{
}

void Streamer::initialize(char* buf, int nb)
{
    base = curr = buf;
    size = nb;
}

uint8_t Streamer::read_char()
{
    uint8_t value = 0;
    get_as_type(value);
    return value;
}

uint16_t Streamer::read_short()
{
    uint16_t value = 0;
    get_as_type(value);
    return value;
}

uint32_t Streamer::read_int()
{
    uint32_t value = 0;
    get_as_type(value);
    return value;
}

uint64_t Streamer::read_long()
{
    uint64_t value = 0;
    get_as_type(value);
    return value;
}

char* Streamer::read_nbytes(int nb)
{
    char* buf = curr;
    assert((curr+nb)<=(base+size));
    curr += nb;
    return buf;
}

void Streamer::skip(int nb)
{
    assert((curr+nb)<=(base+size));
    curr += nb;
}

void Streamer::write_char(char value)
{
    *curr++ = value;
}

void Streamer::write_short(uint16_t value)
{
    set_as_type(value);
}

void Streamer::write_int(uint32_t value)
{
    set_as_type(value);
}

void Streamer::write_long(uint64_t value)
{
    set_as_type(value);
}

void Streamer::write_nbytes(char* buf, int nb)
{
    memcpy(curr, buf, nb);
    curr += nb;
}
