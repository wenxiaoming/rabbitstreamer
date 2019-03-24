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

#ifndef CORE_BUFFER_H_
#define CORE_BUFFER_H_

#include "socket.h"

namespace core {

class RsBuffer {

public:
    RsBuffer();
    virtual ~RsBuffer();

public:
    int current_size();
    char* bytes();
    void set_buffer(int size);

public:
    char read_byte();

    char* read_nbytes(int size);

    void skip(int size);

    int fill_buffer(RsSocket* io, int size);

private:
    /*****************************************
	               buffer_size
	    ________________|________________
	   |                                 |
	 buffer     curr_ptr   end_ptr       |
	   |____________|___________|________|

    ******************************************/
    char* curr_ptr; //the pointer for the current position of the using buffer
    char* end_ptr; //the pointer for the end position of the using buffer

    char* buffer;//the pointer for the base position of the buffer
    int buffer_size;//the total size of the buffer
};

} /* namespace core */

#endif /* CORE_BUFFER_H_ */
