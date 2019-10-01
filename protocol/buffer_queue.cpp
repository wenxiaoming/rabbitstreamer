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

#include "buffer_queue.h"
#include "core/core_struct.h"
#include "core/error_code.h"
#include <stdio.h>

namespace rs {
namespace protocol {

RsBufferQueue::RsBufferQueue(const MD5_Hash_Str& hash, const string& name, bool source)
{
    queue_index = 0;
    dequeue_index = 0;
    min_block_id = 0;
    min_block_index = 0;
    buffer_full_flag = false;
    buffer_number = 0;
    source_hash = hash;
    source_name = name;
    is_source = source;
    time(&create_time);
}

RsBufferQueue::~RsBufferQueue() {

}

void RsBufferQueue::reserve_buffer(int size) {
    buffer_number = size;
    for(int i = 0; i < buffer_number; i++) {
        media_buffer* buf = new media_buffer;
        buf->flag = BUFFER_AVAILABLE;
        buf->buffer = new char[MEDIA_BLOCK_SIZE];
        buf->block_id = 0;
        buf->block_size = MEDIA_BLOCK_SIZE;
        buffer_vector.push_back(buf);
    }
}

media_buffer* RsBufferQueue::queue_buffer() {
    media_buffer* buf = buffer_vector[queue_index];
    buf->flag = BUFFER_WRITING;

    if(buffer_full_flag) {
        min_block_id++;
        min_block_index = (queue_index+1)%(buffer_number);//the min block's index is the next one of queue index
    }

    //printf("RsBufferQueue::queue_buffer() min_block_id:%d \n", min_block_id);
    return buf;
}

int RsBufferQueue::update_buffer_attr(int block_id, BUFFER_FLAG flag) {
    int ret = ERROR_SUCCESS;
    media_buffer* buf = buffer_vector[queue_index];
    buf->block_id = block_id;
    buf->flag = flag;
    if(!buffer_full_flag && ((queue_index+1) == buffer_number))
        buffer_full_flag = true;

    queue_index = (queue_index+1)%(buffer_number);
    return ret;
}

media_buffer* RsBufferQueue::dequeue_buffer(int block_id) {
    int curr_buf_size = buffer_full_flag ?  buffer_number : queue_index;
    if((min_block_id > block_id)||(block_id >= (min_block_id + curr_buf_size))) { //TODO:confirm >= or >
        printf("block_id:%d min_block_id:%d curr_buf_size:%d \n", block_id, min_block_id, curr_buf_size);
        printf("RsBufferQueue::dequeue_buffer() return NULL \n");
        return NULL;
    } else
        printf("RsBufferQueue::dequeue_buffer() block_id:%d min_block_id:%d \n", block_id, min_block_id);

    int offset = block_id - min_block_id;
    if(offset < (buffer_number-min_block_index))
        return buffer_vector[min_block_index+offset];
    else
        return buffer_vector[offset-((buffer_number-min_block_index))];
}

int RsBufferQueue::get_buffer_interval(int& start, int& end) {
    start = min_block_id;
    if(buffer_full_flag)
        end = start+buffer_number;
    else
        end = start+queue_index;
    return 0;
}

void RsBufferQueue::set_header(const char* header, int size) {
    media_header.media_type = new char[size];//FIXME:where to release it?
    memcpy(media_header.media_type, header, size);
    media_header.header_size = size;
}

void RsBufferQueue::get_header(char*& header, int& size) {
    header = media_header.media_type;
    size = media_header.header_size;
}

}
}
}// namespace rs::protocol