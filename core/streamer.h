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

#ifndef CORE_STREAMER_H_
#define CORE_STREAMER_H_

#include <string.h>
#include <stdint.h>
#include <assert.h>

class RsStreamer {
public:
	RsStreamer();
    virtual ~RsStreamer();
    void initialize(char* buf, int nb);
private:
    template <class T> void get_as_type(T& x)
    {
        int typesize = sizeof(T);
        assert((curr+typesize)<=(base+size));
        memcpy((char*)&x, curr, typesize);
        curr += typesize;
    }
    template <class T> void set_as_type(T& x)
    {
        int typesize = sizeof(T);
        assert((curr+typesize)<=(base+size));
        memcpy(curr, (char*)&x, typesize);
        curr += typesize;
    }
public:
    char* get_buffer()
    {
        return base;
    }

    uint32_t get_size()
    {
        return size;
    }

public:
    //for read
    uint8_t read_char();
    uint16_t read_short();
    uint32_t read_int();
    uint64_t read_long();
    void skip(int nb);
    char* read_nbytes(int nb);
public:
    //for write
    void write_char(char value);
    void write_short(uint16_t value);
    void write_int(uint32_t value);
    void write_long(uint64_t value);
    void write_nbytes(char* buf, int nb);
private:
    char* base;
    char* curr;
    int size;
};

#endif /* CORE_STREAMER_H_ */
